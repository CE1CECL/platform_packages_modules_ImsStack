/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101203  sukhwan.mun@              Created
    </table>

    Description
*/

#ifndef _IMS_IPSEC_TYPE_H_
#define _IMS_IPSEC_TYPE_H_

#include "IMSTypeDef.h"

// IP Security Type class
class IPSecType
{
public:
    // Upper Layer Protocol
    enum
    {
        TRANS_PROTOCOL_UDP = 0,
        TRANS_PROTOCOL_TCP,
        TRANS_PROTOCOL_ANY
    };

    // Action
    enum
    {
        ACTION_APPLY = 0,
        ACTION_PERMIT,
        ACTION_DROP,
        ACTION_BYPASS
    };

    // Direction
    enum
    {
        DIRECTION_INBOUND = 0,
        DIRECTION_OUTBOUND,
        DIRECTION_ANY
    };

    // Mode
    enum
    {
        MODE_TRANSPORT = 0,
        MODE_TUNNEL
    };

    // Security Protocol
    enum
    {
        SECURITY_PROTOCOL_AH = 0,
        SECURITY_PROTOCOL_ESP
    };

    // Integrity Algorithm
    enum
    {
        INTEGRITY_ALGORITHM_HMAC_MD5_96 = 0,
        INTEGRITY_ALGORITHM_HMAC_SHA_1_96
    };

    // Encryption Algorithm
    enum
    {
        ENCRYPTION_ALGORITHM_DES_EDE3_CBC = 0,
        ENCRYPTION_ALGORITHM_AES_CBC,
        ENCRYPTION_ALGORITHM_NO
    };
};

#endif // _IMS_IPSEC_TYPE_H_
