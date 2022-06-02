/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_UNKNOWN_HEADERS_H_
#define _SIP_UNKNOWN_HEADERS_H_

#include "AString.h"

class SipUnknownHeaders
{
private:
    class Header
    {
    public:
        explicit Header(IN CONST AString& strName_);
        Header(IN CONST Header& objRHS);
        ~Header();

    public:
        Header& operator=(IN CONST Header& objRHS);

    public:
        void Clear();
        IMS_BOOL Equals(IN CONST AString& strName) const;

    private:
        void SetName(IN CONST AString& strName);

    public:
        AString strCompactName;
        AString strName;
        IMSList<AString> objBodys;
    };

public:
    SipUnknownHeaders();
    SipUnknownHeaders(IN CONST SipUnknownHeaders& objRHS);
    ~SipUnknownHeaders();

public:
    SipUnknownHeaders& operator=(IN CONST SipUnknownHeaders& objRHS);

public:
    IMS_RESULT AddHeader(IN CONST AString& strName, IN CONST AString& strBody);
    void Clear();
    IMS_SINT32 GetCount() const;
    AString GetHeader(IN CONST AString& strName, IN IMS_SINT32 nIndex = 0) const;
    AString GetHeaderBodys(IN IMS_SINT32 nPos) const;
    AString GetHeaderBodys(IN CONST AString& strName) const;
    IMS_SINT32 GetHeaderCount(IN CONST AString& strName) const;
    const AString& GetHeaderName(IN IMS_SINT32 nPos, IN IMS_BOOL bCompactForm = IMS_FALSE) const;
    IMSList<AString> GetHeaders(IN CONST AString& strName) const;
    IMS_BOOL IsHeaderPresent(IN CONST AString& strName) const;
    IMS_BOOL OverwriteHeaders(IN CONST SipUnknownHeaders& objOther);
    IMS_RESULT PrependHeader(IN CONST AString& strName, IN CONST AString& strBody);
    void RemoveHeader(IN CONST AString& strName);
    IMS_RESULT SetHeader(IN CONST AString& strName, IN CONST AString& strBody);

private:
    void DeleteHeader(IN CONST AString& strName);
    Header* FindHeader(IN CONST AString& strName) const;

private:
    IMSList<Header*> objHeaders;
};

#endif  // _SIP_UNKNOWN_HEADERS_H_
