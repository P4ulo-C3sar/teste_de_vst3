#include "PluginProcessor.h"
#include "PluginEditor.h"
juce::AudioProcessorValueTreeState::ParameterLayout VocalClarityProcessor::createParameters() {
  using P=juce::AudioParameterFloat; juce::AudioProcessorValueTreeState::ParameterLayout p;
  p.add(std::make_unique<juce::AudioParameterBool>("learn","Noise Learn",false));
  auto add=[&p](const char* id,const char* name,float def){p.add(std::make_unique<P>(id,name,juce::NormalisableRange<float>(0.0f,1.0f),def));};
  add("reduction","Noise Reduction",.45f);add("hiss","Hiss",.25f);add("hum","Hum",0.0f);add("clicks","Clicks",0.0f);add("sibilance","Sibilance",0.0f);add("dereverb","De-reverb",0.0f);add("mix","Mix",1.0f);return p;
}
VocalClarityProcessor::VocalClarityProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),parameters(*this,nullptr,"VocalClarity",createParameters()){}
bool VocalClarityProcessor::isBusesLayoutSupported(const BusesLayout& l)const{return l.getMainInputChannelSet()==juce::AudioChannelSet::stereo()&&l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
void VocalClarityProcessor::prepareToPlay(double sr,int){sampleRate=sr;delay.setMaximumDelayInSamples(int(sr*.18));delay.prepare({sr,512,2});delay.setDelay(float(sr*.12));delay.reset();for(auto& c:envelope)c.fill(0.0f);}
void VocalClarityProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&){
  juce::ScopedNoDenormals guard;const int ch=juce::jmin(2,b.getNumChannels()),n=b.getNumSamples();auto v=[this](const char* id){return parameters.getRawParameterValue(id)->load();};
  const float learn=v("learn"),nr=v("reduction"),hiss=v("hiss"),hum=v("hum"),clicks=v("clicks"),sib=v("sibilance"),rev=v("dereverb"),mix=v("mix");
  for(int i=0;i<n;++i){const float mono=ch==2?.5f*(b.getSample(0,i)+b.getSample(1,i)):b.getSample(0,i);const float energy=std::abs(mono);const size_t band=std::min<size_t>(bands-1,size_t(std::clamp(std::log1p(energy*900.0f)/7.5f,0.0f,1.0f)*(bands-1)));
    float profile=learnedNoise[band].load(std::memory_order_relaxed);if(learn>.5f){profile=profile*.9995f+energy*.0005f;learnedNoise[band].store(profile,std::memory_order_relaxed);}const float floor=profile*(1.0f+nr*5.0f),gate=energy>floor?1.0f:std::clamp(energy/(floor+1.0e-6f),.08f,1.0f);
    for(int c=0;c<ch;++c){float x=b.getSample(c,i),y=x*(1.0f-nr*(1.0f-gate));const float prev=envelope[c][band];envelope[c][band]=prev*.94f+std::abs(x)*.06f;
      const float hi=band>bands*2/3?1.0f:0.0f;y*=1.0f-hi*(hiss*nr*.22f+sib*std::max(0.0f,std::abs(x)-.12f)*1.5f);
      if(i>0&&clicks>0){const float d=y-b.getSample(c,i-1),limit=.35f+clicks*.08f;if(std::abs(d)>limit)y=b.getSample(c,i-1)+std::copysign(limit,d);}
      const float hz=float(band)/float(bands-1)*18000.0f;float humGain=1.0f;for(int h=1;h<=8;++h)for(float f:{50.0f,60.0f}){const float d=hz-f*h;humGain*=1.0f-hum*.68f*std::exp(-d*d/(2.0f*7.0f*7.0f));}y*=humGain;
      const float tail=std::clamp((envelope[c][band]-std::abs(y)*1.3f)/(envelope[c][band]+1.0e-5f),0.0f,1.0f);y*=1.0f-rev*tail*.55f;delay.pushSample(c,y);y-=rev*.035f*delay.popSample(c);b.setSample(c,i,x*(1.0f-mix)+y*mix);
    }
  }
}
juce::AudioProcessorEditor* VocalClarityProcessor::createEditor(){return new VocalClarityEditor(*this);}
void VocalClarityProcessor::getStateInformation(juce::MemoryBlock& m){auto s=parameters.copyState();for(size_t i=0;i<bands;++i)s.setProperty("noise"+juce::String(i),learnedNoise[i].load(),nullptr);auto x=s.createXml();copyXmlToBinary(*x,m);}
void VocalClarityProcessor::setStateInformation(const void* d,int n){if(auto x=getXmlFromBinary(d,n)){auto s=juce::ValueTree::fromXml(*x);parameters.replaceState(s);for(size_t i=0;i<bands;++i)learnedNoise[i].store(float(s.getProperty("noise"+juce::String(i),0.0f)));}}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new VocalClarityProcessor();}
