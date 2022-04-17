/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170117  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"

#include "TextParser.h"
#include "SIPHeader.h"
#include "SIPStackHelper.h"

namespace SIPStack
{

/*

Remarks

*/
GLOBAL IMS_BOOL DecodeHeaderComponent(IN CONST SipAddrSpec *pAddrSpec,
        OUT IMSList<ISIPHeader*> &objHeaders)
{
    //---------------------------------------------------------------------------------------------

    if (pAddrSpec == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (pAddrSpec->u.pSipUrl->pHeader == IMS_NULL)
    {
        return IMS_TRUE;
    }

    AString strHeaders = pAddrSpec->u.pSipUrl->pHeader;

    return DecodeHeaderComponent(strHeaders, objHeaders);
}

/*

Remarks

*/
GLOBAL IMS_BOOL DecodeHeaderComponent(IN CONST AString &strHeaders,
        OUT IMSList<ISIPHeader*> &objHeaders)
{
    IMSList<AString> objTokens = strHeaders.Split(TextParser::CHAR_AMPERSAND);

    //---------------------------------------------------------------------------------------------

    if (objTokens.IsEmpty())
    {
        return IMS_TRUE;
    }

    for (IMS_UINT32 i = 0; i < objTokens.GetSize(); ++i)
    {
        const AString &strHeader = objTokens.GetAt(i);
        IMS_SINT32 nPosOfEqual = strHeader.GetIndexOf(TextParser::CHAR_EQUAL);

        // Invalid header format
        if (nPosOfEqual == AString::NPOS)
        {
            return IMS_FALSE;
        }

        // No header value field
        if (nPosOfEqual == (strHeader.GetLength() - 1))
        {
            return IMS_FALSE;
        }

        SIPHeader *pHeader = new SIPHeader();

        if (pHeader != IMS_NULL)
        {
            pHeader->SetName(strHeader.GetSubStr(0, nPosOfEqual));
            pHeader->SetHeaderValue(
                    TextParser::DoPercentDecoding(strHeader.GetSubStr(nPosOfEqual + 1)));

            if (!objHeaders.Append(pHeader))
            {
                delete pHeader;
                return IMS_FALSE;
            }
        }
    }

    return IMS_TRUE;
}

}
