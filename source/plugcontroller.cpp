
#include "../include/plugcontroller.h"
#include "../include/plugids.h"
//
#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include <string>
#include "public.sdk/source/vst/hosting/stringconvert.h"
//
namespace Carlsound
{
	namespace Gilberts 
	{
		//-----------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API PlugController::initialize 
		(
			FUnknown* context
		)
		{
			Steinberg::tresult result = EditController::initialize (context);
			if (result == Steinberg::kResultTrue)
			{
				//---Create Parameters------------
				parameters.addParameter 
				(
					STR16 ("Bypass"), // title
			        0, // units
			        1, // stepCount
			        0, // defaultValueNormalized
                    Steinberg::Vst::ParameterInfo::kCanAutomate | Steinberg::Vst::ParameterInfo::kIsBypass, // flags
		            GilbertsParams::kBypassId, // tag
			        0, // unitUD
			        STR16("Bypass") // shortTitle
				); 
				//
				parameters.addParameter 
				(
					STR16 ("Speed"), // title
		            STR16 ("sec"), // units
			        99, // stepCount
			        0.1, // defaultValueNormalized
                    Steinberg::Vst::ParameterInfo::kCanAutomate, // flags
					GilbertsParams::kParamSpeedId, // tag
			        0, // unitID
		            STR16 ("Speed") // shortTitle
				); 
				/*
				m_speedRangeParameter = std::make_shared<Steinberg::Vst::RangeParameter> 
				(
					STR16("Speed"), // title
			        GilbertsParams::kParamSpeedId, // ParamID
			        STR16("sec"), // units
			        0.1, // minPlain
			        10.0, // maxPlain
			        1.0, // defaultValuePlain
			        99, // stepCount
			        Steinberg::Vst::ParameterInfo::kCanAutomate, // flags
			        0, // unitID
			        STR16("Speed") // shortTitle
				); 
				parameters.addParameter
				(
					m_speedRangeParameter->getInfo()
				);
				*/
			}
			return Steinberg::kResultTrue;
		}
		//------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API PlugController::setComponentState 
		(
			Steinberg::IBStream* state
		)
		{
			// we receive the current state of the component (processor part)
			// we read our parameters and bypass value...
			if (!state)
				return Steinberg::kResultFalse;
			//
			Steinberg::IBStreamer streamer 
			(
				state, 
				kLittleEndian
			);
			//
			float savedParam1 = 0.f;
			if (streamer.readFloat(savedParam1) == false)
			{
				return Steinberg::kResultFalse;
			}
			setParamNormalized 
			(
				GilbertsParams::kParamSpeedId, 
				savedParam1
			);
			//
			// read the bypass
			Steinberg::int32 bypassState;
			if (streamer.readInt32(bypassState) == false)
			{
				return Steinberg::kResultFalse;
			}	
			setParamNormalized 
			(
				kBypassId, 
				bypassState ? 1 : 0
			);
			//
			return Steinberg::kResultOk;
		}
		//------------------------------------------------------------------------
		Steinberg::Vst::ParamValue PlugController::normalizedParamToPlain
		(
			Steinberg::Vst::ParamID tag,
			Steinberg::Vst::ParamValue valueNormalized
		)
		{
			if(kParamSpeedId == tag)
			{
				return ((valueNormalized * ((10.0 - 0.1) / 1.0)) + 0.10);
			}
			else
			{
				return valueNormalized;
			}
		}
		//------------------------------------------------------------------------
		Steinberg::Vst::ParamValue PlugController::plainParamToNormalized
		(
			Steinberg::Vst::ParamID tag,
			Steinberg::Vst::ParamValue value
		)
		{
			if (kParamSpeedId == tag)
			{
				return (value / 10.0);
			}
			else
			{
				return value;
			}
		}
		//------------------------------------------------------------------------
		Steinberg::tresult PlugController::getParamStringByValue
		(
			Steinberg::Vst::ParamID tag,
			Steinberg::Vst::ParamValue valueNormalized,
			Steinberg::Vst::String128 string
		)
		{
			if(GilbertsParams::kBypassId == tag)
			{
				if(valueNormalized) // on
				{
                    std::basic_string<char16_t> str16 = u"On";
                    str16.copy(string, str16.length());
                    
                    //std::string str = "On";
                    //VST3::StringConvert::convert(str, string);
                    //string = VST3::toTChar(str);
                    //char status[] = {'O','n','\0'};
                    
                    if(SMTG_OS_WINDOWS)
                    {
                        //_tcscpy(string, L"On");
                        //VST3::StringConvert::convert(str, string);
                    }
                    
                    if(SMTG_OS_OSX)
                    {
						//std::basic_string<char16_t> str16 = u"On";
                        //str16.copy(string, str16.length());
                        //for(int i = 0; i< status.length(); i++)
                        //for(int i = 0; i< sizeof(status); i++)
                        //{
                            //string[i] = status[i];
                        //}
                        //*string = u'O';
                        //*(string+1) = u'n';
                        //*(string+2) = u'\0';
                    }
				}
				else // off
				{
                    std::basic_string<char16_t> str16 = u"Off";
                    str16.copy(string, str16.length());
                    
                    //std::string str = "Off";
                    //VST3::StringConvert::convert(str, string);
                    /*
                    if(SMTG_OS_WINDOWS)
                    {
					   _tcscpy(string, L"Off");
                    }
                     */
                    /*
                    if(SMTG_OS_OSX)
                    {
                        std::basic_string<char16_t> str16 = u"Off";
                        str16.copy(string, str16.length());
                        //for(int i = 0; i< str.length(); i++)
                        //{
                            // *string[i] = str[i];
                        //}
                        //*string = u'O';
                        //*(string+1) = u'f';
                        //*(string+2) = u'f';
                        //*(string+3) = u'\0';
                    }
                     */
                }
			}
			if (GilbertsParams::kParamSpeedId == tag)
			{
                float valuePlain = ((valueNormalized * ((10.0 - 0.1) / 1.0)) + 0.10);
                
                std::string str16 = std::to_string(valuePlain);
                //std::basic_string<char16_t> str16 = std::to_wstring(valuePlain);
                str16.copy(string, str16.length());
                
                //
                /*
                if(SMTG_OS_WINDOWS)
                {
                    std::wstring wstr = std::to_wstring
                    (
                        valuePlain
                    );
                    tcscpy(string, wstr.c_str());
                }
                 */
                /*
                if(SMTG_OS_OSX)
                {
                    std::string str = std::to_string
                    (
                        valuePlain
                    );
                    str.copy(string, str.length());
                    //for(int i = 0; i< str.length(); i++)
                    //{
                        // *string[i] = str[i];
                    //}
                    //string = new TCHAR[str.size() + 1];
                    //string[str.size()] = '\0';
                    //std::copy(str.begin(), str.end(), string);
                }
                */
			}
			return Steinberg::kResultOk;
		}
		//------------------------------------------------------------------------
	} // namespace Gilberts
} // namespace Carlsound
