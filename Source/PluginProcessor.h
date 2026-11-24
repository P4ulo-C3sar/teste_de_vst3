#pragma once
#include <JuceHeader.h>
#include <array>
#include <memory>
#include <rnnoise.h>
class VoxClarityProcessor final : public juce::AudioProcessor {
public:
  VoxClarityProcessor();
  ~VoxClarityProcessor() override;
  void prepareToPlay(double, int) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout&) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }
  const juce::String getName() const override { return "VoxClarity"; }
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
  std::array<std::array<float, 480>, 2> channelFrames{};
  std::array<std::array<float, 480>, 2> channelOutputs{};
  std::array<DenoiseState*, 2> rnStates{};
  int framePosition = 0;
  float gateEnvelope = 0.0f;
  size_t outputRead = 0;
  size_t outputWrite = 0;
  std::array<std::vector<float>, 2> outputRing;
  juce::AudioBuffer<float> workBuffer;
  juce::AudioBuffer<float> processedBuffer;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoxClarityProcessor)
};
