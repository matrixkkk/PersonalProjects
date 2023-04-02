// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIFASTALPHAACCUMULATOR_H
#define NIFASTALPHAACCUMULATOR_H

#include <NiAccumulator.h>

class NiFastAlphaAccumulator : public NiAccumulator
{
public:
    // creation and destruction
    NiFastAlphaAccumulator();
    virtual ~NiFastAlphaAccumulator();

    virtual void FinishAccumulating();
    virtual void RegisterObjectArray(NiVisibleArray& kArray);

protected:
    NiSortedObjectList m_kItems;
    NiSortedObjectList m_kNoTestItems;
};

typedef NiPointer<NiFastAlphaAccumulator> NiFastAlphaAccumulatorPtr;

#include "NiFastAlphaAccumulator.inl"

#endif

