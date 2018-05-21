///------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace Carlsound {
namespace Gilberts {

//-----------------------------------------------------------------------------
class PlugController : public Steinberg::Vst::EditController
{
public:
//------------------------------------------------------------------------
	// create function required for Plug-in factory,
	// it will be called to create new instances of this controller
//------------------------------------------------------------------------
	static FUnknown* createInstance (void*)
	{
		return (Steinberg::Vst::IEditController*) new PlugController ();
	}

	//---from IPluginBase--------
	Steinberg::tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;

	//---from EditController-----
	Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
};

//------------------------------------------------------------------------
} // namespace Gilberts
} // namespace Carlsound
