/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20120105  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_ZLIB_H_
#define _INTERFACE_IMS_ZLIB_H_

#include "ByteArray.h"

class IZLib
{
public:
    /*
     Compresses the specified raw data with a default options.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    objData                 Raw data to be compressed
    objCompData             Compressed data
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_BOOL                IMS_TRUE/IMS_FALSE
    </table>
    */
    virtual IMS_BOOL Compress(IN const ByteArray &objData, OUT ByteArray &objCompData) = 0;

    /*
     Uncompresses the compressed data with a default options.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    objCompData             Data to be uncompressed
    objData                 Uncompressed data
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_BOOL                IMS_TRUE/IMS_FALSE
    </table>
    */
    virtual IMS_BOOL Uncompress(IN const ByteArray &objCompData, OUT ByteArray &objData) = 0;
};

#endif // _INTERFACE_IMS_ZLIB_H_
