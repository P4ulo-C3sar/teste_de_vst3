#pragma once
#include "PluginProcessor.h"
class VocalClarityEditor final:public juce::AudioProcessorEditor{
public: explicit VocalClarityEditor(VocalClarityProcessor&);void paint(juce::Graphics&)override;void resized()override;
private: VocalClarityProcessor& processor;juce::Label title;juce::ToggleButton learn{"LEARN NOISE"};std::array<juce::Slider,7> sliders;std::array<juce::Label,7> labels;using Attachment=juce::AudioProcessorValueTreeState::SliderAttachment;std::vector<std::unique_ptr<Attachment>> attachments;juce::AudioProcessorValueTreeState::ButtonAttachment learnAttachment;JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalClarityEditor)
};
