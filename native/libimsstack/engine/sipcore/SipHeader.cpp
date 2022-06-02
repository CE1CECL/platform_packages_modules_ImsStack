/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description
     This class provides generic SIP header parser helper. It can be used to parse base string
    header values that are read from SIP message using e.g. SipConnection::GetHeader() method.
    It uses generic format to parse the header value and parameters following the syntax given
    in RFC 3261.
    - field-name: field-value *(;parameter-name=parameter-value)
    - auth-header-name: auth-scheme LWS auth-param *(COMMA auth-param)
*/

#include "ServiceMemory.h"
#include "AStringBuffer.h"
#include "SipPrivate.h"
#include "SipHeaderName.h"
#include "SipAddress.h"
#include "SipParameter.h"
#include "SipHeader.h"

// Constant Header Names
PUBLIC GLOBAL const IMS_CHAR* SipHeader::NAME[] = {
        SipHeaderName::ALLOW,
        SipHeaderName::ALLOW_EVENTS,
        SipHeaderName::AUTHORIZATION,
        SipHeaderName::CALL_ID,
        SipHeaderName::CONTACT,
        SipHeaderName::CONTACT,
        SipHeaderName::CONTACT,
        SipHeaderName::CONTENT_DISPOSITION,
        SipHeaderName::CONTENT_ENCODING,
        SipHeaderName::CONTENT_LENGTH,
        SipHeaderName::CONTENT_TYPE,
        SipHeaderName::CSEQ,
        SipHeaderName::EVENT,
        SipHeaderName::EXPIRES,
        SipHeaderName::EXPIRES,
        SipHeaderName::EXPIRES,
        SipHeaderName::ACCEPT,
        SipHeaderName::MIN_EXPIRES,
        SipHeaderName::FROM,
        SipHeaderName::MAX_FORWARDS,
        SipHeaderName::MIME_VERSION,
        SipHeaderName::PRIVACY,
        SipHeaderName::P_PREFERRED_IDENTITY,
        SipHeaderName::P_ASSERTED_IDENTITY,
        SipHeaderName::MIN_SE,
        SipHeaderName::PATH,
        SipHeaderName::P_ASSOCIATED_URI,
        SipHeaderName::P_CALLED_PARTY_ID,
        SipHeaderName::P_VISITED_NETWORK_ID,
        SipHeaderName::P_CHARGING_FUNCTION_ADDRESSES,
        SipHeaderName::P_ACCESS_NETWORK_INFO,
        SipHeaderName::P_CHARGING_VECTOR,
        SipHeaderName::SERVICE_ROUTE,
        SipHeaderName::HISTORY_INFO,
        SipHeaderName::REQUEST_DISPOSITION,
        SipHeaderName::ACCEPT_CONTACT,
        SipHeaderName::REJECT_CONTACT,
        SipHeaderName::JOIN,
        SipHeaderName::SIP_IF_MATCH,
        SipHeaderName::SIP_ETAG,
        SipHeaderName::PROXY_AUTHENTICATE,
        SipHeaderName::PROXY_AUTHORIZATION,
        SipHeaderName::RACK,
        SipHeaderName::RECORD_ROUTE,
        SipHeaderName::REFERRED_BY,
        SipHeaderName::REFER_TO,
        SipHeaderName::REPLACES,
        SipHeaderName::REQUIRE,
        SipHeaderName::ROUTE,
        SipHeaderName::RSEQ,
        SipHeaderName::SECURITY_CLIENT,
        SipHeaderName::SECURITY_VERIFY,
        SipHeaderName::SECURITY_SERVER,
        SipHeaderName::SESSION_EXPIRES,
        SipHeaderName::SUBSCRIPTION_STATE,
        SipHeaderName::SUPPORTED,
        SipHeaderName::TIMESTAMP,
        SipHeaderName::TO,
        SipHeaderName::UNSUPPORTED,
        SipHeaderName::VIA,
        SipHeaderName::WARNING,
        SipHeaderName::WWW_AUTHENTICATE,
        IMS_NULL,
        SipHeaderName::RETRY_AFTER,
        SipHeaderName::RETRY_AFTER,
        SipHeaderName::RETRY_AFTER,
        SipHeaderName::P_EARLY_MEDIA,
        SipHeaderName::RESOURCE_PRIORITY,
        SipHeaderName::ACCEPT_RESOURCE_PRIORITY,
        SipHeaderName::DATE,
        IMS_NULL,
};

/*
 Constructs a new SipHeader without any values.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC
SipHeader::SipHeader() :
        nType(ISipHeader::ANY),
        strName(AString::ConstNull()),
        strBody(AString::ConstNull()),
        pAddress(IMS_NULL)
{
}

/*
 Constructs a new SIPHeaader with the type only.

Remarks

Parameters
<table>
parameter               description
----------              ----------
nType_                  The header type
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC
SipHeader::SipHeader(IN IMS_SINT32 nType_) :
        nType(nType_),
        strName(SipStack::GetHeaderNameFromType(nType_)),
        strBody(AString::ConstNull()),
        pAddress(IMS_NULL)
{
}

/*
 Constructs a new SIPHeaader with the name only.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strName_                The header name
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC
SipHeader::SipHeader(IN CONST AString& strName_) :
        nType(ISipHeader::ANY),
        strName(strName_),
        strBody(AString::ConstNull()),
        pAddress(IMS_NULL)
{
    //---------------------------------------------------------------------------------------------

    nType = SipStack::GetHeaderTypeFromName(strName);
}

/*
 Constructs a new SipHeader from the pointer to SipHeaderBase structure.

Remarks

Parameters
<table>
parameter               description
----------              ----------
pstHeader               Pointer to SipHeaderBase (which is defined by SIP stack)
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC
SipHeader::SipHeader(IN CONST SipHeaderBase* pstHeader) :
        nType(ISipHeader::ANY),
        strName(AString::ConstNull()),
        strBody(AString::ConstNull()),
        pAddress(IMS_NULL)
{
    //---------------------------------------------------------------------------------------------

    nType = SipStack::GetHeaderType(pstHeader);

    if (nType == ISipHeader::UNKNOWN)
    {
        strName = SipStack::GetUnknownHeaderName(const_cast<SipHeaderBase*>(pstHeader));
    }
    else
    {
        strName = SipStack::GetHeaderNameFromType(nType);
    }

    SipStack::EncodeHeaderBody(pstHeader, IMS_FALSE, strBody);

    // If the header type is unknown, decode the body according to the general syntax rule...
    if (nType == ISipHeader::UNKNOWN)
    {
        ParseUnknownBody(strBody);
    }

    if (SipStack::IsAddressFormatHeader(nType, strName))
    {
        pAddress = new SipAddress(strBody);
    }

    // P-Preferred-Identity & P-Asserted-Identity
    //    : name-addr / addr-spec -> no header parameters
    if ((nType != ISipHeader::P_PREFERRED_IDENTITY) && (nType != ISipHeader::P_ASSERTED_IDENTITY) &&
            (nType != ISipHeader::UNKNOWN))
    {
        objParams = SipStack::ExtractParameters(const_cast<SipHeaderBase*>(pstHeader));
    }
}

/*
 Destructs a SipHeader instance.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC VIRTUAL SipHeader::~SipHeader()
{
    //---------------------------------------------------------------------------------------------

    if (pAddress != IMS_NULL)
        delete pAddress;

    if (!objParams.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
        {
            SipParameter* pParameter = objParams.GetAt(i);

            if (pParameter != IMS_NULL)
                delete pParameter;
        }

        objParams.Clear();
    }
}

/*
 Destroys the SipHeader instance.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC VIRTUAL void SipHeader::Destroy()
{
    //---------------------------------------------------------------------------------------------

    delete this;
}

/*
 Clones and returns a SipHeader.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
new SipHeader           Pointer to a new SipHeader
</table>
*/
PUBLIC VIRTUAL ISipHeader* SipHeader::Clone() const
{
    SipHeader* pHeader = new SipHeader(nType);

    //---------------------------------------------------------------------------------------------

    if (pHeader == IMS_NULL)
    {
        return IMS_NULL;
    }

    pHeader->strName = strName;
    pHeader->strBody = strBody;

    if (pAddress != IMS_NULL)
    {
        pHeader->pAddress = new SipAddress(*pAddress);
    }

    for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
    {
        const SipParameter* pParameter = objParams.GetAt(i);
        SipParameter* pNewParameter = new SipParameter(*pParameter);

        if (pNewParameter != IMS_NULL)
            pHeader->objParams.Append(pNewParameter);
    }

    return pHeader;
}

/*
 Checks if the given SipHeader is the same.

Remarks

Parameters
<table>
parameter               description
----------              ----------
piHeader                Pointer to ISipHeader which is compared
</table>

Returns
<table>
return                  description
----------              ----------
IMS_TRUE                If both SIP headers matched, this value returns
IMS_FALSE               If both SIP headers are not matched, this value returns
</table>
*/
PUBLIC VIRTUAL IMS_BOOL SipHeader::Equals(IN CONST ISipHeader* piHeader) const
{
    const SipHeader* pHeader = DYNAMIC_CAST(const SipHeader*, piHeader);

    //---------------------------------------------------------------------------------------------

    if (pHeader == IMS_NULL)
        return IMS_FALSE;

    if (nType != pHeader->nType)
        return IMS_FALSE;

    if ((nType == ISipHeader::UNKNOWN) && (pHeader->nType == ISipHeader::UNKNOWN))
    {
        const IMS_CHAR* pszFName = SipStack::GetHeaderName(nType, strName);
        const IMS_CHAR* pszOtherFName = SipStack::GetHeaderName(pHeader->nType, pHeader->strName);

        if (AString::CompareIgnoreCase(pszFName, pszOtherFName) != 0)
            return IMS_FALSE;
    }

    // Compare the header value field
    if (SipStack::IsAddressFormatHeader(nType, strName))
    {
        SipAddress objAddress(strBody);
        SipAddress objOtherAddress(pHeader->strBody);

        if (!objAddress.Equals(objOtherAddress))
            return IMS_FALSE;
    }
    else
    {
        if (!strBody.EqualsIgnoreCase(pHeader->strBody))
            return IMS_FALSE;
    }

    // Compare the header parameters
    if (objParams.GetSize() != pHeader->objParams.GetSize())
        return IMS_FALSE;

    // TODO:: comparison of parameter fields

    return IMS_TRUE;
}

/*
 Returns the pointer to SipAddress if this header is a format of SIP address.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
SipAddress*             If the header is a format of SIP address, it returns this value
NULL pointer            If the header is not a format of SIP address, it returns NULL pointer
</table>
*/
PUBLIC VIRTUAL const SipAddress* SipHeader::GetSipAddress() const
{
    //---------------------------------------------------------------------------------------------

    return pAddress;
}

/*
 Returns the full header value including the header parameters.
For example,
    Bruce <sip:bruce@ims.com>;tag=IMS_1928301774

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
Non-NULL                The full header value including the header parameters
Empty or NULL           If the header has only a header name
</table>
*/
PUBLIC VIRTUAL AString SipHeader::GetHeaderValue() const
{
    //---------------------------------------------------------------------------------------------

    if (objParams.IsEmpty())
        return strBody;

    AString strHeaderValue(strBody);

    for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
    {
        const SipParameter* pParameter = objParams.GetAt(i);

        if (pParameter != IMS_NULL)
        {
            strHeaderValue += TextParser::CHAR_SEMICOLON + pParameter->ToString();
        }
    }

    return strHeaderValue;
}

/*
 Returns the header name of this SipHeader.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
String                  The header name of this SipHeader
</table>
*/
PUBLIC VIRTUAL const AString& SipHeader::GetName() const
{
    //---------------------------------------------------------------------------------------------

    return strName;
}

/*
 Returns the pointer to SipParameter of one header parameter.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strName                 The parameter name to be returned
</table>

Returns
<table>
return                  description
----------              ----------
SipParameter*           Parameter to the specified parameter name
</table>
*/
PUBLIC VIRTUAL const SipParameter* SipHeader::GetParameter(IN CONST AString& strName) const
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
    {
        const SipParameter* pParameter = objParams.GetAt(i);

        if (pParameter->GetName().EqualsIgnoreCase(strName))
            return pParameter;
    }

    return IMS_NULL;
}

/*
 Returns the names of all the header parameters. If there are no header parameters,
the method returns an empty list.

Remarks

Parameters
<table>
parameter               description
----------              ----------
objPNames               Names of the header parameters
</table>

Returns
<table>
return                  description
----------              ----------
IMS_SUCCESS             The operation is successfully done
IMS_FAILURE             An error occurs
</table>
*/
PUBLIC VIRTUAL IMS_RESULT SipHeader::GetParameterNames(OUT IMSList<AString>& objPNames) const
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
    {
        const SipParameter* pParameter = objParams.GetAt(i);

        if (!objPNames.Append(pParameter->GetName()))
        {
            SipPrivate::SetLastError(SipError::LIST_OPERATION_FAILED);
            return IMS_FAILURE;
        }
    }

    SipPrivate::SetLastError(SipError::NO_ERROR);
    return IMS_SUCCESS;
}

/*
 Returns the list of all the header parameters in this SipHeader.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
List of SipParameter    The list of SipParameter for this SipHeader
</table>
*/
PUBLIC VIRTUAL const IMSList<SipParameter*>& SipHeader::GetParameters() const
{
    //---------------------------------------------------------------------------------------------

    return objParams;
}

/*
 Returns the enumeration type of this SipHeader which corresponds to the header name.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
Type of header          Enumeration type of this SipHeader
</table>
*/
PUBLIC VIRTUAL IMS_SINT32 SipHeader::GetType() const
{
    //---------------------------------------------------------------------------------------------

    return nType;
}

/*
 Returns the header value without header parameters. For example,
for header <sip:user@192.168.200.201>;expires=3600,
this method returns <sip:user@192.168.200.201>.
In the case of an authorization or authentication header,
it returns only the authentication scheme e.g. "Digest".

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
Header body             The header value without header parameters
                        It is an empty string if the value was set to be NULL or empty string
</table>
*/
PUBLIC VIRTUAL const AString& SipHeader::GetValue() const
{
    //---------------------------------------------------------------------------------------------

    return strBody;
}

/*
 Returns the header value without header parameters. It is the same method with GetValue()
except for returning an integer value for the header value.

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
Header body             The header value without header parameters
                        It is -1 if the value cannot have an integer value
</table>
*/
PUBLIC VIRTUAL IMS_SINT32 SipHeader::GetValueInt() const
{
    //---------------------------------------------------------------------------------------------

    switch (nType)
    {
        case ISipHeader::CONTENT_LENGTH:
        case ISipHeader::EXPIRES_SEC:
        case ISipHeader::EXPIRES_ANY:
        case ISipHeader::MIN_EXPIRES:
        case ISipHeader::MAX_FORWARDS:
        case ISipHeader::MIN_SE:
        case ISipHeader::RETRY_AFTER_SEC:
        case ISipHeader::RETRY_AFTER_ANY:
        case ISipHeader::RSEQ:
        case ISipHeader::SESSION_EXPIRES:
        {
            IMS_BOOL bOK = IMS_FALSE;
            IMS_SINT32 nValue = strBody.ToInt32(&bOK);

            if (!bOK)
            {
                break;
            }

            return nValue;
        }
        default:
            break;
    }

    return (-1);
}

/*
 Removes the header parameter if it is found in this SipHeader.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strName                 Name of the header parameter
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC VIRTUAL void SipHeader::RemoveParameter(IN CONST AString& strName)
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
    {
        SipParameter* pParameter = objParams.GetAt(i);

        if (pParameter != IMS_NULL)
        {
            if (pParameter->GetName().Equals(strName))
            {
                objParams.RemoveAt(i);
                delete pParameter;
                return;
            }
        }
    }
}

/*
 Sets the header name, for example "Contact". If the argument has a leading
and trailing white spaces, those are ignored.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strName                 Name of the header
</table>

Returns
<table>
return                  description
----------              ----------
</table>
*/
PUBLIC VIRTUAL void SipHeader::SetName(IN CONST AString& strName)
{
    //---------------------------------------------------------------------------------------------

    this->strName = strName.Trim();

    nType = SipStack::GetHeaderTypeFromName(this->strName);
}

/*
 Sets the value of header parameter. If the parameter does not exist, it will be added.
For example, for header value "<sip:user@192.168.200.201>" calling
SetParameter("expires", "3600") will construct header value
"<sip:user@192.168.200.201>;expires=3600".
If the value is NULL or empty string, the parameter is interpreted
as a parameter without value.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strName                 Name of the header parameter
strValue                Value of the header parameter (NULL or empty string is allowed)
</table>

Returns
<table>
return                  description
----------              ----------
IMS_SUCCESS             The header parameter is successfully set
IMS_FAILURE             An error occurs
</table>
*/
PUBLIC VIRTUAL IMS_RESULT SipHeader::SetParameter(
        IN CONST AString& strName, IN CONST AString& strValue)
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
    {
        SipParameter* pParameter = objParams.GetAt(i);

        if (pParameter->GetName().EqualsIgnoreCase(strName))
        {
            return pParameter->SetValue(strValue);
        }
    }

    // Now, no existing parameter in the parameter list

    SipParameter* pParameter = new SipParameter(strName, strValue);

    if (pParameter == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);
        return IMS_FAILURE;
    }

    if (!objParams.Append(pParameter))
    {
        delete pParameter;

        SipPrivate::SetLastError(SipError::LIST_OPERATION_FAILED);
        return IMS_FAILURE;
    }

    SipPrivate::SetLastError(SipError::NO_ERROR);
    return IMS_SUCCESS;
}

/*
 Sets the header value as string with any header parameters.
If the header value is an empty string or NULL, it means a header with no value.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strHeaderValue          The full header value
</table>

Returns
<table>
return                  description
----------              ----------
IMS_SUCCESS             The full header value is successfully set
IMS_FAILURE             An error occurs
</table>
*/
PUBLIC VIRTUAL IMS_RESULT SipHeader::SetHeaderValue(IN CONST AString& strHeaderValue)
{
    //---------------------------------------------------------------------------------------------

    if (!Decode(strHeaderValue))
        return IMS_FAILURE;

    return IMS_SUCCESS;
}

/*
 Sets the header value as string without parameters. For example,
for the header "<sip:user@192.168.200.201>", the existing (if any) header parameter values
are not modified.
If the header value is an empty string or NULL, it means a header with no value.

For an authorization and authentication header, this method sets the authentication scheme
e.g. "Digest".

Remarks

Parameters
<table>
parameter               description
----------              ----------
strValue                The value of the header
</table>

Returns
<table>
return                  description
----------              ----------
IMS_SUCCESS             The header value is successfully set
IMS_FAILURE             An error occurs
</table>
*/
PUBLIC VIRTUAL IMS_RESULT SipHeader::SetValue(IN CONST AString& strValue)
{
    //---------------------------------------------------------------------------------------------

    if (!Decode(strValue, IMS_FALSE))
        return IMS_FAILURE;

    return IMS_SUCCESS;
}

/*
 Sets the header value as string without parameters. It is the same method with SetValue(...)
except for setting an integer value for the header value.
If the header value cannot have an integer value, it fails.

Remarks

Parameters
<table>
parameter               description
----------              ----------
nValue                  The value of the header
</table>

Returns
<table>
return                  description
----------              ----------
IMS_SUCCESS             The header value is successfully set
IMS_FAILURE             An error occurs
</table>
*/
PUBLIC VIRTUAL IMS_RESULT SipHeader::SetValueInt(IN IMS_SINT32 nValue)
{
    //---------------------------------------------------------------------------------------------

    switch (nType)
    {
        case ISipHeader::CONTENT_LENGTH:
        case ISipHeader::EXPIRES_SEC:
        case ISipHeader::EXPIRES_ANY:
        case ISipHeader::MIN_EXPIRES:
        case ISipHeader::MAX_FORWARDS:
        case ISipHeader::MIN_SE:
        case ISipHeader::RSEQ:
        case ISipHeader::SESSION_EXPIRES:
        {
            if (nValue >= 0)
                strBody.SetNumber(nValue);

            return IMS_SUCCESS;
        }
        default:
            break;
    }

    return IMS_FAILURE;
}

/*
 Returns the string representation of the header according to header type.
For example,
    - From: Bruce < sip:bruce@ims.com>;tag=192168200201
    - WWW-Authenticate: Digest realm="ims.com", domain="sip:ims.com", auth="auth",
         nonce="f84f1cec41e6cbe5aea9c8e88d359", opaque ="", stale=FALSE, algorithm=MD5

 The value part of the header may be missing if the header was created with empty string
or NULL as strValue and has not been set using SetValue().

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
String                  String representation of this SipHeader
</table>
*/
PUBLIC VIRTUAL AString SipHeader::ToString() const
{
    AString strHeader;

    //---------------------------------------------------------------------------------------------

    strHeader.Append(strName);
    strHeader.Append(TextParser::CHAR_COLON);
    strHeader.Append(TextParser::CHAR_SP);
    strHeader.Append(ToStringWithoutName());

    return strHeader;
}

/*
 Returns the string representation of the header according to header type without header name.
For example,
    - From: Bruce < sip:bruce@ims.com>;tag=192168200201
       => Bruce < sip:bruce@ims.com>;tag=192168200201
    - WWW-Authenticate: Digest realm="ims.com", domain="sip:ims.com", auth="auth",
        nonce="f84f1cec41e6cbe5aea9c8e88d359", opaque ="", stale=FALSE, algorithm=MD5
       => Digest realm="ims.com", domain="sip:ims.com", auth="auth",
        nonce="f84f1cec41e6cbe5aea9c8e88d359", opaque ="", stale=FALSE, algorithm=MD5

 The value part of the header may be missing if the header was created with empty string
or NULL as strValue and has not been set using SetValue().

Remarks

Parameters
<table>
parameter               description
----------              ----------
</table>

Returns
<table>
return                  description
----------              ----------
String                  String representation of this SipHeader without header name
</table>
*/
PUBLIC VIRTUAL AString SipHeader::ToStringWithoutName() const
{
    AStringBuffer objHeaderValue(512);

    //---------------------------------------------------------------------------------------------

    if (pAddress != IMS_NULL)
    {
        objHeaderValue.Append(pAddress->ToString());

        if (!objParams.IsEmpty())
        {
            const AString& strTemp = static_cast<const AStringBuffer&>(objHeaderValue).GetString();

            if (strTemp.Contains('<') && strTemp.Contains('>'))
            {
                // no-op
            }
            else
            {
                objHeaderValue.Prepend('<');
                objHeaderValue.Append('>');
            }
        }
    }
    else
    {
        objHeaderValue.Append(strBody);
    }

    for (IMS_UINT32 i = 0; i < objParams.GetSize(); ++i)
    {
        const SipParameter* pParameter = objParams.GetAt(i);

        if (pParameter != IMS_NULL)
        {
            objHeaderValue.Append(TextParser::CHAR_SEMICOLON);
            // Do not add the white space...
            // strHeader.Append(TextParser::CHAR_SP);
            objHeaderValue.Append(pParameter->ToString());
        }
    }

    if (nType == PRIVACY)
    {
        // Remove the semi-colon; for the microsip stack consistency
        objHeaderValue.Erase(0, 1);
    }

    return static_cast<const AStringBuffer&>(objHeaderValue).GetString();
}

/*
 Checks if the header type is valid.

Remarks

Parameters
<table>
parameter               description
----------              ----------
nType                   The header type
</table>

Returns
<table>
return                  description
----------              ----------
IMS_TRUE                The header type is valid
IMS_FALSE               The header type is not valid
</table>
*/
PUBLIC GLOBAL IMS_BOOL SipHeader::IsValidType(IN IMS_SINT32 nType)
{
    //---------------------------------------------------------------------------------------------

    if ((nType > ISipHeader::INVALID) && (nType < ISipHeader::ANY))
        return IMS_TRUE;

    return IMS_FALSE;
}

/*
 Parses the header body field.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strBody_                The header body of SIP header
bParseParameter         Flag to indicate if the header parameter needs to be parsed
</table>

Returns
<table>
return                  description
----------              ----------
IMS_TRUE                The SIP header is successfully parsed
IMS_FALSE               An error occurs during parsing
</table>
*/
PRIVATE
IMS_BOOL SipHeader::Decode(IN CONST AString& strBody_, IN IMS_BOOL bParseParameter /* = IMS_TRUE */)
{
    AString strTrimmedBody = strBody_.Trim();

    //---------------------------------------------------------------------------------------------

    if (strTrimmedBody.IsEmpty() || strTrimmedBody.IsNULL())
    {
        strBody = strTrimmedBody;

        SipPrivate::SetLastError(SipError::NO_ERROR);
        return IMS_TRUE;
    }

    SipHeaderBase* pstHeader = SipStack::DecodeHeader(nType, strName, strTrimmedBody);

    if (pstHeader == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::PARSING_ERROR);
        return IMS_FALSE;
    }

    // Process the header type which has an ANY type: Contact, Expires, Retry-After
    nType = SipStack::GetHeaderType(pstHeader);

    SipStack::EncodeHeaderBody(pstHeader, IMS_FALSE, strBody);

    // If the header type is unknown, decode the body according to the general syntax rule...
    if (nType == ISipHeader::UNKNOWN)
    {
        ParseUnknownBody(strBody);
    }

    if (pAddress != IMS_NULL)
    {
        delete pAddress;
        pAddress = IMS_NULL;
    }

    if (SipStack::IsAddressFormatHeader(nType, strName))
    {
        pAddress = new SipAddress(strBody);

        if (SipStack::IsAquotRequiredForAddressFormat(nType, strName))
        {
            pAddress->SetAquotRequired(IMS_TRUE);
        }
    }

    // P-Preferred-Identity & P-Asserted-Identity
    //    : name-addr / addr-spec -> no header parameters
    if ((bParseParameter == IMS_TRUE) && (nType != ISipHeader::P_PREFERRED_IDENTITY) &&
            (nType != ISipHeader::P_ASSERTED_IDENTITY) && (nType != ISipHeader::UNKNOWN))
    {
        objParams = SipStack::ExtractParameters(pstHeader);
    }

    SipStack::FreeHeaderEx(pstHeader);

    SipPrivate::SetLastError(SipError::NO_ERROR);
    return IMS_TRUE;
}

/*
 Parses the unknown header body field.

Remarks

Parameters
<table>
parameter               description
----------              ----------
strBody_                The header body of SIP header
</table>

Returns
<table>
return                  description
----------              ----------
IMS_TRUE                The SIP header is successfully parsed
IMS_FALSE               An error occurs during parsing
</table>
*/
PRIVATE
IMS_BOOL SipHeader::ParseUnknownBody(IN CONST AString& strBody_)
{
    // Find ';'
    IMS_SINT32 nSemiColon = TextParser::GetIndexOfDelimiter(strBody_, TextParser::CHAR_SEMICOLON);

    //---------------------------------------------------------------------------------------------

    // No parameters
    if (nSemiColon == AString::NPOS)
    {
        strBody = strBody_;
        return IMS_TRUE;
    }

    objParams = SipStack::ExtractParameters(
            strBody_.GetSubStr(nSemiColon + 1), TextParser::CHAR_SEMICOLON);

    strBody = strBody_.GetSubStr(0, nSemiColon).Trim();

    return IMS_TRUE;
}
