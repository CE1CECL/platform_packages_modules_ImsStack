/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100912  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "AStringBuffer.h"
#include "TextParser.h"
#include "ISipHeader.h"
#include "SipParameter.h"
#include "SipSecurityHeader.h"



PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MECHANISM_DIGEST[] = "digest";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MECHANISM_TLS[] = "tls";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MECHANISM_IPSEC_IKE[] = "ipsec-ike";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MECHANISM_IPSEC_MAN[] = "ipsec-man";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MECHANISM_IPSEC_3GPP[] = "ipsec-3gpp";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_ALG_HMAC_MD5_96[] = "hmac-md5-96";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_ALG_HMAC_SHA_1_96[] = "hmac-sha-1-96";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_PROT_AH[] = "ah";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_PROT_ESP[] = "esp";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MOD_TRANS[] = "trans";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MOD_TUN[] = "tun";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_MOD_UDP_ENC_TUN[] = "UDP-enc-tun";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_EALG_DES_EDE3_CBC[] = "des-ede3-cbc";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_EALG_AES_CBC[] = "aes-cbc";
PUBLIC GLOBAL
const IMS_CHAR SipSecurityHeader::P_VALUE_EALG_NULL[] = "null";

PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_PREFERENCE[] = "q";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_ALGORITHM[] = "alg";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_PROTOCOL[] = "prot";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_MODE[] = "mod";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_ENCRYPTION_ALGORITHM[] = "ealg";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_SPI_C[] = "spi-c";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_SPI_S[] = "spi-s";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_PORT_C[] = "port-c";
PRIVATE GLOBAL
const IMS_CHAR SipSecurityHeader::P_NAME_PORT_S[] = "port-s";



PUBLIC
SipSecurityHeader::SipSecurityHeader(IN IMS_SINT32 nMechanism_ /* = MECHANISM_IPSEC_3GPP */,
        IN CONST AString &strMechanism_ /* = AString::ConstNull() */,
        IN IMS_BOOL bParameterRequired_ /* = IMS_TRUE */)
    : bParameterRequired(bParameterRequired_)
    , nMechanism(nMechanism_)
    , strMechanism(strMechanism_)
    , strPreference(AString::ConstNull())
    , nAlgorithm(ALG_HMAC_SHA_1_96)
    , nProtocol(PROTOCOL_ESP)
    , nMode(MODE_TRANSPORT)
    , nEncryptionAlgorithm(EALG_NULL)
    , nSPI_C(0)
    , nSPI_S(0)
    , nPort_C(0)
    , nPort_S(0)
    , objUnknownParamValues(IMSMap<IMS_SINT32, AString>())
    , objExtensions(IMSMap<AString, AString>())
    , nMechParams(0)
    , bSPI_3GPP(IMS_TRUE)
{
}

PUBLIC
SipSecurityHeader::SipSecurityHeader(IN CONST SipSecurityHeader &objRHS)
    : bParameterRequired(objRHS.bParameterRequired)
    , nMechanism(objRHS.nMechanism)
    , strMechanism(objRHS.strMechanism)
    , strPreference(objRHS.strPreference)
    , nAlgorithm(objRHS.nAlgorithm)
    , nProtocol(objRHS.nProtocol)
    , nMode(objRHS.nMode)
    , nEncryptionAlgorithm(objRHS.nEncryptionAlgorithm)
    , nSPI_C(objRHS.nSPI_C)
    , nSPI_S(objRHS.nSPI_S)
    , nPort_C(objRHS.nPort_C)
    , nPort_S(objRHS.nPort_S)
    , objUnknownParamValues(objRHS.objUnknownParamValues)
    , objExtensions(objRHS.objExtensions)
    , nMechParams(objRHS.nMechParams)
    , bSPI_3GPP(objRHS.bSPI_3GPP)
{
}

PUBLIC
SipSecurityHeader::~SipSecurityHeader()
{
}

PUBLIC
SipSecurityHeader& SipSecurityHeader::operator=(IN CONST SipSecurityHeader &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        bParameterRequired = objRHS.bParameterRequired;

        nMechanism = objRHS.nMechanism;
        strMechanism = objRHS.strMechanism;
        strPreference = objRHS.strPreference;
        nAlgorithm = objRHS.nAlgorithm;
        nProtocol = objRHS.nProtocol;
        nMode = objRHS.nMode;
        nEncryptionAlgorithm = objRHS.nEncryptionAlgorithm;
        nSPI_C = objRHS.nSPI_C;
        nSPI_S = objRHS.nSPI_S;
        nPort_C = objRHS.nPort_C;
        nPort_S = objRHS.nPort_S;
        objUnknownParamValues = objRHS.objUnknownParamValues;
        objExtensions = objRHS.objExtensions;

        nMechParams = objRHS.nMechParams;

        bSPI_3GPP = objRHS.bSPI_3GPP;
    }

    return (*this);
}

PUBLIC
IMS_SINT32 SipSecurityHeader::GetMechanism() const
{
    //---------------------------------------------------------------------------------------------

    return nMechanism;
}

PUBLIC
const AString& SipSecurityHeader::GetUnknownMechanism() const
{
    //---------------------------------------------------------------------------------------------

    return strMechanism;
}

PUBLIC
const AString& SipSecurityHeader::GetPreference() const
{
    //---------------------------------------------------------------------------------------------

    return strPreference;
}

PUBLIC
IMS_SINT32 SipSecurityHeader::GetAlgorithm() const
{
    //---------------------------------------------------------------------------------------------

    return nAlgorithm;
}

PUBLIC
IMS_SINT32 SipSecurityHeader::GetEncryptionAlgorithm() const
{
    //---------------------------------------------------------------------------------------------

    return nEncryptionAlgorithm;
}

PUBLIC
IMS_SINT32 SipSecurityHeader::GetMode() const
{
    //---------------------------------------------------------------------------------------------

    return nMode;
}

PUBLIC
IMS_SINT32 SipSecurityHeader::GetPortC() const
{
    //---------------------------------------------------------------------------------------------

    return nPort_C;
}

PUBLIC
IMS_SINT32 SipSecurityHeader::GetPortS() const
{
    //---------------------------------------------------------------------------------------------

    return nPort_S;
}

PUBLIC
IMS_SINT32 SipSecurityHeader::GetProtocol() const
{
    //---------------------------------------------------------------------------------------------

    return nProtocol;
}

PUBLIC
IMS_UINT32 SipSecurityHeader::GetSpiC() const
{
    //---------------------------------------------------------------------------------------------

    return nSPI_C;
}

PUBLIC
IMS_UINT32 SipSecurityHeader::GetSpiS() const
{
    //---------------------------------------------------------------------------------------------

    return nSPI_S;
}

PUBLIC
const IMSMap<AString,AString>& SipSecurityHeader::GetExtensionParameters() const
{
    //---------------------------------------------------------------------------------------------

    return objExtensions;
}

PUBLIC
const AString& SipSecurityHeader::GetUnknownParameterValue(IN IMS_SINT32 nName) const
{
    IMS_SLONG nIndex = objUnknownParamValues.GetIndexOfKey(nName);

    //---------------------------------------------------------------------------------------------

    if (nIndex < 0)
    {
        return AString::ConstNull();
    }

    return objUnknownParamValues.GetValueAt(nIndex);
}

PUBLIC
const IMSMap<IMS_SINT32, AString>& SipSecurityHeader::GetUnknownParameterValues() const
{
    //---------------------------------------------------------------------------------------------

    return objUnknownParamValues;
}

PUBLIC
IMS_BOOL SipSecurityHeader::IsParameterPresent(IN IMS_SINT32 nParam) const
{
    //---------------------------------------------------------------------------------------------

    return ((nMechParams & nParam) != 0);
}

PUBLIC
void SipSecurityHeader::SetPreference(IN CONST AString &strPreference)
{
    //---------------------------------------------------------------------------------------------

    this->strPreference = strPreference;
}

PUBLIC
void SipSecurityHeader::SetAlgorithm(IN IMS_SINT32 nAlgorithm)
{
    //---------------------------------------------------------------------------------------------

    this->nAlgorithm = nAlgorithm;
}

PUBLIC
void SipSecurityHeader::SetEncryptionAlgorithm(IN IMS_SINT32 nEncryptionAlgorithm)
{
    //---------------------------------------------------------------------------------------------

    this->nEncryptionAlgorithm = nEncryptionAlgorithm;
}

PUBLIC
void SipSecurityHeader::SetMode(IN IMS_SINT32 nMode)
{
    //---------------------------------------------------------------------------------------------

    this->nMode = nMode;
}

PUBLIC
void SipSecurityHeader::SetPort(IN IMS_SINT32 nPortC, IN IMS_SINT32 nPortS)
{
    //---------------------------------------------------------------------------------------------

    this->nPort_C = nPortC;
    this->nPort_S = nPortS;

    if (nPortC >= 0)
    {
        nMechParams |= PORT_C;
    }
    else
    {
        nMechParams &= (~PORT_C);
    }

    if (nPortS >= 0)
    {
        nMechParams |= PORT_S;
    }
    else
    {
        nMechParams &= (~PORT_S);
    }
}

PUBLIC
void SipSecurityHeader::SetProtocol (IN IMS_SINT32 nProtocol)
{
    //---------------------------------------------------------------------------------------------

    this->nProtocol = nProtocol;
}

PUBLIC
void SipSecurityHeader::SetSpi(IN IMS_UINT32 nSPIC, IN IMS_UINT32 nSPIS,
        IN IMS_BOOL bSPI_3GPP /* = IMS_TRUE */)
{
    //---------------------------------------------------------------------------------------------

    this->nSPI_C = nSPIC;
    this->nSPI_S = nSPIS;
    this->bSPI_3GPP = bSPI_3GPP;

    nMechParams |= SPI_C;
    nMechParams |= SPI_S;
}

PUBLIC
void SipSecurityHeader::SetSpiOption(IN IMS_BOOL bSPI_3GPP)
{
    //---------------------------------------------------------------------------------------------

    this->bSPI_3GPP = bSPI_3GPP;
}

PUBLIC
IMS_BOOL SipSecurityHeader::SetExtensionParameter(IN CONST AString &strName,
        IN CONST AString &strValue)
{
    IMS_SLONG nIndex = objExtensions.GetIndexOfKey(strName);

    //---------------------------------------------------------------------------------------------

    if (nIndex < 0)
    {
        // new one
        return objExtensions.Add(strName, strValue);
    }

    return objExtensions.SetValueAt(nIndex, strValue);
}

PUBLIC
IMS_BOOL SipSecurityHeader::SetUnknownParameterValue(IN IMS_SINT32 nName,
        IN CONST AString &strValue)
{
    IMS_SLONG nIndex = objUnknownParamValues.GetIndexOfKey(nName);

    //---------------------------------------------------------------------------------------------

    if (nIndex < 0)
    {
        // new one
        return objUnknownParamValues.Add(nName, strValue);
    }

    return objUnknownParamValues.SetValueAt(nIndex, strValue);
}

PUBLIC
AString SipSecurityHeader::ToString() const
{
    AStringBuffer objBuffer(256);

    //---------------------------------------------------------------------------------------------

    if (nMechanism == MECHANISM_IPSEC_3GPP)
        objBuffer.Append(P_VALUE_MECHANISM_IPSEC_3GPP);
    else if (nMechanism == MECHANISM_DIGEST)
        objBuffer.Append(P_VALUE_MECHANISM_DIGEST);
    else if (nMechanism == MECHANISM_TLS)
        objBuffer.Append(P_VALUE_MECHANISM_TLS);
    else if (nMechanism == MECHANISM_IPSEC_IKE)
        objBuffer.Append(P_VALUE_MECHANISM_IPSEC_IKE);
    else if (nMechanism == MECHANISM_IPSEC_MAN)
        objBuffer.Append(P_VALUE_MECHANISM_IPSEC_MAN);
    else
        objBuffer.Append(strMechanism);

    if (!bParameterRequired)
    {
        // When the additional parameter is not required...
        return static_cast<const AStringBuffer&>(objBuffer).GetString();
    }

    // preference ('q')
    if (strPreference.GetLength() > 0)
    {
        objBuffer.Append(TextParser::CHAR_SEMICOLON);
        objBuffer.Append(P_NAME_PREFERENCE);
        objBuffer.Append(TextParser::CHAR_EQUAL);
        objBuffer.Append(strPreference);
    }

    // algorithm
    if (nAlgorithm != ALG_UNSPECIFIED)
    {
        AString strAlg;

        if (nAlgorithm == ALG_HMAC_MD5_96)
        {
            strAlg = P_VALUE_ALG_HMAC_MD5_96;
        }
        else if (nAlgorithm == ALG_HMAC_SHA_1_96)
        {
            strAlg = P_VALUE_ALG_HMAC_SHA_1_96;
        }
        else if (nAlgorithm == ALG_UNKNOWN)
        {
            strAlg = GetUnknownParameterValue(SEC_P_ALG);
        }

        if (strAlg.GetLength() > 0)
        {
            objBuffer.Append(TextParser::CHAR_SEMICOLON);
            objBuffer.Append(P_NAME_ALGORITHM);
            objBuffer.Append(TextParser::CHAR_EQUAL);
            objBuffer.Append(strAlg);
        }
    }

    // protocol
    if (nProtocol != PROTOCOL_UNSPECIFIED)
    {
        AString strProtocol;

        if (nProtocol == PROTOCOL_AH)
        {
            strProtocol = P_VALUE_PROT_AH;
        }
        else if (nProtocol == PROTOCOL_ESP)
        {
            strProtocol = P_VALUE_PROT_ESP;
        }
        else if (nProtocol == PROTOCOL_UNKNOWN)
        {
            strProtocol = GetUnknownParameterValue(SEC_P_PROTOCOL);
        }

        if (strProtocol.GetLength() > 0)
        {
            objBuffer.Append(TextParser::CHAR_SEMICOLON);
            objBuffer.Append(P_NAME_PROTOCOL);
            objBuffer.Append(TextParser::CHAR_EQUAL);
            objBuffer.Append(strProtocol);
        }
    }

    // mode
    if (nMode != MODE_UNSPECIFIED)
    {
        AString strMode;

        if (nMode == MODE_TUNNEL)
        {
            strMode = P_VALUE_MOD_TUN;
        }
        else if (nMode == MODE_TRANSPORT)
        {
            strMode = P_VALUE_MOD_TRANS;
        }
        else if (nMode == MODE_UDP_ENC_TUN)
        {
            strMode = P_VALUE_MOD_UDP_ENC_TUN;
        }
        else if (nMode == MODE_UNKNOWN)
        {
            strMode = GetUnknownParameterValue(SEC_P_MODE);
        }

        if (strMode.GetLength() > 0)
        {
            objBuffer.Append(TextParser::CHAR_SEMICOLON);
            objBuffer.Append(P_NAME_MODE);
            objBuffer.Append(TextParser::CHAR_EQUAL);
            objBuffer.Append(strMode);
        }
    }

    // encryption algorithm
    if (nEncryptionAlgorithm != EALG_UNSPECIFIED)
    {
        AString strEAlg;

        if (nEncryptionAlgorithm == EALG_DES_EDE3_CBC)
        {
            strEAlg = P_VALUE_EALG_DES_EDE3_CBC;
        }
        else if (nEncryptionAlgorithm == EALG_AES_CBC)
        {
            strEAlg = P_VALUE_EALG_AES_CBC;
        }
        else if (nEncryptionAlgorithm == EALG_NULL)
        {
            strEAlg = P_VALUE_EALG_NULL;
        }
        else if (nEncryptionAlgorithm == EALG_UNKNOWN)
        {
            strEAlg = GetUnknownParameterValue(SEC_P_EALG);
        }

        if (strEAlg.GetLength() > 0)
        {
            objBuffer.Append(TextParser::CHAR_SEMICOLON);
            objBuffer.Append(P_NAME_ENCRYPTION_ALGORITHM);
            objBuffer.Append(TextParser::CHAR_EQUAL);
            objBuffer.Append(strEAlg);
        }
    }

    // spi-c, spi-s
    AString strTmp;

    if (bSPI_3GPP)
    {
        if (IsParameterPresent(SPI_C))
        {
            strTmp.Sprintf(";spi-c=%010u", nSPI_C);
            objBuffer.Append(strTmp);
        }

        if (IsParameterPresent(SPI_S))
        {
            strTmp.Sprintf(";spi-s=%010u", nSPI_S);
            objBuffer.Append(strTmp);
        }
    }
    else
    {
        if (IsParameterPresent(SPI_C))
        {
            strTmp.Sprintf(";spi-c=%u", nSPI_C);
            objBuffer.Append(strTmp);
        }

        if (IsParameterPresent(SPI_S))
        {
            strTmp.Sprintf(";spi-s=%u", nSPI_S);
            objBuffer.Append(strTmp);
        }
    }

    // port-c, port-s
    if (IsParameterPresent(PORT_C))
    {
        strTmp.Sprintf(";port-c=%d", nPort_C);
        objBuffer.Append(strTmp);
    }

    if (IsParameterPresent(PORT_S))
    {
        strTmp.Sprintf(";port-s=%d", nPort_S);
        objBuffer.Append(strTmp);
    }

    // extensions
    for (IMS_UINT32 i = 0; i < objExtensions.GetSize(); ++i)
    {
        const AString &strName = objExtensions.GetKeyAt(i);
        const AString &strValue = objExtensions.GetValueAt(i);

        objBuffer.Append(TextParser::CHAR_SEMICOLON);
        objBuffer.Append(strName);

        if (strValue.GetLength() > 0)
        {
            objBuffer.Append(TextParser::CHAR_EQUAL);
            objBuffer.Append(strValue);
        }
    }

    return static_cast<const AStringBuffer&>(objBuffer).GetString();
}

PUBLIC GLOBAL
SipSecurityHeader* SipSecurityHeader::FromSipHeader(IN ISipHeader *piHeader)
{
    //---------------------------------------------------------------------------------------------

    if (piHeader == IMS_NULL)
    {
        return IMS_NULL;
    }

    IMS_SINT32 nType = piHeader->GetType();

    if ((nType != ISipHeader::SECURITY_CLIENT)
            && (nType != ISipHeader::SECURITY_SERVER)
            && (nType != ISipHeader::SECURITY_VERIFY))
    {
        return IMS_NULL;
    }

    IMS_SINT32 nMechanism = MECHANISM_UNKNOWN;
    const AString &strMechanism = piHeader->GetValue();

    if (strMechanism.GetLength() == 0)
    {
        return IMS_NULL;
    }

    if (strMechanism.EqualsIgnoreCase(P_VALUE_MECHANISM_IPSEC_3GPP))
        nMechanism = MECHANISM_IPSEC_3GPP;
    else if (strMechanism.EqualsIgnoreCase(P_VALUE_MECHANISM_DIGEST))
        nMechanism = MECHANISM_DIGEST;
    else if (strMechanism.EqualsIgnoreCase(P_VALUE_MECHANISM_TLS))
        nMechanism = MECHANISM_TLS;
    else if (strMechanism.EqualsIgnoreCase(P_VALUE_MECHANISM_IPSEC_IKE))
        nMechanism = MECHANISM_IPSEC_IKE;
    else if (strMechanism.EqualsIgnoreCase(P_VALUE_MECHANISM_IPSEC_MAN))
        nMechanism = MECHANISM_IPSEC_MAN;

    SipSecurityHeader *pSecurityHeader = new SipSecurityHeader(nMechanism, strMechanism);

    if (pSecurityHeader == IMS_NULL)
    {
        return IMS_NULL;
    }

    pSecurityHeader->nAlgorithm = ALG_UNSPECIFIED;
    pSecurityHeader->nProtocol = PROTOCOL_UNSPECIFIED;
    pSecurityHeader->nMode = MODE_UNSPECIFIED;
    pSecurityHeader->nEncryptionAlgorithm = EALG_UNSPECIFIED;

    const IMSList<SipParameter*> &objParameters = piHeader->GetParameters();

    for (IMS_UINT32 i = 0; i < objParameters.GetSize(); ++i)
    {
        const SipParameter *pParameter = objParameters.GetAt(i);
        const AString &strName = pParameter->GetName();
        const AString &strValue = pParameter->GetValue();

        // algorithm
        if (strName.EqualsIgnoreCase(P_NAME_ALGORITHM))
        {
            if (strValue.EqualsIgnoreCase(P_VALUE_ALG_HMAC_MD5_96))
                pSecurityHeader->nAlgorithm = ALG_HMAC_MD5_96;
            else if (strValue.EqualsIgnoreCase(P_VALUE_ALG_HMAC_SHA_1_96))
                pSecurityHeader->nAlgorithm = ALG_HMAC_SHA_1_96;
            else
            {
                pSecurityHeader->nAlgorithm = ALG_UNKNOWN;
                pSecurityHeader->SetUnknownParameterValue(SEC_P_ALG, strValue);
            }
        }
        // protocol
        else if (strName.EqualsIgnoreCase(P_NAME_PROTOCOL))
        {
            if (strValue.EqualsIgnoreCase(P_VALUE_PROT_AH))
                pSecurityHeader->nProtocol = PROTOCOL_AH;
            else if (strValue.EqualsIgnoreCase(P_VALUE_PROT_ESP))
                pSecurityHeader->nProtocol = PROTOCOL_ESP;
            else
            {
                pSecurityHeader->nProtocol = PROTOCOL_UNKNOWN;
                pSecurityHeader->SetUnknownParameterValue(SEC_P_PROTOCOL, strValue);
            }
        }
        // mode
        else if (strName.EqualsIgnoreCase(P_NAME_MODE))
        {
            if (strValue.EqualsIgnoreCase(P_VALUE_MOD_TUN))
                pSecurityHeader->nMode = MODE_TUNNEL;
            else if (strValue.EqualsIgnoreCase(P_VALUE_MOD_TRANS))
                pSecurityHeader->nMode = MODE_TRANSPORT;
            else if (strValue.EqualsIgnoreCase(P_VALUE_MOD_UDP_ENC_TUN))
                pSecurityHeader->nMode = MODE_UDP_ENC_TUN;
            else
            {
                pSecurityHeader->nMode = MODE_UNKNOWN;
                pSecurityHeader->SetUnknownParameterValue(SEC_P_MODE, strValue);
            }
        }
        // preference ('q')
        else if (strName.EqualsIgnoreCase(P_NAME_PREFERENCE))
        {
            pSecurityHeader->strPreference = pParameter->GetValue();
        }
        // encryption-algorithm
        else if (strName.EqualsIgnoreCase(P_NAME_ENCRYPTION_ALGORITHM))
        {
            if (strValue.EqualsIgnoreCase(P_VALUE_EALG_DES_EDE3_CBC))
                pSecurityHeader->nEncryptionAlgorithm = EALG_DES_EDE3_CBC;
            else if (strValue.EqualsIgnoreCase(P_VALUE_EALG_AES_CBC))
                pSecurityHeader->nEncryptionAlgorithm = EALG_AES_CBC;
            else if (strValue.EqualsIgnoreCase(P_VALUE_EALG_NULL))
                pSecurityHeader->nEncryptionAlgorithm = EALG_NULL;
            else
            {
                pSecurityHeader->nEncryptionAlgorithm = EALG_UNKNOWN;
                pSecurityHeader->SetUnknownParameterValue(SEC_P_EALG, strValue);
            }
        }
        // spi-c
        else if (strName.EqualsIgnoreCase(P_NAME_SPI_C))
        {
            pSecurityHeader->nSPI_C = pParameter->GetValue().ToUInt32();
            pSecurityHeader->nMechParams |= SPI_C;

            if (pParameter->GetValue().GetLength() < SPI_MAX_DIGITS)
            {
                pSecurityHeader->bSPI_3GPP = IMS_FALSE;
            }
        }
        // spi-s
        else if (strName.EqualsIgnoreCase(P_NAME_SPI_S))
        {
            pSecurityHeader->nSPI_S = pParameter->GetValue().ToUInt32();
            pSecurityHeader->nMechParams |= SPI_S;

            if (pParameter->GetValue().GetLength() < SPI_MAX_DIGITS)
            {
                pSecurityHeader->bSPI_3GPP = IMS_FALSE;
            }
        }
        // port-c
        else if (strName.EqualsIgnoreCase(P_NAME_PORT_C))
        {
            pSecurityHeader->nPort_C = pParameter->GetValue().ToInt32();
            pSecurityHeader->nMechParams |= PORT_C;
        }
        // port-s
        else if (strName.EqualsIgnoreCase(P_NAME_PORT_S))
        {
            pSecurityHeader->nPort_S = pParameter->GetValue().ToInt32();
            pSecurityHeader->nMechParams |= PORT_S;
        }
        // extensions
        else
        {
            pSecurityHeader->SetExtensionParameter(strName, pParameter->GetValue());
        }
    }

    return pSecurityHeader;
}
