/*
 * MFSoundManager.cpp
 *
 *  Created on: 07.01.2021
 *      Author: michl
 */

#include "MFSoundManager.h"

MFSoundManager::MFSoundManager() {

}

MFSoundManager::~MFSoundManager() {
  for(MFSoundPlaybackData* pD:vecInternalDeletion){
    delete pD;
  }
  vecInternalDeletion.clear();
}
void MFSoundManager::addDeletable(MFSoundPlaybackData* pD){
  lockVecInternalDeletion.lock();
  vecInternalDeletion.push_back(pD);
  lockVecInternalDeletion.unlock();
}

MFSFXRSoundData* MFSoundManager::createLaserSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::laser);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}
void MFSoundManager::creationHelper(
    bool addToManager,MFSoundPlaybackData* pD,bool deleteByManager){
  if(addToManager){
    addSound(pD,false);
  }
  if(deleteByManager){
    addDeletable(pD);
  }
}

MFSFXRSoundData* MFSoundManager::createShootSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::laser);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

MFSFXRSoundData* MFSoundManager::createPowerupSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::powerup);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
  return nullptr;
}

MFSFXRSoundData* MFSoundManager::createPickupSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::pickup);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

MFSFXRSoundData* MFSoundManager::createBlipSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::blip);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

MFSFXRSoundData* MFSoundManager::createSelectSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::blip);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

MFSFXRSoundData* MFSoundManager::createJumpSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::jump);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

MFSFXRSoundData* MFSoundManager::createHitSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::hit);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

MFSFXRSoundData* MFSoundManager::createExplosionSound(bool addToManager,bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(E_SoundBaseSetup::explosion);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

void MFSoundManager::addSound(MFSoundPlaybackData* pData,bool deleteByManager){
  if(pData==nullptr)return;
  uint32_t index=0;
  lockVecSoundCollection.lock();
  index=vecSoundCollection.size();
  pData->setSoundManagerIndex(index);
  vecSoundCollection.push_back(pData);
  lockVecSoundCollection.unlock();
  if(deleteByManager)
    addDeletable(pData);

}

MFSFXRSoundData* MFSoundManager::createSFXRSound(
    MFSFXRSoundSetup* pSetup,
    bool addToManager,
    bool deleteByManager){
  MFSFXRSoundData* pSFXRData=new MFSFXRSoundData(pSetup);
  creationHelper(addToManager,pSFXRData,deleteByManager);
  return pSFXRData;
}

