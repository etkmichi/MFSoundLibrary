/*
 * MFSoundPlayer.h
 *
 *  Created on: 15.01.2021
 *      Author: michl
 */

#ifndef MFSOUNDPLAYER_H_
#define MFSOUNDPLAYER_H_
#include <MFObjects/MFObject.h>
#include <MFThreadSystem/MFTaskThread.h>
#include <MFBasicDefines.h>
#include <mutex>
#include "../MFSoundPlaybackData.h"
#include "../MFSoundTasks/MFTaskSoundMixer.h"
#include "../MFSoundTasks/MFTaskSoundPlayer.h"


/**
 * This class is for playing sounds. The added audio will be fed as pices to the audio stream.
 * The mixing will be done by a separated thread.
 *
 */
class MFSoundPlayer : public MFTaskThread {
private:
  std::mutex
  lockVecSoundInput,
  lockVecSoundPlayback;

  std::vector<uint32_t>
  vecUnusedIndices;

  std::vector<MFSoundPlaybackData*>
  *pVecSoundsInput,
  *pVecSoundsPlayback;

  MFTaskSoundMixer
  *pTaskMixer;

  MFTaskSoundPlayer
  *pTaskPlayer;

  SDL_AudioDeviceID
  audioDev;

  std::string
  deviceName;

  SDL_AudioSpec
  desired,
  obtained;

  E_SampleCount
  sampleCount;

  uint32_t
  sampleCountValue,
  sampleUnitSize=4;

protected:/*virtual functions MFTaskThread*/
  virtual void preExecution();
  virtual void postExecution();

public:
  /**
   *
   * @param sampleCount - used to calculate desired specifications when device is opened
   * @param deviceName - name of a device given by MFSoundDeviceManager::getVecDeviceNames(). If
   * deviceName.data()=="" then default device will be opened (if supported by os).
   */
  MFSoundPlayer(
      SDL_AudioFormat format,
      E_SampleCount sampleCount,
      std::string deviceName);

  virtual ~MFSoundPlayer();
  bool initSoundPlayer();
  void addSound(MFSoundPlaybackData *pSound);

  /**
   * Can be used to get a info string of the demanded/provided device setup.
   * @return a string with the desired device setup and the actually obtained device setup
   */
  std::string getOpenSecsString();
//  static void SDLAudioCallback(void *userdata,Uint8 *stream, int len);
};

#endif /* MFSOUNDPLAYER_H_ */
