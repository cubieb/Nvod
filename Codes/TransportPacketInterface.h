#ifndef _TransportPacketInterface_h_
#define _TransportPacketInterface_h_

/* Entity */
#include "BaseType.h"

/**********************class TransportPacketHelperInterface**********************/
//iso13818-1, 2.4.3.2 Transport Stream packet layer
class TransportPacketHelperInterface
{
public:
    TransportPacketHelperInterface() {};
    virtual ~TransportPacketHelperInterface() {};
    static TransportPacketHelperInterface* CreateInstance(uchar_t *tsPacketHeader);
	static TransportPacketHelperInterface* CreateInstance(uchar_t *tsPacketHeader, 
		PayloadUnitStartIndicator payloadUnitStartIndicator, Pid pid, 
		ContinuityCounter& continuityCounter);

    virtual SyncByte GetSyncByte() const = 0;
    virtual void SetSyncByte(SyncByte syncByte) = 0;

    virtual TransportErrorIndicator GetTransportErrorIndicator() const = 0;
    virtual void SetTransportErrorIndicator(TransportErrorIndicator transportErrorIndicator) = 0;

    virtual PayloadUnitStartIndicator GetPayloadUnitStartIndicator() const = 0;
    virtual void SetPayloadUnitStartIndicator(PayloadUnitStartIndicator payloadUnitStartIndicator) = 0;

    virtual TransportPriority GetTransportPriority() const = 0;
    virtual void SetTransportPriority(TransportPriority transportPriority) = 0;

    virtual Pid GetPid() const = 0;
    virtual void SetPid(Pid) = 0;

    virtual TransportScramblingControl GetTransportScramblingControl() const = 0;
    virtual void SetTransportScramblingControl(TransportScramblingControl transportScramblingControl) = 0;

    virtual AdaptationFieldControl GetAdaptationFieldControl() const = 0;
    virtual void SetAdaptationFieldControl(AdaptationFieldControl adaptationFieldControl) = 0;

    virtual ContinuityCounter GetContinuityCounter() const = 0;
    virtual void SetContinuityCounter(ContinuityCounter continuityCounter) = 0;

    virtual PcrFlag GetPcrFlag() const = 0;
    virtual void SetPcrFlag(PcrFlag pcrFlag) = 0;

    virtual Pcr GetPcr() const = 0;
    virtual void SetPcr(Pcr pcr) = 0;

    virtual AdaptationFieldLength GetAdaptationFieldLength() const = 0;
    virtual void SetAdaptationFieldLength(AdaptationFieldLength adaptationFieldLength) = 0;

    virtual uchar_t* GetPayLoadHeader() const = 0;
    virtual void FillPad(size_t payLoadSize) = 0;

    virtual uchar_t* GetHeader() const = 0;
    virtual size_t GetSize() const = 0;
};

/**********************class PatHelperInterface**********************/
class PatHelperInterface
{
public:
    PatHelperInterface() {};
    virtual ~PatHelperInterface() {};
	static PatHelperInterface* CreateInstance(uchar_t *patHeader);
	static PatHelperInterface* CreateInstance(uchar_t *patHeader, TsId tsId);

    virtual SiPsiTableId GetTableId() const = 0;

    virtual SectionLength GetSectionLength() const = 0;
    virtual void SetSectionLength(SectionLength sectionLength) = 0;

    virtual TsId GetTransportStreamId() const = 0;
    virtual void SetTransportStreamId(TsId tsId) = 0;

    virtual VersionNumber GetVersionNumber() const = 0;
    virtual void SetVersionNumber(VersionNumber versionNumber) = 0;

    virtual CurrentNextIndicator GetCurrentNextIndicator() const = 0;
    virtual void SetCurrentNextIndicator(CurrentNextIndicator currentNextIndicator) = 0;

	virtual SectionNumber GetSectionMumber() const = 0;
	virtual void SetSectionMumber(SectionNumber sectionMumber) = 0;

	virtual SectionNumber GetLastSectionMumber() const = 0;
	virtual void SetLastSectionMumber(SectionNumber lastSectionMumber) = 0;

    virtual uchar_t* GetHeader() const = 0;
    /* size which not included in section_length field */
    virtual size_t GetMinSectionLength() const = 0;
    virtual size_t GetMinSize() const = 0;
    virtual size_t GetCrcCodeSize() const = 0;
    virtual size_t GetSize() const = 0;

    virtual void UpdateCrcCode() = 0;
};

class PatElementaryInterface
{
public:
    PatElementaryInterface() {};
    virtual ~PatElementaryInterface() {};
    static PatElementaryInterface* CreateInstance(uchar_t *patElementary);

    virtual ProgramNumber GetProgramNumber() const = 0;
    virtual void SetProgramNumber(ProgramNumber programNumber) = 0;

    virtual Pid GetPmtPid() const = 0;
    virtual void SetPmtPid(Pid pmtPid) = 0;

    //static size_t GetMinSize()
    //{
    //    return 4;
    //}
    virtual size_t GetSize() const = 0;
};

/**********************class PmtHelperInterface**********************/
class PmtHelperInterface
{
public:
    PmtHelperInterface() {};
    virtual ~PmtHelperInterface() {};
	static PmtHelperInterface* CreateInstance(uchar_t *pmtHeader);
	static PmtHelperInterface* CreateInstance(uchar_t *pmtHeader, ProgramNumber programNumber);

    virtual SiPsiTableId GetTableId() const = 0;

    virtual SectionLength GetSectionLength() const = 0;
    virtual void SetSectionLength(SectionLength sectionLength) = 0;

    virtual ProgramNumber GetProgramNumber() const = 0;
    virtual void SetProgramNumber(ProgramNumber programNumber) = 0;

    virtual VersionNumber GetVersionNumber() const = 0;
    virtual void SetVersionNumber(VersionNumber versionNumber) = 0;

    virtual CurrentNextIndicator GetCurrentNextIndicator() const = 0;
    virtual void SetCurrentNextIndicator(CurrentNextIndicator currentNextIndicator) = 0;

    virtual SectionNumber GetSectionMumber() const = 0;
    virtual void SetSectionMumber(SectionNumber sectionMumber) = 0;

    virtual SectionNumber GetLastSectionMumber() const = 0;
    virtual void SetLastSectionMumber(SectionNumber lastSectionMumber) = 0;

    virtual Pid GetPcrPid() const = 0;
    virtual void SetPcrPid(Pid pcrPid) = 0;

    virtual ProgramInfoLength GetProgramInfoLength() const = 0;
    virtual void SetProgramInfoLength(ProgramInfoLength programInfoLength) = 0;
    virtual uchar_t* GetProgramInfoes() const = 0;

    virtual uchar_t* GetHeader() const = 0;
    /* size which not included in section_length field */
    virtual size_t GetMinSectionLength() const = 0;
    virtual size_t GetMinSize() const = 0;
    virtual size_t GetCrcCodeSize() const = 0;
    virtual size_t GetSize() const = 0;

    virtual void UpdateCrcCode() = 0;
};

class PmtElementaryInterface
{
public:
    PmtElementaryInterface() {};
    virtual ~PmtElementaryInterface() {};
    static PmtElementaryInterface* CreateInstance(uchar_t *pmtElementary);

    virtual StreamType GetStreamType() const = 0;
    virtual void SetStreamType(StreamType streamType) = 0;

    virtual Pid GetElementaryPid() const = 0;
    virtual void SetElementaryPid(Pid elementaryPid) = 0;

    virtual EsInfoLength GetEsInfoLength() const = 0;
    virtual void SetEsInfoLength(EsInfoLength esInfoLength) = 0;

    virtual size_t GetSize() const = 0;
};

/**********************class DitHelperInterface**********************/
class DitHelperInterface
{
public:
    DitHelperInterface() {}
    virtual ~DitHelperInterface() {}
    static DitHelperInterface* CreateInstance(uchar_t *ditHeader);
	static DitHelperInterface* CreateInstance(uchar_t *ditHeader, VersionNumber versionNumber);

    virtual SiPsiTableId GetTableId() const = 0;

    virtual SectionLength GetSectionLength() const = 0;
    virtual void SetSectionLength(SectionLength sectionLength) = 0;

    virtual VersionNumber GetVersionNumber() const = 0;
    virtual void SetVersionNumber(VersionNumber versionNumber) = 0;

    virtual DitSectionNumber GetSectionMumber() const = 0;
    virtual void SetSectionMumber(DitSectionNumber sectionMumber) = 0;

    virtual DitSectionNumber GetLastSectionMumber() const = 0;
    virtual void SetLastSectionMumber(DitSectionNumber lastSectionMumber) = 0;

    virtual uchar_t* GetHeader() const = 0;
    /* size which not included in section_length field */
    virtual size_t GetMinSectionLength() const = 0;
    virtual size_t GetMinSize() const = 0;
    virtual size_t GetCrcCodeSize() const = 0;
    virtual size_t GetSize() const = 0;

    virtual void UpdateCrcCode() = 0;
};

class DitElementaryInterface
{
public: 
	DitElementaryInterface() {}
	virtual ~DitElementaryInterface() {}
    static DitElementaryInterface* CreateInstance(uchar_t *ditElementary);

    virtual uint16_t GetFileId() const = 0;
    virtual void SetFileId(uint16_t fileId) = 0;

	virtual uint16_t GetEventLoopLength() const = 0;
	virtual void SetEventLoopLength(uint16_t eventLoopLength) = 0;

    virtual uchar_t* GetHeader() const = 0;
	virtual size_t GetSize() const = 0;
};

/**********************class DdtHelperInterface**********************/
class DdtHelperInterface
{
public:
    DdtHelperInterface() {}
    virtual ~DdtHelperInterface() {}
    static DdtHelperInterface* CreateInstance(uchar_t *ddtHeader);
	static DdtHelperInterface* CreateInstance(uchar_t *ddtHeader, PosterId posterId);

    virtual SiPsiTableId GetTableId() const = 0;

    virtual SectionLength GetSectionLength() const = 0;
    virtual void SetSectionLength(SectionLength sectionLength) = 0;
    
    virtual PosterId GetPosterId() const = 0;
    virtual void SetPosterId(PosterId posterId) = 0;

    virtual VersionNumber GetVersionNumber() const = 0;
    virtual void SetVersionNumber(VersionNumber versionNumber) = 0;

    virtual DdtSectionNumber GetSectionMumber() const = 0;
    virtual void SetSectionMumber(DdtSectionNumber sectionMumber) = 0;

    virtual DdtSectionNumber GetLastSectionMumber() const = 0;
    virtual void SetLastSectionMumber(DdtSectionNumber lastSectionMumber) = 0;

    virtual uchar_t* GetHeader() const = 0;
    virtual size_t GetMinSectionLength() const = 0;
    virtual size_t GetMinSize() const = 0;
    virtual size_t GetCrcCodeSize() const = 0;
    virtual size_t GetSize() const = 0;

    virtual void UpdateCrcCode() = 0;
};

#endif