

#include "../include/plugprocessor.h"
#include "../include/plugids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

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
							mParam1 = value;
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
        mOscillatorLeft.phaseReset(0.0);
        mOscillatorRight.phaseReset(M_PI);
        //
        //data.outputs[0].channelBuffers64;

        // (simplification) we suppose in this example that we have the same input channel count than the output
        int32 numChannels = data.inputs[0].numChannels;

        //---get audio buffers----------------
        uint32 sampleFramesSize = getSampleFramesSizeInBytes (processSetup, data.numSamples);
        void** in = getChannelBuffersPointer (processSetup, data.inputs[0]);
        void** out = getChannelBuffersPointer (processSetup, data.outputs[0]);

        //---check if silence---------------
        // normally we have to check each channel (simplification)
        if (data.inputs[0].silenceFlags != 0)
        {
            // mark output silence too
            data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

            // the Plug-in has to be sure that if it sets the flags silence that the output buffer are clear
            for (int32 i = 0; i < numChannels; i++)
            {
                // do not need to be cleared if the buffers are the same (in this case input buffer are already cleared by the host)
                if (in[i] != out[i])
                {
                    memset (out[i], 0, sampleFramesSize);
                }
            }

            // nothing to do at this point
            return kResultOk;
        }

        // mark our outputs has not silent
        data.outputs[0].silenceFlags = 0;

        //---in bypass mode outputs should be like inputs-----
        if (bBypass)
        {
            for (int32 i = 0; i < numChannels; i++)
            {
                // do not need to be copied if the buffers are the same
                if (in[i] != out[i])
                {
                    memcpy (out[i], in[i], sampleFramesSize);
                }
            }
            // in this example we do not update the VuMeter in Bypass
        }
        else
        {
            float fVuPPM = 0.f;

            //---apply gain factor----------
            float gain = (fGain - fGainReduction);
            if (bHalfGain)
            {
                gain = gain * 0.5f;
            }

            // if the applied gain is nearly zero, we could say that the outputs are zeroed and we set the silence flags.
            if (gain < 0.0000001)
            {
                for (int32 i = 0; i < numChannels; i++)
                {
                    memset (out[i], 0, sampleFramesSize);
                }
                data.outputs[0].silenceFlags = (1 << numChannels) - 1;  // this will set to 1 all channels
            }
            else
            {
                if (data.symbolicSampleSize == kSample32)
                    fVuPPM = processAudio<Sample32> ((Sample32**)in, (Sample32**)out, numChannels,
                            data.numSamples, gain);
                else
                    fVuPPM = processAudio<Sample64> ((Sample64**)in, (Sample64**)out, numChannels,
                            data.numSamples, gain);
            }

            //---3) Write outputs parameter changes-----------
            IParameterChanges* outParamChanges = data.outputParameterChanges;
            // a new value of VuMeter will be send to the host
            // (the host will send it back in sync to our controller for updating our editor)
            if (outParamChanges && fVuPPMOld != fVuPPM)
            {
                int32 index = 0;
                IParamValueQueue* paramQueue = outParamChanges->addParameterData (kVuPPMId, index);
                if (paramQueue)
                {
                    int32 index2 = 0;
                    paramQueue->addPoint (0, fVuPPM, index2);
                }
            }
            fVuPPMOld = fVuPPM;
        }
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

	mParam1 = savedParam1;
	mBypass = savedBypass > 0;

	return Steinberg::kResultOk;
}

//------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API PlugProcessor::getState (Steinberg::IBStream* state)
{
	// here we need to save the model (preset or project)

	float toSaveParam1 = mParam1;
	Steinberg::int32 toSaveBypass = mBypass ? 1 : 0;

	Steinberg::IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat (toSaveParam1);
	streamer.writeInt32 (toSaveBypass);

	return Steinberg::kResultOk;
}

//------------------------------------------------------------------------
} // namespace Gilberts
} // namespace Carlsound
