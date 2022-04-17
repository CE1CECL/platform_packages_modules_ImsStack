/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20150705  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "IMSStrLib.h"
#include "SystemConfig.h"

PRIVATE GLOBAL
IMS_SINT32 SystemConfig::nMultiSimConfig = (-1);

PRIVATE GLOBAL
IMS_SINT32 SystemConfig::nGlobalConfigs = 0;

PRIVATE GLOBAL
AString SystemConfig::strPackageName = AString::ConstNull();

PUBLIC
SystemConfig::SystemConfig()
    : nSlotId(0)
    , strOperator(AString::ConstNull())
    , strCountry(AString::ConstNull())
    , strEnablerType(AString::ConstNull())
    , nExtraInfo(EXTRA_INFO_NONE)
    , nFeatures(0)
    , nServiceFeatures(0)
{
}

PUBLIC
SystemConfig::SystemConfig(IN const __SystemConfig *pstConfig)
{
    if (pstConfig != IMS_NULL)
    {
        nSlotId = pstConfig->nSlotId;

        strOperator = static_cast<const IMS_CHAR*>(pstConfig->acOperator);
        strCountry = static_cast<const IMS_CHAR*>(pstConfig->acCountry);

        strEnablerType = static_cast<const IMS_CHAR*>(pstConfig->acEnablerType);
        nExtraInfo = pstConfig->nExtraInfo;

        nFeatures = pstConfig->nFeatures;
        nServiceFeatures = pstConfig->nServiceFeatures;
    }
    else
    {
        nSlotId = 0;

        strOperator = AString::ConstNull();
        strCountry = AString::ConstNull();

        strEnablerType = AString::ConstNull();
        nExtraInfo = EXTRA_INFO_NONE;

        nFeatures = 0;
        nServiceFeatures = 0;
    }
}

PUBLIC
SystemConfig::SystemConfig(IN const SystemConfig &objRHS)
    : nSlotId(objRHS.nSlotId)
    , strOperator(objRHS.strOperator)
    , strCountry(objRHS.strCountry)
    , strEnablerType(objRHS.strEnablerType)
    , nExtraInfo(objRHS.nExtraInfo)
    , nFeatures(objRHS.nFeatures)
    , nServiceFeatures(objRHS.nServiceFeatures)
{
}

PUBLIC
SystemConfig::~SystemConfig()
{
}

/*

Remarks

*/
PUBLIC
SystemConfig& SystemConfig::operator=(IN const SystemConfig &objRHS)
{
    if (this != &objRHS)
    {
        nSlotId = objRHS.nSlotId;

        strOperator = objRHS.strOperator;
        strCountry = objRHS.strCountry;

        strEnablerType = objRHS.strEnablerType;
        nExtraInfo = objRHS.nExtraInfo;

        nFeatures = objRHS.nFeatures;
        nServiceFeatures = objRHS.nServiceFeatures;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SystemConfig::Equals(IN const SystemConfig &objOther) const
{
    return (nSlotId == objOther.nSlotId)
            && strOperator.EqualsIgnoreCase(objOther.strOperator)
            && strCountry.EqualsIgnoreCase(objOther.strCountry)
            && strEnablerType.EqualsIgnoreCase(objOther.strEnablerType)
            && (nExtraInfo == objOther.nExtraInfo)
            && (nFeatures == objOther.nFeatures)
            && (nServiceFeatures == objOther.nServiceFeatures);
}

/*

Remarks

*/
PUBLIC
AString SystemConfig::ToString() const
{
    AString strSystemConfig;

    strSystemConfig.Sprintf("[ SystemConfig :: slotId=%d, op=%s, co=%s"
            ", enablerType=%s, extraInfo=%08x"
            ", features=%08x, serviceFeatures=%08x ]",
            nSlotId, strOperator.GetStr(), strCountry.GetStr(),
            strEnablerType.GetStr(), nExtraInfo,
            nFeatures, nServiceFeatures);

    return strSystemConfig;
}

/*

Remarks
 The caller MUTS free the memory of the pointer variable which returns from this method.
*/
PUBLIC
__SystemConfig* SystemConfig::ToSystemConfig() const
{
    __SystemConfig *pstConfig = new __SystemConfig;

    pstConfig->nSlotId = nSlotId;

    IMS_StrCpy(pstConfig->acOperator, IMS_SC_SIZE_16 + 1, strOperator.GetStr());
    IMS_StrCpy(pstConfig->acCountry, IMS_SC_SIZE_8 + 1, strCountry.GetStr());

    IMS_StrCpy(pstConfig->acEnablerType, IMS_SC_SIZE_16 + 1, strEnablerType.GetStr());
    pstConfig->nExtraInfo = nExtraInfo;

    pstConfig->nFeatures = nFeatures;
    pstConfig->nServiceFeatures = nServiceFeatures;

    return pstConfig;
}

/*

Remarks

*/
PUBLIC GLOBAL
const AString& SystemConfig::GetPackageName()
{
    return strPackageName;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_SINT32 SystemConfig::GetMaxSimSlot()
{
    IMS_SINT32 nMSimConfig = GetMultiSimConfig();

    switch (nMSimConfig)
    {
    case MULTI_SIM_DSDS:
    case MULTI_SIM_DSDA:
        return 2;
    case MULTI_SIM_TSTS:
        return 3;
    default:
        return 1;
    }
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_SINT32 SystemConfig::GetMultiSimConfig()
{
    if (nMultiSimConfig < 0)
    {
        // FIXME: multi-sim config.
        nMultiSimConfig = MULTI_SIM_NONE;
    }

    return nMultiSimConfig;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsMultiImsEnabled()
{
    // As a default, single IMS is required on dual SIM environment.
    return (nGlobalConfigs & CONFIG_MULTI_IMS) != 0;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsMultiImsEnabledOnDssv()
{
    // DSSV-DV
    return IMS_FALSE;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsMultiLteEnabled()
{
    // LTE+LTE DSDS
    return (nGlobalConfigs & CONFIG_MULTI_LTE) != 0;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsMultiSimEnabled()
{
    return (GetMultiSimConfig() != MULTI_SIM_NONE);
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsOperatorChanged(IN const SystemConfig* pOldConfig,
            IN const SystemConfig* pNewConfig)
{
    if ((pOldConfig == IMS_NULL) && (pNewConfig == IMS_NULL))
    {
        return IMS_FALSE;
    }

    if (pOldConfig == IMS_NULL)
    {
        return pNewConfig->GetOperator().GetLength() > 0;
    }
    else if (pNewConfig == IMS_NULL)
    {
        return pOldConfig->GetOperator().GetLength() > 0;
    }

    if ((pNewConfig->GetOperator().GetLength() == 0)
            && (pOldConfig->GetOperator().GetLength() == 0))
    {
        return IMS_FALSE;
    }

    if (pNewConfig->GetOperator().Equals(pOldConfig->GetOperator()))
    {
        if (pNewConfig->GetEnablerType().Equals("global"))
        {
            return !pNewConfig->GetCountry().Equals(pOldConfig->GetCountry());
        }

        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsServiceFeatureChanged(IN const SystemConfig* pOldConfig,
        IN const SystemConfig* pNewConfig)
{
    if ((pOldConfig == IMS_NULL) || (pNewConfig == IMS_NULL))
    {
        return IMS_FALSE;
    }

    return pNewConfig->GetServiceFeatures() != pOldConfig->GetServiceFeatures();
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsDdsChanged(IN const SystemConfig* pOldConfig,
        IN const SystemConfig* pNewConfig)
{
    if ((pOldConfig == IMS_NULL) || (pNewConfig == IMS_NULL))
    {
        return IMS_FALSE;
    }

    return (pNewConfig->IsDds() && !pOldConfig->IsDds())
            || (!pNewConfig->IsDds() && pOldConfig->IsDds());
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SystemConfig::IsSimMobilityChanged(IN const SystemConfig* pOldConfig,
        IN const SystemConfig* pNewConfig)
{
    if ((pOldConfig == IMS_NULL) || (pNewConfig == IMS_NULL))
    {
        return IMS_FALSE;
    }

    return (pNewConfig->IsSimMobilityEnabled() && !pOldConfig->IsSimMobilityEnabled())
            || (!pNewConfig->IsSimMobilityEnabled() && pOldConfig->IsSimMobilityEnabled());
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SystemConfig::IsExtraInfoSet(IN IMS_SINT32 nExtraInfo) const
{
    return (this->nExtraInfo & nExtraInfo) != 0;
}

/*

Remarks

*/
PRIVATE GLOBAL
void SystemConfig::CacheGlobalConfigs()
{
    nGlobalConfigs = 0;

    strPackageName = "com.android.imsstack";
}

/*

Remarks

*/
PRIVATE GLOBAL
void SystemConfig::UpdateGlobalConfigsOnFeatureChanged()
{
    // DSDV-SV
    // nGlobalConfigs |= CONFIG_MULTI_IMS;
}
