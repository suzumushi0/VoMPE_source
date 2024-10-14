//
// Copyright (c) 2024 suzumushi
//
// 2024-8-14		VMLPC.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

namespace suzumushi {

//
// Autocorrelation 
//
// N		signal length
// M		autocorrelation length
//
template <typename TYPE, int N, int M>
void autocorrelation (const TYPE (&signal) [N], TYPE (&ac) [M]) 
{
	for (int i = 0; i < M; i++) {
		ac [i] = 0.0;
		TYPE tmp [N];
		for (int j = 0; j < N - i; j++)
			tmp [j] = signal [j] * signal [i + j];		// SIMD
		for (int j = 0; j < N - i; j++)
			ac [i] += tmp [j];
		// ac [0] is the variance of the signal. If it is 0, (i.e. all signal values are 0), 
		// the autocorrelation is undefined. To handle this exception, change ac [0] as follows.
		if (ac [0] == 0.0)
			ac [0] = 1.0;
	}
}

//
// Levinson Durbin algorithm based Linear Prediction Coefficients (LPC)
//
// M		autocorrelation and lpc length (= LPC model order + 1)
// 
// ac		autocorrelation sequence
// lpc, a	linear prediction coefficients
// ref		reflection (partial auto-correlation: PARCOR) coefficients 
// error	prediction error
//
template <typename TYPE, int M>
void ac_to_LPC (const TYPE (&ac) [M], TYPE (&lpc) [M])
{
	TYPE a [M], tmp [M];
	TYPE error = ac [0];
	for (int i = 1; i < M; i++) {
		TYPE ref = ac [i];
		for (int j = 1; j < i; j++)
			tmp [j] =  a [j] * ac [i - j];
		for (int j = 1; j < i; j++)
			ref += tmp [j];								// SIMD
		ref /= error;
		for (int j = 1; j < i; j++)
			tmp [j] = ref * a [i - j];					// SIMD
		if (i != M - 1) {
			for (int j = 1; j < i; j++)
				a [j] -= tmp [j];						// SIMD
			a [i] = -ref;
			error *= (1.0 - ref * ref);
		} else {
			lpc [0] = 1.0;
			for (int j = 1; j < i; j++)
				lpc [j] = a [j] - tmp [j];				// SIMD
			lpc [i] = -ref;
		}
	}
}

} // namespace suzumushi
