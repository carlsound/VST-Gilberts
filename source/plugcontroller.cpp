

#include "../include/plugcontroller.h"
#include "../include/plugids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"

namespace Carlsound {
namespace Gilberts {

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugController::initialize (FUnknown* context)
{
    Steinberg::tresult result = EditController::initialize (context);
	if (result == Steinberg::kResultTrue)
	{
		//---Create Parameters------------
		parameters.addParameter (STR16 ("Bypass"), 0, 1, 0,
                                 Steinberg::Vst::ParameterInfo::kCanAutomate | Steinberg::Vst::ParameterInfo::kIsBypass,
		                         GilbertsParams::kBypassId, 0, STR16("Bypass"));

		parameters.addParameter (STR16 ("Speed"), STR16 ("sec"), 0, 1,
                                 Steinberg::Vst::ParameterInfo::kCanAutomate, GilbertsParams::kParamSpeedId, 0,
		                         STR16 ("Speed"));
	}
    return Steinberg::kResultTrue;
}

//------------------------------------------------------------------------
    Steinberg::tresult PLUGIN_API PlugController::setComponentState (Steinberg::IBStream* state)
{
	// we receive the current state of the component (processor part)
	// we read our parameters and bypass value...
	if (!state)
		return Steinberg::kResultFalse;

	Steinberg::IBStreamer streamer (state, kLittleEndian);

	float savedParam1 = 0.f;
	if (streamer.readFloat (savedParam1) == false)
        return Steinberg::kResultFalse;
	setParamNormalized (GilbertsParams::kParamSpeedId, savedParam1);

	// read the bypass
	Steinberg::int32 bypassState;
	if (streamer.readInt32 (bypassState) == false)
        return Steinberg::kResultFalse;
	setParamNormalized (kBypassId, bypassState ? 1 : 0);

    return Steinberg::kResultOk;
}

//------------------------------------------------------------------------
} // namespace Gilberts
} // namespace Carlsound
