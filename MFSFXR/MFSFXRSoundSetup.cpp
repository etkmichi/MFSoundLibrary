/*
 * MFSFXRSoundSetup.cpp
 *
 *  Created on: 07.01.2021
 *      Author: michl
 */

#include "../MFSFXR/MFSFXRSoundSetup.h"
#include <MFBasicDefines.h>

#define PI 3.14159265f

MFSFXRSoundSetup::MFSFXRSoundSetup(E_SoundBaseSetup baseSetup) {
  setAttackScale(100.0f*4096.0f*2);//TODO get from SDL_SoundSpecs
  setSustainScale(100.0f*4096.0f*2);
  setDecayScale(100.0f*4096*2);
  resetParams();
  setupBaseParameters(baseSetup);
}

MFSFXRSoundSetup::~MFSFXRSoundSetup() {
  // TODO Auto-generated destructor stub
}

void MFSFXRSoundSetup::setupBaseParameters(E_SoundBaseSetup baseSetup){
  this->baseSetup=baseSetup;
  resetParams();
  switch(baseSetup){
  case E_SoundBaseSetup::pickup: // pickup/coin
    setupPickupParams();
    break;
  case E_SoundBaseSetup::laser: // laser/shoot
    setupLaserParams();
    break;
  case E_SoundBaseSetup::explosion: // explosion
    setupExplosionParams();
    break;
  case E_SoundBaseSetup::powerup: // powerup
    setupPowerupParams();
    break;
  case E_SoundBaseSetup::hit: // hit/hurt
    setupHitParams();
    break;
  case E_SoundBaseSetup::jump: // jump
    setupJumpParams();
    break;
  case E_SoundBaseSetup::blip: // blip/select
    setupBlipParams();
    break;
  default:
    break;
  }
}

void MFSFXRSoundSetup::resetParams(){
  wave_type=E_WaveType::wave_type_square;

  p_base_freq=0.3f;
  p_freq_limit=0.0f;
  p_freq_ramp=0.0f;
  p_freq_dramp=0.0f;
  p_duty=0.0f;
  p_duty_ramp=0.0f;

  p_vib_strength=0.0f;
  p_vib_speed=0.0f;
  p_vib_delay=0.0f;

  p_env_attack=0.0f;
  p_env_sustain=0.3f;
  p_env_decay=0.4f;
  p_env_punch=0.0f;

  filter_on=false;
  p_lpf_resonance=0.0f;
  p_lpf_freq=1.0f;
  p_lpf_ramp=0.0f;
  p_hpf_freq=0.0f;
  p_hpf_ramp=0.0f;

  p_pha_offset=0.0f;
  p_pha_ramp=0.0f;

  p_repeat_speed=0.0f;

  p_arp_speed=0.0f;
  p_arp_mod=0.0f;
}

void MFSFXRSoundSetup::randomizeParams(){
//
  p_base_freq=pow(frnd(2.0f)-1.0f, 2.0f);
  if(rnd(1))
    p_base_freq=pow(frnd(2.0f)-1.0f, 3.0f)+0.5f;
  p_freq_limit=0.0f;
  p_freq_ramp=pow(frnd(2.0f)-1.0f, 5.0f);
  if(p_base_freq>0.7f && p_freq_ramp>0.2f)
    p_freq_ramp=-p_freq_ramp;
  if(p_base_freq<0.2f && p_freq_ramp<-0.05f)
    p_freq_ramp=-p_freq_ramp;
  p_freq_dramp=pow(frnd(2.0f)-1.0f, 3.0f);
  p_duty=frnd(2.0f)-1.0f;
  p_duty_ramp=pow(frnd(2.0f)-1.0f, 3.0f);
  p_vib_strength=pow(frnd(2.0f)-1.0f, 3.0f);
  p_vib_speed=frnd(2.0f)-1.0f;
  p_vib_delay=frnd(2.0f)-1.0f;
  p_env_attack=pow(frnd(2.0f)-1.0f, 3.0f);
  p_env_sustain=pow(frnd(2.0f)-1.0f, 2.0f);
  p_env_decay=frnd(2.0f)-1.0f;
  p_env_punch=pow(frnd(0.8f), 2.0f);
  if(p_env_attack+p_env_sustain+p_env_decay<0.2f)
  {
    p_env_sustain+=0.2f+frnd(0.3f);
    p_env_decay+=0.2f+frnd(0.3f);
  }
  p_lpf_resonance=frnd(2.0f)-1.0f;
  p_lpf_freq=1.0f-pow(frnd(1.0f), 3.0f);
  p_lpf_ramp=pow(frnd(2.0f)-1.0f, 3.0f);
  if(p_lpf_freq<0.1f && p_lpf_ramp<-0.05f)
    p_lpf_ramp=-p_lpf_ramp;
  p_hpf_freq=pow(frnd(1.0f), 5.0f);
  p_hpf_ramp=pow(frnd(2.0f)-1.0f, 5.0f);
  p_pha_offset=pow(frnd(2.0f)-1.0f, 3.0f);
  p_pha_ramp=pow(frnd(2.0f)-1.0f, 3.0f);
  p_repeat_speed=frnd(2.0f)-1.0f;
  p_arp_speed=frnd(2.0f)-1.0f;
  p_arp_mod=frnd(2.0f)-1.0f;

//  setupBaseParameters(baseSetup);
}

bool MFSFXRSoundSetup::saveSetup(const std::string &filename){
  FILE* file=fopen(filename.data(), "wb");
  if(!file){
    MFObject::printErr("MFSFXRSoundSetup::loadSetup - failed to open file! file: "+
        filename);
    return false;
  }

  int version=102;
  fwrite(&version, 1, sizeof(int), file);

  fwrite(&wave_type, 1, sizeof(int), file);

  fwrite(&sound_vol, 1, sizeof(float), file);

  fwrite(&p_base_freq, 1, sizeof(float), file);
  fwrite(&p_freq_limit, 1, sizeof(float), file);
  fwrite(&p_freq_ramp, 1, sizeof(float), file);
  fwrite(&p_freq_dramp, 1, sizeof(float), file);
  fwrite(&p_duty, 1, sizeof(float), file);
  fwrite(&p_duty_ramp, 1, sizeof(float), file);

  fwrite(&p_vib_strength, 1, sizeof(float), file);
  fwrite(&p_vib_speed, 1, sizeof(float), file);
  fwrite(&p_vib_delay, 1, sizeof(float), file);

  fwrite(&p_env_attack, 1, sizeof(float), file);
  fwrite(&p_env_sustain, 1, sizeof(float), file);
  fwrite(&p_env_decay, 1, sizeof(float), file);
  fwrite(&p_env_punch, 1, sizeof(float), file);

  fwrite(&filter_on, 1, sizeof(bool), file);
  fwrite(&p_lpf_resonance, 1, sizeof(float), file);
  fwrite(&p_lpf_freq, 1, sizeof(float), file);
  fwrite(&p_lpf_ramp, 1, sizeof(float), file);
  fwrite(&p_hpf_freq, 1, sizeof(float), file);
  fwrite(&p_hpf_ramp, 1, sizeof(float), file);

  fwrite(&p_pha_offset, 1, sizeof(float), file);
  fwrite(&p_pha_ramp, 1, sizeof(float), file);

  fwrite(&p_repeat_speed, 1, sizeof(float), file);

  fwrite(&p_arp_speed, 1, sizeof(float), file);
  fwrite(&p_arp_mod, 1, sizeof(float), file);

  fclose(file);
  return true;

}

bool MFSFXRSoundSetup::loadSetup(const std::string &filename){
  FILE* file=fopen(filename.data(), "rb");
  if(!file){
    MFObject::printErr("MFSFXRSoundSetup::loadSetup - failed to open file! file: "+
        filename);
    return false;
  }

  size_t n;
  (void)n;
  int version=0;
  n = fread(&version, 1, sizeof(int), file);
  if(version!=100 && version!=101 && version!=102){
    MFObject::printErr("MFSFXRSoundSetup::loadSetup - version mismatch! "
        "valid versions (100,101,102) current vesion:"+std::to_string(version)+ "\n file: "+
        filename);
    return false;
  }

  n = fread(&wave_type, 1, sizeof(int), file);

  sound_vol=0.5f;
  if(version==102)
    n = fread(&sound_vol, 1, sizeof(float), file);

  n = fread(&p_base_freq, 1, sizeof(float), file);
  n = fread(&p_freq_limit, 1, sizeof(float), file);
  n = fread(&p_freq_ramp, 1, sizeof(float), file);
  if(version>=101)
    n = fread(&p_freq_dramp, 1, sizeof(float), file);
  n = fread(&p_duty, 1, sizeof(float), file);
  n = fread(&p_duty_ramp, 1, sizeof(float), file);

  n = fread(&p_vib_strength, 1, sizeof(float), file);
  n = fread(&p_vib_speed, 1, sizeof(float), file);
  n = fread(&p_vib_delay, 1, sizeof(float), file);

  n = fread(&p_env_attack, 1, sizeof(float), file);
  n = fread(&p_env_sustain, 1, sizeof(float), file);
  n = fread(&p_env_decay, 1, sizeof(float), file);
  n = fread(&p_env_punch, 1, sizeof(float), file);

  n = fread(&filter_on, 1, sizeof(bool), file);
  n = fread(&p_lpf_resonance, 1, sizeof(float), file);
  n = fread(&p_lpf_freq, 1, sizeof(float), file);
  n = fread(&p_lpf_ramp, 1, sizeof(float), file);
  n = fread(&p_hpf_freq, 1, sizeof(float), file);
  n = fread(&p_hpf_ramp, 1, sizeof(float), file);

  n = fread(&p_pha_offset, 1, sizeof(float), file);
  n = fread(&p_pha_ramp, 1, sizeof(float), file);

  n = fread(&p_repeat_speed, 1, sizeof(float), file);

  if(version>=101)
  {
    n = fread(&p_arp_speed, 1, sizeof(float), file);
    n = fread(&p_arp_mod, 1, sizeof(float), file);
  }

  fclose(file);
  return true;
}

void MFSFXRSoundSetup::setupPickupParams(){
  P_INF();
  p_base_freq=0.4f+frnd(0.5f);
  p_env_attack=0.0f;
  p_env_sustain=frnd(0.1f);
  p_env_decay=0.1f+frnd(0.4f);
  p_env_punch=0.3f+frnd(0.3f);
  if(rnd(1))
  {
    p_arp_speed=0.5f+frnd(0.2f);
    p_arp_mod=0.2f+frnd(0.4f);
  }
}

void MFSFXRSoundSetup::setupLaserParams(){
  P_INF();
  wave_type=E_WaveType(rnd(2));
  if(wave_type==E_WaveType::wave_type_sine && rnd(1))
    wave_type=E_WaveType(rnd(1));
  p_base_freq=0.5f+frnd(0.5f);
  p_freq_limit=p_base_freq-0.2f-frnd(0.6f);
  if(p_freq_limit<0.2f) p_freq_limit=0.2f;
  p_freq_ramp=-0.15f-frnd(0.2f);
  if(rnd(2)==0)
  {
    p_base_freq=0.3f+frnd(0.6f);
    p_freq_limit=frnd(0.1f);
    p_freq_ramp=-0.35f-frnd(0.3f);
  }
  if(rnd(1))
  {
    p_duty=frnd(0.5f);
    p_duty_ramp=frnd(0.2f);
  }
  else
  {
    p_duty=0.4f+frnd(0.5f);
    p_duty_ramp=-frnd(0.7f);
  }
  p_env_attack=0.0f;
  p_env_sustain=0.1f+frnd(0.2f);
  p_env_decay=frnd(0.4f);
  if(rnd(1))
    p_env_punch=frnd(0.3f);
  if(rnd(2)==0)
  {
    p_pha_offset=frnd(0.2f);
    p_pha_ramp=-frnd(0.2f);
  }
  if(rnd(1))
    p_hpf_freq=frnd(0.3f);
}

void MFSFXRSoundSetup::setupExplosionParams(){
    P_INF();
  wave_type=E_WaveType::wave_type_noise;
  if(rnd(1))
  {
    p_base_freq=0.1f+frnd(0.4f);
    p_freq_ramp=-0.1f+frnd(0.4f);
  }
  else
  {
    p_base_freq=0.2f+frnd(0.7f);
    p_freq_ramp=-0.2f-frnd(0.2f);
  }
  p_base_freq*=p_base_freq;
  if(rnd(4)==0)
    p_freq_ramp=0.0f;
  if(rnd(2)==0)
    p_repeat_speed=0.3f+frnd(0.5f);
  p_env_attack=0.0f;
  p_env_sustain=0.1f+frnd(0.3f);
  p_env_decay=frnd(0.5f);
  if(rnd(1)==0)
  {
    p_pha_offset=-0.3f+frnd(0.9f);
    p_pha_ramp=-frnd(0.3f);
  }
  p_env_punch=0.2f+frnd(0.6f);
  if(rnd(1))
  {
    p_vib_strength=frnd(0.7f);
    p_vib_speed=frnd(0.6f);
  }
  if(rnd(2)==0)
  {
    p_arp_speed=0.6f+frnd(0.3f);
    p_arp_mod=0.8f-frnd(1.6f);
  }
}

void MFSFXRSoundSetup::setupPowerupParams(){
    P_INF();
  if(rnd(1))
    wave_type=E_WaveType(rnd(1));
  else
    p_duty=frnd(0.6f);
  if(rnd(1))
  {
    p_base_freq=0.2f+frnd(0.3f);
    p_freq_ramp=0.1f+frnd(0.4f);
    p_repeat_speed=0.4f+frnd(0.4f);
  }
  else
  {
    p_base_freq=0.2f+frnd(0.3f);
    p_freq_ramp=0.05f+frnd(0.2f);
    if(rnd(1))
    {
      p_vib_strength=frnd(0.7f);
      p_vib_speed=frnd(0.6f);
    }
  }
  p_env_attack=0.0f;
  p_env_sustain=frnd(0.4f);
  p_env_decay=0.1f+frnd(0.4f);
}

void MFSFXRSoundSetup::setupHitParams(){
    P_INF();
  wave_type=E_WaveType(rnd(2));//sq | st | sin
  if(wave_type==E_WaveType::wave_type_sine)
    wave_type=E_WaveType::wave_type_noise;
  if(wave_type==0)
    p_duty=frnd(0.6f);
  p_base_freq=0.2f+frnd(0.6f);
  p_freq_ramp=-0.3f-frnd(0.4f);
  p_env_attack=0.0f;
  p_env_sustain=frnd(0.1f);
  p_env_decay=0.1f+frnd(0.2f);
  if(rnd(1))
    p_hpf_freq=frnd(0.3f);
}

void MFSFXRSoundSetup::setupJumpParams(){
    P_INF();
  wave_type=E_WaveType::wave_type_square;
  p_duty=frnd(0.6f);
  p_base_freq=0.3f+frnd(0.3f);
  p_freq_ramp=0.1f+frnd(0.2f);
  p_env_attack=0.0f;
  p_env_sustain=0.1f+frnd(0.3f);
  p_env_decay=0.1f+frnd(0.2f);
  if(rnd(1))
    p_hpf_freq=frnd(0.3f);
  if(rnd(1))
    p_lpf_freq=1.0f-frnd(0.6f);
}

uint32_t MFSFXRSoundSetup::calculateAttackSampleCount(){
  P_INF(std::to_string((int)(p_env_attack*p_env_attack*attackScale)));
  P_INF(std::to_string((uint32_t)(p_env_attack*p_env_attack*attackScale)));
  return (uint32_t)(p_env_attack*p_env_attack*attackScale);
}

uint32_t MFSFXRSoundSetup::calculateSustainSampleCount(){
  P_INF(std::to_string((int)(p_env_sustain*p_env_sustain*attackScale)));
  P_INF(std::to_string((uint32_t)(p_env_sustain*p_env_sustain*attackScale)));
  return (uint32_t)(p_env_sustain*p_env_sustain*sustainScale);
}

uint32_t MFSFXRSoundSetup::calculateDecaySampleCount(){
  P_INF(std::to_string((int)(p_env_decay*p_env_decay*attackScale)));
  P_INF(std::to_string((uint32_t)(p_env_decay*p_env_decay*attackScale)));
  return (uint32_t)(p_env_decay*p_env_decay*decayScale);
}

void MFSFXRSoundSetup::setupBlipParams(){
    P_INF();
  wave_type=E_WaveType(rnd(1));
  if(wave_type==0)
    p_duty=frnd(0.6f);
  p_base_freq=0.2f+frnd(0.4f);
  p_env_attack=0.0f;
  p_env_sustain=0.1f+frnd(0.1f);
  p_env_decay=frnd(0.2f);
  p_hpf_freq=0.1f;
}

uint32_t MFSFXRSoundSetup::calculateBufferSize(){
  uint32_t attackTime=calculateAttackSampleCount();
  uint32_t sustainTime=calculateSustainSampleCount();
  uint32_t decayTime=calculateDecaySampleCount();
  uint32_t size=attackTime+sustainTime+decayTime;
  return size;
}
