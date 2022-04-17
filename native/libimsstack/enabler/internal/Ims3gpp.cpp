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

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "DomDocumentBuilderFactory.h"
#include "DocumentBuilderFactory.h"
#include "DocumentBuilder.h"
#include "IDocument.h"
#include "IElement.h"
#include "Ims3gpp.h"

__IMS_TRACE_TAG_USER_DECL__("Ims3gpp");

PUBLIC GLOBAL
const IMS_CHAR Ims3gpp::ELEMENT_ACTION[] = "action";
PUBLIC GLOBAL
const IMS_CHAR Ims3gpp::ELEMENT_ALTERNATIVE_SERVICE[] = "alternative-service";
PUBLIC GLOBAL
const IMS_CHAR Ims3gpp::ELEMENT_IMS_3GPP[] = "ims-3gpp";
PUBLIC GLOBAL
const IMS_CHAR Ims3gpp::ELEMENT_REASON[] = "reason";
PUBLIC GLOBAL
const IMS_CHAR Ims3gpp::ELEMENT_SERVICE_INFO[] = "service-info";
PUBLIC GLOBAL
const IMS_CHAR Ims3gpp::ELEMENT_TYPE[] = "type";
PUBLIC GLOBAL
const IMS_CHAR Ims3gpp::ATTR_VERSION[] = "version";

PUBLIC
Ims3gpp::Ims3gpp()
    : nType(TYPE_UNKNOWN)
{
}

PUBLIC
Ims3gpp::Ims3gpp(IN CONST AString &str3gppIms)
    : nType(TYPE_UNKNOWN)
{
    Parse(str3gppIms);
}

PUBLIC
Ims3gpp::~Ims3gpp()
{
}

/*

Remarks

*/
PUBLIC
const Ims3gpp::AlternativeService& Ims3gpp::GetAlternativeService() const
{
    return objAlternativeService;
}

/*

Remarks

*/
PUBLIC
const Ims3gpp::ServiceInfo& Ims3gpp::GetServiceInfo() const
{
    return objServiceInfo;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 Ims3gpp::GetType() const
{
    return nType;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL Ims3gpp::Parse(IN CONST AString &str3gppIms)
{
    DomDocumentBuilderFactory* pBuilderFactory = DomDocumentBuilderFactory::GetInstance();
    DocumentBuilder *pDocumentBuilder = pBuilderFactory->NewDocumentBuilder();

    if (pDocumentBuilder == IMS_NULL)
    {
        return IMS_FALSE;
    }

    IDocument *piDocument = pDocumentBuilder->Parse(str3gppIms);

    if (piDocument == IMS_NULL)
    {
        pBuilderFactory->DestroyDocumentBuilder(pDocumentBuilder);

        IMS_TRACE_E(0, "Parsing a '3gpp-ims' XML failed", 0, 0, 0);
        return IMS_FALSE;
    }

    IElement *piElement = piDocument->GetDocumentElement();

    if (piElement == IMS_NULL)
    {
        piDocument->DestroyDocument();
        pBuilderFactory->DestroyDocumentBuilder(pDocumentBuilder);

        IMS_TRACE_E(0, "No root element", 0, 0, 0);
        return IMS_FALSE;
    }

    const AString& strIms3gpp = piElement->GetTagName();

    if (!strIms3gpp.EqualsIgnoreCase(ELEMENT_IMS_3GPP))
    {
        IMS_TRACE_E(0, "Root element (%s) is not matched in '3gpp-ims'",
                strIms3gpp.GetStr(), 0, 0);

        piDocument->DestroyDocument();
        pBuilderFactory->DestroyDocumentBuilder(pDocumentBuilder);
        return IMS_FALSE;
    }

    // "version" attribute
    const AString& strVersion = piElement->GetAttribute(ATTR_VERSION);

    IMS_TRACE_D("'ims-3gpp' version is %s", strVersion.GetStr(), 0, 0);

    // "service-info" / "alternative-service" element
    INode *piNode = piElement->GetFirstChild();

    if (piNode != IMS_NULL)
    {
        const AString& strName = piNode->GetLocalName();

        if (strName.EqualsIgnoreCase(ELEMENT_ALTERNATIVE_SERVICE))
        {
            nType = TYPE_ALTERNATIVE_SERVICE;
            CreateAlternativeService(piNode);
        }
        else if (strName.EqualsIgnoreCase(ELEMENT_SERVICE_INFO))
        {
            nType = TYPE_SERVICE_INFO;
            CreateServiceInfo(piNode);
        }
        else
        {
            IMS_TRACE_D("ims-3gpp :: unknown element (%s)", strName.GetStr(), 0, 0);
        }
    }

    piDocument->DestroyDocument();
    pBuilderFactory->DestroyDocumentBuilder(pDocumentBuilder);

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
void Ims3gpp::CreateAlternativeService(IN INode *piNode)
{
    // Value of type
    const IMS_CHAR VALUE_EMERGENCY[] = "emergency";
    const IMS_CHAR VALUE_RESTORATION[] = "restoration";
    // Value of action
    const IMS_CHAR VALUE_EMERGENCY_REGISTRATION[] = "emergency-registration";
    const IMS_CHAR VALUE_INITIAL_REGISTRATION[] = "initial-registration";

    if (piNode == IMS_NULL)
    {
        return;
    }

    INode *piElement = piNode->GetFirstChild();

    while (piElement != IMS_NULL)
    {
        const AString& strName = piElement->GetLocalName();

        if (strName.EqualsIgnoreCase(ELEMENT_TYPE))
        {
            INode *piNode_Value = piElement->GetFirstChild();

            if (piNode_Value != IMS_NULL)
            {
                objAlternativeService.strType = piNode_Value->GetNodeValue();

                if (objAlternativeService.strType.Equals(VALUE_EMERGENCY))
                {
                    objAlternativeService.nType = AlternativeService::TYPE_EMERGENCY;
                }
                else if (objAlternativeService.strType.Equals(VALUE_RESTORATION))
                {
                    objAlternativeService.nType = AlternativeService::TYPE_RESTORATION;
                }
            }
        }
        else if (strName.EqualsIgnoreCase(ELEMENT_ACTION))
        {
            INode *piNode_Value = piElement->GetFirstChild();

            if (piNode_Value != IMS_NULL)
            {
                objAlternativeService.strAction = piNode_Value->GetNodeValue();

                if (objAlternativeService.strAction.Equals(VALUE_EMERGENCY_REGISTRATION))
                {
                    objAlternativeService.nAction
                            = AlternativeService::ACTION_EMERGENCY_REGISTRATION;
                }
                else if (objAlternativeService.strAction.Equals(VALUE_INITIAL_REGISTRATION))
                {
                    objAlternativeService.nAction
                            = AlternativeService::ACTION_INITIAL_REGISTRATION;
                }
            }
        }
        else if (strName.EqualsIgnoreCase(ELEMENT_REASON))
        {
            INode *piNode_Value = piElement->GetFirstChild();

            if (piNode_Value != IMS_NULL)
            {
                objAlternativeService.strReason = piNode_Value->GetNodeValue();
            }
        }

        piElement = piElement->GetNextSibling();
    }

    IMS_TRACE_I("ims-3gpp :: alternative-service :: type=%s, action=%s, reason=%s",
            objAlternativeService.strType.GetStr(),
            objAlternativeService.strAction.GetStr(),
            objAlternativeService.strReason.GetStr());
    IMS_TRACE_D("ims-3gpp :: alternative-service :: type=%d, action=%d",
            objAlternativeService.nType, objAlternativeService.nAction, 0);
}

/*

Remarks

*/
PRIVATE
void Ims3gpp::CreateServiceInfo(IN INode *piNode)
{
    if (piNode == IMS_NULL)
    {
        return;
    }

    INode *piNode_Value = piNode->GetFirstChild();

    if (piNode_Value != IMS_NULL)
    {
        objServiceInfo.strServiceInfo = piNode_Value->GetNodeValue();
    }

    IMS_TRACE_I("ims-3gpp :: service-info :: service-info=%s",
            objServiceInfo.strServiceInfo.GetStr(), 0, 0);
}
