/*
 * MFISampleSource.h
 *
 *  Created on: 29.01.2021
 *      Author: michl
 */

#ifndef MFISOUND_MFISAMPLESOURCE_H_
#define MFISOUND_MFISAMPLESOURCE_H_
#include <MFData/MFStructurableData.h>
#include <MFData/MFInterfacesData/MFIBufferSink.h>
#include <MFBasicDefines.h>
class MFISampleSource {
public:/*virtual functions MFISampleSource*/
  /**
   * The returned sample can be used to play its sound data. Take care to return the sample
   * to its source by using MFISampleSource::getSampleSink()!
   * @return a sample which is ready to play.
   */
  virtual MFStructurableData* getNextSample(){return nullptr;};
  virtual void printSourceInfo(){P_INF("No info!");};
public:
  MFISampleSource();
  virtual ~MFISampleSource();
};

#endif /* MFISOUND_MFISAMPLESOURCE_H_ */
