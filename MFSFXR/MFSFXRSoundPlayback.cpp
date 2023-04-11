///*
// * MFSFXRSoundPlayback.cpp
// *
// *  Created on: 13.01.2021
// *      Author: michl
// */
//
//#include "../MFSFXR/MFSFXRSoundPlayback.h"
//
//MFSFXRSoundPlayback::MFSFXRSoundPlayback(MFSFXRSoundSetup* pSetup) {
//  this->pSoundSetup=pSetup;
//  file_sampleswritten=0;
//  resetSample(true);
//}
//
//MFSFXRSoundPlayback::~MFSFXRSoundPlayback() {
//  // TODO Auto-generated destructor stub
//}
//
//void MFSFXRSoundPlayback::resetSample(bool restart){
//  if(!restart)
//    phase=0;
//  fperiod=100.0/(pSoundSetup->p_base_freq*pSoundSetup->p_base_freq+0.001);
//  period=(int)fperiod;
//  fmaxperiod=100.0/(pSoundSetup->p_freq_limit*pSoundSetup->p_freq_limit+0.001);
//  fslide=1.0-pow((double)pSoundSetup->p_freq_ramp, 3.0)*0.01;
//  fdslide=-pow((double)pSoundSetup->p_freq_dramp, 3.0)*0.000001;
//  square_duty=0.5f-pSoundSetup->p_duty*0.5f;
//  square_slide=-pSoundSetup->p_duty_ramp*0.00005f;
//  if(pSoundSetup->p_arp_mod>=0.0f)
//    arp_mod=1.0-pow((double)pSoundSetup->p_arp_mod, 2.0)*0.9;
//  else
//    arp_mod=1.0+pow((double)pSoundSetup->p_arp_mod, 2.0)*10.0;
//  arp_time=0;
//  arp_limit=(int)(pow(1.0f-pSoundSetup->p_arp_speed, 2.0f)*20000+32);
//  if(pSoundSetup->p_arp_speed==1.0f)
//    arp_limit=0;
//  if(!restart)
//  {
//    // reset filter
//    fltp=0.0f;
//    fltdp=0.0f;
//    fltw=pow(pSoundSetup->p_lpf_freq, 3.0f)*0.1f;
//    fltw_d=1.0f+pSoundSetup->p_lpf_ramp*0.0001f;
//    fltdmp=5.0f/(1.0f+pow(pSoundSetup->p_lpf_resonance, 2.0f)*20.0f)*(0.01f+fltw);
//    if(fltdmp>0.8f) fltdmp=0.8f;
//    fltphp=0.0f;
//    flthp=pow(pSoundSetup->p_hpf_freq, 2.0f)*0.1f;
//    flthp_d=1.0+pSoundSetup->p_hpf_ramp*0.0003f;
//    // reset vibrato
//    vib_phase=0.0f;
//    vib_speed=pow(pSoundSetup->p_vib_speed, 2.0f)*0.01f;
//    vib_amp=pSoundSetup->p_vib_strength*0.5f;
//    // reset envelope
//    env_vol=0.0f;
//    env_stage=0;
//    env_time=0;
//    env_length[0]=(int)(pSoundSetup->p_env_attack*pSoundSetup->p_env_attack*100000.0f);
//    env_length[1]=(int)(pSoundSetup->p_env_sustain*pSoundSetup->p_env_sustain*100000.0f);
//    env_length[2]=(int)(pSoundSetup->p_env_decay*pSoundSetup->p_env_decay*100000.0f);
//
//    fphase=pow(pSoundSetup->p_pha_offset, 2.0f)*1020.0f;
//    if(pSoundSetup->p_pha_offset<0.0f) fphase=-fphase;
//    fdphase=pow(pSoundSetup->p_pha_ramp, 2.0f)*1.0f;
//    if(pSoundSetup->p_pha_ramp<0.0f) fdphase=-fdphase;
//    iphase=abs((int)fphase);
//    ipp=0;
//    for(int i=0;i<1024;i++)
//      phaser_buffer[i]=0.0f;
//
//    for(int i=0;i<32;i++)
//      noise_buffer[i]=frnd(2.0f)-1.0f;
//
//    rep_time=0;
//    rep_limit=(int)(pow(1.0f-pSoundSetup->p_repeat_speed, 2.0f)*20000+32);
//    if(pSoundSetup->p_repeat_speed==0.0f)
//      rep_limit=0;
//  }
//}
//
//bool MFSFXRSoundPlayback::exportWAV(const char* filename){
//  FILE* foutput=fopen(filename, "wb");
//  if(!foutput)
//    return false;
//  // write wav header
//  unsigned int dword=0;
//  unsigned short word=0;
//  fwrite("RIFF", 4, 1, foutput); // "RIFF"
//  dword=0;
//  fwrite(&dword, 1, 4, foutput); // remaining file size
//  fwrite("WAVE", 4, 1, foutput); // "WAVE"
//
//  fwrite("fmt ", 4, 1, foutput); // "fmt "
//  dword=16;
//  fwrite(&dword, 1, 4, foutput); // chunk size
//  word=1;
//  fwrite(&word, 1, 2, foutput); // compression code
//  word=1;
//  fwrite(&word, 1, 2, foutput); // channels
//  dword=wav_freq;
//  fwrite(&dword, 1, 4, foutput); // sample rate
//  dword=wav_freq*wav_bits/8;
//  fwrite(&dword, 1, 4, foutput); // bytes/sec
//  word=wav_bits/8;
//  fwrite(&word, 1, 2, foutput); // block align
//  word=wav_bits;
//  fwrite(&word, 1, 2, foutput); // bits per sample
//
//  fwrite("data", 4, 1, foutput); // "data"
//  dword=0;
//  int foutstream_datasize=ftell(foutput);
//  fwrite(&dword, 1, 4, foutput); // chunk size
//
//  // write sample data
////  mute_stream=true;
//  file_sampleswritten=0;
//  filesample=0.0f;
//  fileacc=0;
//  resetSample(false);
//  playing_sample=true;
//  while(playing_sample)
//    synthesizeSample(256, NULL, foutput);
////  mute_stream=false;
//
//  // seek back to header and write size info
//  fseek(foutput, 4, SEEK_SET);
//  dword=0;
//  dword=foutstream_datasize-4+file_sampleswritten*wav_bits/8;
//  fwrite(&dword, 1, 4, foutput); // remaining file size
//  fseek(foutput, foutstream_datasize, SEEK_SET);
//  dword=file_sampleswritten*wav_bits/8;
//  fwrite(&dword, 1, 4, foutput); // chunk size (data)
//  fclose(foutput);
//
//  return true;
//}
//
//void MFSFXRSoundPlayback::synthesizeSample(int length, float* buffer, FILE* file){
//  for(int i=0;i<length;i++)
//  {
//    rep_time++;
//    if(rep_limit!=0 && rep_time>=rep_limit)
//    {
//      rep_time=0;
//      resetSample(true);
//    }
//
//    // frequency envelopes/arpeggios
//    arp_time++;
//    if(arp_limit!=0 && arp_time>=arp_limit)
//    {
//      arp_limit=0;
//      fperiod*=arp_mod;
//    }
//    fslide+=fdslide;
//    fperiod*=fslide;
//    if(fperiod>fmaxperiod)
//    {
//      fperiod=fmaxperiod;
//      if(pSoundSetup->p_freq_limit>0.0f)
//        playing_sample=false;
//    }
//    float rfperiod=fperiod;
//    if(vib_amp>0.0f)
//    {
//      vib_phase+=vib_speed;
//      rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
//    }
//    period=(int)rfperiod;
//    if(period<8) period=8;
//    square_duty+=square_slide;
//    if(square_duty<0.0f) square_duty=0.0f;
//    if(square_duty>0.5f) square_duty=0.5f;
//    // volume envelope
//    env_time++;
//    if(env_time>env_length[env_stage])
//    {
//      env_time=0;
//      env_stage++;
//      if(env_stage==3)
//        playing_sample=false;
//    }
//    if(env_stage==0)
//      env_vol=(float)env_time/env_length[0];
//    if(env_stage==1)
//      env_vol=1.0f+pow(1.0f-(float)env_time/env_length[1], 1.0f)*
//      2.0f*pSoundSetup->p_env_punch;
//    if(env_stage==2)
//      env_vol=1.0f-(float)env_time/env_length[2];
//
//    // phaser step
//    fphase+=fdphase;
//    iphase=abs((int)fphase);
//    if(iphase>1023) iphase=1023;
//
//    if(flthp_d!=0.0f)
//    {
//      flthp*=flthp_d;
//      if(flthp<0.00001f) flthp=0.00001f;
//      if(flthp>0.1f) flthp=0.1f;
//    }
//
//    float ssample=0.0f;
//    for(int si=0;si<8;si++) // 8x supersampling
//    {
//      float sample=0.0f;
//      phase++;
//      if(phase>=period)
//      {
////        phase=0;
//        phase%=period;
//        if(pSoundSetup->wave_type==E_WaveType::noise)
//          for(int i=0;i<32;i++)
//            noise_buffer[i]=frnd(2.0f)-1.0f;
//      }
//      // base waveform
//      float fp=(float)phase/period;
//      switch(pSoundSetup->wave_type)
//      {
//      case E_WaveType::square: // square
//        if(fp<square_duty)
//          sample=0.5f;
//        else
//          sample=-0.5f;
//        break;
//      case E_WaveType::sawtooth: // sawtooth
//        sample=1.0f-fp*2;
//        break;
//      case E_WaveType::sine: // sine
//        sample=(float)sin(fp*2*M_PI);
//        break;
//      case E_WaveType::noise: // noise
//        sample=noise_buffer[phase*32/period];
//        break;
//      }
//      // lp filter
//      float pp=fltp;
//      fltw*=fltw_d;
//      if(fltw<0.0f) fltw=0.0f;
//      if(fltw>0.1f) fltw=0.1f;
//      if(pSoundSetup->p_lpf_freq!=1.0f)
//      {
//        fltdp+=(sample-fltp)*fltw;
//        fltdp-=fltdp*fltdmp;
//      }
//      else
//      {
//        fltp=sample;
//        fltdp=0.0f;
//      }
//      fltp+=fltdp;
//      // hp filter
//      fltphp+=fltp-pp;
//      fltphp-=fltphp*flthp;
//      sample=fltphp;
//      // phaser
//      phaser_buffer[ipp&1023]=sample;
//      sample+=phaser_buffer[(ipp-iphase+1024)&1023];
//      ipp=(ipp+1)&1023;
//      // final accumulation and envelope application
//      ssample+=sample*env_vol;
//    }
//    ssample=ssample/8*(pSoundSetup->master_vol);
//
//    ssample*= (2.0f*(pSoundSetup->sound_vol));
//
//    if(buffer!=NULL)
//    {
//      if(ssample>1.0f) ssample=1.0f;
//      if(ssample<-1.0f) ssample=-1.0f;
//      *buffer++=ssample;
//    }
//    if(file!=NULL)
//    {
//      // quantize depending on format
//      // accumulate/count to accomodate variable sample rate?
//      ssample*=4.0f; // arbitrary gain to get reasonable output volume...
//      if(ssample>1.0f) ssample=1.0f;
//      if(ssample<-1.0f) ssample=-1.0f;
//      filesample+=ssample;
//      fileacc++;
//      if(wav_freq==44100 || fileacc==2)
//      {
//        filesample/=fileacc;
//        fileacc=0;
//        if(wav_bits==16)
//        {
//          short isample=(short)(filesample*32000);
//          fwrite(&isample, 1, 2, file);
//        }
//        else
//        {
//          unsigned char isample=(unsigned char)(filesample*127+128);
//          fwrite(&isample, 1, 1, file);
//        }
//        filesample=0.0f;
//      }
//      file_sampleswritten++;
//    }
//  }
//}
