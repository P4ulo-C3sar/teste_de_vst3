#pragma once
#include "PluginProcessor.h"
class VoxClarityEditor final:public juce::AudioProcessorEditor{
public: explicit VoxClarityEditor(VoxClarityProcessor&);void paint(juce::Graphics&)override;void resized()override;
private: VoxClarityProcessor& processor;juce::Label title,subtitle;juce::ToggleButton diff{"DIFF"};std::array<juce::Slider,4> sliders;std::array<juce::Label,4> labels;using Attachment=juce::AudioProcessorValueTreeState::SliderAttachment;std::vector<std::unique_ptr<Attachment>> attachments;juce::AudioProcessorValueTreeState::ButtonAttachment diffAttachment;JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoxClarityEditor)
};
