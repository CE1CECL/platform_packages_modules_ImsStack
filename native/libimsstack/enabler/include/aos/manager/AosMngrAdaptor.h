#ifndef _AOS_MNGR_ADAPTOR_H_
#define _AOS_MNGR_ADAPTOR_H_

#include "IMSTypeDef.h"
#include "IMSList.h"

#include "manager/ImsAosManager.h"

class AString;
class AosMngr;

class AosMngrAdaptor
    : public ImsAosManager
{
public:
    AosMngrAdaptor(IN const AString& strAppName, IN IMS_SINT32 nSlotId);
    virtual ~AosMngrAdaptor();

private:
    AosMngrAdaptor(IN const AosMngrAdaptor& objRhs);
    AosMngrAdaptor& operator=(IN const AosMngrAdaptor& objRhs);

public:
    virtual IImsAos* GetImsAos(IN const AString& strAppId, IN const AString& strServiceId);
    virtual IMSList<IImsAos*> GetImsAosList(IN const AString& strAppId,
            IN const AString& strServiceId);
    virtual IMSList<IImsAos*> GetImsAosList(IN const AString& strAppId);

private:
    IMS_SINT32 m_nSlotId;
    AosMngr* m_pAdaptee;
};
#endif // _AOS_MNGR_ADAPTOR_H_
