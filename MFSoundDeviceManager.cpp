/*
 * MFAudioDeviceManager.cpp
 *
 *  Created on: 21.01.2021
 *      Author: michl
 */

#include "MFSoundDeviceManager.h"
#include <MFBasicDefines.h>

MFSoundDeviceManager::MFSoundDeviceManager() {
  defaultDevice=0;
}

MFSoundDeviceManager::~MFSoundDeviceManager() {
  // TODO Auto-generated destructor stub
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool MFSoundDeviceManager::initAudioManager(){
  bool ret=true;
  if(!sdlAudioInitialized){
    int code=SDL_InitSubSystem(SDL_INIT_AUDIO);
    if(code!=0){
      std::string err(SDL_GetError());
      P_ERR("Failed to init sdl subsystem! SDL_Err: "+ err);
      ret=false;
    }else{
      sdlAudioInitialized=true;
    }
  }
  if(!updateDeviceList()){
    P_ERR("Failed to update the device list!");
    ret=false;
  }
  if(!updateDefaultDevice()){
    P_ERR("Failed to open default device!");
    ret=false;
  }
  return ret;
}

int MFSoundDeviceManager::getAudioDeviceIndex(const std::string &name){
  int ind=0;
  for(std::string dev:vecAudioDevices){
    if(dev == name){
      return ind;
    }
    ind++;
  }
  P_ERR("no device with specified name found! name: "+name+"\n "
      "take care SDL deviceID!=device index!");
  return -1;
}

bool MFSoundDeviceManager::updateDeviceList(){
  int32_t deviceCount=SDL_GetNumAudioDevices(0);
  if(deviceCount<=0){
    P_ERR("Failed to query audio device count or no audio available!");
    return false;
  }
  vecAudioDevices.clear();
  std::string info=std::string("Audio devices:\n");
  for(uint32_t v=0;v<deviceCount;v++){
    const char* pAD=SDL_GetAudioDeviceName(v, 0);
    if(pAD!=nullptr){
      std::string dev(pAD);
      vecAudioDevices.push_back(pAD);
      info+=dev+"\n";
    }else{
      P_WRN("SDL_GetAudioDeviceName returned nullptr!");
    }
  }
  P_INF(info);
  return true;
}

bool MFSoundDeviceManager::updateDefaultDevice(){
//  defaultDevice=SDL_OpenAudio(&desired, &obtained);
//  defaultDevice=SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
//  if(defaultDevice<0){
//    std::string err(SDL_GetError());
//    P_ERR("Failed to open default audio device! SDL_Err: "+ err);
//    return false;
//  }
  return true;
}

uint32_t MFSoundDeviceManager::getDeviceCount(){
  return vecAudioDevices.size();
}

SDL_AudioDeviceID MFSoundDeviceManager::getDefaultDevice(){
  if(defaultDevice<0){
    P_WRN("MFAudioDeviceManager::getDefaultDevice - device == nullptr! "
        "Audio manager may not be initialized!");
    P_INF("MFAudioDeviceManager::getDefaultDevice - calling init of device manager!");
    initAudioManager();
    if(defaultDevice<=0){
      P_ERR("MFAudioDeviceManager::getDefaultDevice - no default device despite initialization!");
    }
  }
  return defaultDevice;
}

const std::vector<std::string>& MFSoundDeviceManager::getVecDeviceNames(){
  if(vecAudioDevices.size()==0){
    P_WRN("getVecDeviceNames - empy device list! updating list!");
    updateDeviceList();
    if(vecAudioDevices.size()==0)
      P_ERR("no devices found!");
  }
  return vecAudioDevices;
}
