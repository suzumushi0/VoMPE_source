//
// Copyright (c) 2024 suzumushi
//
// 2024-8-27	VMcontroller.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "VMcontroller.h"
#include "VMcids.h"
#include "VMvst3editor.h"
#include "vstgui/plugin-bindings/vst3editor.h"

// suzumushi:
#include "base/source/fstreamer.h"

using namespace Steinberg;

namespace suzumushi {

//------------------------------------------------------------------------
// VoMPEController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEController:: initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk) {
		return result;
	}

	// Here you could register some parameters

	Vst::StringListParameter* fo_bw_param = new Vst::StringListParameter (
		STR16 ("Formant bandwidth"), fo_bw.tag, nullptr, fo_bw.flags);
	fo_bw_param -> appendString (STR16 ("500 Hz"));
	fo_bw_param -> appendString (STR16 ("750 Hz"));
	fo_bw_param -> appendString (STR16 ("1,000 Hz"));
	fo_bw_param -> appendString (STR16 ("1,250 Hz"));
	fo_bw_param -> appendString (STR16 ("1,500 Hz"));
	parameters.addParameter (fo_bw_param);

	Vst::StringListParameter* num_fo_param = new Vst::StringListParameter (
		STR16 ("Number of formant bands"), num_fo.tag, nullptr, num_fo.flags);
	num_fo_param -> appendString (STR16 ("2"));
	num_fo_param -> appendString (STR16 ("3"));
	num_fo_param -> appendString (STR16 ("4"));
	num_fo_param -> appendString (STR16 ("5"));
	num_fo_param -> appendString (STR16 ("6"));
	num_fo_param -> appendString (STR16 ("7"));
	num_fo_param -> appendString (STR16 ("8"));
	parameters.addParameter (num_fo_param);

	Vst::LogTaperParameter* pitch_param = new Vst::LogTaperParameter (
		STR16 ("Formant and MIDI frequency ratio"), pitch.tag, STR16 (""),
		pitch.min, pitch.max, pitch.def, pitch.steps, pitch.flags);
	pitch_param -> setPrecision (precision2);
	parameters.addParameter (pitch_param);

	Vst::RangeParameter* thresh_param = new Vst::RangeParameter (
		STR16 ("MIDI note on/off threshold"), thresh.tag, STR16 ("dB"),
		thresh.min, thresh.max, thresh.def, thresh.steps, thresh.flags);
	thresh_param -> setPrecision (precision1);
	parameters.addParameter (thresh_param);

	Vst::StringListParameter* pbend_param = new Vst::StringListParameter (
		STR16 ("MIDI pitch bend"), pbend.tag, nullptr, pbend.flags);
	pbend_param -> appendString (STR16 ("Off"));
	pbend_param -> appendString (STR16 ("On"));
	parameters.addParameter (pbend_param);

	Vst::RangeParameter* delay_param = new Vst::RangeParameter (
		STR16 ("Audio output delay"), delay.tag, STR16 ("ms"),
		delay.min, delay.max, delay.def, delay.steps, delay.flags);
	delay_param -> setPrecision (precision1);
	parameters.addParameter (delay_param);

	Vst::RangeParameter* pause_param = new Vst::RangeParameter (
		STR16 ("Pause to view drawing"), pause.tag, STR16 (""),
		pause.min, pause.max, pause.def, pause.steps, pause.flags);
	parameters.addParameter (pause_param);

	SpectrumParameter = new Vst::SpectrumParameter (
		STR16 ("Spectrum"), spec.tag, STR16 (""), spec.def, spec.steps, spec.flags);
	parameters.addParameter (SpectrumParameter);

	RulerParameter = new Vst::RulerParameter (
		STR16 ("Ruler"), ruler.tag, STR16 (""), ruler.def, ruler.steps, ruler.flags);
	parameters.addParameter (RulerParameter);

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEController:: terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEController:: setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	// suzumushi:
	ParamValue dtmp;
	int32 itmp;
	int32 version;
	IBStreamer streamer (state, kLittleEndian);

	if (streamer.readInt32 (version) == false)
		return (kResultFalse);

	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (fo_bw.tag, plainParamToNormalized (fo_bw.tag, (ParamValue)itmp));

	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (num_fo.tag, plainParamToNormalized (num_fo.tag, (ParamValue)itmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (pitch.tag, plainParamToNormalized (pitch.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (thresh.tag, plainParamToNormalized (thresh.tag, dtmp));

	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (pbend.tag, plainParamToNormalized (pbend.tag, (ParamValue)itmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (delay.tag, plainParamToNormalized (delay.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (pause.tag, plainParamToNormalized (pause.tag, dtmp));

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEController:: setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEController:: getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API VoMPEController:: createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor)) {
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VMVST3Editor (this, "view", "VoMPE.uidesc");

		// suzumushi:
		std::vector<double> zoom_factors = {0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0};
		view->setAllowedZoomFactors (zoom_factors);

		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VoMPEController:: notify (Vst::IMessage* message)
{
	const char *in_message_id  = message -> getMessageID ();
	if (strcmp (in_message_id, "VMspec")  == 0) {
		const spectrumParameter *sp;
		uint32 size;
		if (message -> getAttributes() -> getBinary ("spec", (const void *&)sp, size) == kResultTrue) {
			for (int i = 0; i < FFT_len / 2; i++)
				SpectrumParameter -> spectrum.spectral_envelope [i] = sp -> spectral_envelope [i];
			for (int i = 0; i < max_FB; i++)
				SpectrumParameter -> spectrum.peak_freq [i] = sp -> peak_freq [i];
			if (! sp -> pause)
				SpectrumParameter -> changed ();
		}
		retry = false;
		return (Vst::EditControllerEx1:: notify (message));
	}
	if (strcmp (in_message_id, "VMruler")  == 0) {
		const rulerParameter *rp;
		uint32 size;
		if (message -> getAttributes() -> getBinary ("ruler", (const void *&)rp, size) == kResultTrue) {
			RulerParameter -> ruler.fo_bw = rp -> fo_bw;
			RulerParameter -> ruler.num_fo = rp -> num_fo;
			RulerParameter -> ruler.thresh = rp -> thresh;
			RulerParameter -> ruler.SR = rp -> SR;
			RulerParameter -> changed ();
		}
		return (Vst::EditControllerEx1:: notify (message));
	}
	if (strcmp (in_message_id, "VMbusy")  == 0) {
		if (! retry) {
			retry = true;
			Steinberg::Vst::IMessage* message = allocateMessage ();
			message -> setMessageID ("VMtimer");
			sendMessage (message);
			message -> release();
		} else {
			retry = false;
		}
	}
	return (Vst::EditControllerEx1:: notify (message));
}

} // namespace suzumushi
