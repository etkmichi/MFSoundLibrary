/*
 * MFSFXRSoundData.h
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

#ifndef MFSFXR_MFSFXRSOUNDDATA_H_
#define MFSFXR_MFSFXRSOUNDDATA_H_

#include "../MFSoundPlaybackData.h"
#include <MFBasicDefines.h>

#include "../MFSFXR/MFSFXRSoundSetup.h"
/**
 * This class stores the resulting sound which was created with MFSFXRSoundCreation.
 * Additional to MFSoundData it provides access to the sound setup of the underliing SFXR sound.
 */
class MFSFXRSoundData: public MFSoundPlaybackData {
protected:
  /*sample parameters*/
  bool playing_sample=false;
  int phase;
  double fperiod;
  double fmaxperiod;
  double fslide;
  double fdslide;
  int period;
  float square_duty;
  float square_slide;
  uint32_t env_stage;
  uint32_t env_time;
  uint32_t env_length[3];
  float env_vol;
  float fphase;
  float fdphase;
  int iphase;
  float phaser_buffer[1024];
  int ipp;
  float noise_buffer[32];
  float fltp;
  float fltdp;
  float fltw;
  float fltw_d;
  float fltdmp;
  float fltphp;
  float flthp;
  float flthp_d;
  float vib_phase;
  float vib_speed;
  float vib_amp;
  int rep_time;
  int rep_limit;
  int arp_time;
  int arp_limit;
  double arp_mod;

  int wav_bits=16;
  int wav_freq=44100;

  int file_sampleswritten;
  float filesample=0.0f;
  int fileacc=0;

private:
  MFSFXRSoundSetup
  *pInternalSoundSetup,
  *pSoundSetup;

  void
  *pData;
protected:/*virtual functions MFSoundPlaybackData*/
  /**
   * Creates a sound data object of the current setup.
   * @return
   */
  bool createSoundData(MFStructurableData* pSoundData);

  std::string getSoundInfo();

public:
  /**
   * Creates a SFXR sound object witch provides functionality to synthesize the sound data and
   * some playback extras.
   * @param pSetup - if a sound setup already exists, this value can be set. If null then the
   * sound data will create its own sound setup.
   */
  MFSFXRSoundData(MFSFXRSoundSetup* pSetup=nullptr);

  MFSFXRSoundData(E_SoundBaseSetup sfxrBaseSetup);

  virtual ~MFSFXRSoundData();

  /**
   *
   * @param externalData - if data was already created for this sound, the external data must be
   * set to point to the already created data!
   * @return
   */
//  bool initSFXRSound(MFStructurableData* externalData,MFSFXRSoundSetup* externalSetup);
  /**
   * manipulated copy of void ResetSample(bool restart) from main.cpp
   * @param restart
   */
  void resetSample(bool restart);
  void synthesizeSample(int byteLength, float* buffer, FILE* file);
  bool exportWAV(const char* filename);

  MFSFXRSoundSetup* getSoundSetup(){return pSoundSetup;}

private:
  float calculateSample();
  void createNoiseData();
  void writeFile(std::FILE *file, float &sample);
  void writeBuffer(float* buffer, float &sample);
  void updateEnvelopment();
  uint32_t calculateBufferSize();
};

#endif /* MFSFXR_MFSFXRSOUNDDATA_H_ */
