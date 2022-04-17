/*
 * ServiceHttpConnection.h
 *
 *  Created on: 2016. 4. 2.
 *      Author: changsuk.ro
 */

#ifndef SERVICEHTTPCONNECTION_H_
#define SERVICEHTTPCONNECTION_H_

#include "AString.h"

// platform
#include "IMSActivityEx.h"


class AndroidHttpConnectionManager;

class HttpConnectionService
    : public IMSActivityEx
{
private:
    HttpConnectionService();
    ~HttpConnectionService();

public:
    IHttpConnectionManager* CreateConnectionManager(IN CONST AString &strUserName, IN CONST AString &strUserPwd);
    void DestroyConnectionManager(IN IHttpConnectionManager *&pHttpConnectionManager);
    void DispatchServiceMessage(IN IMSMSG &objMsg);
    static HttpConnectionService* GetInstance();

protected:
    // IMSActivityEx
    virtual IMS_BOOL OnMessage(IN IMSMSG &objMSG);

private:
    IMS_SINT32 GetIndexOfHttpConnectionManager(IN AndroidHttpConnectionManager* pinAndroidHttpConnectionManager);

private:
    static HttpConnectionService *spHttpConnectionService;
    IMSList<AndroidHttpConnectionManager*> m_objHttpConnectionManagers;
};



#endif /* SERVICEHTTPCONNECTION_H_ */
