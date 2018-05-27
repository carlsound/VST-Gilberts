

#include "../include/plugprocessor.h"
#include "../include/plugids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include <public.sdk/source/vst/vstaudioprocessoralgo.h>

namespace Carlsound {
namespace Gilberts {

//-----------------------------------------------------------------------------
PlugProcessor::PlugProcessor ()
{
	// register its editor class
	setControllerClass (MyControllerUID);
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugProcessor::initialize (FUnknown* context)
{
	//---always initialize the parent-------
	Steinberg::tresult result = AudioEffect::initialize (context);
	if (result != Steinberg::kResultTrue)
		return Steinberg::kResultFalse;

	//---create Audio In/Out buses------
	// we want a stereo Input and a Stereo Output
	addAudioInput (STR16 ("AudioInput"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("AudioOutput"), Steinberg::Vst::SpeakerArr::kStereo);

	mOscillatorLeft.phaseReset(0.0);
	mOscillatorRight.phaseReset(0.0);

	return Steinberg::kResultTrue;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugProcessor::setBusArrangements (Steinberg::Vst::SpeakerArrangement* inputs,
                                                            Steinberg::int32 numIns,
                                                            Steinberg::Vst::SpeakerArrangement* outputs,
                                                            Steinberg::int32 numOuts)
{
	// we only support one in and output bus and these buses must have the same number of channels
	if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0])
	{
		return AudioEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
	}
	return Steinberg::kResultFalse;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugProcessor::setActive (Steinberg::TBool state)
{
	if (state) // Initialize
	{
		// Allocate Memory Here
		// Ex: algo.create ();
	}
	else // Release
	{
		// Free Memory if still allocated
		// Ex: if(algo.isCreated ()) { algo.destroy (); }
	}
	return AudioEffect::setActive (state);
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugProcessor::process (Steinberg::Vst::ProcessData& data)
{
	//--- Read inputs parameter changes-----------
	if (data.inputParameterChanges)
	{
		Steinberg::int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (Steinberg::int32 index = 0; index < numParamsChanged; index++)
		{
			Steinberg::Vst::IParamValueQueue* paramQueue =
			    data.inputParameterChanges->getParameterData (index);
			if (paramQueue)
			{
				Steinberg::Vst::ParamValue value;
				Steinberg::int32 sampleOffset;
				Steinberg::int32 numPoints = paramQueue->getPointCount ();
				switch (paramQueue->getParameterId ())
				{
					case GilbertsParams::kParamSpeedId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
								Steinberg::kResultTrue)
							mSpeed = value;
						break;
					case GilbertsParams::kBypassId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
								Steinberg::kResultTrue)
							mBypass = (value > 0.5f);
						break;
				}
			}
		}
	}

	//--- Process Audio---------------------
	//--- ----------------------------------
	if (data.numInputs == 0 || data.numOutputs == 0)
	{
		// nothing to do
		return Steinberg::kResultOk;
	}

	if (data.numSamples > 0)
	{
		// Process Algorithm
		// Ex: algo.process (data.inputs[0].channelBuffers32, data.outputs[0].channelBuffers32,
		// data.numSamples);
        
        //
        //data.outputs[0].channelBuffers64;

        // assume the same input channel count as the output
		Steinberg::int32 numChannels = data.inputs[0].numChannels;

        //---get audio buffers----------------
		Steinberg::uint32 sampleFramesSize = getSampleFramesSizeInBytes (processSetup, data.numSamples);
        void** in = getChannelBuffersPointer (processSetup, data.inputs[0]);
        void** out = getChannelBuffersPointer (processSetup, data.outputs[0]);

        //---check if silence---------------
        // normally we have to check each channel (simplification)
        if (data.inputs[0].silenceFlags != 0)
        {
            // mark output silence too
            data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

            // the Plug-in has to be sure that if it sets the flags silence that the output buffer are clear
            for (Steinberg::int32 i = 0; i < numChannels; i++)
            {
                // do not need to be cleared if the buffers are the same (in this case input buffer are already cleared by the host)
                if (in[i] != out[i])
                {
                    memset (out[i], 0, sampleFramesSize);
                }
            }

            // nothing to do at this point
            return Steinberg::kResultOk;
        }

        // mark our outputs has not silent
        data.outputs[0].silenceFlags = 0;

		for (int sample = 0; sample < data.numSamples; sample++)
		{
			if(mBypass)
			{
				mGainLeft = 1.0;
				mGainRight = 1.0;
			}
			else
			{
				mGainLeft = mOscillatorLeft.coswave(1.0/mSpeed);
				mGainRight = mOscillatorRight.sinewave(1.0/mSpeed);
			}
			for (int channel = 0; channel < data.inputs->numChannels; channel++)
			{
				if (data.symbolicSampleSize == Steinberg::Vst::kSample32) //32-Bit
				{
					*data.outputs[channel].channelBuffers32[sample] = *data.inputs[channel].channelBuffers32[sample] * mGainLeft;
				}
				else // 64-Bit
				{
					*data.outputs[channel].channelBuffers64[sample] = *data.inputs[channel].channelBuffers64[sample] * mGainRight;
				}
			}
		}

        // Write outputs parameter changes-----------
	    Steinberg::Vst::IParameterChanges* outParamChanges = data.outputParameterChanges;
	}
	return Steinberg::kResultOk;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugProcessor::setState (Steinberg::IBStream* state)
{
	if (!state)
		return Steinberg::kResultFalse;

	// called when we load a preset or project, the model has to be reloaded

	Steinberg::IBStreamer streamer (state, kLittleEndian);

	float savedParam1 = 0.f;
	if (streamer.readFloat (savedParam1) == false)
		return Steinberg::kResultFalse;

	Steinberg::int32 savedBypass = 0;
	if (streamer.readInt32 (savedBypass) == false)
		return Steinberg::kResultFalse;

	mSpeed = savedParam1;
	mBypass = savedBypass > 0;

	return Steinberg::kResultOk;
}

//------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugProcessor::getState (Steinberg::IBStream* state)
{
	// here we need to save the model (preset or project)

	float toSaveParam1 = mSpeed;
	Steinberg::int32 toSaveBypass = mBypass ? 1 : 0;

	Steinberg::IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat (toSaveParam1);
	streamer.writeInt32 (toSaveBypass);

	return Steinberg::kResultOk;
}

//------------------------------------------------------------------------
} // namespace Gilberts
} // namespace Carlsound
