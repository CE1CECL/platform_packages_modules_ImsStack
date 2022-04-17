/*
    Author
    <table>
    Date        Author                        Description
    --------      -----------------            ------------------
    20111203    hwangoo.park@               Created

    </table>

    Description

*/

#ifndef _IU_BASE_PARAM_H_
#define _IU_BASE_PARAM_H_

#include "IMSTypeDef.h"

class IUBaseParam
{
    public:
        inline IUBaseParam()
            {}
        inline virtual ~IUBaseParam()
            {}

    private:
        IUBaseParam(IN CONST IUBaseParam &objRHS);
        IUBaseParam& operator=(IN CONST IUBaseParam &objRHS);
};
#endif                                              // _IU_BASE_PARAM_H_
