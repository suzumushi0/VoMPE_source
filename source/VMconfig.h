//
// Copyright (c) 2024 suzumushi
//
// 2024-8-18		VMconfig.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

namespace suzumushi {

constexpr float internal_SR {16'000.0};					// internal processing sampling rate 
constexpr int frame_len {20};							// frame length [ms]
constexpr int PE {9};									// power exponent of window and FFT length  
constexpr int FFT_len {1 << PE};						// window and FFT length
constexpr int order {18};								// order of LPC
constexpr int max_FB {8};								// maximum number of formant band

}

