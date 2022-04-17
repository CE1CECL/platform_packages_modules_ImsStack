/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20130213  hwangoo.park@             Created
    </table>

    Description
     This class defines the methods to get the information of 3GPP IM CN subsystem XML body.
*/

#ifndef _IMS_3GPP_H_
#define _IMS_3GPP_H_

#include "AString.h"

class INode;

class Ims3gpp
{
public:
    class AlternativeService
    {
    public:
        inline AlternativeService()
            : nType(TYPE_UNKNOWN)
            , strType(AString::ConstNull())
            , nAction(ACTION_UNKNOWN)
            , strAction(AString::ConstNull())
            , strReason(AString::ConstNull())
        {}
        inline ~AlternativeService()
        {}

    private:
        AlternativeService(IN CONST AlternativeService &objRHS);
        AlternativeService& operator=(IN CONST AlternativeService &objRHS);

    public:
        inline IMS_SINT32 GetAction() const
        { return nAction; }
        inline const AString& GetReason() const
        { return strReason; }
        inline IMS_SINT32 GetType() const
        { return nType; }
        inline const AString& GetUnknownAction() const
        { return strAction; }
        inline const AString& GetUnknownType() const
        { return strType; }

    public:
        // type in alternative-service
        enum
        {
            TYPE_UNKNOWN = 0,
            TYPE_EMERGENCY = 1,
            TYPE_RESTORATION = 2
        };

        // action in alternative-service
        enum
        {
            ACTION_UNKNOWN = 0,
            ACTION_EMERGENCY_REGISTRATION = 1,
            ACTION_INITIAL_REGISTRATION = 2
        };

    private:
        friend class Ims3gpp;

        IMS_SINT32 nType;
        AString strType;
        IMS_SINT32 nAction;
        AString strAction;
        AString strReason;
    };

    class ServiceInfo
    {
    public:
        inline ServiceInfo()
            : strServiceInfo(AString::ConstNull())
        {}
        inline ~ServiceInfo()
        {}

    private:
        ServiceInfo(IN CONST ServiceInfo &objRHS);
        ServiceInfo& operator=(IN CONST ServiceInfo &objRHS);

    public:
        inline const AString& GetServiceInfo() const
        { return strServiceInfo; }

    private:
        friend class Ims3gpp;

        AString strServiceInfo;
    };

public:
    Ims3gpp();
    explicit Ims3gpp(IN CONST AString &str3gppIms);
    ~Ims3gpp();

private:
    Ims3gpp(IN CONST Ims3gpp &objRHS);
    Ims3gpp& operator=(IN CONST Ims3gpp &objRHS);

public:
    const AlternativeService& GetAlternativeService() const;
    const ServiceInfo& GetServiceInfo() const;
    IMS_SINT32 GetType() const;

    IMS_BOOL Parse(IN CONST AString &str3gppIms);

private:
    void CreateAlternativeService(IN INode *piNode);
    void CreateServiceInfo(IN INode *piNode);

public:
    // "alternative-service" or "service-info"
    enum
    {
        TYPE_UNKNOWN = 0,
        TYPE_ALTERNATIVE_SERVICE = 1,
        TYPE_SERVICE_INFO = 2
    };

    static const IMS_CHAR ELEMENT_ACTION[];
    static const IMS_CHAR ELEMENT_ALTERNATIVE_SERVICE[];
    static const IMS_CHAR ELEMENT_IMS_3GPP[];
    static const IMS_CHAR ELEMENT_REASON[];
    static const IMS_CHAR ELEMENT_SERVICE_INFO[];
    static const IMS_CHAR ELEMENT_TYPE[];
    static const IMS_CHAR ATTR_VERSION[];

private:
    IMS_SINT32 nType;
    AlternativeService objAlternativeService;
    ServiceInfo objServiceInfo;
};

#endif // _IMS_3GPP_H_
