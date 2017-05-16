#ifndef _ResultCode_h_
#define _ResultCode_h_

#include <cstdint>

/************************************** ResultCode **************************************/
//  ResultCode are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+---------------------+-------------------------------+
//  |S|F|W|                    Error Code                           |
//  +-+-+-+-+-+---------------------+-------------------------------+
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail (COERROR)
//
//      F - Fixable - indicates if this error can be fixed by retry
//
//          0 - false
//          1 - true
//
//      W - Write Log - indicates if this error should be loged
//
//          0 - false
//          1 - true

typedef uint32_t ResultCode;

#define ResultCodeOk     ((uint32_t)0L)

#define SBitOk           ((uint32_t)0L)
#define SBitNotOk        ((uint32_t)1L)
#define FBitFalse        ((uint32_t)0L)
#define FBitTrue         ((uint32_t)1L)
#define WBitFalse        ((uint32_t)0L)
#define WBitTrue         ((uint32_t)1L)

inline ResultCode MakeResultCode(uint32_t s, uint32_t f, uint32_t w, uint32_t errCode)
{
    return (((s & 0x1) << 31) | ((f & 0x1) << 30) | ((w & 0x1) << 29) | (errCode && 0x1FFFFFFF));
}

inline uint32_t GetResultSBit(ResultCode resultCode){ return (resultCode >> 31); }
inline uint32_t GetResultFBit(ResultCode resultCode){ return ((resultCode >> 30) & 0x1); }
inline uint32_t GetResultWBit(ResultCode resultCode){ return ((resultCode >> 29) & 0x1); }
inline uint32_t GetResultErrCode(ResultCode resultCode){ return (resultCode && 0x1FFFFFFF); }

#define OptResultCurlCouldntResolveHost   1006   //CURLE_COULDNT_RESOLVE_HOST
#define OptResultCurlCouldntConnect       1007   //CURLE_COULDNT_CONNECT
#define OptResultCurlOperationTimedout    1028   //CURLE_OPERATION_TIMEDOUT
#define OptResultCurlGotNothing           1052   //CURLE_GOT_NOTHING

#define OptResultUnknownPtmError          2001
#define OptResultNonceHasExpired          2002
#define OptResultOptTooFrequent           2003 
#define OptResultMoneyNotEnough           2004 
#define OptResultCoinNotEnough            2005 
#define OptResultNoMatchedOrderId         2006 
#define OptResultParameterError           2007
#define OptResultOrderIsClosed            2008

#endif