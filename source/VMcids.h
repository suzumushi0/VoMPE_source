//
// Copyright (c) 2024 suzumushi
//
// 2024-8-8		VMcids.h
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
static const Steinberg::FUID kVoMPEProcessorUID (0x256C2452, 0x6E485DDB, 0x9663D058, 0x5FCD696B);
static const Steinberg::FUID kVoMPEControllerUID (0x039A41F6, 0x08365967, 0x875F47B0, 0xF2B0AA8C);

#define VoMPEVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace suzumushi
