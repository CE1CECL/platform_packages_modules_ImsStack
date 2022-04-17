/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20141122  hwangoo.park@             Refactoring the initial impl. (based on qcril_cm)
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "TextParser.h"
#include "USSDParser.h"

__IMS_TRACE_TAG_ADAPT__;


// Table for unescaped GSM 7 bit characters & ANSI codepage unicode
LOCAL const IMS_UINT16 GSM_7BIT_CHAR_TO_UTF8[] =
{
    // 0    1    2    3    4    5    6    7
    // 00   01   02   03   04   05   06   07
    // @    �    $    Yen  e\   e/   u\   i\.
    0x0040, 0xC2A3, 0x0024, 0xC2A5, 0xC3A8, 0xC3A9, 0xC3B9, 0xC3AC,

    // 8    9    10    11    12    13    14    15
    // 08   09   0A    0B    0C    0D    0E    0F
    // o\   C,   LF    O|    o|    CR    A0    a0
    0xC3B2, 0xC387, 0x000A, 0xC398, 0xC3B8, 0x000D, 0xC385, 0xC3A5,

    // 16    17    18    19    20    21    22    23
    // 10    11    12    13    14    15    16    17
    // Delta  _    ... Greek ...
    0xCE94, 0x005F, 0xCEA6, 0xCE93, 0xCE9B, 0xCEA9, 0xCEA0, 0xCEA8,

    // 24    25    26    27    28    29    30    31
    // 18    19    1A    1B    1C    1D    1E    1F
    // ... Greek ...     Esc   AE    ae    GrossS E/
    0xCEA3, 0xCE98, 0xCE9E, 0x0020, 0xC386, 0xC3A6, 0xC39F, 0xC389,

    // 32    33    34    35    36    37    38    39
    // 20    21    22    23    24    25    26    27
    // SPC   !     "     #     OX    %     &     '
    0x0020, 0x0021, 0x0022, 0x0023, 0xC2A4, 0x0025, 0x0026, 0x0027,

    // 40    41    42    43    44    45    46    47
    // 28    29    2A    2B    2C    2D    2E    2F
    // (     )     *     +     ,     -     .     /
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,

    // 48    49    50    51    52    53    54    55
    // 30    31    32    33    34    35    36    37
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,

    // 56    57    58    59    60    61    62    63
    // 38    39    3A    3B    3C    3D    3E    3F
    // 8     9     :     ;     <     =     >     ?
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,

    // 64    65    66    67    68    69    70    71
    // 40    41    42    43    44    45    46    47
    // .|    A     B     C     D     E     F     G
    0xC2A1, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,

    // 72    73    74    75    76    77    78    79
    // 48    49    4A    4B    4C    4D    4E    4F
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,

    // 80    81    82    83    84    85    86    87
    // 50    51    52    53    54    55    56    57
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,

    // 88    89    90    91    92    93    94    95
    // 58    59    5A    5B    5C    5D    5E    5F
    // X     Y     Z     A"    0"    N~    U"    S\S
    0x0058, 0x0059, 0x005A, 0xC384, 0xC396, 0xC391, 0xC39C, 0xC2A7,

    // 96    97    98    99    100    101    102    103
    // 60    61    62    0X63  64     65     66     67
    // ';    a     b     c     d      e      f      g
    0xC2BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,

    // 104    105    106    107    108    109    110    111
    // 68     69     6A     6B     6C     6D     6E     6F
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,

    // 112    113    114    115    116    117    118    119
    // 70     71     72     73     74     75     76     77
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,

    // 120    121    122    123    124    125    126    127
    // 78     79     7A     7B     7C     7D     7E     7F
    // x      y      z      a"     o"     n~     u"     a
    0x0078, 0x0079, 0x007A, 0xC3A4, 0xC3B6, 0xC3B1, 0xC3BC, 0xC3A0
};



/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL USSDParser::Parse(IN CONST AString &strGsm7Bit, OUT AString &strUtf8)
{
    IMS_TRACE_I("Parse :: Gsm7Bit string length = %d", strGsm7Bit.GetLength(), 0, 0);

    if (strGsm7Bit.GetLength() == 0)
    {
        IMS_TRACE_D("Gsm7Bit string is empty", 0, 0, 0);
        return IMS_FALSE;
    }

    ByteArray objGsm8Bit;

    UnpackUssd(strGsm7Bit, objGsm8Bit);
    ConvertGsm8BitStringToUtf8(objGsm8Bit, strUtf8);

    if (strUtf8.GetLength() == 0)
    {
        IMS_TRACE_D("USSDParser :: Decoding failed", 0, 0, 0);
        return IMS_FALSE;
    }

    // Spare bits are set to '0' as documented in 3GPP TS 24.008 Section 10.5.3.5a, and
    // this unpacks it assuming USSD packing (packing for 7 spare bits is carriage return (0x0D)).
    IMS_CHAR cSrc = (IMS_CHAR)strGsm7Bit[strGsm7Bit.GetLength() - 1];
    IMS_CHAR cDest = (IMS_CHAR)strUtf8[strUtf8.GetLength() - 1];

    if (((strGsm7Bit.GetLength() % 7) == 0)
            && ((cSrc & 0xFE) == 0) && (cDest == TextParser::CHAR_AT))
    {
        IMS_TRACE_D("Detected 7 spare bits in network name, removing trailing @", 0, 0, 0);
        strUtf8.Chop(1);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE GLOBAL
void USSDParser::ConvertGsm8BitStringToUtf8(
        IN CONST ByteArray &objGsm8Bit, OUT AString &strUtf8)
{
    if (objGsm8Bit.GetLength() == 0)
    {
        return;
    }

    const IMS_BYTE *pbyData = objGsm8Bit.GetData();

    for (IMS_SINT32 i = 0; i < objGsm8Bit.GetLength(); ++i)
    {
        // Ignore CR character
        if (pbyData[i] == TextParser::CHAR_CR)
        {
            IMS_TRACE_D("Ignore CR character at index(%d)", i, 0, 0);
            continue;
        }

        IMS_UINT32 nChar = static_cast<IMS_UINT32>(pbyData[i]);

        if (nChar <= 127)
        {
            IMS_UINT16 nUnicode = GSM_7BIT_CHAR_TO_UTF8[nChar];
            IMS_UINT8 nUtf8_H = static_cast<IMS_UINT8>((nUnicode & 0xFF00) >> 8);
            IMS_UINT8 nUtf8_L = static_cast<IMS_UINT8>(nUnicode & 0x00FF);

            if (nUtf8_H > 0)
            {
                strUtf8.Append(static_cast<const IMS_CHAR>(nUtf8_H));
            }

            strUtf8.Append(static_cast<const IMS_CHAR>(nUtf8_L));
        }
        else
        {
            strUtf8.Append(static_cast<const IMS_CHAR>(nChar));
        }
    }
}

/*

Remarks

*/
PRIVATE GLOBAL
void USSDParser::UnpackUssd(IN CONST AString &strGsm7Bit, OUT ByteArray &objGsm8Bit)
{
    if (strGsm7Bit.GetLength() == 0)
    {
        return;
    }

    const IMS_BYTE *pbyPackedData = reinterpret_cast<const IMS_BYTE*>(strGsm7Bit.GetStr());
    IMS_SINT32 nPackedIndex = 0;
    IMS_SINT32 nUnpackedIndex = 0;
    IMS_BYTE byPrev = 0;
    IMS_BYTE byCurr = 0;
    IMS_BYTE byShift = 0;

    while (nPackedIndex < strGsm7Bit.GetLength())
    {
        byShift = nUnpackedIndex & 0x07;
        byCurr = pbyPackedData[nPackedIndex++];

        // A 7-bit character can be split at the most between two bytes of packed data.
        objGsm8Bit.Append(
                static_cast<IMS_BYTE>(((byCurr << byShift) | (byPrev >> (8 - byShift))) & 0x7F));
        ++nUnpackedIndex;

        // Special case where the whole of the next 7-bit character fits
        // inside the current byte of packed data.
        if (byShift == 6)
        {
            // If the next 7-bit character is a CR (0x0D) and it is the last character,
            // then it indicates a padding character. Drop it.
            if (((byCurr >> 1) == TextParser::CHAR_CR)
                    && (nPackedIndex == strGsm7Bit.GetLength()))
            {
                break;
            }

            objGsm8Bit.Append(static_cast<IMS_BYTE>(byCurr >> 1));
            ++nUnpackedIndex;
        }

        byPrev = byCurr;
    }
}
