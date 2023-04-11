/*
 * MFAudioDeviceManager.h
 *
 *  Created on: 21.01.2021
 *      Author: michl
 */

#ifndef MFSOUNDDEVICEMANAGER_H_
#define MFSOUNDDEVICEMANAGER_H_
#include <MFBasicDefines.h>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
class MFSoundDeviceManager {
  std::vector<std::string>
  vecAudioDevices;

  SDL_AudioDeviceID
  defaultDevice;

  bool
  sdlAudioInitialized=false;

  SDL_AudioSpec
  desired,
  obtained;
public:
  MFSoundDeviceManager();
  virtual ~MFSoundDeviceManager();
  bool initAudioManager();
  bool updateDeviceList();
  bool updateDefaultDevice();
  uint32_t getDeviceCount();
  int getAudioDeviceIndex(const std::string &name);

  SDL_AudioDeviceID getDefaultDevice();
  const std::vector<std::string>& getVecDeviceNames();
};

#endif /* MFSOUNDDEVICEMANAGER_H_ */
