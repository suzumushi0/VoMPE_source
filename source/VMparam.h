//
// Copyright (c) 2024 suzumushi
//
// 2024-10-8		VMparam.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
using Steinberg::Vst::ParameterInfo;
using Steinberg::Vst::ParamValue;
using Steinberg::int32;

#include "VMconfig.h"
#include "SOextparam.h"

namespace suzumushi {

// definition of GUI and host facing parameters tag

constexpr ParamID FO_BW {0};			// formant bandwidth
constexpr ParamID NUM_FO {1};			// number of formant bands
constexpr ParamID PITCH {2};			// frequency ratio between formant and MIDI note
constexpr ParamID THRESH {3};			// MIDI note on/off threshold [dB]
constexpr ParamID PBEND {4};			// MIDI pitch bend mode
constexpr ParamID DELAY {5};			// audio output delay [ms]
constexpr ParamID PAUSE {10};			// pause to view drawing
constexpr ParamID SPEC {100};			// parameter for displaying spectrum
constexpr ParamID RULER {101};			// parameter for displaying ruler

// attributes of GUI and host facing parameter

// fo_bw: formant bandwidth
constexpr struct stringListParameter fo_bw = {
	FO_BW,								// tag
	{ParameterInfo::kIsList}			// flags
};

enum class FO_BW_L {
	B500,
	B750,
	B1000,
	B1250,
	B1500,
	LIST_LEN
};

// 16KHz and 48KHz sample rate
constexpr int fo_bw_val [(int)FO_BW_L::LIST_LEN]  = {
	(FFT_len / 2 / max_FB) / 2,
	(FFT_len / 2 / max_FB) * 3 / 4,
	(FFT_len / 2 / max_FB),
	(FFT_len / 2 / max_FB) * 5 / 4,
	(FFT_len / 2 / max_FB) * 3 / 2
};

// 44.1KHz sample rate
constexpr int fo_bw_441_val [(int)FO_BW_L::LIST_LEN]  = {
	fo_bw_val [0] * 480 / 441,
	fo_bw_val [1] * 480 / 441,
	fo_bw_val [2] * 480 / 441,
	fo_bw_val [3] * 480 / 441,
	fo_bw_val [4] * 480 / 441
};

// num_fo: number of formant bands
constexpr struct stringListParameter num_fo = {
	NUM_FO,								// tag
	{ParameterInfo::kIsList}			// flags
};

enum class NUM_FO_L {
	N2,
	N3,
	N4,
	N5,
	N6,
	N7,
	N8,
	LIST_LEN
};

constexpr int num_fo_val [(int)NUM_FO_L::LIST_LEN]  = {
	2,
	3,
	4,
	5,
	6,
	7,
	8
};

// pitch: frequency ratio between formant and MIDI note
constexpr struct logTaperParameter pitch = {
	PITCH,								// tag
	{0.1},								// min
	{2.0},								// max
	{0.3},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// thresh: MIDI note on/off threshold [dB]
constexpr struct rangeParameter thresh = {
	THRESH,								// tag
	{-20.0},							// min
	{40.0},								// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// pbend: MIDI pitch bend mode
constexpr struct stringListParameter pbend = {
	PBEND,								// tag
	{ParameterInfo::kIsList}			// flags
};

enum class PBEND_L {
	OFF,
	ON,
	LIST_LEN
};

// delay: audio output delay [ms]
constexpr struct rangeParameter delay = {
	DELAY,								// tag
	{0.0},								// min
	{40.0},								// max
	{8.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};
constexpr int ODL_len {(int)(48.0 * delay.max + 0.5) + 1};	

// pause: pause to view drawing
constexpr struct rangeParameter pause = {
	PAUSE,								// tag
	{0.0},								// min, false
	{1.0},								// max, true
	{0.0},								// default
	{1},								// toggle
	{ParameterInfo::kCanAutomate}		// flags
};

// spec: parameter for displaying spectrum
constexpr struct rangeParameter spec = {
	SPEC,								// tag
	{0.0},								// min
	{1.0},								// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kIsHidden}			// flags
};

// ruler: parameter for displaying ruler
constexpr struct rangeParameter ruler = {
	RULER,								// tag
	{0.0},								// min
	{1.0},								// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kIsHidden}			// flags
};

//  GUI and host facing parameters in processor class

struct GUI_param {
	int32 fo_bw {(int32)FO_BW_L::B1000};
	int32 num_fo {(int32)NUM_FO_L::N8};
	ParamValue pitch {suzumushi::pitch.def};
	ParamValue thresh {suzumushi::thresh.def};
	int32 pbend {(int32)PBEND_L::OFF};
	ParamValue delay {suzumushi::delay.def};
	ParamValue pause {suzumushi::pause.def};
	float SR {16'000.0f};				// sample rate
	bool ruler_changed {false};
};

}
