//
// Copyright (c) 2024 suzumushi
//
// 2024-10-14		VMcids.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace suzumushi {
//------------------------------------------------------------------------
static const Steinberg::FUID kVoMPEProcessorUID (0xFFAE99B6, 0xC1EF50FD, 0x8A922F1B, 0x437EC7AE);
static const Steinberg::FUID kVoMPEControllerUID (0x039A41F6, 0x08365967, 0x875F47B0, 0xF2B0AA8C);

#define VoMPEVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace suzumushi
