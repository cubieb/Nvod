#ifndef _TmssPlayerCookie_h_
#define _TmssPlayerCookie_h_

/* Foundation */
#include "BaseType.h"
#include "SystemInclude.h"

using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

class TsEntity;
class TmssEntity;
class TmssEventEntity;
class MovieEntity;

class MovieRuntimeInfoEntity;

/**********************class TmssPlayerCookie**********************/
class TmssPlayerCookie
{
public:
    TmssPlayerCookie(shared_ptr<TmssEntity> tmss);
    ~TmssPlayerCookie();

    shared_ptr<TsEntity> GetTs() const;
    shared_ptr<TmssEntity> GetTmss() const;

    void ForwardTmssEvent();
    shared_ptr<TmssEventEntity> GetCurTmssEvent() const;

    void ForwardMovie();
    shared_ptr<MovieEntity> GetCurMovie() const;

    shared_ptr<MovieRuntimeInfoEntity> GetMovieRuntimeInfo() const;

	Duration GetEventDuration();
	void SetEventDuration(Duration eventDuration);

private:
    typedef list<shared_ptr<TmssEventEntity>>::iterator TmssEventIter;
    typedef list<shared_ptr<MovieEntity>>::iterator MovieIter;
    shared_ptr<TsEntity> ts;
    shared_ptr<TmssEntity> tmss;
    
    shared_ptr<TmssEventIter> curTmssEvent;
    shared_ptr<MovieIter> curMovie;
    shared_ptr<MovieRuntimeInfoEntity> movieRuntimeInfo;

	Duration eventDuration;
};

/**********************class MovieRuntimeInfoEntity**********************/
class MovieRuntimeInfoEntity
{
public:
    MovieRuntimeInfoEntity();
    ~MovieRuntimeInfoEntity();

    void Reset();

    /* Pmt Pid in Source .ts File */
    shared_ptr<Pid> GetPmtPid() const;
    void SetPmtPid(shared_ptr<Pid> pmtPid);
    void SetPmtPid(Pid pmtPid);

    /* Pcr Pid in Source .ts File */
    shared_ptr<Pid> GetPcrPid() const;
    void SetPcrPid(shared_ptr<Pid> pcrPid);
    void SetPcrPid(Pid pcrPid);

    /* Audio Pid in Source .ts File */
    shared_ptr<Pid> GetAudioPid() const;
    void SetAudioPid(shared_ptr<Pid> audioPid);
    void SetAudioPid(Pid audioPid);

    /* Video Pid in Source .ts File */
    shared_ptr<Pid> GetVideoPid() const;
    void SetVideoPid(shared_ptr<Pid> videoPid);
    void SetVideoPid(Pid videoPid);

    /* Pcr in Source .ts File */
    shared_ptr<Pcr> GetFirstPcr() const;
    void SetFirstPcr(shared_ptr<Pcr> firstPcr);
    void SetFirstPcr(Pcr firstPcr);

	/* Pcr in Source .ts File */
	shared_ptr<Pcr> GetLastPcr() const;
	void SetLastPcr(shared_ptr<Pcr> lastPcr);
	void SetLastPcr(Pcr lastPcr);

    shared_ptr<TimePoint> GetStartTimePoint() const;
    void SetStartTimePoint(shared_ptr<TimePoint> startTimePoint);
    void SetStartTimePoint(TimePoint startTimePoint);

    uchar_t* GetBufferHead() const;
    size_t GetBufferSize() const;
    uchar_t* GetPcrPacketPtr() const;
    void SetPcrPacketPtr(uchar_t* pcrPacketPtr);

private:
    size_t bufferSize;
    uchar_t *buffer;
    uchar_t *pcrPacketPtr;

    shared_ptr<Pid> pmtPid;
    shared_ptr<Pid> pcrPid;
    shared_ptr<Pid> audioPid;
    shared_ptr<Pid> videoPid;
    shared_ptr<Pcr> firstPcr;
	shared_ptr<Pcr> lastPcr;
    shared_ptr<TimePoint> startTimePoint;
};

#endif