/*
 * MFSound.h
 *
 *  Created on: 20.01.2021
 *      Author: michl
 */

#ifndef MFSOUNDHELPER_H_
#define MFSOUNDHELPER_H_

enum E_SampleCount{
  E_Sound_512_Samples,
  E_Sound_1024_Samples,
  E_Sound_2048_Samples,
  E_Sound_4096_Samples,
  E_Sound_8192_Samples
};

class MFSoundHelper {
public:
  MFSoundHelper();
  virtual ~MFSoundHelper();
};

#endif /* MFSOUNDHELPER_H_ */
