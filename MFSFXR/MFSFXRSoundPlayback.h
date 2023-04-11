///*
// * MFSFXRSoundPlayback.h
// *
// *  Created on: 13.01.2021
// *      Author: michl
// */
//
//#ifndef MFSFXRSOUNDPLAYBACK_H_
//#define MFSFXRSOUNDPLAYBACK_H_
//
//class MFSFXRSoundPlayback;
//
//#include "../MFSFXR/MFSFXRHelpers.h"
//#include "../MFSFXR/MFSFXRSoundSetup.h"
//#include <iostream>
//
///**
// * not used anymore, moved to MFSFXRSoundData
// * This class only exist for dividing the playback parameters from the sfxr sound setup.
// */
//class MFSFXRSoundPlayback {
//private:
//  MFSFXRSoundSetup
//  *pSoundSetup;
//public:
//  /*sample parameters*/
//  bool playing_sample=false;
//  int phase;
//  double fperiod;
//  double fmaxperiod;
//  double fslide;
//  double fdslide;
//  int period;
//  float square_duty;
//  float square_slide;
//  int env_stage;
//  int env_time;
//  int env_length[3];
//  float env_vol;
//  float fphase;
//  float fdphase;
//  int iphase;
//  float phaser_buffer[1024];
//  int ipp;
//  float noise_buffer[32];
//  float fltp;
//  float fltdp;
//  float fltw;
//  float fltw_d;
//  float fltdmp;
//  float fltphp;
//  float flthp;
//  float flthp_d;
//  float vib_phase;
//  float vib_speed;
//  float vib_amp;
//  int rep_time;
//  int rep_limit;
//  int arp_time;
//  int arp_limit;
//  double arp_mod;
//
//  int wav_bits=16;
//  int wav_freq=44100;
//
//  int file_sampleswritten;
//  float filesample=0.0f;
//  int fileacc=0;
//public:
//  MFSFXRSoundPlayback(MFSFXRSoundSetup* pSetup);
//  virtual ~MFSFXRSoundPlayback();
//
//  /**
//   * manipulated copy of void ResetSample(bool restart) from main.cpp
//   * @param restart
//   */
//  void resetSample(bool restart);
//  void synthesizeSample(int length, float* buffer, FILE* file);
//  bool exportWAV(const char* filename);
//};
//
//#endif /* MFSFXRSOUNDPLAYBACK_H_ */
