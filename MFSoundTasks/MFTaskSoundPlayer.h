/*
 * MFTaskSoundPlayer.h
 *
 *  Created on: 20.01.2021
 *      Author: michl
 */

#ifndef MFSOUNDTASKS_MFTASKSOUNDPLAYER_H_
#define MFSOUNDTASKS_MFTASKSOUNDPLAYER_H_
#include <MFTasks/MFAbstractTask.h>
#include <MFData/MFStructurableData.h>
#include <MFBasicDefines.h>
#include <MFData/MFInterfacesData/MFIBufferSink.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "../MFSoundHelper.h"
#include "../MFISound/MFISampleSource.h"

class MFTaskSoundPlayer : public MFAbstractTask{
private:
  SDL_AudioDeviceID
  audioDev;

  std::string
  deviceName;

  MFISampleSource
  *pSource;

public:/*virtual functions of MFAbstractTask*/
  virtual bool doWork();

public:
  MFTaskSoundPlayer(
      SDL_AudioDeviceID devId,
      std::string deviceName,
      MFISampleSource* pSource=nullptr);
  virtual ~MFTaskSoundPlayer();

  /**
   * If a sample source is added, this task will check for samples during execution.
   * Samples will only be taken from source, if latest played sample is going to be finished.
   * @param pSource
   */
  void setSampleSource(MFISampleSource* pSource){this->pSource=pSource;};


};

#endif /* MFSOUNDTASKS_MFTASKSOUNDPLAYER_H_ */
