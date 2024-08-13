// Copyright 2024 Google LLC

#include "ServiceTrace.h"

#include "text/TextProfileUtil.h"
#include "text/TextSdpGenerator.h"

__IMS_TRACE_TAG_MEDIA__;

PUBLIC TextSdpGenerator::TextSdpGenerator() :
        SdpGenerator(MEDIA_TYPE_TEXT)
{
    IMS_TRACE_I("+TextSdpGenerator()", 0, 0, 0);
}

PUBLIC VIRTUAL TextSdpGenerator::~TextSdpGenerator()
{
    IMS_TRACE_I("~TextSdpGenerator()", 0, 0, 0);
}

PUBLIC
IMS_BOOL TextSdpGenerator::Generate(OUT ISessionDescriptor* pSessionDescriptor,
        OUT IMediaDescriptor* pDescriptor, IN MediaBaseProfile* pBaseProfile)
{
    if (pSessionDescriptor == IMS_NULL || pDescriptor == IMS_NULL || pBaseProfile == IMS_NULL)
    {
        return IMS_FALSE;
    }

    IMS_TRACE_I("Generate() - PayloadSize[%d]", pBaseProfile->GetPayloadList().GetSize(), 0, 0);

    GenerateCommonAttributes(pSessionDescriptor, pDescriptor, pBaseProfile);

    TextProfile* pProfile = static_cast<TextProfile*>(pBaseProfile);

    CheckRedPayloadSubTypeValidity(pProfile);
    GeneratePayload(pDescriptor, pProfile);
    GenerateDirection(pDescriptor, pProfile);

    return IMS_TRUE;
}

PRIVATE
void TextSdpGenerator::CheckRedPayloadSubTypeValidity(OUT TextProfile* pProfile)
{
    if (pProfile == IMS_NULL)
    {
        return;
    }

    for (IMS_UINT32 i = 0; i < pProfile->GetPayloadList().GetSize(); i++)
    {
        TextProfile::Payload* pPayload = pProfile->GetPayloadAt(i);

        if (pPayload == IMS_NULL)
        {
            continue;
        }

        if (pPayload->GetRtpMap().GetPayloadType().EqualsIgnoreCase("red"))
        {
            TextProfile::RedFmtp* pRedFmtp =
                    static_cast<TextProfile::RedFmtp*>(pPayload->GetFmtp());

            if (pRedFmtp == IMS_NULL)
            {
                continue;
            }

            IMS_TRACE_I("CheckRedPayloadSubTypeValidity() - fmtp, Redundancy Level [%d], Red "
                        "Payload[%d]",
                    pRedFmtp->GetRedLevel(), pRedFmtp->GetRedPayload(), 0);

            IMS_BOOL bRedSubPTExist = IMS_FALSE;

            for (IMS_UINT32 j = 0; j < pProfile->GetPayloadList().GetSize(); j++)
            {
                TextProfile::Payload* pTempPayload = pProfile->GetPayloadAt(j);

                if (pTempPayload == IMS_NULL)
                {
                    continue;
                }

                IMS_TRACE_I("CheckRedPayloadSubTypeValidity() - RedSubPT, PT[%d] of PL(%d) / Red "
                            "Payload [%d]",
                        pTempPayload->GetRtpMap().GetPayloadNumber(), j, pRedFmtp->GetRedPayload());

                if (pTempPayload->GetRtpMap().GetPayloadNumber() ==
                        (IMS_UINT32)pRedFmtp->GetRedPayload())
                {
                    bRedSubPTExist = IMS_TRUE;
                    continue;
                }
            }

            if (bRedSubPTExist == IMS_FALSE)
            {
                IMS_TRACE_E(0,
                        "CheckRedPayloadSubTypeValidity() - SubPayloadtype for Redundancy isn't "
                        "exist. skip Payload, Payload[%s], PT[%d]",
                        pPayload->GetRtpMap().GetPayloadType().GetStr(),
                        pPayload->GetRtpMap().GetPayloadNumber(), 0);
                pProfile->GetPayloadList().RemoveAt(i);
                delete pPayload;
            }
        }
    }

    IMS_TRACE_I("CheckRedPayloadSubTypeValidity() - After Checking Validity, PayloadSize[%d]",
            pProfile->GetPayloadList().GetSize(), 0, 0);
}

PRIVATE
void TextSdpGenerator::GeneratePayload(OUT IMediaDescriptor* pDescriptor, IN TextProfile* pProfile)
{
    if (pDescriptor == IMS_NULL || pProfile == IMS_NULL)
    {
        return;
    }

    for (IMS_UINT32 i = 0; i < pProfile->GetPayloadList().GetSize(); i++)
    {
        AString strRtpMap = AString::ConstNull();
        AString strPayloadNum = AString::ConstNull();
        AString strFmtp = AString::ConstNull();

        TextProfile::Payload* pPayload = pProfile->GetPayloadAt(i);
        if (pPayload == IMS_NULL)
        {
            continue;
        }

        GenerateRtpMap(strRtpMap, strPayloadNum, pPayload->GetRtpMap());

        if (GenerateFmtp(strFmtp, pPayload))
        {
            pDescriptor->SetMediaFormat(
                    SdpMediaFormat::TYPE_RTP, strPayloadNum, strRtpMap, strFmtp);
        }
    }
}

PRIVATE
void TextSdpGenerator::GenerateRtpMap(
        OUT AString& strRtpMap, OUT AString& strPayloadNum, IN MediaBaseProfile::RtpMap& objRtpMap)
{
    IMS_UINT32 nPayloadNumber = objRtpMap.GetPayloadNumber();
    AString strPayloadType = objRtpMap.GetPayloadType();
    IMS_UINT32 nSamplingRate = objRtpMap.GetSamplingRate();

    strPayloadNum.Sprintf("%d", nPayloadNumber);

    strRtpMap.Sprintf("%s/%d", strPayloadType.GetStr(), nSamplingRate);

    IMS_TRACE_D("GenerateRtpMap() - RtpMap [%d %s], ", nPayloadNumber, strRtpMap.GetStr(), 0);
}

PRIVATE
IMS_BOOL TextSdpGenerator::GenerateFmtp(OUT AString& strFmtp, IN TextProfile::Payload* pPayload)
{
    if (pPayload == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (pPayload->GetRtpMap().GetPayloadType().EqualsIgnoreCase("red"))
    {
        TextProfile::RedFmtp* pRedFmtp = (TextProfile::RedFmtp*)pPayload->GetFmtp();

        if (pRedFmtp == IMS_NULL)
        {
            return IMS_FALSE;
        }

        IMS_UINT32 nCount = pRedFmtp->GetRedLevel();
        AString TempSubPT;
        TempSubPT.Sprintf("%d", pRedFmtp->GetRedPayload());

        while (nCount-- > 0)
        {
            if (strFmtp.GetLength() > 0)
            {
                strFmtp.Append("/");
            }

            strFmtp.Append(TempSubPT);
        }

        IMS_TRACE_I("GenerateFmtp() - Add fmtp, nRedundancy[%d], Red Payload[%d], Fmtp[%s]",
                pRedFmtp->GetRedLevel(), pRedFmtp->GetRedPayload(), strFmtp.GetStr());
    }

    return IMS_TRUE;
}

PRIVATE
void TextSdpGenerator::GenerateDirection(
        OUT IMediaDescriptor* pDescriptor, IN TextProfile* pProfile)
{
    if (pDescriptor == IMS_NULL || pProfile == IMS_NULL)
    {
        return;
    }

    IMS_SINT32 nDirection = (IMS_SINT32)pProfile->GetDirection();

    pDescriptor->SetDirection(nDirection);
    IMS_TRACE_D("GenerateDirection() - direction[%d]", nDirection, 0, 0);
}
