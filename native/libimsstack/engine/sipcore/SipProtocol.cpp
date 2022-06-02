/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091126  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "TextParser.h"
#include "SipPrivate.h"
#include "SipManager.h"
#include "SipConnectionNotifier.h"
#include "SipConnectionNotifierImpl.h"
#include "SipClientConnection.h"
#include "SipClientConnectionImpl.h"
#include "SipProtocol.h"

__IMS_TRACE_TAG_SIP__;

PRIVATE
SipProtocol::SipProtocol() :
        Protocol()
{
}

PUBLIC VIRTUAL SipProtocol::~SipProtocol() {}

/*
 Returns a singleton object of SIP protocol.

Remarks

*/
PUBLIC GLOBAL SipProtocol* SipProtocol::GetInstance()
{
    static SipProtocol* pSIP = IMS_NULL;

    //---------------------------------------------------------------------------------------------

    if (pSIP == IMS_NULL)
    {
        pSIP = new SipProtocol();
    }

    return pSIP;
}

/*
 Creates and opens a SIP Connection.

Remarks
 It throws the error as follows:
     ILLEGAL_ARGUMENT,
     CONNECTION_NOT_FOUND
*/
PRIVATE VIRTUAL IConnection* SipProtocol::OpenPrim(IN CONST AString& strName)
{
    AString strScheme;
    AString strTarget;
    AString strParams;

    //---------------------------------------------------------------------------------------------

    Protocol::ParseName(strName, strScheme, strTarget, strParams);

    return OpenPrim(strScheme, strTarget, strParams);
}

/*
 Creates and opens a SIP Connection.

Remarks
 It throws the error as follows:
     ILLEGAL_ARGUMENT,
     CONNECTION_NOT_FOUND
*/
PRIVATE VIRTUAL IConnection* SipProtocol::OpenPrim(
        IN CONST AString& strScheme, IN CONST AString& strTarget, IN CONST AString& strParams)
{
    IMS_SINT32 nScheme;

    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (strScheme.EqualsIgnoreCase(Sip::STR_SIP))
        nScheme = Sip::URI_SCHEME_SIP;
    else
        nScheme = Sip::URI_SCHEME_SIPS;

    // Check if it is SIP connection notifier or not
    if (strTarget.IsEmpty())
    {
        // SIP transaction notifier : dedicated mode (Port will be selected by the system)
        return CreateConnectionNotifier(nScheme, 0, strParams);
    }
    else if (strTarget.Equals(TextParser::CHAR_ASTERISK))
    {
        // SIP transaction notifier : shared mode
        return CreateConnectionNotifier(nScheme, 0, strParams, IMS_TRUE);
    }
    else
    {
        IMS_BOOL bOK = IMS_FALSE;
        IMS_SINT32 nPort = strTarget.ToInt32(&bOK);

        if (bOK == IMS_TRUE)
        {
            // SIP transaction notifier : dedicated mode
            return CreateConnectionNotifier(nScheme, nPort, strParams);
        }
        else
        {
            AString strURI = strScheme + TextParser::CHAR_COLON + strTarget;

            if (strParams.GetLength() > 0)
            {
                strURI.Prepend(TextParser::CHAR_LAQUOT);

                strURI += TextParser::CHAR_SEMICOLON + strParams;

                strURI.Append(TextParser::CHAR_RAQUOT);
            }

            // SIP client transaction
            SipClientConnection* pSCC = new SipClientConnection(strURI);

            if (pSCC == IMS_NULL)
            {
                SipPrivate::SetLastError(SipError::NO_MEMORY);

                IMS_TRACE_E(0, "Allocating SCC failed", 0, 0, 0);
                return IMS_NULL;
            }

            SipClientConnectionImpl* pSCCImpl = new SipClientConnectionImpl(pSCC);

            if (pSCCImpl == IMS_NULL)
            {
                delete pSCC;
                SipPrivate::SetLastError(SipError::NO_MEMORY);

                IMS_TRACE_E(0, "Allocating SCCImpl failed", 0, 0, 0);
                return IMS_NULL;
            }

            return pSCCImpl;
        }
    }
}

PRIVATE
IConnection* SipProtocol::CreateConnectionNotifier(IN IMS_SINT32 nScheme, IN IMS_SINT32 nPort,
        IN CONST AString& strParams, IN IMS_BOOL bSharedMode /* = IMS_FALSE */)
{
    SipConnectionNotifier* pSCN = new SipConnectionNotifier(nScheme, nPort, strParams, bSharedMode);

    //---------------------------------------------------------------------------------------------

    if (pSCN == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);

        IMS_TRACE_E(0, "Allocating SCN failed", 0, 0, 0);
        return IMS_NULL;
    }

    if (SipPrivate::GetLastError() != SipError::NO_ERROR)
    {
        delete pSCN;

        IMS_TRACE_E(0, "Parsing the parameters for SCN failed", 0, 0, 0);
        return IMS_NULL;
    }

    SipConnectionNotifierImpl* pSCNImpl = new SipConnectionNotifierImpl(pSCN);

    if (pSCNImpl == IMS_NULL)
    {
        delete pSCN;
        SipPrivate::SetLastError(SipError::NO_MEMORY);

        IMS_TRACE_E(0, "Allocating SCNImpl failed", 0, 0, 0);
        return IMS_NULL;
    }

    if (!SipManager::GetInstance()->AttachConnectionNotifier(pSCN))
    {
        delete pSCNImpl;

        IMS_TRACE_E(0, "Attaching SCN failed", 0, 0, 0);
        return IMS_NULL;
    }

    return pSCNImpl;
}
