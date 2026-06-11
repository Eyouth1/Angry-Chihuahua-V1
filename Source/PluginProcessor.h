/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class AngryChihuahuaV1AudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    AngryChihuahuaV1AudioProcessor();
    ~AngryChihuahuaV1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter handling
    float getSaturationAmount() const;
    void setSaturationAmount(float value);
    float getMixAmount() const;
    void setMixAmount(float value);
    float getCutoffAmount() const;
    void setCutoffAmount(float value);
    
    // Get reference to apvts for the editor
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    float currentSaturation = 0.0f;
    float currentMix = 0.0f;
    float currentCutoff = 20.0f;
    
    // Filter parameters
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowCutFilter;
    juce::dsp::ProcessSpec processSpec;
    
    void updateFilter();
    float processSample(float sample, float saturation);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AngryChihuahuaV1AudioProcessor)
};
