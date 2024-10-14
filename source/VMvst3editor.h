//
// Copyright (c) 2024 suzumushi
//
// 2024-8-26		VMvst3editor.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "vstgui4/vstgui/plugin-bindings/vst3editor.h"
#include "VMconfig.h"


namespace VSTGUI {

// VST3Editor for VoMPE

class VMVST3Editor: public VST3Editor
{
public:
	VMVST3Editor (suzumushi::VoMPEController* controller, UTF8StringPtr templateName, UTF8StringPtr xmlFile):
		VST3Editor (controller, templateName, xmlFile), 
		SpectrumParameter {controller -> SpectrumParameter}, 
		RulerParameter {controller -> RulerParameter}
		{setIdleRate (suzumushi::frame_len);}

	VMVST3Editor (UIDescription* desc, suzumushi::VoMPEController* controller, UTF8StringPtr templateName, UTF8StringPtr xmlFile = nullptr):
		VST3Editor (desc, controller, templateName, xmlFile = nullptr), 
		SpectrumParameter {controller -> SpectrumParameter}, 
		RulerParameter {controller -> RulerParameter}
		{setIdleRate (suzumushi::frame_len);}

	VSTGUI::CMessageResult notify (VSTGUI::CBaseObject* sender, const char* message)
	{
		if (message == CVSTGUITimer::kMsgTimer) {
			Steinberg::Vst::IMessage* message = controller -> allocateMessage ();
			message -> setMessageID ("VMtimer");
			controller -> sendMessage (message);
			message -> release();
		}
		return (VST3Editor::notify (sender, message));
	}

	Steinberg::Vst::SpectrumParameter *SpectrumParameter;
	Steinberg::Vst::RulerParameter *RulerParameter;
};

} // namespace VSTGUI
