/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipDateHeader.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : July. 27, 2010
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/


/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipDateHeader.h"
#include "platform/sip_pf_string.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "msg/sip_msgutil.h"
//#include "msg/sip_dec.h"
#include "platform/sip_pf_string.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/
#define MAX_WEEK_LEN 3
#define MAX_MONTH_LEN 3
#define STR_GMT "GMT"
#define DATE_VAL 31

#define YEAR_VAL 1000
/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/
const SIP_UINT16 TIME_VAL=60;
const SIP_UINT16 HOUR_VAL = 24;
const SIP_CHAR gaszWeekday[][MAX_WEEK_LEN + SIP_ONE]=
{
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    "Sun"
};
const SIP_CHAR gaszMonth[][MAX_MONTH_LEN + SIP_ONE] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

/*****************************************************************************
 * Function name      : sipGetWeekDayType
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_INT32 sipGetWeekDayType
(
 SIP_CHAR            *pszWeekDay
 )
{
    switch(pszWeekDay[0])
    {
        case 'M':
        case 'm':
            if (SipPf_Strnicmp(pszWeekDay,
                    gaszWeekday[SipDateHeader::MONDAY], MAX_WEEK_LEN) == 0)
            {
                return SipDateHeader::MONDAY;
            }
            break;

        case 'T':
        case 't':
            if (SipPf_Strnicmp(pszWeekDay,
                    gaszWeekday[SipDateHeader::TUESDAY], MAX_WEEK_LEN) == 0)
            {
                return SipDateHeader::TUESDAY;
            }
            else if (SipPf_Strnicmp(pszWeekDay,
                    gaszWeekday[SipDateHeader::THURSDAY], MAX_WEEK_LEN) == 0)
            {
                return SipDateHeader::THURSDAY;
            }
            break;
        case 'W':
        case 'w':
            if (SipPf_Strnicmp(pszWeekDay,
                    gaszWeekday[SipDateHeader::WEDNESDAY], MAX_WEEK_LEN) == 0)
            {
                return SipDateHeader::WEDNESDAY;
            }
            break;
        case 'F':
        case 'f':
            if (SipPf_Strnicmp(pszWeekDay,
                    gaszWeekday[SipDateHeader::FRIDAY], MAX_WEEK_LEN) == 0)
            {
                return SipDateHeader::FRIDAY;
            }
            break;
        case 'S':
        case 's':
            if (SipPf_Strnicmp(pszWeekDay,
                    gaszWeekday[SipDateHeader::SATURDAY], MAX_WEEK_LEN) == 0)
            {
                return SipDateHeader::SATURDAY;
            }
            else if (SipPf_Strnicmp(pszWeekDay,
                    gaszWeekday[SipDateHeader::SUNDAY], MAX_WEEK_LEN) == 0)
            {
                return SipDateHeader::SUNDAY;
            }
            break;
        default:
            break;
    }
    return SipDateHeader::UNKNOWN_DAY;
}
/*****************************************************************************
 * Function name      : sipGetMonthType
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_INT32 sipGetMonthType
(
 SIP_CHAR            *pszMonth
 )
{
    switch(pszMonth[0])
    {
        case 'J':
        case 'j':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::JANUARY], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::JANUARY;
            }
            else if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::JUNE], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::JUNE;
            }
            else if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::JULY], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::JULY;
            }
            break;
        case 'F':
        case 'f':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::FEBRUARY], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::FEBRUARY;
            }
            break;
        case 'M':
        case 'm':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::MARCH], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::MARCH;
            }
            else if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::MAY], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::MAY;
            }
            break;

        case 'A':
        case 'a':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::APRIL], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::APRIL;
            }
            else if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::AUGUST], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::AUGUST;
            }
            break;
        case 's':
        case 'S':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::SEPTEMBER], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::SEPTEMBER;
            }
            break;

        case 'O':
        case 'o':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::OCTOBER], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::OCTOBER;
            }
            break;
        case 'N':
        case 'n':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::NOVEMBER], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::NOVEMBER;
            }
            break;
        case 'D':
        case 'd':
            if (SipPf_Strnicmp(pszMonth,
                    gaszMonth[SipDateHeader::DECEMBER], MAX_MONTH_LEN) == 0)
            {
                return SipDateHeader::DECEMBER;
            }
            break;

        default:
            break;
    }
    return SipDateHeader::UNKNOWN_MONTH;
}


/******************************************************************************
 * Function name      : SipDateHeader::SipDateHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipDateHeader::SipDateHeader()
    : SipHeaderBase(SipHeaderBase::DATE), m_uiDate(SIP_ZERO)
    , m_eMonth(SipDateHeader::UNKNOWN_MONTH), m_uiYear(SIP_ZERO)
    , m_uiHour(SIP_ZERO), m_uiMin(SIP_ZERO)
    , m_uiSec(SIP_ZERO), m_eWkDay(SipDateHeader::UNKNOWN_DAY)
{
}

/******************************************************************************
 * Function name      : SipDateHeader::~SipDateHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipDateHeader::~SipDateHeader()
{
}

/******************************************************************************
 * Function name      : SipDateHeader::~SipDateHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipDateHeader::SipDateHeader(const SipDateHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_uiDate(objHeader.m_uiDate)
    , m_eMonth(objHeader.m_eMonth)
    , m_uiYear(objHeader.m_uiYear), m_uiHour(objHeader.m_uiHour)
    , m_uiMin(objHeader.m_uiMin), m_uiSec(objHeader.m_uiSec)
    , m_eWkDay(objHeader.m_eWkDay)
{
}


/******************************************************************************
 * Function name      : SipDateHeader::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     /*bParams = SIP_TRUE*/
)
{
    if (EncodeDate(ppucCurrPos) ==  SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Date Encoding failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}


SIP_BOOL SipDateHeader::EncodeWeek(
        SIP_CHAR     **ppucCurrPos
        )
{
    SipPf_Strcpy(*ppucCurrPos,gaszWeekday[m_eWkDay]);
    SipEnc_UpdateCurrPos(ppucCurrPos);
    return SIP_TRUE;
}

SIP_BOOL  SipDateHeader::EncodeTime(
        SIP_CHAR     **ppucCurrPos
        )
{
    SipPf_Sprintf(*ppucCurrPos, (SIP_CHAR*)"%02u:%02u:%02u", m_uiHour, m_uiMin, m_uiSec);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return SIP_TRUE;
}

SIP_BOOL  SipDateHeader::EncodeDate(
        SIP_CHAR     **ppucCurrPos
        )
{
    SIP_CHAR    *pucMsgCurrPtr    =    *ppucCurrPos;

    //Date: Thu, 21 Feb 2002 13:02:03 GMT
    EncodeWeek(&pucMsgCurrPtr);

    pucMsgCurrPtr[0] = COMMA;
    pucMsgCurrPtr++;

    pucMsgCurrPtr[0] = SPACE;
    pucMsgCurrPtr++;

    SipPf_Sprintf(pucMsgCurrPtr, (SIP_CHAR*)"%02u %s %4u", m_uiDate,
            gaszMonth[m_eMonth], m_uiYear);
    SipEnc_UpdateCurrPos(&pucMsgCurrPtr);

    pucMsgCurrPtr[0] = SPACE;
    pucMsgCurrPtr++;

    EncodeTime(&pucMsgCurrPtr);

    pucMsgCurrPtr[0] = SPACE;
    pucMsgCurrPtr++;

    SipPf_Strcpy(pucMsgCurrPtr,STR_GMT);
    SipEnc_UpdateCurrPos(&pucMsgCurrPtr);

    *ppucCurrPos = pucMsgCurrPtr;
    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipDateHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    /*Date: Thu, 21 Feb 2002 13:02:03 GMT*/

    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Empty buffer", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR *pucTempPos = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COMMA) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pszTempValue = sipCreateString(pucStartPt, pucTempPos);
    if (pszTempValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    m_eWkDay = sipGetWeekDayType(pszTempValue);
    if (m_eWkDay == SipDateHeader::UNKNOWN_DAY)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid week day", SIP_ZERO, SIP_ZERO);
        delete[] pszTempValue;
        return SIP_FALSE;
    }
    delete[] pszTempValue;

    if (*(pucTempPos + SIP_TWO) != SPACE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Skip comma and a space and point to start of date */
    pucStartPt = pucTempPos + SIP_THREE;
    pucTempPos = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Get the value of date*/
    pszTempValue = sipCreateString(pucStartPt, pucTempPos);
    if (pszTempValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    m_uiDate = SipPf_Atoi(pszTempValue);
    delete[] pszTempValue;

    if (m_uiDate > DATE_VAL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Update the start point to the start of Month*/
    pucStartPt = pucTempPos + SIP_TWO;
    pucTempPos = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    pszTempValue = sipCreateString(pucStartPt, pucTempPos);
    if (pszTempValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    m_eMonth = sipGetMonthType(pszTempValue);
    if (m_eMonth == SipDateHeader::UNKNOWN_MONTH)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid month", SIP_ZERO, SIP_ZERO);
        delete[] pszTempValue;
        return SIP_FALSE;
    }
    delete[] pszTempValue;

    /*Update the start point to the start of year*/
    pucStartPt = pucTempPos + SIP_TWO;
    pucTempPos = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pszTempValue = sipCreateString(pucStartPt, pucTempPos);
    if (pszTempValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT16 usTempLen = SipPf_Strlen(pszTempValue);
    if (usTempLen != SIP_FOUR)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Year", SIP_ZERO, SIP_ZERO);
        delete[] pszTempValue;
        return SIP_FALSE;
    }

    m_uiYear = SipPf_Atoi(pszTempValue);
    delete[] pszTempValue;

    /*Update the start point to the start of Time(Hour)*/
    pucStartPt = pucTempPos + SIP_TWO;
    pucTempPos = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format",SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pszTempValue = sipCreateString(pucStartPt, pucTempPos);
    if (pszTempValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation failed",SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    m_uiHour= SipPf_Atoi(pszTempValue);
    delete[] pszTempValue;

    if (m_uiHour >= HOUR_VAL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Hour", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    /*Update the start point to the start of Time(Min)*/
    pucStartPt = pucTempPos + SIP_TWO;
    pucTempPos = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pszTempValue = sipCreateString(pucStartPt, pucTempPos);
    if (pszTempValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    m_uiMin = SipPf_Atoi(pszTempValue);
    delete[] pszTempValue;

    if (m_uiMin >= TIME_VAL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Minute Value", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    /*Update the start point to the start of Time(Sec)*/
    pucStartPt = pucTempPos + SIP_TWO;
    pucTempPos = SIP_NULL;
    /*Check validity of Sec*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Date Format", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pszTempValue = sipCreateString(pucStartPt, pucTempPos);
    if (pszTempValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    m_uiSec = SipPf_Atoi(pszTempValue);
    delete[] pszTempValue;
    if (m_uiSec >= TIME_VAL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Second Value", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Check for GMT*/
    pucStartPt = pucTempPos + SIP_TWO;
    pucTempPos = SIP_NULL;
    SIP_CHAR *pucTempGMT = sipCreateString(pucStartPt, pucEndPt);
    if ((pucTempGMT != SIP_NULL) && (SipPf_Strcmp(pucTempGMT, "GMT") != 0))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "GMT not matching", SIP_ZERO,SIP_ZERO);
        delete[] pucTempGMT;
        return SIP_FALSE;
    }
    else if (pucTempGMT != SIP_NULL)
    {
        delete[] pucTempGMT;
    }

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipDateHeader::SetDate
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::SetDate
(
 const SIP_UINT16    usDate
 )
{
    if (usDate <= DATE_VAL && usDate > SIP_ZERO)
    {
        m_uiDate = usDate;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}


/******************************************************************************
 * Function name      : SipDateHeader::SetMonth
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::SetMonth
(
 SIP_INT32        eMonth
 )
{
    if ((eMonth >= SipDateHeader::JANUARY) && (eMonth < SipDateHeader::UNKNOWN_MONTH))
    {
        m_eMonth = eMonth;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}


/******************************************************************************
 * Function name      : SipDateHeader::SetYear
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::SetYear
(
 const SIP_UINT16    usYear
 )
{
    if (usYear >= YEAR_VAL)
    {
        m_uiYear = usYear;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}


/******************************************************************************
 * Function name      : SipDateHeader::SetHour
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::SetHour
(
 const SIP_UINT16    usHour
 )
{
    if (usHour <= TIME_VAL)
    {
        m_uiHour = usHour;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}


/******************************************************************************
 * Function name      : SipDateHeader::SetMinute
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::SetMinute
(
 const SIP_UINT16    usMin
 )
{
    if (usMin <= TIME_VAL)
    {
        m_uiMin = usMin;
        return SIP_TRUE;
    }
    return SIP_FALSE;

}

/******************************************************************************
 * Function name      : SipDateHeader::SetSecond
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::SetSecond
(
 const SIP_UINT16    usSec
 )
{
    if (usSec <= TIME_VAL)
    {
        m_uiSec = usSec;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}


/******************************************************************************
 * Function name      : SipDateHeader::SetWkDay
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipDateHeader::SetWkDay
(
 SIP_INT32    eWkDay
 )
{
    if ((eWkDay >= SipDateHeader::MONDAY) && (eWkDay < SipDateHeader::UNKNOWN_DAY))
    {
        m_eWkDay = eWkDay;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}

SipHeaderBase* SipDateHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipDateHeader(*reinterpret_cast<SipDateHeader*>(pHeader));
    }
    return new SipDateHeader();
}
