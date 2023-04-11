/*
 * MFTaskSoundMixer.h
 *
 *  Created on: 15.01.2021
 *      Author: michl
 */

#ifndef MFSOUNDTASKS_MFTASKSOUNDMIXER_H_
#define MFSOUNDTASKS_MFTASKSOUNDMIXER_H_

#include <MFTasks/MFAbstractTask.h>
#include <SDL2/SDL.h>
#include <MFData/MFStructurableDataBuffer.h>

#include "../MFSoundPlaybackData.h"
#include "../MFISound/MFISampleSource.h"

/**
 * This task must be initialized!
 * The sound mixer adds all added sounds to one sample which can be player with MFSoundPlayer.
 * The sound mixer should be executed before MFSoundPlaybackTask to avoid playback of
 * data which wasnt mixed before!
 * This task uses a large buffer for mixing. A returned playback buffer (getNextSample())
 * is a window of sampleByteSize size of the large buffer. If a playback buffer is taken,
 * newly added sounds will be mixed to the data behind the playback window.
 */
class MFTaskSoundMixer: public MFAbstractTask , public MFISampleSource{
private:
  struct S_Playback{
    MFSoundPlaybackData* pSource=nullptr;
    int32_t bufferOffset=-1;
    uint32_t playbackOffset=0;
  };
  std::vector<MFSoundPlaybackData*>
  *pVecCurrentInput=nullptr,
  *pVecInput1,
  *pVecInput2;

  std::vector<S_Playback*>
  *pVecCurrentProcessing=nullptr,
  *pVecProcessing1,
  *pVecProcessing2;

  uint32_t
  sampleBufferByteSize,
  continuousTakeCounter=0,
  sampleUnitSize=4;

  std::mutex
  lockVecInput,
  lockVecProcessing,
  lockMixer;

  MFStructurableData
  *pSampleMixerBuffer;/*used for mixing*/

  int32_t/*glbal pb offset -> offset of playback data which is already in use. Overwriting will do nothing*/
  pbBufferWriteOffset=0/*buffer offset with playable data*/,
  pbBufferReadOffset=0,
  highestValidOffset=0;
  MFStructurableData
  *pNextPlaybackBuffer,
  *pGlobalPlaybackBuffer;
  MFDataObject
  *pNextPlaybackData;

  SDL_AudioSpec
  audioSpecs;
public:/*virtual functions MFISampleSource*/
  /**
   * The returned sample can be used to play its sound data. Take care to return the sample
   * to its source by using MFISampleSource::getSampleSink()!
   * @return a sample which is ready to play.
   */
  virtual MFStructurableData* getNextSample();

  virtual void printSourceInfo();

public:/*virtual functions of MFAbstractTask*/
  /**
   * Don't call this function multiple times at once!
   * @return
   */
  virtual bool doWork();

protected:
  virtual bool mixSoundData(
      MFSoundPlaybackData* pSoundSrc,
      void* dst,
      uint32_t srcPlaybackOffset);

private:
  void updateProcessingData();
  /**
   * Checks if the S_Playback::bufferOffset is higher than heighestValidOffset.
   * @param pPb
   */
  void updateHeighestValidOffset(S_Playback* pPb);

public:
  /**
   *
   * @param sampleCount - enum for determining the cound of sampels per buffer
   * @param bufferCount - count of buffers. Each buffer provides sampleCount float values.
   */
  MFTaskSoundMixer(SDL_AudioSpec audioSpecs);
  virtual ~MFTaskSoundMixer();
  bool initTask();
  void addSound(MFSoundPlaybackData *pSound,bool resetPlayback=true);

};

#endif /* MFSOUNDTASKS_MFTASKSOUNDMIXER_H_ */
