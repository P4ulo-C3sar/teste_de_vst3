cmake_minimum_required(VERSION 3.22)
project(VoxClarity VERSION 0.3.0 LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.12
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)
FetchContent_Declare(RNVoice
  GIT_REPOSITORY https://github.com/werman/noise-suppression-for-voice.git
  GIT_TAG master
  GIT_SHALLOW TRUE)
FetchContent_GetProperties(RNVoice)
if(NOT rnvoice_POPULATED)
  FetchContent_Populate(RNVoice)
endif()
add_subdirectory(${rnvoice_SOURCE_DIR}/external/rnnoise ${rnvoice_BINARY_DIR}/rnnoise)

juce_add_plugin(VoxClarity
  COMPANY_NAME "VoxClarity Audio"
  BUNDLE_ID "audio.voxclarity.rncleanup"
  PLUGIN_MANUFACTURER_CODE VxAu
  PLUGIN_CODE Vx01
  FORMATS VST3
  PRODUCT_NAME "VoxClarity"
  DESCRIPTION "RNNoise vocal noise suppression"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  VST3_CATEGORIES Fx Restoration)
target_compile_definitions(VoxClarity PRIVATE JUCE_FORCE_USE_LEGACY_PARAM_IDS=1)

juce_generate_juce_header(VoxClarity)
target_sources(VoxClarity PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_features(VoxClarity PRIVATE cxx_std_17)
target_compile_definitions(VoxClarity PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(VoxClarity PRIVATE juce::juce_audio_utils RnNoise PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
