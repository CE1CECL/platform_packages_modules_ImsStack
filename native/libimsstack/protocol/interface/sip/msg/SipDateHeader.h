/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipDateHeader.h
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : Month. Date,10
 *
 * Edit History         Modification description(s)
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/

#ifndef __SIP_DATE_HEADER_H__
#define __SIP_DATE_HEADER_H__

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipHeaderBase.h"


/****************************************************************************
  Macro Definitions
 *****************************************************************************/


/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/

class SipDateHeader : public SipHeaderBase
{
    public:
        /*Enumration for week day*/
        enum
        {
            MONDAY,
            TUESDAY,
            WEDNESDAY,
            THURSDAY,
            FRIDAY,
            SATURDAY,
            SUNDAY,
            UNKNOWN_DAY
        };


        /*Enumration for month*/
        enum
        {
            JANUARY,
            FEBRUARY,
            MARCH,
            APRIL,
            MAY,
            JUNE,
            JULY,
            AUGUST,
            SEPTEMBER,
            OCTOBER,
            NOVEMBER,
            DECEMBER,
            UNKNOWN_MONTH
        };

    private:
        /*Date */
        SIP_UINT16                m_uiDate;
        SIP_INT32            m_eMonth;
        SIP_UINT16            m_uiYear;

        /*Time*/
        SIP_UINT16                m_uiHour;
        SIP_UINT16                m_uiMin;
        SIP_UINT16                m_uiSec;

        SIP_INT32            m_eWkDay;

        SIP_BOOL EncodeWeek(
                SIP_CHAR     **ppucCurrPos
                );

        SIP_BOOL EncodeMonth(
                SIP_CHAR     **ppucCurrPos
                );

        SIP_BOOL EncodeTime(
                SIP_CHAR     **ppucCurrPos
                );

        SIP_BOOL EncodeDate(
                SIP_CHAR     **ppucCurrPos
                );

    public:
        /*constructor*/
        SipDateHeader();

        /*destructor*/
        ~SipDateHeader();

        SipDateHeader(const SipDateHeader &objHeader);
        static SipHeaderBase* GetNewObj(SIP_INT32,SipHeaderBase*);

        /*virtual methods*/
        /*Function for encoding of headers*/
        SIP_BOOL EncodeHdr
            (
             SIP_CHAR    **ppucCurrPos,
             SIP_BOOL    bParams = SIP_TRUE
            );

        /*Function for decoding of headers*/
        SIP_BOOL DecodeHdr
            (
             SIP_CHAR    *pucStartPt,
             SIP_UINT32  uiDecLen
            );

        /*set methods*/
        SIP_BOOL SetDate
            (
             const SIP_UINT16    usDate
            );

        SIP_BOOL SetMonth
            (
             SIP_INT32        eMonth
            );

        SIP_BOOL SetYear
            (
             const SIP_UINT16    usYear
            );

        SIP_BOOL SetHour
            (
             const SIP_UINT16    usHour
            );

        SIP_BOOL SetMinute
            (
             const SIP_UINT16    usMin
            );

        SIP_BOOL SetSecond
            (
             const SIP_UINT16    usSec
            );

        SIP_BOOL SetWkDay
            (
             SIP_INT32    eWkDay
            );


        /*Get methods*/
        inline SIP_UINT16 GetDate() const
        {
            return m_uiDate;
        }

        inline SIP_INT32 GetMonth() const
        {
            return m_eMonth;
        }

        inline SIP_UINT16  GetYear() const
        {
            return m_uiYear;
        }

        inline SIP_UINT16 GetHour() const
        {
            return m_uiHour;
        }

        inline SIP_UINT16 GetMinute() const
        {
            return m_uiMin;
        }

        inline SIP_UINT16 GetSecond() const
        {
            return m_uiSec;
        }

        inline SIP_INT32 GetWkDay() const
        {
            return m_eWkDay;
        }
};
#endif //__SIP_DATE_HEADER_H__
