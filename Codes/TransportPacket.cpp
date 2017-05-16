#include "TransportPacket.h"

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"
#include "PacketHelper.h"
#include "Crc32.h"

using namespace std;

/**********************class TransportPacketHelperInterface**********************/
TransportPacketHelperInterface* TransportPacketHelperInterface::CreateInstance(uchar_t *tsPacketHeader)
{
    return new TransportPacketHelper(tsPacketHeader);
}

TransportPacketHelperInterface* TransportPacketHelperInterface::CreateInstance(uchar_t *tsPacketHeader,
	PayloadUnitStartIndicator payloadUnitStartIndicator, Pid pid,
	ContinuityCounter& continuityCounter)
{
	return new TransportPacketHelper(tsPacketHeader, payloadUnitStartIndicator, pid, continuityCounter);
}

/**********************class PatHelperInterface**********************/
PatHelperInterface* PatHelperInterface::CreateInstance(uchar_t *patHeader)
{
    return new PatHelper(patHeader);
}

PatHelperInterface* PatHelperInterface::CreateInstance(uchar_t *patHeader, TsId tsId)
{
	return new PatHelper(patHeader, tsId);
}

PatElementaryInterface* PatElementaryInterface::CreateInstance(uchar_t *patElementary)
{
    return new PatElementary(patElementary);
}

/**********************class PmtHelperInterface**********************/
PmtHelperInterface* PmtHelperInterface::CreateInstance(uchar_t *pmtHeader)
{
    return new PmtHelper(pmtHeader);
}

PmtHelperInterface* PmtHelperInterface::CreateInstance(uchar_t *pmtHeader, ProgramNumber programNumber)
{
	return new PmtHelper(pmtHeader, programNumber);
}

PmtElementaryInterface* PmtElementaryInterface::CreateInstance(uchar_t *pmtElementary)
{
    return new PmtElementary(pmtElementary);
}

/**********************class DitHelperInterface**********************/
DitHelperInterface* DitHelperInterface::CreateInstance(uchar_t *ditHeader)
{
    return new DitHelper(ditHeader);
}

DitHelperInterface* DitHelperInterface::CreateInstance(uchar_t *ditHeader, VersionNumber versionNumber)
{
	return new DitHelper(ditHeader, versionNumber);
}

DitElementaryInterface* DitElementaryInterface::CreateInstance(uchar_t *ditElementary)
{
    return new DitElementary(ditElementary);
}

/**********************class DdtHelperInterface**********************/
DdtHelperInterface* DdtHelperInterface::CreateInstance(uchar_t *ddtHeader)
{
    return new DdtHelper(ddtHeader);
}

DdtHelperInterface* DdtHelperInterface::CreateInstance(uchar_t *ddtHeader, PosterId posterId)
{
	return new DdtHelper(ddtHeader, posterId);
}

/**********************class TransportPacketHelper**********************/
TransportPacketHelper::TransportPacketHelper(uchar_t *tsPacketHeader)
    : tsPacketHeader(reinterpret_cast<transport_packet*>(tsPacketHeader))
{}

TransportPacketHelper::TransportPacketHelper(uchar_t *tsPacketHeader, 
	PayloadUnitStartIndicator payloadUnitStartIndicator, 
	Pid pid, ContinuityCounter& continuityCounter)
	: tsPacketHeader(reinterpret_cast<transport_packet*>(tsPacketHeader))
{
	SetSyncByte(0x47);
	SetTransportErrorIndicator(0);
	SetPayloadUnitStartIndicator(payloadUnitStartIndicator);
	SetTransportPriority(0);
	SetPid(pid);
	SetTransportScramblingControl(0);
	SetAdaptationFieldControl(1);
	SetContinuityCounter(continuityCounter);
	continuityCounter = (continuityCounter + 1) & 0x0F;
}

TransportPacketHelper::~TransportPacketHelper()
{}

SyncByte TransportPacketHelper::GetSyncByte() const
{	
    return ReadBuffer(&tsPacketHeader->sync_byte, 0, 8);
}

void TransportPacketHelper::SetSyncByte(SyncByte syncByte)
{
    WriteBuffer(&tsPacketHeader->sync_byte, syncByte, 0, 8);
}

TransportErrorIndicator TransportPacketHelper::GetTransportErrorIndicator() const
{
	return ReadBuffer(&tsPacketHeader->pseudo_member1, 15, 1);
}

void TransportPacketHelper::SetTransportErrorIndicator(TransportErrorIndicator transportErrorIndicator)
{
    WriteBuffer(&tsPacketHeader->pseudo_member1, transportErrorIndicator, 15, 1);
}

PayloadUnitStartIndicator TransportPacketHelper::GetPayloadUnitStartIndicator() const
{
    return ReadBuffer(&tsPacketHeader->pseudo_member1, 14, 1);
}

void TransportPacketHelper::SetPayloadUnitStartIndicator(PayloadUnitStartIndicator payloadUnitStartIndicator)
{
    if (payloadUnitStartIndicator == 1)
    {
        WriteBuffer((uchar_t*)tsPacketHeader + 4, (uchar_t)0, 0, 8);
    }
    WriteBuffer(&tsPacketHeader->pseudo_member1, payloadUnitStartIndicator, 14, 1);
}

TransportPriority TransportPacketHelper::GetTransportPriority() const
{
    return ReadBuffer(&tsPacketHeader->pseudo_member1, 13, 1);
}

void TransportPacketHelper::SetTransportPriority(TransportPriority transportPriority)
{
    WriteBuffer(&tsPacketHeader->pseudo_member1, transportPriority, 13, 1);
}

Pid TransportPacketHelper::GetPid() const
{
    return ReadBuffer(&tsPacketHeader->pseudo_member1, 0, 13);
}

void TransportPacketHelper::SetPid(Pid pid)
{
    WriteBuffer(&tsPacketHeader->pseudo_member1, pid, 0, 13);
}

TransportScramblingControl TransportPacketHelper::GetTransportScramblingControl() const
{
    return ReadBuffer(&tsPacketHeader->pseudo_member2, 6, 2);
}

void TransportPacketHelper::SetTransportScramblingControl(TransportScramblingControl transportScramblingControl)
{
    WriteBuffer(&tsPacketHeader->pseudo_member2, transportScramblingControl, 6, 2);
}

AdaptationFieldControl TransportPacketHelper::GetAdaptationFieldControl() const
{
    return ReadBuffer(&tsPacketHeader->pseudo_member2, 4, 2);
}

void TransportPacketHelper::SetAdaptationFieldControl(AdaptationFieldControl adaptationFieldControl)
{
    WriteBuffer(&tsPacketHeader->pseudo_member2, adaptationFieldControl, 4, 2);
}

ContinuityCounter TransportPacketHelper::GetContinuityCounter() const
{
    return ReadBuffer(&tsPacketHeader->pseudo_member2, 0, 4);
}

void TransportPacketHelper::SetContinuityCounter(ContinuityCounter continuityCounter)
{
    WriteBuffer(&tsPacketHeader->pseudo_member2, continuityCounter, 0, 4);
}

AdaptationFieldLength TransportPacketHelper::GetAdaptationFieldLength() const 
{
    AdaptationFieldControl afc = GetAdaptationFieldControl();
    assert(afc == 0x2 || afc == 0x3);

    return ReadBuffer(&tsPacketHeader->adaptation_field_length, 0, 8);
}

void TransportPacketHelper::SetAdaptationFieldLength(AdaptationFieldLength adaptationFieldLength)
{
    AdaptationFieldControl afc = GetAdaptationFieldControl();
    assert(afc == 0x2 || afc == 0x3);

    WriteBuffer(&tsPacketHeader->adaptation_field_length, adaptationFieldLength, 0, 8);
}

PcrFlag TransportPacketHelper::GetPcrFlag() const
{
	assert(GetAdaptationFieldLength() != 0);

    return ReadBuffer(&tsPacketHeader->pseudo_member3, 4, 1);
}

void TransportPacketHelper::SetPcrFlag(PcrFlag pcrFlag)
{
    assert(GetAdaptationFieldLength() != 0);

    WriteBuffer(&tsPacketHeader->pseudo_member3, pcrFlag, 4, 1);
}

Pcr TransportPacketHelper::GetPcr() const
{
    assert(GetPcrFlag() == 1);

	Pcr pcrBase, pcrExt;
    pcrBase = ReadBuffer(&tsPacketHeader->pseudo_member4, 31, 33);
    pcrExt = ReadBuffer(&tsPacketHeader->pseudo_member4, 16, 9);

    return (pcrBase * 300 + pcrExt);
}

void TransportPacketHelper::SetPcr(Pcr pcr)
{
    assert(GetPcrFlag() == 1);

    Pcr pcrBase, reserve, pcrExt;
	pcrBase = (pcr / 300) & 0x1FFFFFFFF;
    reserve = 0x3F;
	pcrExt = pcr % 300;

    WriteBuffer(&tsPacketHeader->pseudo_member4, pcrBase, 31, 33);
    WriteBuffer(&tsPacketHeader->pseudo_member4, reserve, 25, 6);
    WriteBuffer(&tsPacketHeader->pseudo_member4, pcrExt, 16, 9);
}

uchar_t* TransportPacketHelper::GetPayLoadHeader() const
{
    return  (uchar_t*)tsPacketHeader + GetSize();
}

void TransportPacketHelper::FillPad(size_t payLoadSize)
{
    uchar_t *ptr = (uchar_t*)tsPacketHeader + GetSize() + payLoadSize;
    assert(ptr <= (uchar_t*)tsPacketHeader + TsPacketSize);

    while (ptr != (uchar_t*)tsPacketHeader + TsPacketSize)
    {
        ptr = ptr + WriteBuffer(ptr, (uchar_t)0xFF);
    }        
}

uchar_t* TransportPacketHelper::GetHeader() const
{
    return reinterpret_cast<uchar_t*>(tsPacketHeader);
}

size_t TransportPacketHelper::GetSize() const
{
    size_t size = 4;
    AdaptationFieldControl afc = GetAdaptationFieldControl();
    if (afc == 0x2 || afc == 0x3)
    {
        size = size + GetAdaptationFieldLength() + sizeof(AdaptationFieldLength);
    }

    if (GetPayloadUnitStartIndicator() == 1)
    {
        size = size + 1;
    }

    assert(size <= TsPacketSize);
    return size;
}

/**********************class PatHelper**********************/
PatHelper::PatHelper(uchar_t *patHeader)
    : patHeader(reinterpret_cast<program_association_section*>(patHeader))
{
    WriteBuffer(&this->patHeader->table_id, (uint8_t)0x0, 0, 8);
    WriteBuffer(&this->patHeader->pseudo_member1, (uint16_t)0x1, 15, 1);
    WriteBuffer(&this->patHeader->pseudo_member1, (uint16_t)0x0, 14, 1);
    WriteBuffer(&this->patHeader->pseudo_member1, (uint16_t)0x3, 12, 2);
    WriteBuffer(&this->patHeader->pseudo_member2, (uint8_t)0x3, 6, 2);
}

PatHelper::PatHelper(uchar_t *patHeader, TsId tsId)
	: PatHelper(patHeader)
{
	SetSectionLength(GetMinSectionLength());
	SetTransportStreamId(tsId);
	SetVersionNumber(0);
	SetCurrentNextIndicator(1);
	SetSectionMumber(0);
	SetLastSectionMumber(0);
}

PatHelper::~PatHelper()
{}

SiPsiTableId PatHelper::GetTableId() const
{
    return ReadBuffer(&patHeader->table_id, 0, 8);
}

SectionLength PatHelper::GetSectionLength() const
{
    SectionLength sectionLength = ReadBuffer(&patHeader->pseudo_member1, 0, 12);
    assert(sectionLength <= 1021);
    return ReadBuffer(&patHeader->pseudo_member1, 0, 12);
}

void PatHelper::SetSectionLength(SectionLength sectionLength)
{
    assert(sectionLength <= 1021);
    WriteBuffer(&patHeader->pseudo_member1, sectionLength, 0, 12);
}

TsId PatHelper::GetTransportStreamId() const
{
    return ReadBuffer(&patHeader->transport_stream_id, 0, 16);
}

void PatHelper::SetTransportStreamId(TsId transportStreamId)
{
    WriteBuffer(&patHeader->transport_stream_id, transportStreamId, 0, 16);
}

VersionNumber PatHelper::GetVersionNumber() const
{
    return ReadBuffer(&patHeader->pseudo_member2, 1, 5);
}

void PatHelper::SetVersionNumber(VersionNumber versionNumber)
{
    WriteBuffer(&patHeader->pseudo_member2, versionNumber, 1, 5);
}

CurrentNextIndicator PatHelper::GetCurrentNextIndicator() const
{
    return ReadBuffer(&patHeader->pseudo_member2, 0, 1);
}

void PatHelper::SetCurrentNextIndicator(CurrentNextIndicator currentNextIndicator)
{
    WriteBuffer(&patHeader->pseudo_member2, currentNextIndicator, 0, 1);
}

SectionNumber PatHelper::GetSectionMumber() const
{
    return ReadBuffer(&patHeader->section_number, 0, 8);
}

void PatHelper::SetSectionMumber(SectionNumber sectionMumber)
{
    WriteBuffer(&patHeader->section_number, sectionMumber, 0, 8);
}

SectionNumber PatHelper::GetLastSectionMumber() const
{
    return ReadBuffer(&patHeader->last_section_number, 0, 8);
}

void PatHelper::SetLastSectionMumber(SectionNumber lastSectionMumber)
{
    WriteBuffer(&patHeader->last_section_number, lastSectionMumber, 0, 8);
}

uchar_t* PatHelper::GetHeader() const
{
    return reinterpret_cast<uchar_t*>(patHeader);
}

size_t PatHelper::GetMinSectionLength() const
{
    return GetMinSize() - 3;
}

size_t PatHelper::GetMinSize() const
{
    /* table_id ~ last_section_number, without any program infor */
    return sizeof(program_association_section) + GetCrcCodeSize();
}

size_t PatHelper::GetCrcCodeSize() const
{
    return 4;
}

size_t PatHelper::GetSize() const
{
    return 3 /* table_id, pseudo_member1*/ + GetSectionLength();
}

void PatHelper::UpdateCrcCode()
{
    uchar_t *crc = (uchar_t*)&patHeader->pseudo_member1 + sizeof(uint16_t) + GetSectionLength() - 4;
    WriteBuffer((uint32_t*)crc, Crc32::CalculateCrc((uchar_t*)patHeader, crc - (uchar_t*)patHeader));
}

PatElementary::PatElementary(uchar_t *patElementary)
    : patElementary(reinterpret_cast<TsPatElementary*>(patElementary))
{
    WriteBuffer(&this->patElementary->pseudo_member1, (uint16_t)0x7, 13, 3);
}

PatElementary::~PatElementary()
{}

ProgramNumber PatElementary::GetProgramNumber() const
{
    return ReadBuffer(&patElementary->program_number, 0, 16);
}

void PatElementary::SetProgramNumber(ProgramNumber programNumber)
{
    WriteBuffer(&patElementary->program_number, programNumber, 0, 16);
}

Pid PatElementary::GetPmtPid() const
{
    return ReadBuffer(&patElementary->pseudo_member1, 0, 13);
}

void PatElementary::SetPmtPid(Pid pmtPid)
{
    WriteBuffer(&patElementary->pseudo_member1, pmtPid, 0, 13);
}

size_t PatElementary::GetSize() const
{
    return 4;
}

/**********************class PmtHelper**********************/
PmtHelper::PmtHelper(uchar_t *pmtHeader)
    : pmtHeader(reinterpret_cast<TS_program_map_section*>(pmtHeader))
{
    WriteBuffer(&this->pmtHeader->table_id, (uint8_t)0x2, 0, 8);

    /* section_syntax_indicator */
    WriteBuffer(&this->pmtHeader->pseudo_member1, (uint16_t)0x1, 15, 1);
    /* '0' */
    WriteBuffer(&this->pmtHeader->pseudo_member1, (uint16_t)0x0, 14, 1);
    /* reserved1 */
    WriteBuffer(&this->pmtHeader->pseudo_member1, (uint16_t)0x3, 12, 2);

    /* reserved2 */
    WriteBuffer(&this->pmtHeader->pseudo_member2, (uint8_t)0x3, 6, 2);

    /* reserved3 */
    WriteBuffer(&this->pmtHeader->pseudo_member3, (uint16_t)0x7, 13, 3);

    /* reserved4 */
    WriteBuffer(&this->pmtHeader->pseudo_member4, (uint16_t)0xF, 12, 4);
}

PmtHelper::PmtHelper(uchar_t *pmtHeader, ProgramNumber programNumber)
	: PmtHelper(pmtHeader)
{
	SetSectionLength(GetMinSectionLength());
	SetProgramNumber(programNumber);
	SetVersionNumber(0);
	SetCurrentNextIndicator(1);
	SetSectionMumber(0);
	SetLastSectionMumber(0);
	SetPcrPid(0);
	SetProgramInfoLength(0);
}

PmtHelper::~PmtHelper()
{}

SiPsiTableId PmtHelper::GetTableId() const
{
    return ReadBuffer(&this->pmtHeader->table_id, 0, 8);
}

SectionLength PmtHelper::GetSectionLength() const
{
    SectionLength sectionLength = ReadBuffer(&pmtHeader->pseudo_member1, 0, 12);
    assert(sectionLength <= 1021);
    return sectionLength;
}

void PmtHelper::SetSectionLength(SectionLength sectionLength)
{
    assert(sectionLength <= 1021);
    WriteBuffer(&pmtHeader->pseudo_member1, sectionLength, 0, 12);
}

ProgramNumber PmtHelper::GetProgramNumber() const
{
    return ReadBuffer(&pmtHeader->program_number, 0, 16);
}

void PmtHelper::SetProgramNumber(ProgramNumber programNumber)
{
    WriteBuffer(&pmtHeader->program_number, programNumber, 0, 16);
}

VersionNumber PmtHelper::GetVersionNumber() const
{
    return ReadBuffer(&pmtHeader->pseudo_member2, 1, 5);
}

void PmtHelper::SetVersionNumber(VersionNumber versionNumber)
{
    WriteBuffer(&pmtHeader->pseudo_member2, versionNumber, 1, 5);
}

CurrentNextIndicator PmtHelper::GetCurrentNextIndicator() const
{
    return ReadBuffer(&pmtHeader->pseudo_member2, 0, 1);
}

void PmtHelper::SetCurrentNextIndicator(CurrentNextIndicator currentNextIndicator)
{
    WriteBuffer(&pmtHeader->pseudo_member2, currentNextIndicator, 0, 1);
}

SectionNumber PmtHelper::GetSectionMumber() const
{
    return ReadBuffer(&pmtHeader->section_number, 0, 8);
}

void PmtHelper::SetSectionMumber(SectionNumber sectionMumber)
{
    WriteBuffer(&pmtHeader->section_number, sectionMumber, 0, 8);
}

SectionNumber PmtHelper::GetLastSectionMumber() const
{
    return ReadBuffer(&pmtHeader->last_section_number, 0, 8);
}

void PmtHelper::SetLastSectionMumber(SectionNumber lastSectionMumber)
{
    WriteBuffer(&pmtHeader->last_section_number, lastSectionMumber, 0, 8);
}

Pid PmtHelper::GetPcrPid() const
{
    return ReadBuffer(&pmtHeader->pseudo_member3, 0, 13);
}

void PmtHelper::SetPcrPid(Pid pcrPid)
{
    WriteBuffer(&pmtHeader->pseudo_member3, pcrPid, 0, 13);
}

ProgramInfoLength PmtHelper::GetProgramInfoLength() const
{
    return ReadBuffer(&pmtHeader->pseudo_member4, 0, 12);
}

void PmtHelper::SetProgramInfoLength(ProgramInfoLength programInfoLength)
{
    WriteBuffer(&pmtHeader->pseudo_member4, programInfoLength, 0, 12);
}

uchar_t* PmtHelper::GetProgramInfoes() const
{
    return (uchar_t*)&pmtHeader->pseudo_member4 + sizeof(uint16_t);
}

uchar_t* PmtHelper::GetHeader() const
{
    return reinterpret_cast<uchar_t*>(pmtHeader);
}

size_t PmtHelper::GetMinSectionLength() const
{
    return GetMinSize() - 3;
}

size_t PmtHelper::GetMinSize() const
{
    return sizeof(TS_program_map_section) + GetCrcCodeSize();
}

size_t PmtHelper::GetCrcCodeSize() const
{
    return 4;
}

size_t PmtHelper::GetSize() const
{
    return 3 + GetSectionLength();
}

void PmtHelper::UpdateCrcCode()
{
    uchar_t *crc = (uchar_t*)pmtHeader + GetSize() - GetCrcCodeSize();
    WriteBuffer((uint32_t*)crc, Crc32::CalculateCrc((uchar_t*)pmtHeader, GetSize() - GetCrcCodeSize()));
}

/**********************class PmtElementary**********************/
PmtElementary::PmtElementary(uchar_t *pmtElementary)
    : pmtElementary(reinterpret_cast<TsPmtElementary*>(pmtElementary))
{
    /* reserved_1 */
    WriteBuffer(&this->pmtElementary->pseudo_member1, (uint16_t)0x7, 13, 3);

    /* reserved_2 */
    WriteBuffer(&this->pmtElementary->pseudo_member2, (uint16_t)0xF, 12, 4);
}

PmtElementary::~PmtElementary()
{}

StreamType PmtElementary::GetStreamType() const
{
    return ReadBuffer(&pmtElementary->stream_type, 0, 8);
}

void PmtElementary::SetStreamType(StreamType streamType)
{
    WriteBuffer(&pmtElementary->stream_type, streamType, 0, 8);
}

Pid PmtElementary::GetElementaryPid() const
{
    return ReadBuffer(&pmtElementary->pseudo_member1, 0, 13);
}

void PmtElementary::SetElementaryPid(Pid elementaryPid)
{
    WriteBuffer(&pmtElementary->pseudo_member1, elementaryPid, 0, 13);
}

EsInfoLength PmtElementary::GetEsInfoLength() const
{
    return ReadBuffer(&pmtElementary->pseudo_member2, 0, 12);
}

void PmtElementary::SetEsInfoLength(EsInfoLength esInfoLength)
{
    WriteBuffer(&pmtElementary->pseudo_member2, esInfoLength, 0, 12);
}

size_t PmtElementary::GetSize() const
{
    return 5 + GetEsInfoLength();
}

/**********************class DitHelper**********************/
DitHelper::DitHelper(uchar_t *ditHeader)
    : ditHeader(reinterpret_cast<ts_packet_poster_dit_data*>(ditHeader))
{
    WriteBuffer(&this->ditHeader->table_id, (uint8_t)0x90, 0, 8);

    /* section_syntax_indicator */
    WriteBuffer(&this->ditHeader->pseudo_member1, (uint16_t)0x1, 15, 1);
    /* reserved_future_use */
    WriteBuffer(&this->ditHeader->pseudo_member1, (uint16_t)0x1, 14, 1);
    /* reserved1 */
    WriteBuffer(&this->ditHeader->pseudo_member1, (uint16_t)0x3, 12, 2);

    WriteBuffer(&this->ditHeader->table_id_extend, (uint16_t)0x0, 0, 16);

    /* reserved2 */
    WriteBuffer(&this->ditHeader->pseudo_member2, (uint8_t)0x3, 6, 2);
}

DitHelper::DitHelper(uchar_t *ditHeader, VersionNumber versionNumber)
	: DitHelper(ditHeader)
{
	SetSectionLength(GetMinSectionLength());
	SetVersionNumber(versionNumber);
	SetSectionMumber(0);
	SetLastSectionMumber(0);
}

DitHelper::~DitHelper()
{}

SiPsiTableId DitHelper::GetTableId() const
{
    return ReadBuffer(&ditHeader->table_id, 0, 8);
}

SectionLength DitHelper::GetSectionLength() const
{
    SectionLength sectionLength = ReadBuffer(&ditHeader->pseudo_member1, 0, 12);
    assert(sectionLength <= 1021);
    return sectionLength;
}

void DitHelper::SetSectionLength(SectionLength sectionLength)
{
    assert(sectionLength <= 1021);
    WriteBuffer(&ditHeader->pseudo_member1, sectionLength, 0, 12);
}

VersionNumber DitHelper::GetVersionNumber() const
{
    return ReadBuffer(&ditHeader->pseudo_member2, 1, 5);
}

void DitHelper::SetVersionNumber(VersionNumber versionNumber)
{
    WriteBuffer(&ditHeader->pseudo_member2, versionNumber, 1, 5);
}

DitSectionNumber DitHelper::GetSectionMumber() const
{
    return ReadBuffer(&ditHeader->section_number, 0, sizeof(DitSectionNumber) * 8);
}

void DitHelper::SetSectionMumber(DitSectionNumber sectionMumber)
{
    WriteBuffer(&ditHeader->section_number, sectionMumber, 0, sizeof(DitSectionNumber) * 8);
}

DitSectionNumber DitHelper::GetLastSectionMumber() const
{
    return ReadBuffer(&ditHeader->last_section_number, 0, sizeof(DitSectionNumber) * 8);
}

void DitHelper::SetLastSectionMumber(DitSectionNumber lastSectionMumber)
{
    WriteBuffer(&ditHeader->last_section_number, lastSectionMumber, 0, sizeof(DitSectionNumber) * 8);
}

uchar_t* DitHelper::GetHeader() const
{
    return reinterpret_cast<uchar_t*>(ditHeader);
}

size_t DitHelper::GetMinSectionLength() const
{
    return GetMinSize() - 3;
}

size_t DitHelper::GetMinSize() const
{
    return sizeof(ts_packet_poster_dit_data) + GetCrcCodeSize();
}

size_t DitHelper::GetCrcCodeSize() const
{
    return 4;
}

size_t DitHelper::GetSize() const
{
    return 3 /* table_id, pseudo_member1*/ + GetSectionLength();
}

void DitHelper::UpdateCrcCode()
{
    uchar_t *crc = (uchar_t*)ditHeader + GetSize() - GetCrcCodeSize();
    WriteBuffer((uint32_t*)crc, Crc32::CalculateCrc((uchar_t*)ditHeader, GetSize() - GetCrcCodeSize()));
}

DitElementary::DitElementary(uchar_t *ditElementary)
    : ditElementary(reinterpret_cast<ts_packet_poster_file_info*>(ditElementary))
{}

DitElementary::~DitElementary()
{}

uint16_t DitElementary::GetFileId() const
{
    return ReadBuffer(&ditElementary->file_id, 0, sizeof(uint16_t) * 8);
}

void DitElementary::SetFileId(uint16_t fileId)
{
    WriteBuffer(&ditElementary->file_id, fileId, 0, sizeof(uint16_t) * 8);
}

uint16_t DitElementary::GetEventLoopLength() const
{   
    return ReadBuffer(&ditElementary->event_loop_length, 0, sizeof(uint16_t) * 8);
}

void DitElementary::SetEventLoopLength(uint16_t eventLoopLength)
{
    WriteBuffer(&ditElementary->event_loop_length, eventLoopLength, 0, sizeof(uint16_t) * 8);
}

uchar_t* DitElementary::GetHeader() const
{
    return reinterpret_cast<uchar_t*>(ditElementary);
}

size_t DitElementary::GetSize() const
{
    return 4 + GetEventLoopLength();
}

/**********************class DdtHelperInterface**********************/
DdtHelper::DdtHelper(uchar_t *ddtHeader)
    : ddtHeader(reinterpret_cast<ts_packet_poster_ddt_data*>(ddtHeader))
{
    WriteBuffer(&this->ddtHeader->table_id, (uint8_t)0x91, 0, 8);

    /* section_syntax_indicator */
    WriteBuffer(&this->ddtHeader->pseudo_member1, (uint16_t)0x1, 15, 1);
    /* reserved_future_use */
    WriteBuffer(&this->ddtHeader->pseudo_member1, (uint16_t)0x1, 14, 1);
    /* reserved1 */
    WriteBuffer(&this->ddtHeader->pseudo_member1, (uint16_t)0x3, 12, 2);

    /* reserved2 */
    WriteBuffer(&this->ddtHeader->pseudo_member2, (uint8_t)0x3, 6, 2);

	SetSectionLength(GetMinSectionLength());
	SetPosterId(0);
	SetVersionNumber(0);
	SetSectionMumber(0);
	SetLastSectionMumber(0);
}

DdtHelper::DdtHelper(uchar_t *ddtHeader, PosterId posterId)
	: DdtHelper(ddtHeader)
{
	SetPosterId(posterId);
}

DdtHelper::~DdtHelper()
{}

SiPsiTableId DdtHelper::GetTableId() const
{
    return ReadBuffer(&ddtHeader->table_id, 0, 8);
}

SectionLength DdtHelper::GetSectionLength() const
{
    SectionLength sectionLength = ReadBuffer(&ddtHeader->pseudo_member1, 0, 12);
    assert(sectionLength <= 1021);
    return sectionLength;
}

void DdtHelper::SetSectionLength(SectionLength sectionLength)
{
    assert(sectionLength <= 1021);
    WriteBuffer(&ddtHeader->pseudo_member1, sectionLength, 0, 12);
}

PosterId DdtHelper::GetPosterId() const
{
    return ReadBuffer(&ddtHeader->file_id, 0, 16);
}

void DdtHelper::SetPosterId(PosterId posterId)
{
    WriteBuffer(&ddtHeader->file_id, posterId, 0, 16);
}

VersionNumber DdtHelper::GetVersionNumber() const
{
    return ReadBuffer(&ddtHeader->pseudo_member2, 1, 5);
}

void DdtHelper::SetVersionNumber(VersionNumber versionNumber)
{
    WriteBuffer(&ddtHeader->pseudo_member2, versionNumber, 1, 5);
}

DdtSectionNumber DdtHelper::GetSectionMumber() const
{
    return ReadBuffer(&ddtHeader->section_number, 0, sizeof(DdtSectionNumber) * 8);
}

void DdtHelper::SetSectionMumber(DdtSectionNumber sectionMumber)
{
    WriteBuffer(&ddtHeader->section_number, sectionMumber, 0, sizeof(DdtSectionNumber) * 8);
}

DdtSectionNumber DdtHelper::GetLastSectionMumber() const
{
    return ReadBuffer(&ddtHeader->last_section_number, 0, sizeof(DdtSectionNumber) * 8);
}

void DdtHelper::SetLastSectionMumber(DdtSectionNumber lastSectionMumber)
{
    WriteBuffer(&ddtHeader->last_section_number, lastSectionMumber, 0, sizeof(DdtSectionNumber) * 8);
}

uchar_t* DdtHelper::GetHeader() const
{
    return reinterpret_cast<uchar_t*>(ddtHeader);
}

size_t DdtHelper::GetMinSectionLength() const
{
    return GetMinSize() - 3;
}

size_t DdtHelper::GetMinSize() const
{
    return sizeof(ts_packet_poster_ddt_data) + GetCrcCodeSize();
} 
size_t DdtHelper::GetCrcCodeSize() const
{
    return 4;
}

size_t DdtHelper::GetSize() const
{
    return 3 /* table_id, pseudo_member1*/ + GetSectionLength();
}

void DdtHelper::UpdateCrcCode()
{
    uchar_t *crc = (uchar_t*)ddtHeader + GetSize() - GetCrcCodeSize();
    WriteBuffer((uint32_t*)crc, Crc32::CalculateCrc((uchar_t*)ddtHeader, GetSize() - GetCrcCodeSize()));
}
