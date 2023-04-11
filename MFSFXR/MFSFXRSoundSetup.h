/*
 * MFSFXRSoundSetup.h
 *
 *  Created on: 07.01.2021
 *      Author: michl
 */
/*
   Copyright (c) 2007

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

*/

#ifndef MFSFXR_MFSFXRSOUNDSETUP_H_
#define MFSFXR_MFSFXRSOUNDSETUP_H_

class MFSFXRSoundSetup;

#include <MFObjects/MFObject.h>
#include "../MFSFXR/MFSFXRSoundPlayback.h"
#include "../MFSFXR/MFSFXRHelpers.h"

/**
 * Copy of sound parameters for sound creation (SFXR main.cpp).
 */
class MFSFXRSoundSetup {
public:/*variables of MFSFXRSoundSetup*/
  E_WaveType wave_type;

  /*setup parameters*/
  float p_base_freq;
  float p_freq_limit;
  float p_freq_ramp;
  float p_freq_dramp;
  float p_duty;
  float p_duty_ramp;

  float p_vib_strength;
  float p_vib_speed;
  float p_vib_delay;

  float p_env_attack;
  float p_env_sustain;
  float p_env_decay;
  float p_env_punch;

  bool filter_on;
  float p_lpf_resonance;
  float p_lpf_freq;
  float p_lpf_ramp;
  float p_hpf_freq;
  float p_hpf_ramp;

  float p_pha_offset;
  float p_pha_ramp;

  float p_repeat_speed;

  float p_arp_speed;
  float p_arp_mod;

  float master_vol=0.05f;

  float sound_vol=0.5f;

  E_SoundBaseSetup
  baseSetup;

  float
  attackScale=100000.0f,
  sustainScale=100000.0f,
  decayScale=100000.0f;
public:
  /**
   * Creates a sfxr sound. The resulting sound is close to Dr. Pertterson's sfxr sound creation
   * tool.
   * @param baseSetup
   */
  MFSFXRSoundSetup(E_SoundBaseSetup baseSetup=E_SoundBaseSetup::pickup);
  virtual ~MFSFXRSoundSetup();

  /**
   * Sets the base parameters to sound like the given E_SoundBaseSetup baseSetup.
   * @param baseSetup - enumeration of base parameter setups.
   */
  void setupBaseParameters(E_SoundBaseSetup baseSetup);

  void setBaseSetup(E_SoundBaseSetup base){
    baseSetup=base;
  }

  uint32_t calculateAttackSampleCount();

  uint32_t calculateSustainSampleCount();

  uint32_t calculateDecaySampleCount();

  /**
   *
   */
  void resetParams();
  void randomizeParams();
  bool saveSetup(const std::string &filename);
  bool loadSetup(const std::string &filename);
  void synthesizeSampe(int length, float* buffer, FILE* file);
  uint32_t calculateBufferSize();float getAttackScale() const {
    return attackScale;
  }

  void setAttackScale(float attackScale = 400000.0f) {
    this->attackScale = attackScale;
  }

  float getDecayScale() const {
    return decayScale;
  }

  void setDecayScale(float decayScale = 400000.0f) {
    this->decayScale = decayScale;
  }

  float getSustainScale() const {
    return sustainScale;
  }

  void setSustainScale(float sustainScale = 400000.0f) {
    this->sustainScale = sustainScale;
  }

private:
  void setupPickupParams();
  void setupLaserParams();
  void setupExplosionParams();
  void setupPowerupParams();
  void setupHitParams();
  void setupJumpParams();
  void setupBlipParams();
};

#endif /* MFSFXR_MFSFXRSOUNDSETUP_H_ */
