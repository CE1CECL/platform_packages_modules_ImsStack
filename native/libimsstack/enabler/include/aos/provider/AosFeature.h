#ifndef _AOS_FEATURE_H_
#define _AOS_FEATURE_H_

#include "ServiceTrace.h"

class AosFeature
{
public:
    AosFeature()
        : nFeatures(FEATURE_NONE)
        , nOriginalFeatures(FEATURE_NONE) {
        IMS_TRACE_MEM("AOS_MEM", "AOS_M : AosFeature = %" PFLS_u "/%" PFLS_x,
            sizeof(AosFeature), this, 0);
    }
    virtual ~AosFeature() {
        IMS_TRACE_MEM("AOS_MEM", "AOS_F : AosFeature = %" PFLS_u "/%" PFLS_x,
            sizeof(AosFeature), this, 0);
    }
    void EnableFeature(IN IMS_UINT32 nFeatures) {
        this->nFeatures |= nFeatures;
        OnFeatureEnabled(nFeatures);
    }
    void DisableFeature(IN IMS_UINT32 nFeatures) {
        this->nFeatures &= (~nFeatures);
        OnFeatureDisabled(nFeatures);
    }
    IMS_BOOL IsFeatureEnabled(IN IMS_UINT32 nFeature) {
        return (nFeatures & nFeature) == nFeature;
    }
    IMS_BOOL IsFeatureDisabled(IN IMS_UINT32 nFeature) {
        return !IsFeatureEnabled(nFeature);
    }
    void InitFeatures(IN IMS_UINT32 nFeatures) {
        this->nFeatures = nFeatures;
    }
    void InitOriginFeatures(IN IMS_UINT32 nFeatures) {
        this->nOriginalFeatures = nFeatures;
    }
    IMS_UINT32 GetFeatures() {
        return nFeatures;
    }
    IMS_UINT32 GetOriginFeatures() {
        return nOriginalFeatures;
    }
    virtual void OnFeatureEnabled(IN IMS_UINT32 nFeatures) {
        (void) nFeatures;
    }
    virtual void OnFeatureDisabled(IN IMS_UINT32 nFeatures) {
        (void) nFeatures;
    }

    enum
    {
        FEATURE_NONE = 0
    };

private:
    IMS_UINT32 nFeatures;
    IMS_UINT32 nOriginalFeatures;
};

#endif // _AOS_FEATURE_H_
