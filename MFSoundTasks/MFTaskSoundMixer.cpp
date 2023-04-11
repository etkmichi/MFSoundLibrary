/*
 * MFTaskSoundMixer.cpp
 *
 *  Created on: 15.01.2021
 *      Author: michl
 */

#include "MFTaskSoundMixer.h"
#include "MFBasicDefines.h"
#include <glm/glm.hpp>

MFTaskSoundMixer::MFTaskSoundMixer(SDL_AudioSpec audioSpecs) {
  this->audioSpecs=audioSpecs;
  pVecInput2=new std::vector<MFSoundPlaybackData*>();
  pVecInput1=new std::vector<MFSoundPlaybackData*>();
  pVecCurrentInput=pVecInput1;

  pVecProcessing1=new std::vector<S_Playback*>();
  pVecProcessing2=new std::vector<S_Playback*>();
  pVecCurrentProcessing=pVecProcessing1;

  sampleUnitSize=sizeof(float);
  if(audioSpecs.format==AUDIO_F32SYS)
    sampleUnitSize=sizeof(float);
  if(audioSpecs.format==AUDIO_S16SYS)
    sampleUnitSize=sizeof(int16_t);

  sampleBufferByteSize=audioSpecs.samples*sampleUnitSize*audioSpecs.channels;

  pSampleMixerBuffer=new MFStructurableData();
  pSampleMixerBuffer->addNBitValue(sampleBufferByteSize*8,nullptr)->allocate();

  pGlobalPlaybackBuffer=new MFStructurableData();

  pNextPlaybackBuffer=new MFStructurableData();
  pNextPlaybackData=pNextPlaybackBuffer->addNBitValue(sampleBufferByteSize*8, nullptr);
}

MFTaskSoundMixer::~MFTaskSoundMixer() {
  delete pNextPlaybackBuffer;
  if(pSampleMixerBuffer->getStructureData()!=nullptr)
    free(pSampleMixerBuffer->getStructureData());
  delete pSampleMixerBuffer;
}

bool MFTaskSoundMixer::initTask() {
  uint32_t size=sampleBufferByteSize*256;
  //P_INF("init task with audio buffer size: "+std::to_string(size));
  MFDataObject* pDO=pGlobalPlaybackBuffer->addNBitValue(8*size, nullptr);
  pDO->allocate();
  memset(pDO->getData(), 0, size);
  //P_INF("init done!");
  return true;
}

bool MFTaskSoundMixer::mixSoundData(
    MFSoundPlaybackData* pSoundSrc,
    void* dst,
    uint32_t srcPlaybackOffset){
  /*add the sound of pData to the next output buffer*/
  lockMixer.lock();
  memset(dst,0, sampleBufferByteSize);
  memset(pSampleMixerBuffer->getStructureData(), 0, pSampleMixerBuffer->getStructureSize());
  audioSpecs.userdata=pSampleMixerBuffer;
  pSoundSrc->copyData(audioSpecs,srcPlaybackOffset);

  uint8_t *dst_ui8=static_cast<uint8_t*>(dst);
  uint8_t *src_ui8=static_cast<uint8_t*>(pSampleMixerBuffer->getStructureData());

  SDL_MixAudioFormat(
      dst_ui8,
      src_ui8,
      audioSpecs.format,
      sampleBufferByteSize,
      SDL_MIX_MAXVOLUME);

  lockMixer.unlock();
  return true;
}

void MFTaskSoundMixer::addSound(MFSoundPlaybackData *pSound,bool resetPlayback){
//  if(resetPlayback)pSound->resetPlayback();
  lockVecInput.lock();
  pVecCurrentInput->push_back(pSound);
  lockVecInput.unlock();
}

void MFTaskSoundMixer::updateProcessingData(){
  lockVecInput.lock();
  std::vector<MFSoundPlaybackData*>* pVecIn=nullptr;
  /*switch the currently used input vector*/
  if(pVecCurrentInput==pVecInput1){
    pVecCurrentInput=pVecInput2;
    pVecIn=pVecInput1;
  }else{
    pVecCurrentInput=pVecInput1;
    pVecIn=pVecInput2;
  }
  lockVecInput.unlock();

  /*take all input sounds and move to processing queue*/
  for(MFSoundPlaybackData* pPB : *pVecIn){
    S_Playback* pPlayback=new S_Playback();
    pPlayback->pSource=pPB;
    pPlayback->bufferOffset=-1;/*-1 -> not played yet*/
    pVecCurrentProcessing->push_back(pPlayback);
  }
  pVecIn->clear();
}

bool MFTaskSoundMixer::doWork(){
  lockVecProcessing.lock();
  updateProcessingData();
  /* processing/mixing of sound data */
  bool ret=true;

  /*iterate over currently available sounds and mix with the pGlobalPlaybackBuffer*/
  std::vector<S_Playback*>* pNextIteration=nullptr;
  if(pVecProcessing1==pVecCurrentProcessing){
    pNextIteration=pVecProcessing2;
  }else{
    pNextIteration=pVecProcessing1;
  }

  for(S_Playback* pPb : *pVecCurrentProcessing) {
    if( pPb == nullptr ) {
      continue;
    }

    if(pPb->bufferOffset<0){
      pPb->bufferOffset=pbBufferWriteOffset;
    }

    /*mix next sound sample with pSD and update the current playback offset of pSD*/
    uint8_t* pDst=static_cast<uint8_t*>(pGlobalPlaybackBuffer->getStructureData());
    pDst+=pPb->bufferOffset;

    if(pPb->bufferOffset!=(pbBufferReadOffset) || (pbBufferWriteOffset==pbBufferReadOffset)){
      ret&=mixSoundData(pPb->pSource,pDst,pPb->playbackOffset);
    }else{
      P_ERR("cant mix data with current playback data!\
          (pbBufferReadOffset==pbBufferWriteOffset");
    }
    pPb->bufferOffset+=sampleBufferByteSize;
    pPb->playbackOffset+=sampleBufferByteSize;

    if((pPb->bufferOffset+sampleBufferByteSize)>=pGlobalPlaybackBuffer->getStructureSize()){
      pPb->bufferOffset=0;
    }

    /*update highest valid offset*/
    /*if highest is less but both offsets are heigher than playback offset*/
    updateHeighestValidOffset(pPb);

    uint32_t playbackLength=pPb->pSource->getPlaybackByteLength();
    uint32_t playbackOffset=pPb->playbackOffset;

    if( playbackOffset < playbackLength ){
      /*sound not yet fully played, add it to the next processing vector*/
      pNextIteration->push_back(pPb);
    }else{
      //P_INF("mixing done!");
      delete pPb;
    }
  }
  pVecCurrentProcessing->clear();
  pVecCurrentProcessing=pNextIteration;
  lockVecProcessing.unlock();

  return ret;
}

void MFTaskSoundMixer::updateHeighestValidOffset(S_Playback* pPb){
  /*heighestValidOffset can be lower than pPb->bufferOffset. This happens if
   * a write reaches the buffer end. If the buffer end end is reached, writing will continue
   * at buffer start!*/
  /*if heighest is less than bufferOffset and write offset is above read window*/
  if(highestValidOffset < pPb->bufferOffset &&
      (highestValidOffset > pbBufferReadOffset &&
          pPb->bufferOffset > pbBufferReadOffset)){
    highestValidOffset=pPb->bufferOffset;
  }
  /*if heighest is less than bufferOffset and write offset is below read window*/
  if(highestValidOffset < pPb->bufferOffset &&
      (highestValidOffset< pbBufferReadOffset &&
          pPb->bufferOffset< pbBufferReadOffset)){
    highestValidOffset=pPb->bufferOffset;
  }

  /*no valid data was available*/
  if(highestValidOffset==pbBufferWriteOffset){
      highestValidOffset=pPb->bufferOffset;
  }
}

MFStructurableData* MFTaskSoundMixer::getNextSample(){
  lockVecProcessing.lock();
  if(pbBufferWriteOffset==highestValidOffset){
    lockVecProcessing.unlock();
    return nullptr;
  }
  uint8_t* pData=static_cast<uint8_t*>(pGlobalPlaybackBuffer->getStructureData());
  pData+=pbBufferWriteOffset;
  pbBufferReadOffset=pbBufferWriteOffset;
  pbBufferWriteOffset+=sampleBufferByteSize;
  if((pbBufferWriteOffset+1)>=pGlobalPlaybackBuffer->getStructureSize()){
    pbBufferWriteOffset=0;
  }
  pNextPlaybackData->setData(pData);
  lockVecProcessing.unlock();
  return pNextPlaybackBuffer;
}

void MFTaskSoundMixer::printSourceInfo(){
  std::string info="MFTaskSoundMixer info:\ncurrent playback setting:";
  info+="\nwindow size="+std::to_string(pGlobalPlaybackBuffer->getStructureSize()/1000)+" kByte";
  info+="\ncurrent playback offset="+std::to_string(pbBufferReadOffset/1000)+" kByte";
  info+="\nlowest data write position="+std::to_string(pbBufferWriteOffset/1000)+" kByte";
  info+="\nhighest valid offset="+std::to_string(highestValidOffset/1000)+" kByte";
  lockVecProcessing.lock();
  info+="\nenqueued sounds="+std::to_string(pVecProcessing1->size()+pVecProcessing2->size());
  uint32_t counter=0;
  for(S_Playback* pPB:*pVecProcessing1){
    if(pPB==nullptr){
      P_ERR("playback == nullptr!");
      continue;
    }
    info+="\nsoundIndex/current sound pb pos/offset to window buffer/sound length:";
    info+="\n"+std::to_string(counter)+"/"+
        std::to_string(pPB->playbackOffset/1000)+" kByte/"+
        std::to_string(pPB->bufferOffset/1000)+" kByte/"+
        std::to_string(pPB->pSource->getPlaybackByteLength()/1000)+" kByte";
    counter++;
  }
  for(S_Playback* pPB:*pVecProcessing2){
    if(pPB==nullptr){
      P_ERR("playback == nullptr!");
      continue;
    }
    info+="\nsoundIndex/current sound pb pos/offset to window buffer/sound length:";
    info+="\n"+std::to_string(counter)+"/"+
        std::to_string(pPB->playbackOffset/1000)+" kByte/"+
        std::to_string(pPB->bufferOffset/1000)+" kByte/"+
        std::to_string(pPB->pSource->getPlaybackByteLength()/1000)+" kByte";
    counter++;
  }
  //P_INF(info);
  lockVecProcessing.unlock();
}

























