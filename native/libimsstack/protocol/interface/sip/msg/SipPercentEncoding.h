#ifndef __SIP_PERCENT_ENCODING_H__
#define __SIP_PERCENT_ENCODING_H__

#include "sip_pf_datatypes.h"

class SipPercentEncoding
{

    public:
        static SIP_CHAR*  DoPercentDecoding(SIP_CHAR* pcString);
        static SIP_CHAR*  DoPercentDecoding(SIP_CHAR* pcString,SIP_INT32* piFinalLength);
        static SIP_CHAR*  DoPerEnc_UserAndHeader(SIP_CHAR* pcString, SIP_CHAR* pcType);
        static SIP_CHAR*  DoPerEnc_Password(SIP_CHAR* pcString);
        static SIP_CHAR*  DoPerEnc_Host(SIP_CHAR* pcString);
        static SIP_CHAR*  DoPerEnc_Param(SIP_CHAR* pcName, SIP_CHAR* pcValue);
        static SIP_CHAR*  DoPerEnc_TokenParam(SIP_CHAR* pcString);

        static SIP_CHAR*  DoPerEnc_MddrParam(SIP_CHAR* pcString);
        static SIP_CHAR*  DoPerEnc_TtlParam(SIP_CHAR* pcString);

        static SIP_CHAR*  DoPerEnc_OtherParam(SIP_CHAR* pcString);
};



#endif //__SIP_PERCENT_ENCODING_H__
