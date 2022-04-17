#ifndef _AOS_HANDLE_MTS_H_
#define _AOS_HANDLE_MTS_H_

#include "handle/AosHandle.h"

class AosHandleMts
    : public AosHandle
{
// Operation
public:
    AosHandleMts
    (
        IN IAosAppContext* piAppContext,
        IN CONST AString& strAppId,
        IN CONST AString& strServiceId,
        IN CONST IMS_SINT32 nServiceType
    );
    virtual ~AosHandleMts();

protected:
    virtual void Init();
    virtual void CleanUp();
    virtual void InitializeServiceBlock();
    virtual void InitializeServiceFeature();
    virtual void ProcessCapabilitiesChanged(
            IN const IMSMap<IMS_UINT32, IMS_UINT32>& objCapabilities);

private:
    IMS_BOOL m_bSmsOverIp;
};
#endif // _AOS_HANDLE_MTS_H_
