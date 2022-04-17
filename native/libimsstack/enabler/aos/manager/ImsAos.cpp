#include "ServiceMemory.h"
#include "manager/AosFactory.h"
#include "manager/ImsAosManager.h"
#include "ImsAos.h"

PUBLIC GLOBAL
IImsAos* ImsAos::GetImsAos(IN const AString& strAppId, IN const AString& strServiceId,
        IN IMS_SINT32 nSlotId /* = IMS_SLOT_0 */)
{
    ImsAosManager* pManager = (ImsAosManager*) AosFactory::GetManager(nSlotId);

    return (pManager != IMS_NULL) ? pManager->GetImsAos(strAppId, strServiceId) : IMS_NULL;
}

PUBLIC GLOBAL
IMSList<IImsAos*> ImsAos::GetImsAosList(IN const AString& strAppId, IN const AString& strServiceId,
        IN IMS_SINT32 nSlotId /* = IMS_SLOT_0 */)
{
    ImsAosManager* pManager = (ImsAosManager*) AosFactory::GetManager(nSlotId);

    return (pManager != IMS_NULL) ? pManager->GetImsAosList(strAppId, strServiceId) :
            IMSList<IImsAos*>();
}

PUBLIC GLOBAL
IMSList<IImsAos*> ImsAos::GetImsAosList(IN const AString& strAppId,
        IN IMS_SINT32 nSlotId /* = IMS_SLOT_0 */)
{
    ImsAosManager* pManager = (ImsAosManager*) AosFactory::GetManager(nSlotId);

    return (pManager != IMS_NULL) ? pManager->GetImsAosList(strAppId) : IMSList<IImsAos*>();
}
