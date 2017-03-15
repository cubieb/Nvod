#ifndef _BaseType_h_
#define _BaseType_h_

#include <cstdint>

#include <cstdint>
#include <chrono>

typedef unsigned char uchar_t;
typedef unsigned int uint_t;

typedef uint32_t TableId;  //Index field type of database table.
#define InvalidTableId    0

typedef uint16_t TsId;     //Transport Stream Id
typedef uint16_t ServiceId;
typedef uint16_t EventId;
typedef uint16_t MovieId;
typedef uint16_t PosterId;

#define InvalidTsId       0
#define InvalidServiceId  0
#define InvalidEventId    0
#define InvalidMovieId    0
#define InvalidPosterId   0

typedef uint8_t  SyncByte;
typedef uint16_t TransportErrorIndicator;
typedef uint16_t PayloadUnitStartIndicator;
typedef uint16_t TransportPriority;
typedef uint16_t Pid;
typedef uint8_t  TransportScramblingControl;
typedef uint8_t  ContinuityCounter;
typedef uint8_t  SiPsiTableId;
typedef uint8_t  AdaptationFieldControl;
typedef uint8_t  AdaptationFieldLength;
typedef uint8_t  PcrFlag;
typedef uint64_t Pcr;
typedef uint16_t SectionLength;
typedef uint8_t  VersionNumber;
typedef uint8_t  CurrentNextIndicator;
typedef uint8_t  SectionNumber;
typedef uint16_t ProgramNumber;

/* PMT Table Member */
typedef uint16_t ProgramInfoLength;
typedef uint8_t  StreamType;
typedef uint16_t EsInfoLength;
typedef uint8_t  DescriptorTag;
typedef uint8_t  DescriptorLength;

/* Dit, Ddt */
typedef uint16_t DitSectionNumber;
typedef uint16_t DdtSectionNumber;

#define PatPid 0x0000

#define TsPacketSize    188
#define TsPacketBufSize 1316 /* (TsPacketSize * 7) */

typedef std::chrono::steady_clock::time_point TimePoint;
typedef std::chrono::steady_clock::duration Duration;
typedef std::chrono::seconds Seconds;
typedef std::chrono::milliseconds Milliseconds;

#endif