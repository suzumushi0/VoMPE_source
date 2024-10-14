//
// Copyright (c) 2024 suzumushi
//
// 2024-8-27		VMextparam.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "VMparam.h"

using Steinberg::Vst::ParameterInfo;
using Steinberg::Vst::ParamID;
using Steinberg::Vst::ParamValue;
using Steinberg::int32;
using Steinberg::int16;


namespace suzumushi {

// templates for parameter definition

struct spectrumParameter {
	float spectral_envelope [FFT_len / 2] {};
	int peak_freq [max_FB] {};
	bool pause {false};
};

struct rulerParameter {
	int32 fo_bw {(int32)FO_BW_L::B1000};
	int32 num_fo {(int32)NUM_FO_L::N8};
	ParamValue thresh {suzumushi::thresh.def};
	float SR {16'000.0f};				// sample rate
};

} // namespace suzumushi

//
// Extended parameters for displaying spectrum and ruler
//

namespace Steinberg {
namespace Vst {

class SpectrumParameter: public Parameter
{
public:
	SpectrumParameter (const TChar* title, ParamID tag, const TChar* units = nullptr, 
		ParamValue defaultValuePlain = 0.0, int32 stepCount = 0, int32 flags = ParameterInfo::kIsHidden, 
		UnitID unitID = kRootUnitId, const TChar* shortTitle = nullptr):
		Parameter (title, tag, units, defaultValuePlain, stepCount, flags, unitID, shortTitle) {}

	suzumushi::spectrumParameter spectrum;

	OBJ_METHODS (SpectrumParameter, Parameter)
};

class RulerParameter: public Parameter
{
public:
	RulerParameter (const TChar* title, ParamID tag, const TChar* units = nullptr,
		ParamValue defaultValuePlain = 0.0, int32 stepCount = 0, int32 flags = ParameterInfo::kIsHidden, 
		UnitID unitID = kRootUnitId, const TChar* shortTitle = nullptr):
		Parameter (title, tag, units, defaultValuePlain, stepCount, flags, unitID, shortTitle) {}

	suzumushi::rulerParameter ruler;

	OBJ_METHODS (RulerParameter, Parameter)
};

}
}
