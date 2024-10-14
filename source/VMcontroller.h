//
// Copyright (c) 2024 suzumushi
//
// 2024-8-25		VMcontroller.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

// suzumushi: 
#include "VMparam.h"
#include "VMextparam.h"

namespace suzumushi {

//------------------------------------------------------------------------
//  VoMPEController
//------------------------------------------------------------------------
class VoMPEController: public Steinberg::Vst::EditControllerEx1
{
public:
//------------------------------------------------------------------------
	VoMPEController () = default;
	~VoMPEController () SMTG_OVERRIDE = default;

    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/) {
		return (Steinberg::Vst::IEditController*)new VoMPEController;
	}

	//--- from IPluginBase -----------------------------------------------
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	//--- from EditController --------------------------------------------
	Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

	// suzumushi:
	Steinberg::tresult PLUGIN_API notify (Steinberg::Vst::IMessage* message) SMTG_OVERRIDE;

 	//---Interface---------
	DEFINE_INTERFACES
		// Here you can add more supported VST3 interfaces
		// DEF_INTERFACE (Vst::IXXX)
	END_DEFINE_INTERFACES (EditController)
    DELEGATE_REFCOUNT (EditController)

//------------------------------------------------------------------------
// suzumushi:
	Steinberg::Vst::SpectrumParameter *SpectrumParameter = nullptr;
	Steinberg::Vst::RulerParameter *RulerParameter = nullptr;
protected:
	bool retry {false};
};

//------------------------------------------------------------------------
} // namespace suzumushi
