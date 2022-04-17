/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170221  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceMutex.h"
#include "SystemConfig.h"

#include "ConfigEnabler.h"
#include "EnablerFactory.h"
#include "service/AosEnabler.h"
#include "service/UceEnabler.h"
#include "service/MtcEnabler.h"
#include "service/MtsEnabler.h"

PUBLIC
EnablerFactory::EnablerFactory()
    : piLock(IMS_NULL)
{
    IMS_SINT32 nSimCount = SystemConfig::GetMaxSimSlot();

    for (IMS_SINT32 i = 0; i < nSimCount; ++i)
    {
        objImsEnablers.Add(i, new IMSList<IEnabler*>());
    }

    if (nSimCount > 1)
    {
        piLock = MutexService::GetMutexService()->CreateMutex();
    }
}

PUBLIC
EnablerFactory::~EnablerFactory()
{
    {
        LockGuard objLock(piLock);

        IMS_SINT32 nSimCount = SystemConfig::GetMaxSimSlot();

        for (IMS_SINT32 i = 0; i < nSimCount; ++i)
        {
            DestroyEnablers(i);

            IMSList<IEnabler*>* pEnablers = objImsEnablers.GetValueAt(i);

            if (pEnablers != IMS_NULL)
            {
                delete pEnablers;
            }
        }

        objImsEnablers.Clear();
    }

    MutexService::GetMutexService()->DestroyMutex(piLock);
}

PUBLIC
void EnablerFactory::CreateEnablers(IN IMS_SINT32 nSlotId)
{
    LockGuard objLock(piLock);

    IMS_SLONG nIndex = objImsEnablers.GetIndexOfKey(nSlotId);

    if (nIndex < 0)
    {
        return;
    }

    IMSList<IEnabler*>* pEnablers = objImsEnablers.GetValueAt(nIndex);

    if ((pEnablers != IMS_NULL) && pEnablers->IsEmpty())
    {
        CreateEnablers(nSlotId, pEnablers);
    }
}

PUBLIC
void EnablerFactory::DestroyEnablers(IN IMS_SINT32 nSlotId)
{
    LockGuard objLock(piLock);

    IMS_SLONG nIndex = objImsEnablers.GetIndexOfKey(nSlotId);

    if (nIndex < 0)
    {
        return;
    }

    IMSList<IEnabler*>* pEnablers = objImsEnablers.GetValueAt(nIndex);

    if ((pEnablers != IMS_NULL) && !pEnablers->IsEmpty())
    {
        IMS_SINT32 i = static_cast<IMS_SINT32>(pEnablers->GetSize() - 1);

        for ( ; i >= 0; i--)
        {
            Enabler *pEnabler = DYNAMIC_CAST(Enabler*, pEnablers->GetAt(i));

            if (pEnabler != IMS_NULL)
            {
                pEnabler->Destroy();
            }
        }

        pEnablers->Clear();
    }
}

PUBLIC
const IMSList<IEnabler*>* EnablerFactory::GetEnablers(IN IMS_SINT32 nSlotId) const
{
    LockGuard objLock(piLock);

    IMS_SLONG nIndex = objImsEnablers.GetIndexOfKey(nSlotId);

    if (nIndex < 0)
    {
        return IMS_NULL;
    }

    return objImsEnablers.GetValueAt(nIndex);
}

PRIVATE
void EnablerFactory::CreateEnablers(IN IMS_SINT32 nSlotId,
        OUT IMSList<IEnabler*>*& pEnablers)
{
    pEnablers->Append(new ConfigEnabler(nSlotId));
    pEnablers->Append(new AosEnabler(nSlotId));
    pEnablers->Append(new MtcEnabler(nSlotId));
    pEnablers->Append(new MtsEnabler(nSlotId));
    pEnablers->Append(new UceEnabler(nSlotId));
}
