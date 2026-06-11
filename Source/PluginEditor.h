/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BinaryData.h"

//==============================================================================
class FilmstripLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FilmstripLookAndFeel()
    {
        filmstrip = juce::ImageCache::getFromMemory(BinaryData::knob_medium_png,
                                                    BinaryData::knob_medium_pngSize);
        if (filmstrip.isValid())
        {
            frameWidth  = filmstrip.getWidth();                     // 100
            frameHeight = filmstrip.getWidth();                     // square frames
            numFrames   = filmstrip.getHeight() / frameHeight;     // 128
        }
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float, float, juce::Slider&) override
    {
        if (!filmstrip.isValid())
            return;

        // Clamp and pick frame
        const int frame = juce::jlimit(0, numFrames - 1,
                                       (int)(sliderPos * (numFrames - 1)));

        // Draw the correct filmstrip frame scaled to fill the knob bounds
        g.drawImage(filmstrip,
                    x, y, width, height,                        // destination
                    0, frame * frameHeight, frameWidth, frameHeight); // source
    }

private:
    juce::Image filmstrip;
    int frameWidth  = 100;
    int frameHeight = 100;
    int numFrames   = 128;
};

//==============================================================================
class AngryChihuahuaV1AudioProcessorEditor : public juce::AudioProcessorEditor,
                                              public juce::Slider::Listener
{
public:
    explicit AngryChihuahuaV1AudioProcessorEditor(AngryChihuahuaV1AudioProcessor&);
    ~AngryChihuahuaV1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    AngryChihuahuaV1AudioProcessor& audioProcessor;

    // Single shared LookAndFeel — declared before knobs so it outlives them
    FilmstripLookAndFeel filmstripLookAndFeel;

    // GUI components
    juce::Slider cutoffKnob;
    juce::Slider saturationKnob;
    juce::Slider mixKnob;
    juce::Label  cutoffLabel;
    juce::Label  saturationLabel;
    juce::Label  mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> saturationAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    // Images
    juce::Image calmImage;
    juce::Image aggressiveImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AngryChihuahuaV1AudioProcessorEditor)
};
