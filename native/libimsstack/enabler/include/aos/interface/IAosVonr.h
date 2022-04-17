/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20191114  sukhwan.mun@              Created
    </table>

    Description
*/


#ifndef _INTERFACE_AOS_VONR_H_
#define _INTERFACE_AOS_VONR_H_

#include "IMSTypeDef.h"

class IAosVonr
{
public:
    virtual IMS_BOOL IsSupported() const = 0;

    virtual IMS_BOOL Set(IN IMS_UINT32 nModule, IN IMS_BOOL bIsSending) = 0;

    enum
    {
        MODULE_NONE = 0,
        MODULE_REG = 0x1,
        MODULE_SUB = 0x2
    };
};

#endif // _INTERFACE_AOS_VONR_H_
