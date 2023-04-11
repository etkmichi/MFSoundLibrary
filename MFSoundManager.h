/*
 * MFSoundManager.h
 *
 *  Created on: 07.01.2021
 *      Author: michl
 */

#ifndef MFSOUNDMANAGER_H_
#define MFSOUNDMANAGER_H_
#include "MFSoundPlaybackData.h"
#include "MFSFXR/MFSFXRSoundData.h"

/**
 * The sound manager provides sounds which were added. Additional to sound management, some
 * functions for sound creation can be used. Call create...Sound to create a sound. The sound
 * must be initialized after creation. Before initialization of MFSFXRSoundData, the sound
 * setup can be manipulated (MFSFXRSoundData::randomize(), MFSFXRSoundData::mutate())!
 */
class MFSoundManager {
private:
  std::vector<MFSoundPlaybackData*>
  vecInternalDeletion,
  vecSoundCollection;
  std::mutex
  lockVecInternalDeletion,
  lockVecSoundCollection;

public:
  MFSoundManager();
  virtual ~MFSoundManager();

  /**
   * This will add the sound to the manager. The pData's manager index will be overwritten!
   * @param pData
   * @param deleteByManager
   */
  void addSound(MFSoundPlaybackData* pData,bool deleteByManager=false);

  MFSFXRSoundData* createLaserSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createShootSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createPowerupSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createPickupSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createBlipSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createSelectSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createJumpSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createHitSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createExplosionSound(bool addToManager=true,bool deleteByManager=false);
  MFSFXRSoundData* createSFXRSound(
      MFSFXRSoundSetup* pSetup=nullptr,
      bool addToManager=true,
      bool deleteByManager=false);
private:
  void addDeletable(MFSoundPlaybackData* pD);
  void creationHelper(bool addToManager,MFSoundPlaybackData* pD,bool deleteByManager);
};

#endif /* MFSOUNDMANAGER_H_ */
