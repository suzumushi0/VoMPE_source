//
// Copyright (c) 2024 suzumushi
//
// 2024-11-3		VMMIDI.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

#include <cmath>
#include "VMparam.h"

namespace suzumushi {

constexpr int noteOnTime {12};		// noteOnTime * frame_len = maximum time [ms] since onteOn (pitchbend mode only)

struct peak_freq {
	int freq {0};					// index to spectrum table index
	float amplitude {0.0f};
};

class VMMIDI
{
public:
	void setup (const GUI_param &gp, const float (&spectral_envelope) [FFT_len / 2], struct peak_freq (&peak_freq) [max_FB]);
	void process (const GUI_param &gp, const struct peak_freq (&peak_freq) [max_FB], Steinberg::Vst::ProcessData& data);
	void reset ();
	constexpr VMMIDI (){
		for (int i = 0; i < 128; i++)
			freq_TBL [i] = 440.0f * std::pow (2.0f, (i - 69) / 12.0f);
	}
private:
	static inline float freq_TBL [128];		// MIDI note frequency table
	struct MIDI_stat {						// MIDI channel status table
		bool noteOn		{false};
		float freq		{0.0f};
		int note_number	{0};
		int noteOnTime	{noteOnTime};
	} MIDI_stat [max_FB];					// indexed by formant band number (not MIDI channel number)
};

} // namespace suzumushi
