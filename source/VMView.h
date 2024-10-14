//
// Copyright (c) 2024 suzumushi
//
// 2024-10-4		VMView.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "vstgui4/vstgui/lib/controls/cparamdisplay.h"
#include "vstgui4/vstgui/lib/cdrawcontext.h"
#include "VMcontroller.h"
#include "VMvst3editor.h"
#include "VMparam.h"


namespace VSTGUI {

class VMFormant: public CParamDisplay
{
public:
	VMFormant (const CRect& size, CBitmap* background = nullptr, int32_t style = 0): CParamDisplay (size, background, style) {};
	VMFormant (const VMFormant& paramDisplay): CParamDisplay (paramDisplay) {};

	void draw (CDrawContext* context)
	{
		drawBack (context);

		context -> setDrawMode (kAntiAliasing);
		CPoint viewPos = getViewSize ().getTopLeft ();
		CDrawContext:: Transform t (*context, CGraphicsTransform ().translate (viewPos));

		if (! listener)
			return;
		Steinberg::Vst::SpectrumParameter* sp = static_cast <VMVST3Editor*> (listener) -> SpectrumParameter;
		double y [suzumushi::FFT_len / 2];
		for (int i = 1; i < suzumushi::FFT_len / 2; i++)
			y [i] = (60.0 - sp -> spectrum.spectral_envelope [i]) * 3.0; 

		context -> setFrameColor (getFrameColor ());
		context -> setLineStyle (kLineSolid);
		context -> setLineWidth (getFrameWidth ());

		for (int i = 1, j = 2; j < suzumushi::FFT_len / 2; i++, j++)
			context -> drawLine (CPoint (i, y [i]), CPoint (j, y [j]));

		for (int i = 0; i < suzumushi::max_FB; i++) {
			int x = sp -> spectrum.peak_freq [i];
			if (x != 0) {
				CRect r (x, y [x], x, y [x]);
				r.extend (getRoundRectRadius () / 2.0, getRoundRectRadius () / 2.0);
				context -> setFillColor (getShadowColor ());
				context -> drawEllipse (r, kDrawFilled);
			}
		}
		setDirty (false);
	}
	CLASS_METHODS (VMFormant, CParamDisplay)
};


template <CCoord VMFx = 256.0, CCoord VMFy = 240.0, CCoord Ox = 20.0, CCoord Oy = 20.0> // VMFormant view size and position
class VMFormantRuler: public CParamDisplay
{
public:
	VMFormantRuler (const CRect& size, CBitmap* background = nullptr, int32_t style = 0): CParamDisplay (size, background, style) {};
	VMFormantRuler (const VMFormantRuler& paramDisplay): CParamDisplay (paramDisplay) {};

	void draw (CDrawContext* context)
	{
		drawBack (context);

		context -> setDrawMode (kAntiAliasing);
		CPoint viewPos = getViewSize ().getTopLeft ();
		CDrawContext:: Transform t (*context, CGraphicsTransform ().translate (viewPos));

		if (! listener)
			return;
		Steinberg::Vst::RulerParameter* rp = static_cast <VMVST3Editor*> (listener) -> RulerParameter;
		int fo_bw = rp -> ruler.fo_bw;
		int fo_bw_val = suzumushi::fo_bw_val [fo_bw];
		if (rp -> ruler.SR == 44'100.0f)
			fo_bw_val = suzumushi::fo_bw_441_val [rp -> ruler.fo_bw];
		int num_fo_val = suzumushi::num_fo_val [rp -> ruler.num_fo];

		context -> setFrameColor (getFrameColor ());
		constexpr CCoord dash [] = {2, 2};
		context -> setLineStyle (CLineStyle (CLineStyle::kLineCapButt, CLineStyle::kLineJoinMiter, 0, 2, dash));
		context -> setLineWidth (getFrameWidth ());

		// y-axis ruler drawing
		for (int i = VMFy / 4, j = i; j < VMFy; j += i)
			context -> drawLine (CPoint (Ox, j + Oy), CPoint (VMFx + Ox, j + Oy));

		constexpr CCoord xpos = 0;
		constexpr CCoord yoffset = 13;
		constexpr CPoint size (18, 10);
		context -> setFont (getFont ());
		context -> setFontColor (getFontColor ());
		context -> drawString ("dB", CRect (CPoint (xpos, 0), size), kRightText);
		context -> drawString ("60", CRect (CPoint (xpos, yoffset), size), kRightText);
		context -> drawString ("40", CRect (CPoint (xpos, 60 + yoffset), size), kRightText);
		context -> drawString ("20", CRect (CPoint (xpos, 120 + yoffset), size), kRightText);
		context -> drawString ("0",	 CRect (CPoint (xpos, 180 + yoffset), size), kRightText);
		context -> drawString ("-20", CRect (CPoint (xpos, 240 + 9), size), kRightText);

		// x-axis ruler drawing
		for (int i = 1; i <= num_fo_val; i++) {
			CCoord xpos = fo_bw_val * i;
			if (xpos > VMFx)
				break;
			if (i == num_fo_val)
				context -> setLineStyle (kLineSolid);
			context -> drawLine (CPoint (xpos + Ox, Oy), CPoint (xpos + Ox, VMFy + Oy));
		}

		constexpr CCoord xoffset = 11;
		constexpr CCoord ypos = 260; 
		CCoord width = 0;
		context -> drawString ("0", CRect (CPoint (xoffset, ypos), size), kCenterText);
		switch (fo_bw) {
			case (int32)suzumushi::FO_BW_L::B500:
			case (int32)suzumushi::FO_BW_L::B1000:
				if (rp -> ruler.SR == 44'100.0f)
					width = suzumushi::fo_bw_441_val [(int32)suzumushi::FO_BW_L::B1000];
				else			// 16KHz or 48KHz
					width = suzumushi::fo_bw_val [(int32)suzumushi::FO_BW_L::B1000];
				context -> drawString ("1", CRect (CPoint (width + xoffset, ypos), size), kCenterText);
				context -> drawString ("2", CRect (CPoint (width * 2 + xoffset, ypos), size), kCenterText);
				context -> drawString ("3", CRect (CPoint (width * 3 + xoffset, ypos), size), kCenterText);
				context -> drawString ("4", CRect (CPoint (width * 4 + xoffset, ypos), size), kCenterText);
				context -> drawString ("5", CRect (CPoint (width * 5 + xoffset, ypos), size), kCenterText);
				context -> drawString ("6", CRect (CPoint (width * 6 + xoffset, ypos), size), kCenterText);
				context -> drawString ("7", CRect (CPoint (width * 7 + xoffset, ypos), size), kCenterText);
				if (rp -> ruler.SR == 44'100.0f) 
					context -> drawString ("KHz", CRect (CPoint (width * 7 + 27, ypos), size), kLeftText);
				else			// 16KHz or 48KHz
					context -> drawString ("8 KHz", CRect (CPoint (width * 8 + xoffset, ypos), CPoint (25, 10)), kLeftText);
				break;
			case (int32)suzumushi::FO_BW_L::B750:
			case (int32)suzumushi::FO_BW_L::B1500:
				if (rp -> ruler.SR == 44'100.0f)
					width = suzumushi::fo_bw_441_val [(int32)suzumushi::FO_BW_L::B1500];
				else			// 16KHz or 48KHz
					width = suzumushi::fo_bw_val [(int32)suzumushi::FO_BW_L::B1500];
				context -> drawString ("1.5", CRect (CPoint (width + xoffset, ypos), size), kCenterText);
				context -> drawString ("3",   CRect (CPoint (width * 2 + xoffset, ypos), size), kCenterText);
				context -> drawString ("4.5", CRect (CPoint (width * 3 + xoffset, ypos), size), kCenterText);
				context -> drawString ("6",   CRect (CPoint (width * 4 + xoffset, ypos), size), kCenterText);
				if (rp -> ruler.SR == 44'100.0f) 
					context -> drawString ("7.4 KHz", CRect (CPoint (width * 5, ypos), size), kLeftText);
				else			// 16KHz or 48KHz
					context -> drawString ("7.5 KHz", CRect (CPoint (width * 5 + 4, ypos), size), kLeftText);
				break;
			case (int32)suzumushi::FO_BW_L::B1250: 
				if (rp -> ruler.SR == 44'100.0f)
					width = suzumushi::fo_bw_441_val [(int32)suzumushi::FO_BW_L::B1250];
				else			// 16KHz or 48KHz
					width = suzumushi::fo_bw_val [(int32)suzumushi::FO_BW_L::B1250];
				context -> drawString ("2.5", CRect (CPoint (width * 2 + xoffset, ypos), size), kCenterText);
				context -> drawString ("5",   CRect (CPoint (width * 4 + xoffset, ypos), size), kCenterText);
				if (rp -> ruler.SR == 44'100.0f) 
					context -> drawString ("7.4 KHz", CRect (CPoint (width * 6, ypos), size), kLeftText);
				else			// 16KHz or 48KHz
					context -> drawString ("7.5 KHz", CRect (CPoint (width * 6 + 4, ypos), size), kLeftText);
				break;
		}

		// threshold ruler drawing
		context -> setFrameColor (getShadowColor ());
		context -> setLineStyle (kLineSolid);
		CCoord thresh_y = (60.0 - rp -> ruler.thresh) * 3.0; 
		context -> drawLine (CPoint (Ox, thresh_y + Oy), CPoint (VMFx + Ox, thresh_y + Oy));

		setDirty (false);
	}
	CLASS_METHODS (VMFormantRuler, CParamDisplay)
};

template <float L = 220.0f/50.0f, int X = 50, int Y = 240>	// map line, columns and rows length
class VMSpectralEnvelope: public CParamDisplay
{
public:
	VMSpectralEnvelope (const CRect& size, CBitmap* background = nullptr, int32_t style = 0): CParamDisplay (size, background, style) {};
	VMSpectralEnvelope (const VMSpectralEnvelope& paramDisplay): CParamDisplay (paramDisplay) {};

	void draw (CDrawContext* context)
	{
		drawBack (context);

		context -> setDrawMode (kAntiAliasing);
		CPoint viewPos = getViewSize ().getTopLeft ();
		CDrawContext:: Transform t (*context, CGraphicsTransform ().translate (viewPos));

		if (! listener)
			return;
		Steinberg::Vst::SpectrumParameter* sp = static_cast <VMVST3Editor*> (listener) -> SpectrumParameter;
		for (int i = 1; i <= Y; i++)
			spectral [Y - i] [head] = color_map [std::max (0, (int)((sp -> spectrum.spectral_envelope [i] + 20.0) / 80.0 * 255.0 + 0.5))];
		for (int i = 0; i < suzumushi::max_FB; i++) {
			int pf = sp -> spectrum.peak_freq [i];
			if (pf != 0 && pf <= Y)
				spectral [Y - pf] [head] = getFontColor ();
		}
		if (++head == X)
			head = 0;

		context -> setLineStyle (kLineSolid);
		for (int i = 0, j = head; i < X; i++, j++) { 
			if (j == X)
				j = 0;
			for (int k = 0; k < Y; k++) {
				context -> setFrameColor (spectral [k] [j]);
				context -> drawLine (CPoint (i * L, k), CPoint ((i + 1) * L, k));
			}
		}
		setDirty (false);
	}
private:
	CColor spectral [Y] [X] {};
	int head {0};
	static const inline CColor color_map [] {
		CColor (35, 23, 27), CColor (39, 26, 40), CColor (43, 28, 52), CColor (47, 30, 63), CColor (51, 32, 74), CColor (54, 35, 85), CColor (57, 37, 95), CColor (59, 40, 105),
		CColor (62, 42, 114), CColor (64, 44, 123), CColor (66, 47, 132), CColor (68, 49, 140), CColor (69, 52, 148), CColor (71, 55, 155), CColor (72, 57, 162), CColor (73, 60, 169),
		CColor (73, 62, 175), CColor (74, 65, 181), CColor (74, 68, 187), CColor (75, 70, 193), CColor (75, 73, 198), CColor (75, 76, 203), CColor (75, 79, 207), CColor (74, 81, 211),
		CColor (74, 84, 215), CColor (74, 87, 219), CColor (73, 89, 223), CColor (73, 92, 226), CColor (72, 95, 229), CColor (71, 98, 232), CColor (70, 101, 234), CColor (69, 103, 237),
		CColor (68, 106, 239), CColor (67, 109, 240), CColor (66, 112, 242), CColor (65, 114, 244), CColor (64, 117, 245), CColor (63, 120, 246), CColor (62, 123, 247), CColor (61, 125, 248),
		CColor (59, 128, 248), CColor (58, 131, 249), CColor (57, 134, 249), CColor (56, 136, 249), CColor (55, 139, 249), CColor (53, 142, 249), CColor (52, 144, 248), CColor (51, 147, 248),
		CColor (50, 150, 247), CColor (49, 152, 246), CColor (48, 155, 246), CColor (47, 158, 245), CColor (46, 160, 244), CColor (45, 163, 242), CColor (44, 165, 241), CColor (43, 168, 240),
		CColor (42, 170, 238), CColor (42, 173, 237), CColor (41, 175, 235), CColor (40, 178, 234), CColor (40, 180, 232), CColor (39, 182, 230), CColor (39, 185, 228), CColor (38, 187, 226),
		CColor (38, 189, 224), CColor (37, 192, 222), CColor (37, 194, 220), CColor (37, 196, 218), CColor (37, 198, 215), CColor (37, 200, 213), CColor (37, 202, 211), CColor (37, 205, 209),
		CColor (37, 207, 206), CColor (38, 209, 204), CColor (38, 210, 201), CColor (38, 212, 199), CColor (39, 214, 196), CColor (39, 216, 194), CColor (40, 218, 191), CColor (41, 220, 189),
		CColor (42, 221, 186), CColor (43, 223, 184), CColor (44, 225, 181), CColor (45, 226, 178), CColor (46, 228, 176), CColor (47, 229, 173), CColor (48, 231, 171), CColor (49, 232, 168),
		CColor (51, 234, 166), CColor (52, 235, 163), CColor (54, 236, 160), CColor (55, 238, 158), CColor (57, 239, 155), CColor (59, 240, 153), CColor (61, 241, 150), CColor (63, 242, 148),
		CColor (65, 243, 145), CColor (67, 244, 143), CColor (69, 245, 140), CColor (71, 246, 138), CColor (73, 247, 135), CColor (75, 248, 133), CColor (78, 249, 131), CColor (80, 249, 128),
		CColor (82, 250, 126), CColor (85, 250, 124), CColor (87, 251, 121), CColor (90, 251, 119), CColor (93, 252, 117), CColor (95, 252, 115), CColor (98, 253, 113), CColor (101, 253, 110),
		CColor (104, 253, 108), CColor (106, 253, 106), CColor (109, 254, 104), CColor (112, 254, 102), CColor (115, 254, 100), CColor (118, 254, 98), CColor (121, 254, 96), CColor (124, 253, 94),
		CColor (127, 253, 93), CColor (130, 253, 91), CColor (133, 253, 89), CColor (136, 252, 87), CColor (139, 252, 86), CColor (142, 252, 84), CColor (145, 251, 82), CColor (149, 251, 81),
		CColor (152, 250, 79), CColor (155, 249, 78), CColor (158, 249, 76), CColor (161, 248, 75), CColor (164, 247, 73), CColor (167, 246, 72), CColor (170, 246, 70), CColor (173, 245, 69),
		CColor (176, 244, 68), CColor (179, 243, 66), CColor (182, 242, 65), CColor (185, 240, 64), CColor (188, 239, 63), CColor (191, 238, 62), CColor (194, 237, 60), CColor (197, 235, 59),
		CColor (200, 234, 58), CColor (203, 233, 57), CColor (205, 231, 56), CColor (208, 230, 55), CColor (211, 228, 54), CColor (213, 227, 53), CColor (216, 225, 52), CColor (219, 223, 52),
		CColor (221, 222, 51), CColor (223, 220, 50), CColor (226, 218, 49), CColor (228, 216, 48), CColor (230, 214, 48), CColor (233, 212, 47), CColor (235, 210, 46), CColor (237, 208, 45),
		CColor (239, 206, 45), CColor (241, 204, 44), CColor (243, 202, 43), CColor (244, 200, 43), CColor (246, 198, 42), CColor (248, 196, 42), CColor (249, 193, 41), CColor (251, 191, 40),
		CColor (252, 189, 40), CColor (253, 186, 39), CColor (255, 184, 39), CColor (255, 181, 38), CColor (255, 179, 38), CColor (255, 177, 37), CColor (255, 174, 37), CColor (255, 172, 36),
		CColor (255, 169, 36), CColor (255, 166, 35), CColor (255, 164, 35), CColor (255, 161, 34), CColor (255, 159, 34), CColor (255, 156, 34), CColor (255, 153, 33), CColor (255, 151, 33),
		CColor (255, 148, 32), CColor (255, 145, 32), CColor (255, 142, 31), CColor (255, 140, 31), CColor (255, 137, 30), CColor (255, 134, 30), CColor (255, 131, 30), CColor (255, 129, 29),
		CColor (255, 126, 29), CColor (255, 123, 28), CColor (255, 120, 28), CColor (255, 117, 27), CColor (255, 115, 27), CColor (255, 112, 26), CColor (254, 109, 26), CColor (252, 106, 26),
		CColor (251, 104, 25), CColor (249, 101, 25), CColor (248, 98, 24), CColor (246, 95, 24), CColor (244, 92, 23), CColor (243, 90, 23), CColor (241, 87, 22), CColor (239, 84, 22),
		CColor (237, 82, 21), CColor (235, 79, 20), CColor (233, 76, 20), CColor (230, 74, 19), CColor (228, 71, 19), CColor (226, 69, 18), CColor (224, 66, 18), CColor (221, 64, 17),
		CColor (219, 61, 16), CColor (216, 59, 16), CColor (214, 56, 15), CColor (211, 54, 15), CColor (209, 52, 14), CColor (206, 49, 13), CColor (203, 47, 13), CColor (201, 45, 12),
		CColor (198, 43, 11), CColor (196, 41, 11), CColor (193, 39, 10), CColor (190, 37, 10), CColor (188, 35, 9), CColor (185, 33, 8), CColor (183, 31, 8), CColor (180, 29, 7),
		CColor (177, 28, 6), CColor (175, 26, 6), CColor (172, 24, 5), CColor (170, 23, 4), CColor (168, 22, 4), CColor (165, 20, 3), CColor (163, 19, 2), CColor (161, 18, 2),
		CColor (159, 17, 1), CColor (157, 16, 0), CColor (155, 15, 0), CColor (154, 14, 0), CColor (152, 14, 0), CColor (150, 13, 0), CColor (149, 12, 0), CColor (148, 12, 0),
		CColor (147, 12, 0), CColor (146, 12, 0), CColor (145, 11, 0), CColor (145, 12, 0), CColor (144, 12, 0), CColor (144, 12, 0), CColor (144, 12, 0), CColor (144, 13, 0),
	};
	CLASS_METHODS (VMSpectralEnvelope, CParamDisplay)
};


template <CCoord VMSEx = 220.0, CCoord VMSEy = 240.0, CCoord Ox = 20.0, CCoord Oy = 20.0> // VMSpectralEnvelope view size and position
class VMSpectralEnvelopeRuler: public CParamDisplay
{
public:
	VMSpectralEnvelopeRuler (const CRect& size, CBitmap* background = nullptr, int32_t style = 0): CParamDisplay (size, background, style) {};
	VMSpectralEnvelopeRuler (const VMSpectralEnvelopeRuler& paramDisplay): CParamDisplay (paramDisplay) {};

	void draw (CDrawContext* context)
	{
		drawBack (context);

		context -> setDrawMode (kAntiAliasing);
		CPoint viewPos = getViewSize ().getTopLeft ();
		CDrawContext:: Transform t (*context, CGraphicsTransform ().translate (viewPos));

		if (! listener)
			return;
		Steinberg::Vst::RulerParameter* rp = static_cast <VMVST3Editor*> (listener) -> RulerParameter;
		int fo_bw = rp -> ruler.fo_bw;
		int fo_bw_val = suzumushi::fo_bw_val [fo_bw];
		if (rp -> ruler.SR == 44'100.0f)
			fo_bw_val = suzumushi::fo_bw_441_val [rp -> ruler.fo_bw];
		int num_fo_val = suzumushi::num_fo_val [rp -> ruler.num_fo];

		context -> setFrameColor (getFrameColor ());
		constexpr CCoord dash [] = {2, 2};
		context -> setLineStyle (CLineStyle (CLineStyle::kLineCapButt, CLineStyle::kLineJoinMiter, 0, 2, dash));
		context -> setLineWidth (getFrameWidth ());

		// y-axis ruler drawing
		for (int i = 1; i <= num_fo_val; i++) {
			CCoord ypos = VMSEy - fo_bw_val * i;
			if (ypos < 0)
				break;
			if (i == num_fo_val)
				context -> setLineStyle (kLineSolid);
			context -> drawLine (CPoint (Ox, ypos + Oy), CPoint (VMSEx + Ox, ypos + Oy));
		}

		constexpr CPoint size (18, 10);
		constexpr CCoord xpos = 0;
		constexpr CCoord yoffset = 13;
		CCoord height = 0;
		context -> setFont (getFont ());
		context -> setFontColor (getFontColor ());
		context -> drawString ("KHz", CRect (CPoint (xpos - 2, 0), CPoint (25, 10)), kRightText);
		switch (fo_bw) {
			case (int32)suzumushi::FO_BW_L::B500:
			case (int32)suzumushi::FO_BW_L::B1000:
				if (rp -> ruler.SR == 44'100.0f)
					height = suzumushi::fo_bw_441_val [(int32)suzumushi::FO_BW_L::B1000];
				else			// 16KHz or 48KHz
					height = suzumushi::fo_bw_val [(int32)suzumushi::FO_BW_L::B1000];
				context -> drawString ("7", CRect (CPoint (xpos, VMSEy - height * 7 + yoffset), size), kRightText);
				context -> drawString ("6", CRect (CPoint (xpos, VMSEy - height * 6 + yoffset), size), kRightText);
				context -> drawString ("5", CRect (CPoint (xpos, VMSEy - height * 5 + yoffset), size), kRightText);
				context -> drawString ("4", CRect (CPoint (xpos, VMSEy - height * 4 + yoffset), size), kRightText);
				context -> drawString ("3", CRect (CPoint (xpos, VMSEy - height * 3 + yoffset), size), kRightText);
				context -> drawString ("2", CRect (CPoint (xpos, VMSEy - height * 2 + yoffset), size), kRightText);
				context -> drawString ("1", CRect (CPoint (xpos, VMSEy - height + yoffset), size), kRightText);
				context -> drawString ("0", CRect (CPoint (xpos, VMSEy + yoffset), size), kRightText);
				break;
			case (int32)suzumushi::FO_BW_L::B750:
			case (int32)suzumushi::FO_BW_L::B1500:
				if (rp -> ruler.SR == 44'100.0f)
					height = suzumushi::fo_bw_441_val [(int32)suzumushi::FO_BW_L::B1500];
				else {			// 16KHz or 48KHz
					height = suzumushi::fo_bw_val [(int32)suzumushi::FO_BW_L::B1500];
					context -> drawString ("7.5", CRect (CPoint (xpos, VMSEy - height * 5 + yoffset), size), kRightText);
				}
				context -> drawString ("6",   CRect (CPoint (xpos, VMSEy - height * 4 + yoffset), size), kRightText);
				context -> drawString ("4.5", CRect (CPoint (xpos, VMSEy - height * 3 + yoffset), size), kRightText);
				context -> drawString ("3",   CRect (CPoint (xpos, VMSEy - height * 2 + yoffset), size), kRightText);
				context -> drawString ("1.5", CRect (CPoint (xpos, VMSEy - height + yoffset), size), kRightText);
				context -> drawString ("0",   CRect (CPoint (xpos, VMSEy + yoffset), size), kRightText);
				break;
			case (int32)suzumushi::FO_BW_L::B1250: 
				if (rp -> ruler.SR == 44'100.0f)
					height = suzumushi::fo_bw_441_val [(int32)suzumushi::FO_BW_L::B1250];
				else {			// 16KHz or 48KHz
					height = suzumushi::fo_bw_val [(int32)suzumushi::FO_BW_L::B1250];
					context -> drawString ("7.5", CRect (CPoint (xpos, VMSEy - height * 6 + yoffset), size), kRightText);
				}
				context -> drawString ("5",   CRect (CPoint (xpos, VMSEy - height * 4 + yoffset), size), kRightText);
				context -> drawString ("2.5", CRect (CPoint (xpos, VMSEy - height * 2 + yoffset), size), kRightText);
				context -> drawString ("0",   CRect (CPoint (xpos, VMSEy + yoffset), size), kRightText);
				break;
		}
	}
	CLASS_METHODS (VMSpectralEnvelopeRuler, CParamDisplay)
};


} // namespace VSTGUI
