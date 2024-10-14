//
// Copyright (c) 2024 suzumushi
//
// 2024-10-4		VMFFT.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include <cmath>
#include <complex>
#include <numbers>
using std::numbers::pi;

namespace suzumushi {

// Fast Fourier Transformer

template <typename TYPE, int PE, int N = (1 << PE)>
class VMFFT {
public:
	TYPE window (const int n);
	void FFT (const TYPE (&f) [N], std::complex <TYPE> (&F) [N]);
	void FFT (const std::complex <TYPE> (&f) [N], std::complex <TYPE> (&F) [N]);
	void IFFT (const TYPE (&F) [N], std::complex <TYPE> (&f) [N]);
	void IFFT (const std::complex <TYPE> (&F) [N], std::complex <TYPE> (&f) [N]);
	void cs_to_as (const std::complex <TYPE> (&F) [N], TYPE (&a) [N / 2]);
	void cs_to_dBas (const std::complex <TYPE> (&F) [N], TYPE (&a) [N / 2], TYPE lower_limit = 1e-3);
	constexpr VMFFT () {
		for (int i = 0; i < N / 2; i++) 
			hamming_TBL [i] = (TYPE)(0.54 - 0.46 * cos (2.0 * pi * i / (N - 1)));
		for (int i = 0; i <= N / 4; i++)
			sin_TBL [i] = (TYPE)sin (2.0 * pi * i / N);
		for (int i = 0; i < N; i++) {
			int k = i;
			rev_TBL [i] = 0;			// re-initialize
			for (int j = 0; j < PE; j++) {
				rev_TBL [i] <<= 1;		// Left shift
				if (k & 1)				// Test LSB 
					rev_TBL [i] |= 1;	// Set LSB
				k >>= 1;				// Right shift
			}
		}
	};
private:
	static inline TYPE hamming_TBL [N / 2];
	static inline TYPE sin_TBL [N / 4 + 1];
	static inline int rev_TBL [N];
	std::complex <TYPE> WN (const int m);
};

template <typename TYPE, int PE, int N>
TYPE VMFFT<TYPE, PE, N>:: window (const int n)
{
	if (n < N / 2)
		return hamming_TBL [n];
	else 
		return hamming_TBL [N - 1 - n]; 
}

template <typename TYPE, int PE, int N>
std::complex <TYPE> VMFFT<TYPE, PE, N>:: WN (const int m)
{
	if (m >= 0)
		if (m < N / 4)
			return (std::complex <TYPE> (sin_TBL [N / 4 - m], -sin_TBL [m]));
		else
			return (std::complex <TYPE> (-sin_TBL [m - N / 4], -sin_TBL [N / 2 - m]));
	else
		if (-m < N / 4)
			return (std::complex <TYPE> (sin_TBL [N / 4 + m], sin_TBL [-m]));
		else
			return (std::complex <TYPE> (-sin_TBL [-m - N / 4], sin_TBL [N / 2 + m]));
}

template <typename TYPE, int PE, int N>
void VMFFT<TYPE, PE, N>:: FFT (const TYPE (&f) [N], std::complex <TYPE> (&F) [N])
{
	int num_DFT = N / 2;			// number of DFTs
	int num_but = 1;				// number of butterflies in a DFT

	int m = 0;
	int n = num_but;
	for (int j = 0; j < num_DFT; j++) {
		TYPE in_m = f [rev_TBL [m]];
		TYPE in_n = f [rev_TBL [n]];
		F [m] = in_m + in_n;
		F [n] = in_m - in_n;
		m += 2;
		n += 2;
	}

	for (int i = 1; i < PE; i++) {
		num_DFT /= 2;
		num_but *= 2;
		for (int j = 0; j < num_DFT; j++) {
			m = j * 2 * num_but;
			n = m + num_but;
			// optimization for WN [0]
			std::complex <TYPE> tmp = F [n];
			F [n++] = F [m] - tmp;
			F [m++] += tmp;
			int o = m + 2 * num_but - 2;
			int p = n - 2;
			for (int k = num_DFT; k < N / 4; k += num_DFT) {
				tmp = F [n] * WN (k);
				F [n] = F [m] - tmp;
				F [m] += tmp;
				F [o--] = std::conj (F [m++]);
				F [p--] = std::conj (F [n++]);
			}
			// optimization for WN [N/4]
			tmp = std::complex <TYPE> (std::imag (F [n]), - std::real (F [n]));
			F [n] = F [m] - tmp;
			F [m] += tmp;
		}
	}
}

template <typename TYPE, int PE, int N>
void VMFFT<TYPE, PE, N>:: FFT (const std::complex <TYPE> (&f) [N], std::complex <TYPE> (&F) [N])
{
	int num_DFT = N / 2;			// number of DFTs
	int num_but = 1;				// number of butterflies in a DFT

	int m = 0;
	int n = num_but;
	for (int j = 0; j < num_DFT; j++) {
		std::complex <TYPE> in_m = f [rev_TBL [m]];
		std::complex <TYPE> in_n = f [rev_TBL [n]];
		F [m] = in_m + in_n;
		F [n] = in_m - in_n;
		m += 2;
		n += 2;
	}

	for (int i = 1; i < PE; i++) {
		num_DFT /= 2;
		num_but *= 2;
		for (int j = 0; j < num_DFT; j++) {
			m = j * 2 * num_but;
			n = m + num_but;
			// optimization for WN [0]
			std::complex <TYPE> tmp = F [n];
			F [n++] = F [m] - tmp;
			F [m++] += tmp;
			for (int k = num_DFT; k < N / 2; k += num_DFT) {
				if (k == N / 4)		// optimization for WN [N/4]
					tmp = std::complex <TYPE> (std::imag (F [n]), - std::real (F [n]));
				else
					tmp = F [n] * WN (k);
				F [n++] = F [m] - tmp;
				F [m++] += tmp;
			}
		}
	}
}

template <typename TYPE, int PE, int N>
void VMFFT<TYPE, PE, N>:: IFFT (const TYPE (&F) [N], std::complex <TYPE> (&f) [N])
{
	int num_DFT = N / 2;			// number of DFTs
	int num_but = 1;				// number of butterflies in a DFT

	int m = 0;
	int n = num_but;
	for (int j = 0; j < num_DFT; j++) {
		TYPE in_m = F [rev_TBL [m]];
		TYPE in_n = F [rev_TBL [n]];
		f [m] = in_m + in_n;
		f [n] = in_m - in_n;
		m += 2;
		n += 2;
	}

	for (int i = 1; i < PE; i++) {
		num_DFT /= 2;
		num_but *= 2;
		for (int j = 0; j < num_DFT; j++) {
			m = j * 2 * num_but;
			n = m + num_but;
			// optimization for WN [0]
			std::complex <TYPE> tmp = f [n];
			f [n++] = f [m] - tmp;
			f [m++] += tmp;
			int o = m + 2 * num_but - 2;
			int p = n - 2;
			for (int k = num_DFT; k < N / 4; k += num_DFT) {
				tmp = f [n] * WN (-k);
				f [n] = f [m] - tmp;
				f [m] += tmp;
				f [o--] = std::conj (f [m++]);
				f [p--] = std::conj (f [n++]);
			}
			// optimization for WN [N/4]
			tmp = std::complex <TYPE> (- std::imag (f [n]), std::real (f [n]));
			f [n] = f [m] - tmp;
			f [m] += tmp;
		}
	}
	for (int i = 0; i < N; i++)
		f [i] /= N;
}

template <typename TYPE, int PE, int N>
void VMFFT<TYPE, PE, N>:: IFFT (const std::complex <TYPE> (&F) [N], std::complex <TYPE> (&f) [N])
{
	int num_DFT = N / 2;			// number of DFTs
	int num_but = 1;				// number of butterflies in a DFT

	int m = 0;
	int n = num_but;
	for (int j = 0; j < num_DFT; j++) {
		std::complex <TYPE> in_m = F [rev_TBL [m]];
		std::complex <TYPE> in_n = F [rev_TBL [n]];
		f [m] = in_m + in_n;
		f [n] = in_m - in_n;
		m += 2;
		n += 2;
	}

	for (int i = 1; i < PE; i++) {
		num_DFT /= 2;
		num_but *= 2;
		for (int j = 0; j < num_DFT; j++) {
			m = j * 2 * num_but;
			n = m + num_but;
			// optimization for WN [0]
			std::complex <TYPE> tmp = f [n];
			f [n++] = f [m] - tmp;
			f [m++] += tmp;
			for (int k = num_DFT; k < N / 2; k += num_DFT) {
				if (k == N / 4)		// optimization for WN [-N/4]
					tmp = std::complex <TYPE> (- std::imag (f [n]), std::real (f [n]));
				else
					tmp = f [n] * WN (-k);
				f [n++] = f [m] - tmp;
				f [m++] += tmp;
			}
		}
	}
	for (int i = 0; i < N; i++)
		f [i] /= N;
}

// complex spectrum to amplitude spectrum
template <typename TYPE, int PE, int N>
void VMFFT<TYPE, PE, N>:: cs_to_as (const std::complex <TYPE> (&F) [N], TYPE (&a) [N / 2]) 
{
	a [0] = std::real (F [0]);
	for (int i = 1; i < N / 2; i++)
		a [i] = std::abs (F [i]);
}

// complex spectrum to amplitude spectrum (dB), lower_limit is ratio.
template <typename TYPE, int PE, int N>
void VMFFT<TYPE, PE, N>:: cs_to_dBas (const std::complex <TYPE> (&F) [N], TYPE (&a) [N / 2], TYPE lower_limit) 
{
	a [0] = (TYPE)20.0 * log10 (std::max (lower_limit, std::real (F [0])));
	for (int i = 1; i < N / 2; i++)
		a [i] = (TYPE)10.0 * log10 (std::max (lower_limit * lower_limit, std::norm (F [i])));
}

} // namespace suzumushi