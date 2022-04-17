/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170221  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _ENABLER_FACTORY_H_
#define _ENABLER_FACTORY_H_

#include "IMSList.h"
#include "IMSMap.h"
#include "IEnabler.h"

class IMutex;

class EnablerFactory
{
public:
    EnablerFactory();
    ~EnablerFactory();

private:
    EnablerFactory(IN const EnablerFactory& objRHS);
    EnablerFactory& operator=(IN const EnablerFactory& objRHS);

public:
    void CreateEnablers(IN IMS_SINT32 nSlotId);
    void DestroyEnablers(IN IMS_SINT32 nSlotId);
    const IMSList<IEnabler*>* GetEnablers(IN IMS_SINT32 nSlotId) const;

private:
    void CreateEnablers(IN IMS_SINT32 nSlotId, OUT IMSList<IEnabler*>*& pEnablers);

private:
    IMutex* piLock;
    // <slotId, enablers>
    IMSMap<IMS_SINT32, IMSList<IEnabler*>* > objImsEnablers;
};

#endif // _ENABLER_FACTORY_H_
