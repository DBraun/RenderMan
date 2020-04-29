/*
  ==============================================================================

    RenderEngine.h
    Created: 19 Feb 2017 9:47:15pm
    Author:  tollie

  ==============================================================================
*/

#ifndef RENDERENGINE_H_INCLUDED
#define RENDERENGINE_H_INCLUDED

#include <random>
#include <array>
#include <iomanip>
#include <sstream>
#include <string>
#include "Maximilian/maximilian.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include <boost/python.hpp>

using namespace juce;

typedef std::vector<std::pair<int, float>>  PluginPatch;

class RenderEngine
{
public:
    RenderEngine (int sr=44100,
                  int bs=512) :
        sampleRate(sr),
        bufferSize(bs),
        plugin(nullptr)
    {
        maxiSettings::setup (sampleRate, 1, bufferSize);
    }

    virtual ~RenderEngine()
    {
        if (plugin)
        {
            plugin->releaseResources();
			plugin.release();
        }
    }

	RenderEngine(RenderEngine&) = default;

    bool loadPreset (const std::string& path);

    bool loadPlugin (const std::string& path);
    
    int nMidiEvents () {
        return midiBuffer.getNumEvents();
    };

    bool loadMidi (const std::string& path);

    void setPatch (const PluginPatch patch);

    float getParameter (const int parameter);

    std::string getParameterAsText(const int parameter);

    void setParameter (const int parameter, const float value);

    const PluginPatch getPatch();
    
    void clearMidi();

    bool addMidiNote(const uint8  midiNote,
                      const uint8  midiVelocity,
                      const double noteStart,
                      const double noteLength);

    int hello () {
        DBG("hello");
        return 1;
    }

    void render (const double renderLength);

    const std::vector<double> getRMSFrames();

    const size_t getPluginParameterSize();

    boost::python::list getPluginParametersDescription();

    bool overridePluginParameter (const int   index,
                                  const float value);

    bool removeOverridenParameter (const int index);

    const std::vector<std::vector<double>> getAudioFrames();

    bool writeToWav(const std::string& path);

private:
    void fillAudioFeatures (const AudioSampleBuffer& data);
    
    void ifTimeSetNoteOff (const double& noteLength,
                           const double& sampleRate,
                           const int&    bufferSize,
                           const uint8&  midiChannel,
                           const uint8&  midiPitch,
                           const uint8&  midiVelocity,
                           const int&    currentBufferIndex,
                           MidiBuffer&   bufferToNoteOff);
                           
    void fillAvailablePluginParameters (PluginPatch& params);


    MidiFile             midiData;
    MidiBuffer           midiBuffer;

    double               sampleRate;
    int                  bufferSize;
	std::unique_ptr<juce::AudioPluginInstance, std::default_delete<juce::AudioPluginInstance>> plugin;
    PluginPatch          pluginParameters;
    PluginPatch          overridenParameters;
    std::vector<double>  processedAudioPreviewLeft;
    std::vector<double>  processedAudioPreviewRight;
    std::vector<double>  rmsFrames;
    double               currentRmsFrame = 0;
};


#endif  // RENDERENGINE_H_INCLUDED
