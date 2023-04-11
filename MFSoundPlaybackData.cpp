/*
 * MFSoundData.cpp
 *
 *  Created on: 07.01.2021
 *      Author: michl
 */

#include "MFSoundPlaybackData.h"
#include <MFBasicDefines.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

MFSoundPlaybackData::MFSoundPlaybackData(
    MFStructurableData* pPlaybackData){
  sourcePosition=glm::vec3(0,0,0);
  receiverPosition=glm::vec3(0,0,0);
//  currentPlaybackOffset=0;
  pSoundData=pPlaybackData;
//  this->playbackDataOffset=playbackDataOffset;
  pInternalSoundData=nullptr;
  soundManagerIndex=-1;
  receiverLookAt=glm::vec3(1,0,0);
}

MFSoundPlaybackData::~MFSoundPlaybackData() {
  if(pInternalSoundData!=nullptr)delete pInternalSoundData;
}

//void MFSoundPlaybackData::resetPlayback(){
//  currentPlaybackOffset=0;
//}

//TODO add bool mix as param and do mixing (like SDL_mix...) in this function
//  -> mixing in this function will prevent copiing of multiple buffers
void MFSoundPlaybackData::copyData(
    const SDL_AudioSpec &audioSpecs,
    uint32_t playbackOffset,
    bool usePositioning,
    bool useSoundScaleVolume){
  MFStructurableData* pDstMixerBuffer=static_cast<MFStructurableData*>(audioSpecs.userdata);
  if(innerVolume==0.0f || volumeScale==0.0f || mute){
    uint8_t* pData=static_cast<uint8_t*>(pDstMixerBuffer->getStructureData());
    uint32_t dstSize=pDstMixerBuffer->getStructureSize();
    memset(pData, 0, dstSize);
    return;
  }
  switch(audioSpecs.format){
  case AUDIO_S16SYS:
    copyAs_SInt16_Samples(
        pDstMixerBuffer,/*copy to pDstMixerBuffer*/
        playbackOffset,
        pDstMixerBuffer->getStructureSize(),
        audioSpecs.channels);
    break;
  case AUDIO_F32SYS:
    copyAsFloatSamples(
        pDstMixerBuffer,/*copy to pDstMixerBuffer*/
        playbackOffset,
        pDstMixerBuffer->getStructureSize(),
        audioSpecs.channels);
    break;
  default:
    P_ERR("No copy function for given audio format!");
    break;
  }
}

void MFSoundPlaybackData::copyAsFloatSamples(
    MFStructurableData* pDst,
    uint32_t playbackByteOffset,
    uint32_t byteSize,
    uint32_t channelCount,
    bool usePositioning,
    bool useSoundScaleVolume){
  uint8_t* pData=static_cast<uint8_t*>(pDst->getStructureData());
  uint32_t dstSize=pDst->getStructureSize();
  float* pDataDst=(float*)(pData);
  float* pDataDstEnd=(float*)(pData+dstSize-sizeof(float));

  pData=static_cast<uint8_t*>(pSoundData->getStructureData());
  uint32_t srcSize=pSoundData->getStructureSize();
  float* pDataSrc=(float*)(pData+playbackByteOffset);
  float* pDataSrcEnd=(float*)(pData+srcSize-sizeof(float));

  while(((uint64_t)(pDataDst))<=((uint64_t)(pDataDstEnd))){
    if(pDataSrc>=pDataSrcEnd){
      //P_INF("end of sound source reached, filling with 0");
      while(((uint64_t)(pDataDst))<=((uint64_t)(pDataDstEnd))){
        (*pDataDst)=0;
        pDataDst++;
      }
      return;
    }
    if((pDataDst+channelCount)>pDataDstEnd){
      //P_INF("end of sound source reached, filling with 0");
      while(((uint64_t)(pDataDst))<=((uint64_t)(pDataDstEnd))){
        (*pDataDst)=0;
        pDataDst++;
      }
      return;
    }else{
      fillWithScaledChannels(pDataDst,(*pDataSrc),channelCount);
      pDataDst+=channelCount;
    }

    pDataSrc++;
  }
  /*TODO scale with positioning and soundScale*/
}

float MFSoundPlaybackData::fillWithScaledChannels(
    float* pDataDst,
    float sample,
    uint32_t channelCount){
  float leftScale=1.0f;//max=2.1; min=0.1
  float rightScale=1.0f;//max=2.1; min=0.1
  glm::vec3 direction=sourcePosition-receiverPosition;
  float distanceScale=0.5f;

  if(sourcePosition!=receiverPosition){//TODO is this valid? better <min(float) || >min(float)
    float length=glm::length2(direction);
    distanceScale=10.0f/length;
    if(distanceScale<0.005f){
      distanceScale=.0f;
      leftScale=.0f;//max=2.1; min=0.1
      rightScale=.0f;//max=2.1; min=0.1
    }else{
      glm::vec2 soundDirection=glm::normalize(glm::vec2(direction));
      glm::vec2 lookAt=glm::normalize(glm::vec2(receiverLookAt));
       //transformation into lookAt as x-Axe coordinates
        //-> y>0 will be left ear louder and y<0 will be right ear louder
      glm::mat2x2 transBA=glm::mat2x2(lookAt,glm::vec2(-lookAt.y,lookAt.x));
      glm::mat2x2 transAB=glm::inverse(transBA);
      glm::vec2 relativePos=transAB*soundDirection;
      leftScale=1.1f+relativePos.y;//max=2.1; min=0.1
      rightScale=1.1f-relativePos.y;//max=2.1; min=0.1
    }
  }

  float val=sample*distanceScale;

  for(uint32_t channel=0;channel<channelCount;channel++){
    switch(channel){
    case E_ChannelSwitch::E_Ch_Left:
      val=val*leftScale;
      break;
    case E_ChannelSwitch::E_Ch_Right:
      val=val*rightScale;
      break;
    default:
      break;
    }
    (*pDataDst)=val;
    (*pDataDst)=(*pDataDst)*innerVolume*volumeScale;
    pDataDst++;
  }
  return val;
}

void MFSoundPlaybackData::copyAs_SInt16_Samples(
    MFStructurableData* pDst,
    uint32_t playbackByteOffset,
    uint32_t byteSize,
    uint32_t channelCount,
    bool usePositioning,
    bool useSoundScaleVolume){
  uint8_t* pData=static_cast<uint8_t*>(pDst->getStructureData());
  uint32_t dstSize=pDst->getStructureSize();
  int16_t* pDataDst=(int16_t*)(pData);
  int16_t* pDataDstEnd=(int16_t*)(pData+dstSize-sizeof(int16_t));

  pData=static_cast<uint8_t*>(pSoundData->getStructureData());
  uint32_t srcSize=pSoundData->getStructureSize();
  float* pDataSrc=(float*)(pData+playbackByteOffset);
  float* pDataSrcEnd=(float*)(pData+srcSize-sizeof(float));

  int16_t sample=0;
  float f=0;
  while(((uint64_t)(pDataDst))<=((uint64_t)(pDataDstEnd))){
    if(pDataSrc>=pDataSrcEnd){
      //P_INF("end of sound source reached, filling with 0");
      while(((uint64_t)(pDataDst))<=((uint64_t)(pDataDstEnd))){
        (*pDataDst)=0;
        pDataDst++;
      }
      return;
    }
    //conv float to sample
    f=(*pDataSrc)*innerVolume*volumeScale;
    if (f < -1.0) f = -1.0;
    if (f > 1.0) f = 1.0;
    sample = (int16_t)(f * 32767);
    //chanel 1
    for(uint32_t i=0;i<channelCount;i++){
      (*pDataDst)=sample;
      pDataDst++;

      if(pDataDst>pDataDstEnd )
        return;
    }
    pDataSrc++;
  }
  /*TODO scale with positioning and soundScale*/
}

bool MFSoundPlaybackData::initSoundData(bool recreate,bool deleteOldData){
  bool ret=true;
  if(pSoundData==nullptr || recreate){
    if(pInternalSoundData!=nullptr && deleteOldData){
      pInternalSoundData->clear();
    }else{
      pInternalSoundData=new MFStructurableData();
    }
    pSoundData=pInternalSoundData;
    ret=createSoundData(pSoundData);
  }
  return ret;
}

void MFSoundPlaybackData::normalizeSoundData(){
  float maxVal=0.0f;
  uint32_t valCount=pSoundData->getStructureSize();
  uint8_t* pData=static_cast<uint8_t*>(pSoundData->getStructureData());
  for(uint32_t i=0;i<valCount;i+=4){
    float val=*(pData+i);
    if((val*val)>(maxVal*maxVal)){
      maxVal=val;
    }
  }

  if(maxVal<0)maxVal*=(-1);

  for(uint32_t i=0;i<valCount;i+=4){
    float* val=(float*)(pData+i);
    (*val) /= maxVal;
  }

}


























