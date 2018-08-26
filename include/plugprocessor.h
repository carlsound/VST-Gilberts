//------------------------------------------------------------------------
#pragma once
//
#include "../include/plugcontroller.h"
#include "../include/plugids.h"
//
#include "maximilian.h"
//
#include "base/source/fstreamer.h"
//
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/vsttypes.h"
//
#include "public.sdk/samples/vst/note_expression_synth/source/note_expression_synth_voice.h"
#include "public.sdk/source/vst/vstaudioeffect.h"
#include <public.sdk/source/vst/vstaudioprocessoralgo.h>
#include <public.sdk/source/vst/vstparameters.h>
//
#define _USE_MATH_DEFINES
#include <cmath>
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
		//------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API setBusArrangements 
		(
			Steinberg::Vst::SpeakerArrangement* inputs,
			Steinberg::int32 numIns,
			Steinberg::Vst::SpeakerArrangement* outputs,
			Steinberg::int32 numOuts
		) 
			SMTG_OVERRIDE;
		//------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API setActive 
		(
			Steinberg::TBool state
		) 
			SMTG_OVERRIDE;
		//------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API process 
		(
			Steinberg::Vst::ProcessData& data
		) 
			SMTG_OVERRIDE;
		//
		Steinberg::tresult PLUGIN_API processInputParameterChanges
		(
			Steinberg::Vst::ProcessData& data
		);
		//
		Steinberg::tresult PLUGIN_API processAudio
		(
			Steinberg::Vst::ProcessData& data
		);
		//
		Steinberg::tresult PLUGIN_API processOutputParameterChanges
		(
			Steinberg::Vst::ProcessData& data
		);
		//------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API setState 
		(
			Steinberg::IBStream* state
		) 
			SMTG_OVERRIDE;
		//------------------------------------------------------------------------
		Steinberg::tresult PLUGIN_API getState 
		(
			Steinberg::IBStream* state
		) 
			SMTG_OVERRIDE;
		//------------------------------------------------------------------------
		static FUnknown* createInstance (void*)
		{
			return (Steinberg::Vst::IAudioProcessor*)new PlugProcessor ();
		}
		//------------------------------------------------------------------------
		template <class T>
		void bufferSampleGain
		(
			T inBuffer, 
			T outBuffer, 
			const int sampleLocation, 
			const double gainValue
		);
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
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
