#ifndef _Type_h_
#define _Type_h_

#include <cstdint>

typedef unsigned char uchar_t;
typedef unsigned int uint_t;

typedef uint32_t Ipv4;
typedef uint16_t UdpPort;

typedef uint16_t EventId;
typedef uint32_t MovieId;
typedef uint16_t Pid;
typedef uint32_t PosterId;
typedef uint16_t ServiceId;
typedef uint32_t PlayerId;
typedef uint16_t TsId;      //Transport Stream Id
typedef uint32_t TableIndex;     //Index field type of database table.
typedef int      TimerId;

#define MakePlayerId(tsId, serviceId) ((tsId << (sizeof(ServiceId) * 8)) | (serviceId))


#define InvalidMovieId    0
#define InvalidPosterId   0
#define InvalidServiceId  0
#define InvalidTsId       0
#define InvalidTableIndex 0
#define InvalidTimerId    -1

//typedef std::chrono::system_clock::time_point TimePoint;
//typedef std::chrono::seconds Seconds;
typedef time_t TimePoint;
typedef time_t Seconds;
typedef time_t Milliseconds;
typedef time_t Microseconds;

#endif