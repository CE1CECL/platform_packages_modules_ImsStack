/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170117  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_STACK_HELPER_SIP_H_
#define _SIP_STACK_HELPER_SIP_H_

#include "msg/SipAddrSpec.h"

#include "AString.h"

class ISIPHeader;

namespace SIPStack
{

GLOBAL IMS_BOOL DecodeHeaderComponent(IN CONST SipAddrSpec *pAddrSpec,
        OUT IMSList<ISIPHeader*> &objHeaders);

GLOBAL IMS_BOOL DecodeHeaderComponent(IN CONST AString &strHeaders,
        OUT IMSList<ISIPHeader*> &objHeaders);

}

#endif // _SIP_STACK_HELPER_SIP_H_
