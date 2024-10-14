//
// Copyright (c) 2024 suzumushi
//
// 2024-8-17		VM1_3downsampler.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "SODDL.h"
#include <numbers>
using std::numbers::pi;


namespace suzumushi {

// FIR-based one-third downsampler 

template <typename TYPE, 
	int IR_LEN = 67,							// logical length of impulse response. IR_LEN must be an odd number.
	int IR_CENTER = (IR_LEN - 1) / 2>			// center of impulse response (don't touch this)
class VM1_3downsampler {
public:
	TYPE process (const SODDL <float, ODL_len> &ODL);
	constexpr VM1_3downsampler ()
	{
		// sinc function 
		IR_TBL [IR_CENTER] = 1.0 / 3.0;
		TYPE omega_cT = IR_TBL [IR_CENTER] * pi;
		for (int i = 0, j = -IR_CENTER; i < IR_CENTER; i++, j++)
			IR_TBL [i] = sin (j * omega_cT) / (j * pi);
		// hamming window
		for (int i = 0; i <= IR_CENTER; i++)
			IR_TBL [i]  *= 0.54 - 0.46 * cos (pi * i / IR_CENTER);
		// normalization
		TYPE sum = 0.0;
		for (int i = 0; i < IR_CENTER; i++)
			sum += IR_TBL [i];
		sum = sum * 2.0 + IR_TBL [IR_CENTER];
		for (int i = 0; i <= IR_CENTER; i++)
			IR_TBL [i] /= sum;
	};
private:
	static inline TYPE IR_TBL [IR_CENTER + 1];				// impulse response table
};

template <typename TYPE, int IR_LEN, int IR_CENTER>
TYPE VM1_3downsampler <TYPE, IR_LEN, IR_CENTER>:: process (const SODDL <float, ODL_len> &ODL)
{
	TYPE sum [IR_CENTER];
	for (int i = 0, j = ODL_len - IR_LEN, k = ODL_len - 1; i < IR_CENTER; i++, j++, k--)
		sum [i] = ODL.read (j) + ODL.read (k);
	TYPE yn = 0.0;
	for (int i = 0; i < IR_CENTER; i++)
		yn += IR_TBL [i] * sum [i];							// SIMD
	return (yn + IR_TBL [IR_CENTER] * ODL.read (ODL_len - IR_LEN + IR_CENTER));
}

} // namespace suzumushi

