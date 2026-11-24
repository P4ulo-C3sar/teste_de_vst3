#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout VoxClarityProcessor::createParameters() {
  using F = juce::AudioParameterFloat;
  juce::AudioProcessorValueTreeState::ParameterLayout p;
  p.add(std::make_unique<F>("strength", "Strength", juce::NormalisableRange<float>(0.f, 1.f), .72f));
  p.add(std::make_unique<F>("mix", "Mix", juce::NormalisableRange<float>(0.f, 1.f), 1.f));
  p.add(std::make_unique<F>("gate", "Voice Gate", juce::NormalisableRange<float>(0.f, 1.f), .12f));
  p.add(std::make_unique<F>("output", "Output", juce::NormalisableRange<float>(-18.f, 12.f), 0.f, "dB"));
  p.add(std::make_unique<juce::AudioParameterBool>("diff", "Listen Removed Noise", false));
  return p;
}
VoxClarityProcessor::VoxClarityProcessor()
  : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, "VoxClarity", createParameters()) {}
VoxClarityProcessor::~VoxClarityProcessor() { for (auto* state : rnStates) if (state) rnnoise_destroy(state); }
bool VoxClarityProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
  return l.getMainInputChannelSet() == l.getMainOutputChannelSet() && (l.getMainInputChannelSet() == juce::AudioChannelSet::mono() || l.getMainInputChannelSet() == juce::AudioChannelSet::stereo());
}
void VoxClarityProcessor::prepareToPlay(double sr, int maxBlock) {
  for (auto*& state : rnStates) { if (state) rnnoise_destroy(state); state = nullptr; }
  framePosition = 0; gateEnvelope = 0.f; outputRead = outputWrite = 0;
  for (auto& frame : channelFrames) frame.fill(0.f);
  for (auto& frame : channelOutputs) frame.fill(0.f);
  for (auto& ring : outputRing) ring.assign(size_t(maxBlock + 480 + 2), 0.f);
  workBuffer.setSize(getTotalNumInputChannels(), maxBlock);
  if (std::abs(sr - 48000.0) <= 1.0) {
    for (int c = 0; c < getTotalNumInputChannels(); ++c) rnStates[size_t(c)] = rnnoise_create(nullptr);
    setLatencySamples(480);
  } else setLatencySamples(0);
}
void VoxClarityProcessor::processBlock(juce::AudioBuffer<float>& b, juce::MidiBuffer&) {
  juce::ScopedNoDenormals guard;
  const int channels = getTotalNumInputChannels(), n = b.getNumSamples();
  for (int c = channels; c < getTotalNumOutputChannels(); ++c) b.clear(c, 0, n);
  auto value = [this](const char* id) { return parameters.getRawParameterValue(id)->load(std::memory_order_relaxed); };
  const float strength = value("strength"), mix = value("mix"), gate = value("gate"), outputGain = juce::Decibels::decibelsToGain(value("output"));
  const bool diff = value("diff") > .5f;
  if (std::abs(getSampleRate() - 48000.0) > 1.0 || !rnStates[0]) { for (int c = 0; c < channels; ++c) b.clear(c, 0, n); return; }
  const size_t capacity = outputRing[0].size();
  for (int i = 0; i < n; ++i) {
    for (int c = 0; c < channels; ++c) channelFrames[size_t(c)][size_t(framePosition)] = juce::jlimit(-1.f, 1.f, b.getSample(c, i) * 32768.f);
    ++framePosition;
    if (framePosition == 480) {
      float vad = 0.f;
      for (int c = 0; c < channels; ++c) vad += rnnoise_process_frame(rnStates[size_t(c)], channelOutputs[size_t(c)].data(), channelFrames[size_t(c)].data());
      gateEnvelope = .8f * gateEnvelope + .2f * (vad / float(channels));
      for (int c = 0; c < channels; ++c) for (size_t k = 0; k < 480; ++k) {
        const size_t dst = (outputWrite + k) % capacity;
        const float dry = channelFrames[size_t(c)][k] / 32768.f;
        const float wet = channelOutputs[size_t(c)][k] / 32768.f;
        const float cleaned = dry * (1.f - strength) + wet * strength;
        const float gateGain = gate <= 0.f ? 1.f : juce::jlimit(.06f, 1.f, gateEnvelope / (gateEnvelope + gate * .8f));
        outputRing[size_t(c)][dst] = (diff ? dry - cleaned : cleaned * gateGain) * outputGain;
      }
      outputWrite = (outputWrite + 480) % capacity;
      framePosition = 0;
    }
    const bool ready = outputWrite != outputRead;
    for (int c = 0; c < channels; ++c) {
      const float dry = b.getSample(c, i);
      const float wet = ready ? outputRing[size_t(c)][outputRead] : 0.f;
      workBuffer.setSample(c, i, dry * (1.f - mix) + wet * mix);
    }
    outputRead = (outputRead + 1) % capacity;
  }
  for (int c = 0; c < channels; ++c) b.copyFrom(c, 0, workBuffer, c, 0, n);
}
juce::AudioProcessorEditor* VoxClarityProcessor::createEditor() { return new VoxClarityEditor(*this); }
void VoxClarityProcessor::getStateInformation(juce::MemoryBlock& m) { if (auto x = parameters.copyState().createXml()) copyXmlToBinary(*x, m); }
void VoxClarityProcessor::setStateInformation(const void* d, int n) { if (auto x = getXmlFromBinary(d, n)) parameters.replaceState(juce::ValueTree::fromXml(*x)); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VoxClarityProcessor(); }
