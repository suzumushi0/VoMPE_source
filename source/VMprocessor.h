//
// Copyright (c) 2024 suzumushi
//
// 2024-8-24		VMprocessor.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

// suzumushi: 
#include "VMparam.h"
#include "VMextparam.h"
#include "SODDL.h"
#include "VM1_3downsampler.h"
#include "VMFFT.h"
#include "VMLPC.h"
#include "VMMIDI.h"

namespace suzumushi {

//------------------------------------------------------------------------
//  VoMPEProcessor
//------------------------------------------------------------------------
class VoMPEProcessor : public Steinberg::Vst::AudioEffect
{
public:
	VoMPEProcessor ();
	~VoMPEProcessor () SMTG_OVERRIDE;

    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/) { 
		return (Steinberg::Vst::IAudioProcessor*)new VoMPEProcessor; 
	}

	//--- ---------------------------------------------------------------------
	// AudioEffect overrides:
	//--- ---------------------------------------------------------------------
	/** Called at first after constructor */
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	
	/** Called at the end before destructor */
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	
	/** Switch the Plug-in on/off */
	Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;

	/** Will be called before any process call */
	Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
	
	/** Asks if a given sample size is supported see SymbolicSampleSizes. */
	Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

	/** Here we go...the process call */
	Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
		
	/** For persistence */
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

	// suzumushi:
	Steinberg::tresult PLUGIN_API notify (Steinberg::Vst::IMessage* message) SMTG_OVERRIDE;

//------------------------------------------------------------------------
protected:
	// suzumushi: 
	// GUI and host facing parameters
	struct GUI_param gp;

	// DSP instances 
	SODDL <float, FFT_len> IDL;
	SODDL <float, ODL_len> ODL;
	VM1_3downsampler <float, 71> DS;
	VMFFT <float, PE> FFT;
	VMMIDI MIDI;

	// internal functions and status
	void gui_param_update (const ParamID paramID, const ParamValue paramValue);
	void reset ();	 	
	int process_len {320};						// process length
	int unprocessed_len {320};					// unprocessed length
	int down_counter {3};						// one-third downsampling counter
	float spectral_envelope [FFT_len / 2] {};	// spectral envelope
	struct peak_freq peak_freq [max_FB];		// peak frequencies 
	bool ready {true};							// ready for spectral envelope and peak frequencies data
};

//------------------------------------------------------------------------
} // namespace suzumushi
