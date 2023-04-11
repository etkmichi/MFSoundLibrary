/*
 * MFSFXRSoundData.cpp
 *
 *  Created on: 07.01.2021
 *      Author: michl
 */

#include "../MFSFXR/MFSFXRSoundData.h"

#include <cstring>

MFSFXRSoundData::MFSFXRSoundData(MFSFXRSoundSetup* pSetup):
MFSoundPlaybackData(nullptr){
  pSoundSetup=pSetup;
  if(pSetup==nullptr){
    pInternalSoundSetup=new MFSFXRSoundSetup();
    pSoundSetup=pInternalSoundSetup;
  }
  file_sampleswritten=0;
  resetSample(false);
  pData=nullptr;
}

MFSFXRSoundData::MFSFXRSoundData(E_SoundBaseSetup sfxrBaseSetup):
    MFSoundPlaybackData(nullptr){
    pInternalSoundSetup=new MFSFXRSoundSetup(sfxrBaseSetup);
    pSoundSetup=pInternalSoundSetup;
    file_sampleswritten=0;
    resetSample(false);
    pData=nullptr;
}

MFSFXRSoundData::~MFSFXRSoundData() {
}

bool MFSFXRSoundData::createSoundData(MFStructurableData* pSoundData){
  bool ret=true;
  if(pData!=nullptr)
    free(pData);
  pData=nullptr;
  pSoundData->clear();

  resetSample(false);

  uint32_t size=calculateBufferSize();
  std::string soundInfo="creating sfxr sound data! "+getSoundInfo()+"\nsize="+std::to_string(size);
  P_INF(soundInfo);
  pData=std::malloc(size);
  if(pData==nullptr){
    P_ERR("failed to allocate data!");
    ret=false;
    return ret;
  }

  memset(pData, 0,size);
  pSoundData->addNBitValue(size*8, pData);
  float* buffer=static_cast<float*>(pData);
  playing_sample=true;
  synthesizeSample(size, buffer, NULL);
  return ret;
}

void MFSFXRSoundData::resetSample(bool restart){
  if(!restart)
    phase=0;
  fperiod=100.0/(pSoundSetup->p_base_freq*pSoundSetup->p_base_freq+0.001);
  period=(int)fperiod;
  fmaxperiod=100.0/(pSoundSetup->p_freq_limit*pSoundSetup->p_freq_limit+0.001);
  fslide=1.0-pow((double)pSoundSetup->p_freq_ramp, 3.0)*0.01;
  fdslide=-pow((double)pSoundSetup->p_freq_dramp, 3.0)*0.000001;
  square_duty=0.5f-pSoundSetup->p_duty*0.5f;
  square_slide=-pSoundSetup->p_duty_ramp*0.00005f;
  if(pSoundSetup->p_arp_mod>=0.0f)
    arp_mod=1.0-pow((double)pSoundSetup->p_arp_mod, 2.0)*0.9;
  else
    arp_mod=1.0+pow((double)pSoundSetup->p_arp_mod, 2.0)*10.0;
  arp_time=0;
  arp_limit=(int)(pow(1.0f-pSoundSetup->p_arp_speed, 2.0f)*20000+32);
  if(pSoundSetup->p_arp_speed==1.0f)
    arp_limit=0;
  if(!restart)
  {
    // reset filter
    fltp=0.0f;
    fltdp=0.0f;
    fltw=pow(pSoundSetup->p_lpf_freq, 3.0f)*0.1f;
    fltw_d=1.0f+pSoundSetup->p_lpf_ramp*0.0001f;
    fltdmp=5.0f/(1.0f+pow(pSoundSetup->p_lpf_resonance, 2.0f)*20.0f)*(0.01f+fltw);
    if(fltdmp>0.8f) fltdmp=0.8f;
    fltphp=0.0f;
    flthp=pow(pSoundSetup->p_hpf_freq, 2.0f)*0.1f;
    flthp_d=1.0+pSoundSetup->p_hpf_ramp*0.0003f;
    // reset vibrato
    vib_phase=0.0f;
    vib_speed=pow(pSoundSetup->p_vib_speed, 2.0f)*0.01f;
    vib_amp=pSoundSetup->p_vib_strength*0.5f;
    // reset envelope
    env_vol=0.0f;
    env_stage=0;
    env_time=0;

    env_length[0]=pSoundSetup->calculateAttackSampleCount();
    env_length[1]=pSoundSetup->calculateSustainSampleCount();
    env_length[2]=pSoundSetup->calculateDecaySampleCount();

    fphase=pow(pSoundSetup->p_pha_offset, 2.0f)*1020.0f;
    if(pSoundSetup->p_pha_offset<0.0f) fphase=-fphase;
    fdphase=pow(pSoundSetup->p_pha_ramp, 2.0f)*1.0f;
    if(pSoundSetup->p_pha_ramp<0.0f) fdphase=-fdphase;
    iphase=abs((int)fphase);
    ipp=0;
    for(int i=0;i<1024;i++)
      phaser_buffer[i]=0.0f;

    createNoiseData();

    rep_time=0;
    rep_limit=(int)(pow(1.0f-pSoundSetup->p_repeat_speed, 2.0f)*20000+32);
    if(pSoundSetup->p_repeat_speed==0.0f)
      rep_limit=0;
  }
}

bool MFSFXRSoundData::exportWAV(const char* filename){
  FILE* foutput=fopen(filename, "wb");
  if(!foutput)
    return false;
  // write wav header
  unsigned int dword=0;
  unsigned short word=0;
  fwrite("RIFF", 4, 1, foutput); // "RIFF"
  dword=0;
  fwrite(&dword, 1, 4, foutput); // remaining file size
  fwrite("WAVE", 4, 1, foutput); // "WAVE"

  fwrite("fmt ", 4, 1, foutput); // "fmt "
  dword=16;
  fwrite(&dword, 1, 4, foutput); // chunk size
  word=1;
  fwrite(&word, 1, 2, foutput); // compression code
  word=1;
  fwrite(&word, 1, 2, foutput); // channels
  dword=wav_freq;
  fwrite(&dword, 1, 4, foutput); // sample rate
  dword=wav_freq*wav_bits/8;
  fwrite(&dword, 1, 4, foutput); // bytes/sec
  word=wav_bits/8;
  fwrite(&word, 1, 2, foutput); // block align
  word=wav_bits;
  fwrite(&word, 1, 2, foutput); // bits per sample

  fwrite("data", 4, 1, foutput); // "data"
  dword=0;
  int foutstream_datasize=ftell(foutput);
  fwrite(&dword, 1, 4, foutput); // chunk size

  // write sample data
//  mute_stream=true;
  file_sampleswritten=0;
  filesample=0.0f;
  fileacc=0;
  resetSample(false);
  playing_sample=true;
  while(playing_sample)
    synthesizeSample(256, NULL, foutput);
//  mute_stream=false;

  // seek back to header and write size info
  fseek(foutput, 4, SEEK_SET);
  dword=0;
  dword=foutstream_datasize-4+file_sampleswritten*wav_bits/8;
  fwrite(&dword, 1, 4, foutput); // remaining file size
  fseek(foutput, foutstream_datasize, SEEK_SET);
  dword=file_sampleswritten*wav_bits/8;
  fwrite(&dword, 1, 4, foutput); // chunk size (data)
  fclose(foutput);

  return true;
}

void MFSFXRSoundData::createNoiseData(){
  for(int i=0;i<32;i++)
    noise_buffer[i]=frnd(2.0f)-1.0f;
}

float MFSFXRSoundData::calculateSample(){
  float ssample=0.0f;
  for(int si=0;si<8;si++) // 8x supersampling
  {
    float sample=0.0f;
    phase++;
    if(phase>=period)
    {
      phase%=period;
      if(pSoundSetup->wave_type==E_WaveType::wave_type_noise)
        createNoiseData();
    }

    // base waveform
    float fp=(float)phase/period;
    switch(pSoundSetup->wave_type)
    {
    case E_WaveType::wave_type_square: // square
      if(fp<square_duty)
        sample=0.5f;
      else
        sample=-0.5f;
      break;
    case E_WaveType::wave_type_sawtooth: // sawtooth
      sample=1.0f-fp*2;
      break;
    case E_WaveType::wave_type_sine: // sine
      sample=(float)sin(fp*2*M_PI);
      break;
    case E_WaveType::wave_type_noise: // noise
      sample=noise_buffer[phase*32/period];
      break;
    }
    // lp filter
    float pp=fltp;
    fltw*=fltw_d;
    if(fltw<0.0f) fltw=0.0f;
    if(fltw>0.1f) fltw=0.1f;
    if(pSoundSetup->p_lpf_freq!=1.0f)
    {
      fltdp+=(sample-fltp)*fltw;
      fltdp-=fltdp*fltdmp;
    }
    else
    {
      fltp=sample;
      fltdp=0.0f;
    }
    fltp+=fltdp;

    // hp filter
    fltphp+=fltp-pp;
    fltphp-=fltphp*flthp;
    sample=fltphp;

    // phaser - ipp starts with 0 and goes up to limit_of((ipp+1)&1023)=1023 if reached
    phaser_buffer[ipp&1023]=sample;
    //iphase starts with |fphase|
    sample+=phaser_buffer[(ipp-iphase+1024)&1023];
    ipp=(ipp+1)&1023;

    // final accumulation and envelope application
    ssample+=sample*env_vol;
  }
  ssample=ssample/8*(pSoundSetup->master_vol);

  ssample*= (2.0f*(pSoundSetup->sound_vol));
  return ssample;
}

void MFSFXRSoundData::synthesizeSample(int byteLength, float* buffer, FILE* file){
  for(int i=0;i<byteLength;i+=4)
  {
    rep_time++;
    if(rep_limit!=0 && rep_time>=rep_limit){
      rep_time=0;
      resetSample(true);
    }

    // frequency envelopes/arpeggios
    arp_time++;
    if(arp_limit!=0 && arp_time>=arp_limit)
    {
      arp_limit=0;
      fperiod*=arp_mod;
    }
    fslide+=fdslide;
    fperiod*=fslide;
    if(fperiod>fmaxperiod)
    {
      fperiod=fmaxperiod;
      if(pSoundSetup->p_freq_limit>0.0f)
        playing_sample=false;
    }
    float rfperiod=fperiod;
    if(vib_amp>0.0f)
    {
      vib_phase+=vib_speed;
      rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
    }
    period=(int)rfperiod;
    if(period<8) period=8;
    square_duty+=square_slide;
    if(square_duty<0.0f) square_duty=0.0f;
    if(square_duty>0.5f) square_duty=0.5f;

    updateEnvelopment();

    // phaser step
    fphase+=fdphase;
    iphase=abs((int)fphase);
    if(iphase>1023) iphase=1023;

    if(flthp_d!=0.0f)
    {
      flthp*=flthp_d;
      if(flthp<0.00001f) flthp=0.00001f;
      if(flthp>0.1f) flthp=0.1f;
    }

    float ssample=0;
    ssample=calculateSample();

    writeBuffer(buffer,ssample);
    buffer++;
    writeFile(file,ssample);
  }
}

void MFSFXRSoundData::updateEnvelopment(){
  // volume envelope
  if(env_stage>3)
    env_stage=0;
  env_time++;
  if(env_time>env_length[env_stage]){//sample count=sum of all vals within env_length[]
    env_time=0;
    env_stage++;
    if(env_stage==3)
      playing_sample=false;
  }
  if(env_stage==0)//attack
    env_vol=(float)env_time/env_length[0];//]0-1]
  if(env_stage==1){//sustain
    float base=1.0f-(float)env_time/env_length[1];
    env_vol=1.0f+pow(base, 1.0f) * 2.0f * pSoundSetup->p_env_punch;
  }
  if(env_stage==2)//decay
    env_vol=1.0f-(float)env_time/env_length[2];
}

uint32_t MFSFXRSoundData::calculateBufferSize(){
  uint32_t size=env_length[0];
  size+=env_length[1];
  size+=env_length[2];
  return size;
}

void MFSFXRSoundData::writeBuffer(float* buffer, float &ssample){
  if(buffer!=NULL)
  {
    if(ssample>1.0f) ssample=1.0f;
    if(ssample<-1.0f) ssample=-1.0f;
    (*buffer)+=ssample;
  }
}
std::string MFSFXRSoundData::getSoundInfo(){
  std::string info="sfxr-sound info:\n";
  std::string base="";
  switch(pSoundSetup->baseSetup){
  case E_SoundBaseSetup::blip:
    base="blip";
    break;
  case E_SoundBaseSetup::explosion:
    base="explosion";
    break;
  case E_SoundBaseSetup::hit:
    base="hit";
    break;
  case E_SoundBaseSetup::jump:
    base="jump";
    break;
  case E_SoundBaseSetup::laser:
    base="laser";
    break;
  case E_SoundBaseSetup::pickup:
    base="pickup";
    break;
  case E_SoundBaseSetup::powerup:
    base="powerup";
    break;
  default:
    base="unknown";
    break;
  }

  std::string wave="";
  switch(pSoundSetup->wave_type){
  case E_WaveType::wave_type_noise:
    wave="noise";
    break;
  case E_WaveType::wave_type_sawtooth:
    wave="sawtooth";
    break;
  case E_WaveType::wave_type_sine:
    wave="sine";
    break;
  case E_WaveType::wave_type_square:
    wave="square";
    break;
  default:
    wave="unknown";
    break;
  }
  std::string size=std::to_string((calculateBufferSize()/1000));
  info+="Sound base: "+base+"\n";
  info+="Wave type: "+wave+"\n";
  info+="Data Size: "+size+" kByte\n";
  //TODO duration in millis / attack sustain decay
  info+="attack samples: "+std::to_string(env_length[0])+"\n";
  info+="sustain samples: "+std::to_string(env_length[1])+"\n";
  info+="decay samples: "+std::to_string(env_length[2])+"\n";
  return info;
}

void MFSFXRSoundData::writeFile(std::FILE *file, float &ssample){
  if(file!=NULL)
  {
    // quantize depending on format
    // accumulate/count to accomodate variable sample rate?
    ssample*=4.0f; // arbitrary gain to get reasonable output volume...
    if(ssample>1.0f) ssample=1.0f;
    if(ssample<-1.0f) ssample=-1.0f;
    filesample+=ssample;
    fileacc++;
    if(wav_freq==44100 || fileacc==2)
    {
      filesample/=fileacc;
      fileacc=0;
      if(wav_bits==16)
      {
        short isample=(short)(filesample*32000);
        fwrite(&isample, 1, 2, file);
      }
      else
      {
        unsigned char isample=(unsigned char)(filesample*127+128);
        fwrite(&isample, 1, 1, file);
      }
      filesample=0.0f;
    }
    file_sampleswritten++;
  }
}
