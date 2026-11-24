#pragma once
#include <JuceHeader.h>
#include <array>
class VocalClarityProcessor final : public juce::AudioProcessor {
public:
  VocalClarityProcessor();
  void prepareToPlay(double, int) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout&) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }
  const juce::String getName() const override { return "VocalClarity"; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.25; }
  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int, const juce::String&) override {}
  void getStateInformation(juce::MemoryBlock&) override;
  void setStateInformation(const void*, int) override;
  juce::AudioProcessorValueTreeState parameters;
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
private:
  static constexpr int bands = 96;
  double sampleRate = 44100.0;
  std::array<std::atomic<float>, bands> learnedNoise{};
  std::array<std::array<float, bands>, 2> envelope{};
  juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay{48000};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalClarityProcessor)
};
