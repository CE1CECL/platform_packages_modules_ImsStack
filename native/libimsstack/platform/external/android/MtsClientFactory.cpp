#include "MtsClientFactory.h"
#include "AndroidJavaWms.h"

#include "AndroidJavaSCBM.h"
#include "ServiceTrace.h"


__IMS_TRACE_TAG_ADAPT__;

/* ------------------------------------------------------------------------------------------------
*Creator of MtsClientFactory
------------------------------------------------------------------------------------------------ */
PUBLIC
MtsClientFactory::MtsClientFactory()
{
    IMS_TRACE_D("MtsClientFactory", 0, 0, 0);
}


/* ------------------------------------------------------------------------------------------------
*Destructor of MtsClientFactory
------------------------------------------------------------------------------------------------ */

PUBLIC VIRTUAL
MtsClientFactory::~MtsClientFactory()
{
    IMS_TRACE_D("~MtsClientFactory", 0, 0, 0);
}

/* ------------------------------------------------------------------------------------------------
*Get a instance of AndroidJavaWMS for MTK
*
*@param nSlotID : SIM Slot ID
*@return pointer of AndroidJavaWMS
------------------------------------------------------------------------------------------------ */
PUBLIC GLOBAL
IMtsClient* MtsClientFactory::GetIMtsJavaClient(IN IMS_SINT32 nSlotID)
{
    IMS_TRACE_D("GetIMtsJavaClient: slot:[%d]", nSlotID, 0, 0);
    return AndroidJavaWMS::GetAndroidJavaWMS(nSlotID);
}

/* ------------------------------------------------------------------------------------------------
*Get a instance of AndroidJavaSCBM for VZW SMS to 911
*
*@param nSlotID : SIM Slot ID
*@return pointer of AndroidJavaWMS
------------------------------------------------------------------------------------------------ */
PUBLIC GLOBAL
IMtsSCBM* MtsClientFactory::GetIMtsSCBMClient(IN IMS_SINT32 nSlotID)
{
    IMS_TRACE_D("GetIMtsSCBMClient: slot:[%d]", nSlotID, 0, 0);
    return AndroidJavaSCBM::GetAndroidJavaSCBM();
}
/* ------------------------------------------------------------------------------------------------
*Destroy a instance of AndroidJavaSCBM for VZW SMS to 911
*
*@param nSlotID : SIM Slot ID
------------------------------------------------------------------------------------------------ */
PUBLIC GLOBAL
void MtsClientFactory::DestroyIMtsSCBMClient(IN IMS_SINT32 nSlotID)
{
    IMS_TRACE_D("DestroyIMtsJavaClient: slot[%d]", nSlotID, 0, 0);
    AndroidJavaSCBM::DestroyAndroidJavaSCBM(nSlotID);
}

/* ------------------------------------------------------------------------------------------------
*Destroy a instance of AndroidJavaWMS for MTK
*
*@param nSlotID : SIM Slot ID
------------------------------------------------------------------------------------------------ */
PUBLIC GLOBAL
void MtsClientFactory::DestroyIMtsJavaClient(IN IMS_SINT32 nSlotID)
{
    IMS_TRACE_D("DestroyIMtsJavaClient: slot[%d]", nSlotID, 0, 0);
    AndroidJavaWMS::DestroyAndroidJavaWMS(nSlotID);
}
