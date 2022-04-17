#ifndef _IMS_AOS_MANAGER_H_
#define _IMS_AOS_MANAGER_H_

#include "IMSTypeDef.h"
#include "IMSApp.h"
#include "AString.h"

class IImsAos;

/**
 * @brief This class provides the base class based on IMS Application for AoS interfaces.
 */

class ImsAosManager
    : public IMSApp
{
public:
    ImsAosManager(IN const AString& strAppName);
    virtual ~ImsAosManager();

    /// Implement child class
    virtual IImsAos* GetImsAos(IN const AString& strAppId, IN const AString& strServiceId);
    virtual IMSList<IImsAos*> GetImsAosList(IN const AString& strAppId,
            IN const AString& strServiceId);
    virtual IMSList<IImsAos*> GetImsAosList(IN const AString& strAppId);

    /// IMSApp Class
    virtual IMS_BOOL OnPreprocess(IN IMSMSG& objMSG);
    virtual IMS_BOOL OnMessage(IN IMSMSG& objMSG);
    virtual IMS_BOOL OnPostprocess(IN IMSMSG& objMSG);
};

#endif // _IMS_AOS_MANAGER_H_
