#ifndef STATIC_CONFIG_H_
#define STATIC_CONFIG_H_

#include "AString.h"

class StaticConfig
{
public:
    StaticConfig() = delete;

public:
    /**
     * @brief Returns the static configuration that matches with the specified configuration name.
     *
     * @param strName The configuration name to retrieve
     */
    static const IMS_CHAR* GetConfig(IN const AString& strName);

    /**
     * @brief Returns the media configuration.
     */
    static const IMS_CHAR* GetMediaConfig();
};

#endif
