/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100302  IMSers                    Created
    </table>

    Description
    This file defines the entry point of IMS service.

*/

#ifndef _IMS_CORE_H_
#define _IMS_CORE_H_

#include "SystemConfig.h"

// For system configuration or re-configuration
GLOBAL void IMSCore_SetConfiguration(
        IN IMS_SINT32 nEvent, IN IMS_SINT32 nCount, IN const __SystemConfig *pstSysConfig);

GLOBAL void IMSCore_Initialize();
GLOBAL void IMSCore_InitConfiguration();
GLOBAL void IMSCore_Start();
GLOBAL void IMSCore_Stop();
GLOBAL void IMSCore_Uninitialize();

#endif // _IMS_CORE_H_
