///------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include <memory>
#include <atlconv.h>
#include <codecvt>
#include <tchar.h>

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

	Steinberg::Vst::ParamValue PLUGIN_API normalizedParamToPlain(Steinberg::Vst::ParamID tag, Steinberg::Vst::ParamValue valueNormalized) SMTG_OVERRIDE;
	Steinberg::Vst::ParamValue PLUGIN_API plainParamToNormalized(Steinberg::Vst::ParamID tag, Steinberg::Vst::ParamValue value) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getParamStringByValue(Steinberg::Vst::ParamID tag, Steinberg::Vst::ParamValue valueNormalized, Steinberg::Vst::String128 string) SMTG_OVERRIDE;

	//std::shared_ptr<Steinberg::Vst::RangeParameter> m_speedRangeParameter;
};

//------------------------------------------------------------------------
} // namespace Gilberts
} // namespace Carlsound
