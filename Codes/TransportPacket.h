#ifndef _TransportPacket_h_
#define _TransportPacket_h_

#include "TransportPacketInterface.h"

#pragma pack(push, 1)
/*
define struct transport_packet just for calculating fixed fields size.
*/
struct transport_packet
{
    uint8_t  sync_byte;                       // 8 uimsbf  -

    uint16_t pseudo_member1;
    //uint16_t transport_error_indicator : 1;       // 1 bslbf    
    //uint16_t payload_unit_start_indicator : 1;    // 1 bslbf    
    //uint16_t transport_priority : 1;              // 1 bslbf
    //uint16_t PID : 13;                            // 13 bslbf  --

    uint8_t pseudo_member2;
    //uchar_t  transport_scrambling_control : 2;    // 2 bslbf
    //uchar_t  adaptation_field_control : 2;        // 2 bslbf
    //uchar_t  continuity_counter : 4;              // 4 bslbf   -

    /* if(adaptation_field_control = = '10' || adaptation_field_control = = '11') */
    uint8_t adaptation_field_length;

    /* if (adaptation_field_length > 0) */
    uint8_t pseudo_member3;
    //uint8_t discontinuity_indicator : 1;
    //uint8_t random_access_indicator : 1;
    //uint8_t elementary_stream_priority_indicator : 1;
    //uint8_t PCR_flag : 1;
    //uint8_t OPCR_flag : 1;
    //uint8_t splicing_point_flag : 1;
    //uint8_t transport_private_data_flag : 1;
    //uint8_t adaptation_field_extension_flag : 1;

    /* if (PCR_flag = = '1') */
    uint64_t pseudo_member4;
    //uint64_t program_clock_reference_base : 33;
    //uint64_t reserved : 6;
    //uint64_t program_clock_reference_extension : 9;
    //uint64_t ... ... : 16;
};
#pragma pack(pop)

/**********************class TransportPacketHelper**********************/
class TransportPacketHelper : public TransportPacketHelperInterface
{
public:
    TransportPacketHelper(uchar_t *tsPacketHeader);
    ~TransportPacketHelper();

    SyncByte GetSyncByte() const;
    void SetSyncByte(SyncByte syncByte);

	TransportErrorIndicator GetTransportErrorIndicator() const;
	void SetTransportErrorIndicator(TransportErrorIndicator transportErrorIndicator);

	PayloadUnitStartIndicator GetPayloadUnitStartIndicator() const;
	void SetPayloadUnitStartIndicator(PayloadUnitStartIndicator payloadUnitStartIndicator);

	TransportPriority GetTransportPriority() const;
	void SetTransportPriority(TransportPriority transportPriority);

    Pid GetPid() const;
    void SetPid(Pid pid);

	TransportScramblingControl GetTransportScramblingControl() const;
	void SetTransportScramblingControl(TransportScramblingControl transportScramblingControl);

	AdaptationFieldControl GetAdaptationFieldControl() const;
	void SetAdaptationFieldControl(AdaptationFieldControl adaptationFieldControl);

	ContinuityCounter GetContinuityCounter() const;
	void SetContinuityCounter(ContinuityCounter continuityCounter);

	AdaptationFieldLength GetAdaptationFieldLength() const;
	void SetAdaptationFieldLength(AdaptationFieldLength adaptationFieldLength);

	PcrFlag GetPcrFlag() const;
	void SetPcrFlag(PcrFlag pcrFlag);

	Pcr GetPcr() const;
	void SetPcr(Pcr pcr);

	uchar_t* GetPayLoadHeader() const;
	void FillPad(size_t payLoadSize);

    size_t GetSize() const;

private:
    transport_packet *tsPacketHeader;
};

/* 2.4.4.3 Program association Table */
#pragma pack(push, 1)
struct program_association_section
{
    uint8_t  table_id;

    uint16_t pseudo_member1;
    //uint16_t section_syntax_indicator : 1;
    //uint16_t zero : 1;
    //uint16_t reserved_1 : 2;
    //uint16_t section_length : 12;

    uint16_t transport_stream_id;

    uint8_t pseudo_member2;
    //uint8_t reserved_2 : 2;
    //uint8_t version_number : 5;
    //uint8_t current_next_indicator : 1;

    uint8_t section_number;
    uint8_t last_section_number;        
};

struct TsPatElementary
{
    uint16_t program_number;

    uint16_t pseudo_member1;
    //uint16_t reserved : 3;
    //uint16_t program_map_PID : 13;
};
#pragma pack(pop)

/**********************class PatHelper**********************/
class PatHelper : public PatHelperInterface
{
public:
    PatHelper(uchar_t *patHeader);
    ~PatHelper();

    SiPsiTableId GetTableId() const;

    SectionLength GetSectionLength() const;
    void SetSectionLength(SectionLength sectionLength);
    
	TsId GetTransportStreamId() const;
    void SetTransportStreamId(TsId transportStreamId);

	VersionNumber GetVersionNumber() const;
	void SetVersionNumber(VersionNumber versionNumber);

	CurrentNextIndicator GetCurrentNextIndicator() const;
	void SetCurrentNextIndicator(CurrentNextIndicator currentNextIndicator);

	SectionNumber GetSectionMumber() const;
	void SetSectionMumber(SectionNumber sectionMumber);

	SectionNumber GetLastSectionMumber() const;
	void SetLastSectionMumber(SectionNumber lastSectionMumber);

    uchar_t* GetHeader() const;
    size_t GetMinSectionLength() const;
    size_t GetMinSize() const;
    size_t GetCrcCodeSize() const;
    size_t GetSize() const;

	void UpdateCrcCode();

private:
    program_association_section *patHeader;
};

class PatElementary : public PatElementaryInterface
{
public:
    PatElementary(uchar_t *patElementary);
    ~PatElementary();

    ProgramNumber GetProgramNumber() const;
    void SetProgramNumber(ProgramNumber programNumber);

    Pid GetPmtPid() const;
    void SetPmtPid(Pid pmtPid);

    size_t GetSize() const;

private:
    TsPatElementary* patElementary;
};

/**********************class PmtHelper**********************/
#pragma pack(push, 1)
struct TS_program_map_section
{
    uint8_t  table_id;

    uint16_t pseudo_member1;
    //uint16_t section_syntax_indicator : 1;
    //uint16_t '0' : 1;
    //uint16_t reserved1 : 2;
    //uint16_t section_length : 12;

    uint16_t program_number;

    uint8_t  pseudo_member2;
    //uint8_t  reserved2 : 2;
    //uint8_t  version_number : 5;
    //uint8_t  current_next_indicator : 1;

    uint8_t  section_number;
    uint8_t  last_section_number;

    uint16_t pseudo_member3;
    //uint16_t reserved3 : 3;
    //uint16_t PCR_PID : 13;

    uint16_t pseudo_member4;
    //uint16_t reserved4 : 4;
    //uint16_t program_info_length : 12;
};

struct TsPmtElementary
{
    uint8_t stream_type;

    uint16_t pseudo_member1;
    //uint16_t   reserved1 : 3;
    //uint16_t   elementary_PID : 13;

    uint16_t pseudo_member2;
    //uint16_t reserved2 : 4;
    //uint16_t  ES_info_length : 12;
};
#pragma pack(pop)

class PmtHelper : public PmtHelperInterface
{
public:
    PmtHelper(uchar_t *pmtHeader);
    ~PmtHelper();
    
    SiPsiTableId GetTableId() const;

    SectionLength GetSectionLength() const;
    void SetSectionLength(SectionLength sectionLength);

    ProgramNumber GetProgramNumber() const;
    void SetProgramNumber(ProgramNumber programNumber);

    VersionNumber GetVersionNumber() const;
    void SetVersionNumber(VersionNumber versionNumber);

    CurrentNextIndicator GetCurrentNextIndicator() const;
    void SetCurrentNextIndicator(CurrentNextIndicator currentNextIndicator);

    SectionNumber GetSectionMumber() const;
    void SetSectionMumber(SectionNumber sectionMumber);

    SectionNumber GetLastSectionMumber() const;
    void SetLastSectionMumber(SectionNumber lastSectionMumber);

    Pid GetPcrPid() const;
    void SetPcrPid(Pid pcrPid);
    
    ProgramInfoLength GetProgramInfoLength() const;
    void SetProgramInfoLength(ProgramInfoLength programInfoLength); 
    uchar_t* GetProgramInfoes() const;
    
    uchar_t* GetHeader() const;
    size_t GetMinSectionLength() const;
    size_t GetMinSize() const;
    size_t GetCrcCodeSize() const;
    size_t GetSize() const;

    void UpdateCrcCode();
    
private:
    TS_program_map_section *pmtHeader;
};

class PmtElementary: public PmtElementaryInterface
{
public:
    PmtElementary(uchar_t *pmtElementary);
    ~PmtElementary();

    StreamType GetStreamType() const;
    void SetStreamType(StreamType streamType);

    Pid GetElementaryPid() const;
    void SetElementaryPid(Pid elementaryPid);

    EsInfoLength GetEsInfoLength() const;
    void SetEsInfoLength(EsInfoLength esInfoLength);

    size_t GetSize() const;

private:
    TsPmtElementary *pmtElementary;
};

/* DIT: Download Infor Table */
#pragma pack(push, 1)
struct ts_packet_poster_dit_data
{
    uint8_t table_id;         //0x90

    uint16_t pseudo_member1;
    //uint16_t section_syntax_indicator : 1;
    //uint16_t reserved_future_use : 1;
    //uint16_t reserved1 : 2;
    //uint16_t section_length : 12;

    uint16_t table_id_extend; //0x0

    uint8_t pseudo_member2;
    //uint8_t reserved2;              : 2
    //uint8_t section_version;        : 5
    //uint8_t current_next_indicator; : 1

    DitSectionNumber  section_number;
    DitSectionNumber  last_section_number;
};

struct ts_packet_poster_file_info
{
    uint16_t      file_id;
    uint16_t      event_loop_length;
    /*
    for(i=0;i<N1;i++){
        event_id;
    }
    */
};

#pragma pack(pop)

/**********************class DitHelper**********************/
class DitHelper : public DitHelperInterface
{
public:
    DitHelper(uchar_t *ditHeader);
    ~DitHelper();

    SiPsiTableId GetTableId() const;

    SectionLength GetSectionLength() const;
    void SetSectionLength(SectionLength sectionLength);

    VersionNumber GetVersionNumber() const;
    void SetVersionNumber(VersionNumber versionNumber);

    DitSectionNumber GetSectionMumber() const;
    void SetSectionMumber(DitSectionNumber sectionMumber);

    DitSectionNumber GetLastSectionMumber() const;
    void SetLastSectionMumber(DitSectionNumber lastSectionMumber);

    uchar_t* GetHeader() const;
    size_t GetMinSectionLength() const;
    size_t GetMinSize() const;
    size_t GetCrcCodeSize() const;
    size_t GetSize() const;
    
    void UpdateCrcCode();

private:
    ts_packet_poster_dit_data* ditHeader;
};

class DitElementary : public DitElementaryInterface
{
public:
	DitElementary(uchar_t *ditElementary);
	~DitElementary();

    uint16_t GetFileId() const;
    void SetFileId(uint16_t fileId);

	uint16_t GetEventLoopLength() const;
	void SetEventLoopLength(uint16_t eventLoopLength);
	
    uchar_t* GetHeader() const;
	size_t GetSize() const;

private:
    ts_packet_poster_file_info *ditElementary;
};

/* DDT: Download Data Table */
#pragma pack(push, 1)
struct ts_packet_poster_ddt_data
{
    uint8_t table_id;         //0x91

    uint16_t pseudo_member1;
    //uint16_t section_syntax_indicator : 1;
    //uint16_t reserved_future_use : 1;
    //uint16_t reserved1 : 2;
    //uint16_t section_length : 12;

    uint16_t file_id; 

    uint8_t pseudo_member2;
    //uint8_t reserved2;              : 2
    //uint8_t section_version;        : 5
    //uint8_t current_next_indicator; : 1

    DdtSectionNumber  section_number;
    DdtSectionNumber  last_section_number;
};
#pragma pack(pop)

/**********************class DdtHelperInterface**********************/
class DdtHelper : public DdtHelperInterface
{
public:
    DdtHelper(uchar_t *ddtHeader);
    ~DdtHelper(); 

    SiPsiTableId GetTableId() const;

    SectionLength GetSectionLength() const;
    void SetSectionLength(SectionLength sectionLength);

    PosterId GetPosterId() const;
    void SetPosterId(PosterId posterId);

    VersionNumber GetVersionNumber() const;
    void SetVersionNumber(VersionNumber versionNumber);

    DdtSectionNumber GetSectionMumber() const;
    void SetSectionMumber(DdtSectionNumber sectionMumber);

    DdtSectionNumber GetLastSectionMumber() const;
    void SetLastSectionMumber(DdtSectionNumber lastSectionMumber);

    uchar_t* GetHeader() const;
    size_t GetMinSectionLength() const;
    size_t GetMinSize() const;
    size_t GetCrcCodeSize() const;
    size_t GetSize() const;

    void UpdateCrcCode();

private:
    ts_packet_poster_ddt_data *ddtHeader;
};

#endif