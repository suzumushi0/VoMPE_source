//
// Copyright (c) 2024 suzumushi
//
// 2024-10-7		VMprocessor.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "VMprocessor.h"
#include "VMcids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

using namespace Steinberg;

namespace suzumushi {
//------------------------------------------------------------------------
// VoMPEProcessor
//------------------------------------------------------------------------
VoMPEProcessor:: VoMPEProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kVoMPEControllerUID);
}

//------------------------------------------------------------------------
VoMPEProcessor:: ~VoMPEProcessor ()
{
	//
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk) {
		return result;
	}

	//--- create Audio IO ------
	// suzumushi:
	addAudioInput (STR16 ("Monaural In"), Steinberg::Vst::SpeakerArr::kMono);
	addAudioOutput (STR16 ("Monaural Out"), Steinberg::Vst::SpeakerArr::kMono);
	addEventOutput (STR16 ("MIDI Out"));

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: setActive (TBool state)
{
	// suzumushi:
	if (state != 0)				// if (state == true)
		reset ();

	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: process (Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

   if (data.inputParameterChanges) {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
        for (int32 index = 0; index < numParamsChanged; index++) {
            if (auto* paramQueue = data.inputParameterChanges->getParameterData (index)) {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
				// suzumushi: get the last change
				if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue) {
					gui_param_update (paramQueue->getParameterId (), value);
				}
			}
		}
	}
		
	//--- Here you have to implement your processing

	// numInputs == 0 and data.numOutputs == 0 mean parameters update only
	if (data.numInputs == 0 || data.numOutputs == 0) {
		return kResultOk;
	}
	// Speaker arrangements (at least mono in and out are required) check.
	if (data.inputs[0].numChannels == 0 || data.outputs[0].numChannels == 0) {
		return kResultOk;
	}

	Vst::Sample32* in = data.inputs[0].channelBuffers32[0];
	Vst::Sample32* out = data.outputs[0].channelBuffers32[0];

	data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;
	int at = ODL_len - 1 - std::min ((int)(gp.delay * processSetup.sampleRate / 1000.0 + 0.5), ODL_len -1);
	for (int32 i = 0; i < data.numSamples; i++) {
		ODL.enqueue (*in);
		if (processSetup.sampleRate == internal_SR) {
			IDL.enqueue (*in++);
		} else {		// not 16KHz sampling
			in++;
			if (--down_counter == 0) {
				down_counter = 3;
				IDL.enqueue (DS.process (ODL));
			}
		}
		*out++ = ODL.read (at);

		if (--unprocessed_len == 0) {
			unprocessed_len = process_len;

			float input [FFT_len] {};				// input signal
			std::complex <float> cs [FFT_len] {};	// complex spectrum		
			float ac [order + 1] {};				// autocorrelation
			float lpc [order + 1] {};				// linear prediction coefficients
			float ir [FFT_len] {};					// impulse response of analysis filter 

			// input signal to linear prediction coefficients
			for (int j = 0; j < FFT_len; j++)
				input [j] = FFT.window (j) * IDL.read (FFT_len - 1 - j);
			autocorrelation <float, FFT_len, order + 1> (input, ac);
			ac_to_LPC <float, order + 1> (ac, lpc);

			// lpc to amplitude spectrum of analysis filter
			for (int j = 0; j < order + 1; j++)
				ir [j] = lpc [j];
			FFT.FFT (ir, cs);
			ready = false;							// for thread safe 
			FFT.cs_to_dBas (cs, spectral_envelope);

			// amplitude spectrum of synthesis filter, i.e., spectral envelope
			for (int j = 0; j < FFT_len / 2; j++)
				spectral_envelope [j] = -spectral_envelope [j];

			// MIDI out
			MIDI.setup (gp, spectral_envelope, peak_freq);
			ready = true;							// for thread safe
			MIDI.process (gp, peak_freq, data);
		}
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: setupProcessing (Vst::ProcessSetup& newSetup)
{
	// suzumushi:
	process_len = newSetup.sampleRate * frame_len / 1'000.0 + 0.5;
	gp.SR = newSetup.sampleRate;
	gp.ruler_changed = true;

	//--- called before any processing ----
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);

	// suzumushi:
	int version;
	if (streamer.readInt32 (version) == false)
		return (kResultFalse);
	if (streamer.readInt32 (gp.fo_bw) == false)
		return (kResultFalse);
	if (streamer.readInt32 (gp.num_fo) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp.pitch) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp.thresh) == false)
		return (kResultFalse);
	if (streamer.readInt32 (gp.pbend) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp.delay) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp.pause) == false)
		return (kResultFalse);

	gp.ruler_changed = true;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEProcessor:: getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	// suzumushi:
	int version = 0;
	if (streamer.writeInt32 (version) == false)
		return (kResultFalse);

	if (streamer.writeInt32 (gp.fo_bw) == false)
		return (kResultFalse);
	if (streamer.writeInt32 (gp.num_fo) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.pitch) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.thresh) == false)
		return (kResultFalse);
	if (streamer.writeInt32 (gp.pbend) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.delay) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.pause) == false)
		return (kResultFalse);

	return kResultOk;
}

//------------------------------------------------------------------------
// suzumushi:

void VoMPEProcessor:: gui_param_update (const ParamID paramID, const ParamValue paramValue) 
{
	switch (paramID) {
		case fo_bw.tag:
			gp.fo_bw = stringListParameter::toPlain (paramValue, (int32)FO_BW_L::LIST_LEN);
			gp.ruler_changed = true;
			break;
		case num_fo.tag:
			gp.num_fo = stringListParameter::toPlain (paramValue, (int32)NUM_FO_L::LIST_LEN);
			gp.ruler_changed = true;
			break;
		case pitch.tag:
			gp.pitch = Vst::LogTaperParameter::toPlain (paramValue, pitch);
			break;
		case thresh.tag:		
			gp.thresh = rangeParameter::toPlain (paramValue, thresh);
			gp.ruler_changed = true;
			break;
		case pbend.tag:
			gp.pbend =  stringListParameter::toPlain (paramValue, (int32)PBEND_L::LIST_LEN);
			break;
		case delay.tag:
			gp.delay =  rangeParameter::toPlain (paramValue, delay);
			break;
		case pause.tag:		
			gp.pause = rangeParameter::toPlain (paramValue, pause);
			break;
	}
}

tresult PLUGIN_API VoMPEProcessor:: notify (Vst::IMessage* message)
{
	const char *in_message_id  = message -> getMessageID ();
	if (strcmp (in_message_id, "VMtimer")  == 0) {
		if (gp.ruler_changed) {
			gp.ruler_changed = false;
			struct rulerParameter rp = {gp.fo_bw, gp.num_fo, gp.thresh, gp.SR};
			// send ruler parameter
			Vst::IMessage* out_message = allocateMessage ();
			out_message -> setMessageID ("VMruler");
			out_message -> getAttributes() -> setBinary ("ruler", &rp, sizeof (rulerParameter));
			sendMessage (out_message);
			out_message -> release();
		}
		if (ready) {
			struct spectrumParameter sp;
			for (int i = 0; i < FFT_len / 2; i++)
				sp.spectral_envelope [i] = spectral_envelope [i];
			for (int i = 0; i < max_FB; i++)
				sp.peak_freq [i] = peak_freq [i].freq;
			sp.pause = gp.pause;
			if (ready) {				// for thread safe 
				ready = false;
				Vst::IMessage* out_message = allocateMessage ();
				out_message -> setMessageID ("VMspec");
				out_message -> getAttributes() -> setBinary ("spec", &sp, sizeof (spectrumParameter));
				sendMessage (out_message);
				out_message -> release();
				return (kResultTrue);
			}
		}
		// spectrum parameter is not ready
		Vst::IMessage* out_message = allocateMessage ();
		out_message -> setMessageID ("VMbusy");
		sendMessage (out_message);
		out_message -> release();
		return (kResultTrue);		
	} 
	return (AudioEffect:: notify (message));
}

void VoMPEProcessor:: reset ()
{
	unprocessed_len = process_len;
	down_counter = 3;
	ready = false;
	IDL.reset ();
	ODL.reset ();
	MIDI.reset ();
}

//------------------------------------------------------------------------
} // namespace suzumushi
