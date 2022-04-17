/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170515  hwangoo.park@             Created
    </table>

    Description

*/

#include "EnablerUtils.h"

PUBLIC GLOBAL
const IMS_CHAR* EnablerUtils::GetPrefixForEnablerThread()
{
    return "ET";
}

PUBLIC GLOBAL
AString EnablerUtils::GetEnablerThreadName(IN IMS_SINT32 nSlotId)
{
    if ((nSlotId <= IMS_SLOT_ANY) || (nSlotId >= IMS_SLOT_MAX))
    {
        nSlotId = IMS_SLOT_0;
    }

    AString strName;

    strName.Sprintf("%s%02d", GetPrefixForEnablerThread(), nSlotId);

    return strName;
}
