#include "IMSTypeDef.h"
#include "ServiceThread.h"

#include "IMtcContext.h"
#include "MtcContextRepository.h"

MtcContextRepository* MtcContextRepository::s_pThis = IMS_NULL;

PUBLIC
MtcContextRepository::MtcContextRepository() :
        m_objContexts(IMSMap<IMS_SINT32, IMtcContext*>())
{
}

PUBLIC
MtcContextRepository::~MtcContextRepository()
{
}

PUBLIC GLOBAL
MtcContextRepository* MtcContextRepository::GetInstance()
{
    if (s_pThis == IMS_NULL)
    {
        s_pThis = new MtcContextRepository();
    }
    return s_pThis;
}

PUBLIC GLOBAL
IMtcContext* MtcContextRepository::GetContext(IN IMS_SINT32 nSlotId/* = INVALID_SLOT_ID*/)
{
    if (nSlotId == INVALID_SLOT_ID)
    {
        nSlotId = ThreadService::GetCurrentSlotId();
    }

    return MtcContextRepository::GetInstance()->GetContextBySlot(nSlotId);
}

PUBLIC
IMtcContext* MtcContextRepository::GetContextBySlot(IN IMS_SINT32 nSlotId)
{
    if (m_objContexts.GetIndexOfKey(nSlotId) < 0)
    {
        return IMS_NULL;
    }
    return m_objContexts.GetValueAt(nSlotId);
}

PUBLIC
void MtcContextRepository::AddContext(IN IMtcContext* piContext)
{
    m_objContexts.Add(ThreadService::GetCurrentSlotId(), piContext);
}

PUBLIC
void MtcContextRepository::RemoveContext()
{
    m_objContexts.Remove(ThreadService::GetCurrentSlotId());
}
