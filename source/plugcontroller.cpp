

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
		parameters.addParameter (STR16 ("Bypass"), // title
			                     0, // units
			                     1, // stepCount
			                     0, // defaultValueNormalized
                                 Steinberg::Vst::ParameterInfo::kCanAutomate | Steinberg::Vst::ParameterInfo::kIsBypass, // flags
		                         GilbertsParams::kBypassId, // tag
			                     0, // unitUD
			                     STR16("Bypass")); // shortTitle

		/*
		parameters.addParameter (STR16 ("Speed"), // title
		                         STR16 ("sec"), // units
			                     0, // stepCount
			                     1, // defaultValueNormalized
                                 Steinberg::Vst::ParameterInfo::kCanAutomate, GilbertsParams::kParamSpeedId, // flags
			                     0, // unitID
		                         STR16 ("Speed")); // shortTitle
		*/

		m_speedParameter = std::make_shared<Steinberg::Vst::RangeParameter> (STR16("Speed"), // title
			                                                                 GilbertsParams::kParamSpeedId, // ParamID
			                                                                 STR16("sec"), // units
			                                                                 0.1, // minPlain
			                                                                 10.0, // maxPlain
			                                                                 1.0, // defaultValuePlain
			                                                                 0, // stepCount
			                                                                 Steinberg::Vst::ParameterInfo::kCanAutomate, // flags
			                                                                 0, // unitID
			                                                                 STR16("Speed")); // shortTitle
		parameters.addParameter(m_speedParameter->getInfo());
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
