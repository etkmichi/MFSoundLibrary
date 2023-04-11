/*
 * MFSoundData.h
 *
 *  Created on: 07.01.2021
 *      Author: michl
 */

#ifndef MFSOUNDPLAYBACKDATA_H_
#define MFSOUNDPLAYBACKDATA_H_
#include <MFData/MFStructurableData.h>
#include <SDL2/SDL_audio.h>
#include <glm/glm.hpp>
#include <mutex>
#include "MFSoundHelper.h"

class MFSoundPlaybackData {
private:
  enum E_ChannelSwitch{
    E_Ch_Left,
    E_Ch_Right
  };

  int32_t
  soundManagerIndex;

  float
  innerVolume=1.0f,
  maxValue=0.0f,
  volumeScale=1.0f;

  glm::vec3
  sourcePosition,
  receiverPosition,
  receiverLookAt;

  std::mutex
  lockSrcPosition,
  lockRecPosition,
  lockCurrentPlaybackPosition,
  lockSoundData;

//  uint32_t
//  currentPlaybackOffset,
//  playbackDataOffset;

  MFStructurableData
  *pSoundData,
  *pInternalSoundData;

  bool
  mute=false;
protected:/*virtual functions MFSoundPlaybackData*/
  /**
   * Creates a sound data object of the current setup.
   * @return
   */
  virtual bool createSoundData(MFStructurableData* pSoundData){
    return true;
  }

  virtual std::string getSoundInfo(){
    return std::string("");
  }

  virtual float fillWithScaledChannels(float* pDataDst,float sample,uint32_t channelCount);
public:
  /**
   *
   * @param pPlaybackData - if playback data is available, it can be specified
   * by the constructor. The virutal createSoundData(..) will not be called during init!
   * @param playbackDataOffset
   */
  MFSoundPlaybackData(
      MFStructurableData* pPlaybackData=nullptr);
  virtual ~MFSoundPlaybackData();

  /**
   * @return
   */
  /**
   * Calls the virtual createSoundData(..) function if no data was specified in constructor or
   * pSoundData was set to null.
   * @param recreate - create data will be called again
   * @param deleteOldData - old internal data will be deleted (if no data was given
   * in constructor). The data pointer of pInternalData will be freed!
   * @return
   */
  bool initSoundData(bool recreate=false,bool deleteOldData=false);

//  void resetPlayback();

  /**
   * Iterates over the underlying data to search the max value. In the second iteration it
   * will divide all values by the max value.
   */
  void normalizeSoundData();

  /**
   * This function will use the data stored in audio specs to determine the correct copy function
   * and the neccessary parameters. audioSpecs.userData must be a valid pointer to the
   * data sink and must by an object of the class MFStructurableData. The size of
   * MFStructurableData (audioSpecs.userData) subdata must be greater or equal to
   * audioSpecs.size!
   * @param audioSpecs
   * @param usePositioning
   * @param useSoundScaleVolume
   */
  void copyData(
      const SDL_AudioSpec &audioSpecs,
      uint32_t playbackOffset,
      bool usePositioning=true,
      bool useSoundScaleVolume=true);


  /**
   * Copies the sample to pDst
   * @param pDst
   */
  void copyAsFloatSamples(
      MFStructurableData* pDst,
      uint32_t playbackOffset,
      uint32_t byteSize,
      uint32_t channelCount,
      bool usePositioning=true,
      bool useSoundScaleVolume=true);

  /**
   * Copies the sample to pDst
   * @param pDst
   */
  void copyAs_SInt16_Samples(
      MFStructurableData* pDst,
      uint32_t playbackOffset,
      uint32_t byteSize,
      uint32_t channelCount,
      bool usePositioning=true,
      bool useSoundScaleVolume=true);

//  uint32_t getCurrentPlaybackOffset(){
//    return currentPlaybackOffset;
//  }

//  void setCurrentPlaybackOffset(const uint32_t& currentPlaybackOffset) {
//    this->currentPlaybackOffset = currentPlaybackOffset;
//  }

  MFStructurableData* getPSoundData(){
   lockSoundData.lock();
   MFStructurableData* pD=pSoundData;
   lockSoundData.unlock();
   return pD;
  }

  void setPSoundData(MFStructurableData *pSoundData) {
    lockSoundData.lock();
    this->pSoundData = pSoundData;
    lockSoundData.unlock();
  }

  const glm::vec3& getReceiverPosition(){
    return receiverPosition;
  }

  void setReceiverPosition(const glm::vec3 &receiverPosition) {
    lockRecPosition.lock();
    this->receiverPosition = receiverPosition;
    lockRecPosition.unlock();
  }

  const glm::vec3& getSourcePosition(){
    return sourcePosition;
  }

  void setSourcePosition(const glm::vec3 &sourcePosition) {
    lockSrcPosition.lock();
    this->sourcePosition = sourcePosition;
    lockSrcPosition.unlock();
  }

  uint32_t getPlaybackByteLength(){
    return pSoundData->getStructureSize();
  }

  int32_t getSoundManagerIndex() const {
    return soundManagerIndex;
  }

  void setSoundManagerIndex(int32_t soundManagerIndex) {
    this->soundManagerIndex = soundManagerIndex;
  }

  float getInnerVolume() const {
    return innerVolume;
  }

  /**
   * Will be multiplied with audio data, if volumeScale==0 then audio data will be set to 0!
   * @param innerVolume
   */
  void setInnerVolume(float innerVolume = 1.0f) {
    this->innerVolume = innerVolume;
  }

  const glm::vec3& getReceiverLookAt() const {
    return receiverLookAt;
  }

  void setReceiverLookAt(const glm::vec3 &receiverLookAt) {
    this->receiverLookAt = receiverLookAt;
  }

  float getVolumeScale() const {
    return volumeScale;
  }

  /**
   * Will be multiplied with audio data, if volumeScale==0 then audio data will be set to 0!
   * @param volumeScale
   */
  void setVolumeScale(float volumeScale = 1.0f) {
    this->volumeScale = volumeScale;
  }

  /**
   * If muted copying of audio data will set all bytes to 0!
   * @param mute
   */
  void muteSound(bool mute){
    this->mute=mute;
  }

  /**
   * If muted copying of audio data will set all bytes to 0!
   * @param mute
   */
  bool isMuted() const {
    return mute;
  }

  /**
   * If muted copying of audio data will set all bytes to 0!
   * @param mute
   */
  void setMute(bool mute = false) {
    this->mute = mute;
  }
};

#endif /* MFSOUNDPLAYBACKDATA_H_ */
