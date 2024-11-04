//
// Copyright (c) 2024 suzumushi
//
// 2024-11-3		VMMIDI.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "VMMIDI.h"

// suzumushi
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"

using namespace Steinberg;

namespace suzumushi {

void VMMIDI:: setup (const GUI_param &gp, const float (&spectral_envelope) [FFT_len / 2], struct peak_freq (&peak_freq) [max_FB])
{
	for (int i = 0; i < max_FB; i++)
		peak_freq [i].freq = 0;
	int fo_bw = fo_bw_val [gp.fo_bw];
	if (gp.SR == 44'100.0f)
		fo_bw = fo_bw_441_val [gp.fo_bw];
	int num_fo = num_fo_val [gp.num_fo];
	float threshold = gp.thresh;

	float diff = 0.0f;
	for (int i = 1; i < FFT_len / 2 - 1; i++) {
		float amp = spectral_envelope [i];
		float d = spectral_envelope [i + 1] - amp;
		if (diff >= 0.0 && d < 0.0 && amp > threshold) {
			int band = i / fo_bw;	
			if (band >= num_fo)
				return;
			int index = peak_freq [band].freq;
			if (index == 0 || amp > peak_freq [band].amplitude) {
				peak_freq [band].freq = i;
				peak_freq [band].amplitude = amp;
			}
		}
		diff = d;
	}
}

void VMMIDI:: process (const GUI_param &gp, const struct peak_freq (&peak_freq) [max_FB], Vst::ProcessData& data)
{
	float pitch = gp.pitch;
	int pbend = gp.pbend;

	Vst::Event oEvent;
	oEvent.busIndex = 0;
	oEvent.sampleOffset = 0;
	oEvent.ppqPosition = 0.0;
	oEvent.flags = 0;

	for (int i = 0; i < max_FB; i++) {
		float freq = peak_freq [i].freq * internal_SR / FFT_len * pitch;
		if (gp.SR == 44'100.0f) {
			freq *= (44.1f / 48.0f);
		}
		if (freq >= freq_TBL [21] && freq <= freq_TBL [108]) {
			int note_number = 12.0f * std::log2 (freq / 440.0f) + 69.0f + 0.5f;
			int velocity = std::max (std::min ((int)((peak_freq [i].amplitude + 20.0f) / 80.0f * 127.0f), 127), 0);
			if (MIDI_stat [i].noteOn && ((pbend && MIDI_stat [i].noteOnTime-- == 0) || (! pbend && note_number != MIDI_stat [i].note_number))) {
				// update MIDI_stat
				MIDI_stat [i].noteOn = false;
				MIDI_stat [i].noteOnTime = noteOnTime;
				// send note off
				oEvent.type = Vst::Event::kNoteOffEvent;
				oEvent.noteOff.channel = i + 1;
				oEvent.noteOff.pitch = MIDI_stat [i].note_number;
				oEvent.noteOff.velocity = 0;
				data.outputEvents->addEvent (oEvent);
			}
			if (pbend && (freq != MIDI_stat [i].freq || ! MIDI_stat [i].noteOn)) {
				// send pitch bend
				float cent = 1'200.0f * std::log2 (freq / freq_TBL [MIDI_stat [i].note_number]);
				int pitch_bend = cent * 8'191.0f / 4'800.0f + 8'192.0f + 0.5f;
				oEvent.type = Vst::Event::kLegacyMIDICCOutEvent;
				oEvent.midiCCOut.controlNumber = Vst::kPitchBend;
				oEvent.midiCCOut.channel = i + 1;
				oEvent.midiCCOut.value = pitch_bend % 128;		// LSB
				oEvent.midiCCOut.value2 = pitch_bend / 128;		// USB
				data.outputEvents->addEvent (oEvent);
			}
			MIDI_stat [i].freq = freq;				
			if (! MIDI_stat [i].noteOn) {
				// update MIDI_stat
				MIDI_stat [i].noteOn = true;
				MIDI_stat [i].note_number = note_number;
				// send note on
				oEvent.type = Vst::Event::kNoteOnEvent;
				oEvent.noteOn.channel = i + 1;
				oEvent.noteOn.pitch = note_number;
				oEvent.noteOn.velocity = velocity;
				data.outputEvents->addEvent (oEvent);
			}
		} else {
			if (MIDI_stat [i].noteOn) {
				// update MIDI_stat
				MIDI_stat [i].noteOn = false;
				MIDI_stat [i].noteOnTime = noteOnTime;
				// send note off
				oEvent.type = Vst::Event::kNoteOffEvent;
				oEvent.noteOff.channel = i + 1;
				oEvent.noteOff.pitch = MIDI_stat [i].note_number;
				oEvent.noteOff.velocity = 0;
				data.outputEvents->addEvent (oEvent);
			}
		}
	}
}

void VMMIDI:: reset ()
{
	for (int i = 0; i < max_FB; i++) {
		MIDI_stat [i].noteOn = false;
		MIDI_stat [i].noteOnTime = noteOnTime;
	}
}

} // namespace suzumushi
