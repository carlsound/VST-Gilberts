//------------------------------------------------------------------------
#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace Carlsound {
namespace Gilberts {

//-----------------------------------------------------------------------------
class PlugProcessor : public Steinberg::Vst::AudioEffect
{
public:
	PlugProcessor ();

	Steinberg::tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setBusArrangements (Steinberg::Vst::SpeakerArrangement* inputs,
													  Steinberg::int32 numIns,
			   										  Steinberg::Vst::SpeakerArrangement* outputs,
													  Steinberg::int32 numOuts) SMTG_OVERRIDE;

	Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;

//------------------------------------------------------------------------
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

	static FUnknown* createInstance (void*)
	{
		return (Steinberg::Vst::IAudioProcessor*)new PlugProcessor ();
	}

protected:
	Steinberg::Vst::ParamValue mParam1 = 0;
	Steinberg::int16 mParam2 = 0;
	bool mBypass = false;
};

//------------------------------------------------------------------------
} // namespace Gilberts
} // namespace Carlsound
