//------------------------------------------------------------------------
#pragma once
//
#define _USE_MATH_DEFINES 
#include <cmath>
//
#include "public.sdk/source/vst/vstaudioeffect.h"
//
#include "plugcontroller.h"
//
#include "maximilian.h"
#include <public.sdk/source/vst/vstparameters.h>
//
namespace Carlsound
{

	namespace Gilberts 
	{
		//-----------------------------------------------------------------------------
		class PlugProcessor : public Steinberg::Vst::AudioEffect
		{
		public:
		PlugProcessor ();
		//
		Steinberg::tresult PLUGIN_API initialize 
		(
			FUnknown* context
		) 
			SMTG_OVERRIDE;
		//
		Steinberg::tresult PLUGIN_API setBusArrangements 
		(
			Steinberg::Vst::SpeakerArrangement* inputs,
			Steinberg::int32 numIns,
			Steinberg::Vst::SpeakerArrangement* outputs,
			Steinberg::int32 numOuts
		) 
			SMTG_OVERRIDE;
		//
		Steinberg::tresult PLUGIN_API setActive 
		(
			Steinberg::TBool state
		) 
			SMTG_OVERRIDE;
		//
		Steinberg::tresult PLUGIN_API process 
		(
			Steinberg::Vst::ProcessData& data
		) 
			SMTG_OVERRIDE;
		//
		//------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API setState 
		(
			Steinberg::IBStream* state
		) 
			SMTG_OVERRIDE;
		//
		Steinberg::tresult PLUGIN_API getState 
		(
			Steinberg::IBStream* state
		) 
			SMTG_OVERRIDE;
		//
		static FUnknown* createInstance (void*)
		{
			return (Steinberg::Vst::IAudioProcessor*)new PlugProcessor ();
		}
		//
		template <class T> void bufferSampleGain
		(
			T inBuffer, 
			T outBuffer, 
			const int sampleLocation, 
			const double gainValue
		);
		//
		protected:
		Steinberg::Vst::ParamValue m_speedNormalizedValue = 0;
		bool m_bypassState = false;
		//
		std::shared_ptr<maxiSettings> m_oscillatorSettings;
		std::shared_ptr<maxiOsc> m_oscillator[2];
		//
		double m_gainValue[2];
		//
		std::shared_ptr<Steinberg::Vst::RangeParameter> m_speedRangeParameter;
		};
		//------------------------------------------------------------------------
	} // namespace Gilberts
} // namespace Carlsound
