/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AngryChihuahuaV1AudioProcessor::AngryChihuahuaV1AudioProcessor()
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      apvts (*this, nullptr, "Parameters",
             { std::make_unique<juce::AudioParameterFloat> ("cutoff", "Cutoff",
                                                           juce::NormalisableRange<float> (20.0f, 20000.0f, 0.1f, 0.2f),
                                                           20.0f),
               std::make_unique<juce::AudioParameterFloat> ("saturation", "Saturation",
                                                           juce::NormalisableRange<float> (0.0f, 1.0f),
                                                           0.0f),
               std::make_unique<juce::AudioParameterFloat> ("mix", "Mix",
                                                           juce::NormalisableRange<float> (0.0f, 1.0f),
                                                           0.0f) })
{
    currentSaturation = 0.0f;
    currentMix = 0.0f;
    currentCutoff = 20.0f;
}

AngryChihuahuaV1AudioProcessor::~AngryChihuahuaV1AudioProcessor()
{
}

float AngryChihuahuaV1AudioProcessor::getSaturationAmount() const
{
    return apvts.getRawParameterValue("saturation")->load();
}

void AngryChihuahuaV1AudioProcessor::setSaturationAmount(float value)
{
    apvts.getParameter("saturation")->setValueNotifyingHost(value);
}

float AngryChihuahuaV1AudioProcessor::getMixAmount() const
{
    return apvts.getRawParameterValue("mix")->load();
}

void AngryChihuahuaV1AudioProcessor::setMixAmount(float value)
{
    apvts.getParameter("mix")->setValueNotifyingHost(value);
}

float AngryChihuahuaV1AudioProcessor::getCutoffAmount() const
{
    return apvts.getRawParameterValue("cutoff")->load();
}

void AngryChihuahuaV1AudioProcessor::setCutoffAmount(float value)
{
    apvts.getParameter("cutoff")->setValueNotifyingHost(value);
}

//==============================================================================
void AngryChihuahuaV1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    processSpec.numChannels = static_cast<juce::uint32> (getTotalNumInputChannels());

    lowCutFilter.prepare(processSpec);
    updateFilter();
}

void AngryChihuahuaV1AudioProcessor::releaseResources()
{
    // Free any resources when playback stops
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AngryChihuahuaV1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Support mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input must match output channels
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

float AngryChihuahuaV1AudioProcessor::processSample(float sample, float saturation)
{
    // If saturation is zero, return the input signal unchanged
    if (saturation <= 0.0f)
        return sample;
        
    // Pre-gain stage with aggressive curve
    float preGain = 1.0f + (saturation * saturation * 40.0f); // Even more aggressive quadratic gain curve
    sample *= preGain;
    
    // Hard clipping with very asymmetric response for aggressive character
    float sign = (sample >= 0) ? 1.0f : -1.0f;
    float absSample = std::abs(sample);
    
    // Aggressive asymmetric clipping
    if (absSample > 0.4f) // Lower threshold for more distortion
    {
        float excess = absSample - 0.4f;
        // Very aggressive clipping on positive side
        if (sample > 0)
        {
            excess = 0.4f * (1.0f - std::exp(-excess * 4.0f)); // More aggressive curve
        }
        // Moderate clipping on negative side for asymmetry
        else
        {
            excess = 0.4f * (1.0f - std::exp(-excess * 2.0f));
        }
        sample = sign * (0.4f + excess);
    }
    
    // Rich harmonic generation
    float evenHarmonics = sample * sample * 0.4f; // Increased even harmonics
    float oddHarmonics = sample * sample * sample * 0.3f; // Increased odd harmonics
    float harmonicMix = (evenHarmonics + oddHarmonics) * saturation;
    
    // Additional high-frequency harmonics for more aggression
    float ultraHarmonics = sample * sample * sample * sample * 0.1f;
    harmonicMix += ultraHarmonics * saturation * saturation;
    
    sample += harmonicMix;
    
    // Post-gain stage with less reduction to maintain aggressive character
    float postGain = 1.0f - (saturation * 0.2f);
    sample *= postGain;
    
    return sample;
}

void AngryChihuahuaV1AudioProcessor::updateFilter()
{
    float cutoff = getCutoffAmount();
    *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(processSpec.sampleRate, cutoff);
}

void AngryChihuahuaV1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get current parameter values
    currentSaturation = getSaturationAmount();
    currentMix = getMixAmount();
    currentCutoff = getCutoffAmount();

    // Update filter if cutoff changed
    updateFilter();

    // Create an AudioBlock object from our buffer
    juce::dsp::AudioBlock<float> block(buffer);
    
    // Store dry signal
    juce::AudioBuffer<float> dryBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    dryBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    if (buffer.getNumChannels() > 1)
        dryBuffer.copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples());

    // Process with filter
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowCutFilter.process(context);

    // Apply saturation and mix
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* dryData = dryBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float wetSignal = processSample(channelData[sample], currentSaturation);
            channelData[sample] = (dryData[sample] * (1.0f - currentMix)) + (wetSignal * currentMix);
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* AngryChihuahuaV1AudioProcessor::createEditor()
{
    return new AngryChihuahuaV1AudioProcessorEditor (*this);
}

//==============================================================================
void AngryChihuahuaV1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AngryChihuahuaV1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AngryChihuahuaV1AudioProcessor();
}
