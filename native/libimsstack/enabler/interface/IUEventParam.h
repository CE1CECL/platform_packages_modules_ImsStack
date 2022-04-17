/*
    Author
    IMSers
    <table>
    Date            Author                    Description
    --------      -----------------        ------------------
    20110423    sukhwan.mun@               Created

    </table>

    Description
--------------------------------------------------

*/

#ifndef _IU_EVENT_PARAM_H_
#define _IU_EVENT_PARAM_H_

//----------------------------------------------------------------------
//----------------------------------------------------------------------

class IUEventParam
{
    public :
        IMS_SINT32    nAppId;
        IMS_SINT32    nEventName;
    public :
        inline IUEventParam(IUEventParam* pParam = NULL) : nAppId(-1), nEventName(-1)
        {
            if (pParam == NULL) return;

            this->nAppId = pParam->nAppId;
            this->nEventName = pParam->nEventName;
        }

        virtual ~IUEventParam() {}
};
#endif                                              // _IU_EVENT_PARAM_H_
