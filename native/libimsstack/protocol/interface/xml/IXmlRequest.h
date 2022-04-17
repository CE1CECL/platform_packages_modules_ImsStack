#ifndef INTERFACE_XML_REQUEST_H_
#define INTERFACE_XML_REQUEST_H_

#include "AString.h"

class IXmlRequest
{
public:
    /**
     * @brief Sets a raw XML string.
     *
     * @param pszRawXml a raw XML string
     */
    virtual void SetRawXml(IN const IMS_CHAR* pszRawXml) = 0;

    /**
     * @brief Sets a raw XML string.
     *
     * @param strRawXml a raw XML string
     */
    virtual void SetRawXml(IN const AString& strRawXml) = 0;

    /**
     * @brief Gets a raw XML string.
     *
     * @return A raw XML string.
     */
    virtual const AString& GetRawXml() const = 0;
};

#endif
