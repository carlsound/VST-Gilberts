//------------------------------------------------------------------------
#pragma once

namespace Carlsound {
namespace Gilberts {

// HERE are defined the parameter Ids which are exported to the host
enum GilbertsParams : Steinberg::Vst::ParamID
{
	kBypassId = 100,
	kParamSpeedId = 102
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const Steinberg::FUID MyProcessorUID (0x53596842, 0x6EBD4D12, 0x82148678, 0x1ACDFEC5);
static const Steinberg::FUID MyControllerUID (0x9EF88A7F, 0x8A1B4FC0, 0x8A23C766, 0x93A0A845);

//------------------------------------------------------------------------
} // namespace Gilberts
} // namespace Carlsound
