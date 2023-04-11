/*
 * MFSoundPlayer.cpp
 *
 *  Created on: 15.01.2021
 *      Author: michl
 */

#include "MFSoundPlayer.h"
#include "SDL2/SDL.h"

MFSoundPlayer::MFSoundPlayer(
    SDL_AudioFormat format,
    E_SampleCount sampleCount,
    std::string deviceName) :
MFTaskThread(std::string("MFTSound"), false){
  pVecSoundsInput=new std::vector<MFSoundPlaybackData*>();
  pVecSoundsPlayback=new std::vector<MFSoundPlaybackData*>();
  pTaskMixer=nullptr;
  pTaskPlayer=nullptr;
  this->audioDev=0;
  this->deviceName=deviceName;
//  deviceName=std::string(SDL_GetAudioDeviceName(audioDev, 0));
  this->sampleCount=sampleCount;
  sampleUnitSize=sizeof(float);
  if(format==AUDIO_F32SYS)
    sampleUnitSize=sizeof(float);
  if(format==AUDIO_S16SYS)
    sampleUnitSize=sizeof(int16_t);
  sampleCountValue=512*pow(2,(int)sampleCount);
  desired.channels=2;//TODO
  desired.format=format;
  desired.callback=NULL;
  desired.freq=44100;
  desired.silence=0;
  desired.padding=0;
  desired.size=sampleCountValue*sampleUnitSize*desired.channels;//TODO size *2 -> size *channelCount
  desired.samples=sampleCountValue;
  desired.userdata=NULL;
}

MFSoundPlayer::~MFSoundPlayer() {
}

bool MFSoundPlayer::initSoundPlayer(){
  /*allocate input buffers (sample buffers)*/
  bool ret=true;
  PRINT_INF("MFSoundPlayer::initSoundPlayer - init of sound player");
  if(deviceName.data()==""){
    audioDev=SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
  }else{
    audioDev=SDL_OpenAudioDevice(deviceName.data(), 0, &desired, &obtained, 0);
  }
  std::string initInfo="deviceName: "+deviceName ;
  if(audioDev<2){
    P_ERR("device id < 2 -> SDL error: "+std::string(SDL_GetError()));
    ret=false;
    initInfo+=" error: "+std::string(SDL_GetError());
  }else{
    initInfo+="\n"+getOpenSecsString();
  }
  if(ret){
    SDL_PauseAudioDevice(audioDev, 0);
    pTaskMixer=new MFTaskSoundMixer(desired);
    pTaskMixer->initTask();
    pTaskPlayer=new MFTaskSoundPlayer(audioDev,deviceName,pTaskMixer);

    addTask(pTaskMixer);
    addTask(pTaskPlayer);
    this->setPauseTime(10);
    startDetached();
  }else{
    P_ERR("failed to init audio device!");
  }
  P_INF("Init info:" +initInfo);
  return ret;
}

std::string MFSoundPlayer::getOpenSecsString(){
  std::string info="desired/obtained:";
  info+="\nfrequency: "+std::to_string(desired.freq)+" / "+std::to_string(obtained.freq);
  info+="\nformat: "+std::to_string(desired.format)+" / "+std::to_string(obtained.format);
  info+="\nchannels: "+std::to_string(desired.channels)+" / "+std::to_string(obtained.channels);
  info+="\nsilence: "+std::to_string(desired.silence)+" / "+std::to_string(obtained.silence);
  info+="\nsamples: "+std::to_string(desired.samples)+" / "+std::to_string(obtained.samples);
  info+="\npadding: "+std::to_string(desired.padding)+" / "+std::to_string(obtained.padding);
  info+="\nsample buffer size: "+std::to_string(desired.size)+" / "+std::to_string(obtained.size);
  info+="\ncallback: "+std::to_string((long)desired.callback)+" / "+
      std::to_string((long)obtained.callback);
  info+="\nuserdata: "+std::to_string((long)desired.userdata)+" / "+
      std::to_string((long)obtained.userdata);
  return info;
}

void MFSoundPlayer::addSound(MFSoundPlaybackData *pSound){
  lockVecSoundInput.lock();
  pVecSoundsInput->push_back(pSound);
  lockVecSoundInput.unlock();
}

void MFSoundPlayer::preExecution(){
  /*added sounds must be mixed (done by pTaskMixer) before playing (done by pTaskPlayer)*/
  lockVecSoundInput.lock();
  for(MFSoundPlaybackData* pSound:*pVecSoundsInput){
    pTaskMixer->addSound(pSound, true);
  }
  pVecSoundsInput->clear();
  lockVecSoundInput.unlock();

  addTask(pTaskMixer);
  addTask(pTaskPlayer);
}

void MFSoundPlayer::postExecution(){
}

