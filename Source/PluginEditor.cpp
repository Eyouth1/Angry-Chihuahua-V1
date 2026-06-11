/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AngryChihuahuaV1AudioProcessorEditor::AngryChihuahuaV1AudioProcessorEditor(AngryChihuahuaV1AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set up cutoff knob
    cutoffKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    cutoffKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    cutoffKnob.setLookAndFeel(&filmstripLookAndFeel);
    cutoffKnob.setRange(20.0f, 20000.0f, 0.1f);
    cutoffKnob.setValue(20.0f);
    cutoffKnob.setSkewFactor(0.2f);
    cutoffKnob.setDoubleClickReturnValue(true, 20.0f);
    addAndMakeVisible(cutoffKnob);
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "cutoff", cutoffKnob);

    // Set up saturation (Growl) knob
    saturationKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    saturationKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    saturationKnob.setLookAndFeel(&filmstripLookAndFeel);
    saturationKnob.setRange(0.0f, 1.0f);
    saturationKnob.setValue(0.0f);
    saturationKnob.setDoubleClickReturnValue(true, 0.0f);
    addAndMakeVisible(saturationKnob);
    saturationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "saturation", saturationKnob);

    saturationKnob.onValueChange = [this]()
    {
        repaint();
    };

    // Set up mix knob
    mixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixKnob.setLookAndFeel(&filmstripLookAndFeel);
    mixKnob.setRange(0.0f, 1.0f);
    mixKnob.setValue(0.0f);
    mixKnob.setDoubleClickReturnValue(true, 0.0f);
    addAndMakeVisible(mixKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "mix", mixKnob);

    // Set up labels
    auto styleLabel = [](juce::Label& lbl, juce::Colour c)
    {
        lbl.setFont(juce::Font(11.0f, juce::Font::bold));
        lbl.setColour(juce::Label::textColourId, c);
        lbl.setJustificationType(juce::Justification::centred);
    };

    addAndMakeVisible(cutoffLabel);
    cutoffLabel.setText("CUTOFF", juce::dontSendNotification);
    styleLabel(cutoffLabel, juce::Colour(0xff4da6ff));

    addAndMakeVisible(saturationLabel);
    saturationLabel.setText("GROWL", juce::dontSendNotification);
    styleLabel(saturationLabel, juce::Colour(0xffff6600));

    addAndMakeVisible(mixLabel);
    mixLabel.setText("DRY/WET", juce::dontSendNotification);
    styleLabel(mixLabel, juce::Colour(0xff38d97a));

    // Load images
    calmImage = juce::ImageCache::getFromMemory(BinaryData::calm_image_png, BinaryData::calm_image_pngSize);
    aggressiveImage = juce::ImageCache::getFromMemory(BinaryData::aggressive_image_png, BinaryData::aggressive_image_pngSize);

    // Ensure valid images, fallback if necessary
    if (!calmImage.isValid() || !aggressiveImage.isValid())
    {
        DBG("One or both images failed to load, creating blank fallback images.");
        calmImage = juce::Image(juce::Image::RGB, 400, 300, true);
        calmImage.clear(calmImage.getBounds(), juce::Colours::white);
        aggressiveImage = juce::Image(juce::Image::RGB, 400, 300, true);
        aggressiveImage.clear(aggressiveImage.getBounds(), juce::Colours::white);
    }

    // Set window size
    setSize(400, 300);
}

AngryChihuahuaV1AudioProcessorEditor::~AngryChihuahuaV1AudioProcessorEditor()
{
    cutoffKnob.setLookAndFeel(nullptr);
    saturationKnob.setLookAndFeel(nullptr);
    mixKnob.setLookAndFeel(nullptr);
}


//==============================================================================
// Optimized paint method for smooth image morphing
void AngryChihuahuaV1AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white); // Background color

    auto bounds = getLocalBounds().toFloat();
    float morphValue = saturationKnob.getValue(); // Get Growl knob value (0.0 - 1.0)

    if (calmImage.isValid() && aggressiveImage.isValid())
    {
        // Draw calm image as base
        g.drawImageWithin(calmImage, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                          juce::RectanglePlacement::stretchToFit);

        // Blend aggressive image on top with dynamic opacity
        g.setOpacity(morphValue);
        g.drawImageWithin(aggressiveImage, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                          juce::RectanglePlacement::stretchToFit);

        g.setOpacity(1.0f); // Reset opacity after drawing
    }
}

//==============================================================================
// Ensure UI elements resize correctly
void AngryChihuahuaV1AudioProcessorEditor::resized()
{
    const int knobSize = 60;
    const int knobSpacing = 15;
    const int totalWidth = (knobSize * 3) + (knobSpacing * 2);
    const int startX = (getWidth() - totalWidth) / 2;
    const int knobY = getHeight() - knobSize - 40; // Move knobs down

    cutoffKnob.setBounds(startX, knobY, knobSize, knobSize);
    cutoffLabel.setBounds(startX - 10, knobY + knobSize + 2, knobSize + 20, 18);

    saturationKnob.setBounds(startX + knobSize + knobSpacing, knobY, knobSize, knobSize);
    saturationLabel.setBounds(startX + knobSize + knobSpacing - 10, knobY + knobSize + 2, knobSize + 20, 18);

    mixKnob.setBounds(startX + (knobSize + knobSpacing) * 2, knobY, knobSize, knobSize);
    mixLabel.setBounds(startX + (knobSize + knobSpacing) * 2 - 10, knobY + knobSize + 2, knobSize + 20, 18);
}

//==============================================================================
// Ensure the processor updates and force UI repaint
void AngryChihuahuaV1AudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &saturationKnob) // Growl knob
    {
        audioProcessor.setSaturationAmount(slider->getValue());
        repaint(); // 🔥 Ensures image morphing updates instantly
    }
    else if (slider == &mixKnob)
    {
        audioProcessor.setMixAmount(slider->getValue());
    }
}
