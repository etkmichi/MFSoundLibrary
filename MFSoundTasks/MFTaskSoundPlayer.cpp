/*
 * MFTaskSoundPlayer.cpp
 *
 *  Created on: 20.01.2021
 *      Author: michl
 */

#include "MFTaskSoundPlayer.h"
#include <glm/glm.hpp>
MFTaskSoundPlayer::MFTaskSoundPlayer(
    SDL_AudioDeviceID devId,
    std::string deviceName,
    MFISampleSource* pSource) {
  setTaskName("MFTaskSoundPlayer");
  this->audioDev=devId;
  this->deviceName=deviceName;
  this->pSource=pSource;
}

MFTaskSoundPlayer::~MFTaskSoundPlayer() {
  // TODO Auto-generated destructor stub
}

bool MFTaskSoundPlayer::doWork(){
  bool ret=true;
  MFStructurableData* pNextSample=pSource->getNextSample();
  if(pNextSample!=nullptr){
    void* pSData=pNextSample->getStructureData();
    uint32_t length=pNextSample->getStructureSize();

    if(0!=SDL_QueueAudio(audioDev,pSData,length)){
      P_ERR("Failed to queue audio!\n SDLError: "+
          std::string(SDL_GetError())+"\n DeviceName: "+deviceName);

      ret=false;
    }else{
      //P_INF("Playing audio with device: "+deviceName + " \nstatus:"+
         // std::to_string(SDL_GetAudioDeviceStatus(audioDev)));
//      memset(pSData, 0, length);
      pSource->printSourceInfo();
    }
  }
  return ret;
}

