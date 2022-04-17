/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20141122  hwangoo.park@             Refactoring the initial impl. (based on qcril_cm)
    </table>

    Description

*/

#ifndef _USSD_PARSER_H_
#define _USSD_PARSER_H_

#include "ByteArray.h"



class USSDParser
{
private:
    USSDParser();
    ~USSDParser();

public:
    static IMS_BOOL Parse(IN CONST AString &strGsm7Bit, OUT AString &strUtf8);

private:
    static void ConvertGsm8BitStringToUtf8(IN CONST ByteArray &objGsm8Bit, OUT AString &strUtf8);
    static void UnpackUssd(IN CONST AString &strGsm7Bit, OUT ByteArray &objGsm8Bit);
};

#endif // _USSD_PARSER_H_
