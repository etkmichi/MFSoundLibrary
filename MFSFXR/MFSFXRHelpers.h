/*
 * MFSFXRHelpers.h
 *
 *  Created on: 14.01.2021
 *      Author: michl
 */

#ifndef MFSFXR_MFSFXRHELPERS_H_
#define MFSFXR_MFSFXRHELPERS_H_
#include <cmath>

#define rnd(n) (rand()%(n+1))

static float frnd(float range){
  return (float)rnd(10000)/10000*range;
};

enum E_SoundBaseSetup {
  pickup,
  laser,
  explosion,
  powerup,
  hit,
  jump,
  blip
  //TODO ,ding,dong,blob,pop,ping,peng(short explosion),????
};

enum E_WaveType {
  wave_type_square,
  wave_type_sawtooth,
  wave_type_sine,
  wave_type_noise
};

enum E_SoundFileType {
  WAVE,
  SFXR,
  MP3
};
class Test{
  Test(){frnd(2.0f);};
};
#endif /* MFSFXR_MFSFXRHELPERS_H_ */
