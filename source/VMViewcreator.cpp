//
// Copyright (c) 2024 suzumushi
//
// 2024-10-3		VMViewcreator.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"
#include "vstgui/uidescription/uiviewfactory.h"
#include "vstgui/uidescription/uiattributes.h"
#include "vstgui/uidescription/iviewcreator.h"

#include "VMView.h"

namespace VSTGUI {
namespace UIViewCreator {

// Creator for VMFormant

struct VMFormantCreator: ViewCreatorAdapter
{
	VMFormantCreator ()
	{
		UIViewFactory:: registerViewCreator (*this);
	}

	IdStringPtr getViewName () const
	{
		return "VMFormant";
	}

	IdStringPtr  getBaseViewName () const
	{
		return kCParamDisplay;
	}

	UTF8StringPtr getDisplayName () const
	{
		return "VMFormant";
	}

	CView* create (const UIAttributes& attrib, const IUIDescription* desc) const
	{
		return new VMFormant (CRect (0, 0, 256, 240));
	}
};

VMFormantCreator __gHVMFormantCreator;


// Creator for VMFormantRuler

struct VMFormantRulerCreator: ViewCreatorAdapter
{
	VMFormantRulerCreator ()
	{
		UIViewFactory:: registerViewCreator (*this);
	}

	IdStringPtr getViewName () const
	{
		return "VMFormantRuler";
	}

	IdStringPtr  getBaseViewName () const
	{
		return kCParamDisplay;
	}

	UTF8StringPtr getDisplayName () const
	{
		return "VMFormantRuler";
	}

	CView* create (const UIAttributes& attrib, const IUIDescription* desc) const
	{
		return new VMFormantRuler (CRect (0, 0, 300, 280));
	}
};

VMFormantRulerCreator __gHVMFormantRulerCreator;


// Creator for VMSpectralEnvelope

struct VMSpectralEnvelopeCreator: ViewCreatorAdapter
{
	VMSpectralEnvelopeCreator ()
	{
		UIViewFactory:: registerViewCreator (*this);
	}

	IdStringPtr getViewName () const
	{
		return "VMSpectralEnvelope";
	}

	IdStringPtr  getBaseViewName () const
	{
		return kCParamDisplay;
	}

	UTF8StringPtr getDisplayName () const
	{
		return "VMSpectralEnvelope";
	}

	CView* create (const UIAttributes& attrib, const IUIDescription* desc) const
	{
		return new VMSpectralEnvelope (CRect (0, 0, 220, 240));
	}
};

VMSpectralEnvelopeCreator __gHVMSpectralEnvelopeCreator;


// Creator for VMSpectralEnvelopeRuler

struct VMSpectralEnvelopeRulerCreator: ViewCreatorAdapter
{
	VMSpectralEnvelopeRulerCreator ()
	{
		UIViewFactory:: registerViewCreator (*this);
	}

	IdStringPtr getViewName () const
	{
		return "VMSpectralEnvelopeRuler";
	}

	IdStringPtr  getBaseViewName () const
	{
		return kCParamDisplay;
	}

	UTF8StringPtr getDisplayName () const
	{
		return "VMSpectralEnvelopeRuler";
	}

	CView* create (const UIAttributes& attrib, const IUIDescription* desc) const
	{
		return new VMSpectralEnvelopeRuler (CRect (0, 0, 240, 280));
	}
};

VMSpectralEnvelopeRulerCreator __gHVMSpectralEnvelopeRulerCreator;


} // namespace UIViewCreator
} // namespace VSTGUI
