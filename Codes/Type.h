#ifndef _Type_h_
#define _Type_h_

#include "SystemInclude.h"

typedef uint32_t Ipv4;
typedef uint16_t UdpPort;

typedef uint16_t EventId;
typedef uint32_t MovieId;
typedef uint16_t Pid;
typedef uint32_t PosterId;
typedef uint16_t ServiceId;
typedef uint16_t TsId;      //Transport Stream Id

typedef std::chrono::system_clock::time_point TimePoint;
typedef std::chrono::seconds Seconds;
typedef std::chrono::milliseconds Milliseconds;
typedef std::chrono::microseconds Microseconds;

#define InvalidAceTimerId (-1)

#endif