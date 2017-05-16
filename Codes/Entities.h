#ifndef _Entities_h_
#define _Entities_h_

#include <string>  // std::string
#include <list>    // std::list
#include <memory>  // std::std::shared_ptr, std::weak_ptr
#include <chrono>  /* time_point, duration, seconds, ... */
#include <Winsock2.h>

#include "BaseType.h"

using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::string;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::system_clock;

class TsEntity;
class RefsEntity;
class RefsEventEntity;
class MovieEntity;
class PosterEntity;
class TmssEntity;
class TmssEventEntity;
class PstsEntity;
class GlobalCfgEntity; 

/**********************class TsEntity**********************/
class TsEntity : public std::enable_shared_from_this<TsEntity>
{
public:
    friend class MemberHelper;

    TsEntity(TableId id, shared_ptr<TsId> tsId, 
        shared_ptr<in_addr> srcAddr, shared_ptr<sockaddr_in> dstAddr);
    TsEntity();
    TsEntity(const TsEntity& right);
    TsEntity(TableId id, TsId tsId,
        const in_addr& srcAddr, const sockaddr_in& dstAddr);
    ~TsEntity();

    TableId GetId() const;
    void SetId(TableId id);

    TsId GetTsId() const;
    void SetTsId(TsId tsId);

    shared_ptr<in_addr> GetSrcAddr() const;
    void SetSrcAddr(shared_ptr<in_addr> srcAddr);
    void SetSrcAddr(const in_addr& srcAddr);

    const sockaddr_in& GetDstAddr() const;
    void SetDstAddr(const sockaddr_in& dstAddr);

    const list<shared_ptr<RefsEntity>>& GetRefses() const;
    list<shared_ptr<RefsEntity>>& GetRefses();
    void SetRefses(list<shared_ptr<RefsEntity>>& refses);
    void Bind(shared_ptr<RefsEntity> refse);
    void Unbind(shared_ptr<RefsEntity> refse);

    const list<shared_ptr<TmssEntity>>& GetTmsses() const;
    list<shared_ptr<TmssEntity>>& GetTmsses();
    void SetTmsses(list<shared_ptr<TmssEntity>>& tmsses);
    void Bind(shared_ptr<TmssEntity> tmss);
    void Unbind(shared_ptr<TmssEntity> tmss);

private:
    /* Bind, Unbind */
    void Bind(weak_ptr<RefsEntity> refs);
    void Unbind(weak_ptr<RefsEntity> refs);

    void Bind(weak_ptr<TmssEntity> tmss);
    void Unbind(weak_ptr<TmssEntity> tmss);

private:
    TableId id;                  /* not null */
    shared_ptr<TsId> tsId;       /* not null */
    shared_ptr<in_addr>     srcAddr; /* null */
    shared_ptr<sockaddr_in> dstAddr; /* not null */

    list<shared_ptr<RefsEntity>>  refses;
    list<shared_ptr<TmssEntity>>  tmsses;
};

/**********************class RefsEntity**********************/
class RefsEntity : public std::enable_shared_from_this<RefsEntity>
{
public:
    friend class MemberHelper;

    RefsEntity(TableId id, shared_ptr<ServiceId> serviceId, shared_ptr<string> description);
	RefsEntity();
    RefsEntity(const RefsEntity& right);
	RefsEntity(TableId id, ServiceId serviceId);
    virtual ~RefsEntity();

    TableId GetId() const;
    void SetId(TableId id);

    ServiceId GetServiceId() const;
    void SetServiceId(ServiceId serviceId);

    shared_ptr<string> GetDescription() const;
    void SetDescription(shared_ptr<string> description);
    void SetDescription(const string& description);
    
    shared_ptr<TsEntity> GetTs() const;
    void SetTs(shared_ptr<TsEntity> ts);

	shared_ptr<PstsEntity> GetPsts() const;
	void SetPsts(shared_ptr<PstsEntity> psts);

    const list<shared_ptr<RefsEventEntity>>& GetRefsEvents() const;
    list<shared_ptr<RefsEventEntity>>& GetRefsEvents();
    void SetRefsEvents(list<shared_ptr<RefsEventEntity>>& refsEvents);
    void Bind(shared_ptr<RefsEventEntity> refsEvent);
    void Unbind(shared_ptr<RefsEventEntity> refsEvent);

	/* the following function is provided just for debug */
    string ToString() const;

private:
    /* odb::access special functions */
    weak_ptr<TsEntity> GetTsPtr() const;
    void SetTsPtr(weak_ptr<TsEntity> ts);

    /* Bind, Unbind */
	void Bind(shared_ptr<PstsEntity> psts);
	void Unbind(shared_ptr<PstsEntity>);
	void Bind(weak_ptr<PstsEntity> psts);
	void Unbind(weak_ptr<PstsEntity>);

    void Bind(shared_ptr<TsEntity> ts);
    void Unbind(shared_ptr<TsEntity>);
    void Bind(weak_ptr<TsEntity> ts);
    void Unbind(weak_ptr<TsEntity>);

    void Bind(weak_ptr<RefsEventEntity> refsEvent);
    void Unbind(weak_ptr<RefsEventEntity> refsEvent);

private:
    TableId id;                       /* not null */
    shared_ptr<ServiceId> serviceId;  /* not null */
    shared_ptr<string> description;   /* null */

    weak_ptr<TsEntity> ts;
	shared_ptr<PstsEntity> psts;
    list<shared_ptr<RefsEventEntity>> refsEvents;
};

/**********************class RefsEventEntity**********************/
class RefsEventEntity : public std::enable_shared_from_this<RefsEventEntity>
{
public:
    friend class MemberHelper;
    typedef list<shared_ptr<MovieEntity>> Movies;

    RefsEventEntity(TableId id, shared_ptr<EventId> eventId,
        shared_ptr<system_clock::time_point> startTimePoint, shared_ptr<seconds> duration);
    RefsEventEntity();
    RefsEventEntity(const RefsEventEntity& right);
    RefsEventEntity(TableId id, EventId eventId,
        system_clock::time_point startTimePoint, seconds duration);
    virtual ~RefsEventEntity();

    TableId GetId() const;
    void SetId(TableId id);

    EventId GetEventId() const;
    void SetEventId(EventId eventId);

    system_clock::time_point GetStartTimePoint() const;
    void SetStartTimePoint(system_clock::time_point startTimePoint);

    seconds GetDuration() const;
    void SetDuration(seconds duration);

    shared_ptr<RefsEntity> GetRefs() const;
    void SetRefs(shared_ptr<RefsEntity> refs);

    const list<shared_ptr<MovieEntity>>& GetMovies() const;
    list<shared_ptr<MovieEntity>>& GetMovies();
    void SetMovies(const list<shared_ptr<MovieEntity>>& movies);
    void Bind(shared_ptr<MovieEntity> movie);
	void Unbind(shared_ptr<MovieEntity> movie);

	const list<shared_ptr<PosterEntity>>& GetPosters() const;
	list<shared_ptr<PosterEntity>>& GetPosters();
	void SetPosters(const list<shared_ptr<PosterEntity>>& posters);
	void Bind(shared_ptr<PosterEntity> poster);
	void Unbind(shared_ptr<PosterEntity> poster);

    const list<weak_ptr<TmssEventEntity>>& GetTmssEvents() const;
    list<weak_ptr<TmssEventEntity>>& GetTmssEvents();
    void SetTmssEvents(const list<weak_ptr<TmssEventEntity>>& tmssEvents);
    void Bind(shared_ptr<TmssEventEntity> tmssEvent);
    void Unbind(shared_ptr<TmssEventEntity> tmssEvent);

    /* the following function is provided just for debug */
    string ToString() const;

private:
    weak_ptr<RefsEntity> GetRefsPtr() const;
    void SetRefsPtr(weak_ptr<RefsEntity> refs);

    void Bind(shared_ptr<RefsEntity> refs);
    void Unbind(shared_ptr<RefsEntity>);
    void Bind(weak_ptr<RefsEntity> refs);
    void Unbind(weak_ptr<RefsEntity>);

    void Bind(weak_ptr<MovieEntity> movie);
	void Unbind(weak_ptr<MovieEntity> movie);

	void Bind(weak_ptr<PosterEntity> poster);
	void Unbind(weak_ptr<PosterEntity> poster);

    void Bind(weak_ptr<TmssEventEntity> tmssEvent);
    void Unbind(weak_ptr<TmssEventEntity> tmssEvent);

private:
    TableId id;                                   /* not null */
    shared_ptr<EventId> eventId;                  /* not null */
    shared_ptr<system_clock::time_point> startTimePoint;         /* not null */
    shared_ptr<seconds> duration;                 /* not null */

    weak_ptr<RefsEntity> refs;                   /* not null */
    list<shared_ptr<MovieEntity>> movies;         /* value not null */
    list<shared_ptr<PosterEntity>> posters;       /* value not null */
    list<weak_ptr<TmssEventEntity>> tmssEvents;   /* value null */
};

/**********************class MovieEntity**********************/
class MovieEntity : public std::enable_shared_from_this<MovieEntity>
{
public:
    friend class MemberHelper;

    MovieEntity(TableId id, shared_ptr<MovieId> movieId, shared_ptr<string> remotePath, shared_ptr<string> localPath);
    MovieEntity();
    MovieEntity(const MovieEntity& right);
    MovieEntity(TableId id, MovieId movieId, const char *remotePath);
	~MovieEntity();

    TableId GetId() const;
    void SetId(TableId id);

    MovieId GetMovieId() const;
    void SetMovieId(MovieId movieId);

    const string& GetRemotePath() const;
    void SetRemotePath(const string& remotePath);

    const string& GetLocalPath() const;
    void SetLocalPath(const string& localPath);

    const list<weak_ptr<RefsEventEntity>>& GetRefsEvents() const;
    list<weak_ptr<RefsEventEntity>>& GetRefsEvents();
    void SetRefsEvents(const list<weak_ptr<RefsEventEntity>>& refsEvents);
    void Bind(shared_ptr<RefsEventEntity> refsEvent);
    void Unbind(shared_ptr<RefsEventEntity> refsEvent);
    
	/* the following function is provided just for debug */
    string ToString() const;

private:
    void Bind(weak_ptr<RefsEventEntity> refsEvent);
    void Unbind(weak_ptr<RefsEventEntity> refsEvent);

private:
    TableId id;                                 /* not null */
    shared_ptr<MovieId> movieId;
    shared_ptr<string> remotePath;
    shared_ptr<string> localPath;  /* tansient, not null */

    list<weak_ptr<RefsEventEntity>> refsEvents;
};

/**********************class PosterEntity**********************/
class PosterEntity : public std::enable_shared_from_this<PosterEntity>
{
public:
	friend class MemberHelper;

	PosterEntity(TableId id, shared_ptr<PosterId> posterId, shared_ptr<string> remotePath, shared_ptr<string> localPath);
	PosterEntity();
	PosterEntity(const PosterEntity& right);
	PosterEntity(TableId id, PosterId posterId, const char *remotePath);
	~PosterEntity();

	TableId GetId() const;
	void SetId(TableId id);

	PosterId GetPosterId() const;
	void SetPosterId(PosterId posterId);

    const string& GetRemotePath() const;
	void SetRemotePath(const string& remotePath);

    const string& GetLocalPath() const;
	void SetLocalPath(const string& localPath);

    const list<weak_ptr<RefsEventEntity>>& GetRefsEvents() const;
    list<weak_ptr<RefsEventEntity>>& GetRefsEvents();
    void SetRefsEvents(const list<weak_ptr<RefsEventEntity>>& refsEvents);
    void Bind(shared_ptr<RefsEventEntity> refsEvent);
    void Unbind(shared_ptr<RefsEventEntity> refsEvent);

	/* the following function is provided just for debug */
	string ToString() const;

private:
    void Bind(weak_ptr<RefsEventEntity> refsEvent);
    void Unbind(weak_ptr<RefsEventEntity> refsEvent);

private:
	TableId id;                    /* not null */
	shared_ptr<PosterId> posterId;
	shared_ptr<string> remotePath;
	shared_ptr<string> localPath;  /* tansient, not null */

    list<weak_ptr<RefsEventEntity>> refsEvents;
};

/**********************class TmssEntity**********************/
class TmssEntity : public std::enable_shared_from_this<TmssEntity>
{
public:
    friend class MemberHelper;
    typedef list<shared_ptr<TmssEventEntity>> TmssEvents;

    TmssEntity(TableId id, shared_ptr<ServiceId> serviceId, 
		shared_ptr<Pid> pmtPid, shared_ptr<Pid> pcrPid, shared_ptr<Pid> audioPid, shared_ptr<Pid> videoPid,
		shared_ptr<string> description);
    TmssEntity();
    TmssEntity(const TmssEntity& right);
	TmssEntity(TableId id, ServiceId serviceId, Pid pmtPid, Pid pcrPid, Pid audioPid, Pid videoPid);
    virtual ~TmssEntity();

    TableId GetId() const;
    void SetId(TableId id);

    ServiceId GetServiceId() const;
    void SetServiceId(ServiceId serviceId);

    Pid GetPmtPid() const;
	void SetPmtPid(Pid pmtPid);

    Pid GetPcrPid() const;
	void SetPcrPid(Pid pcrPid);

    Pid GetAudioPid() const;
	void SetAudioPid(Pid audioPid);

    Pid GetVideoPid() const;
	void SetVideoPid(Pid videoPid);

	shared_ptr<string> GetDescription() const;
	void SetDescription(shared_ptr<string> description);
	void SetDescription(const string& description);

    shared_ptr<TsEntity> GetTs() const;
    void SetTs(shared_ptr<TsEntity> ts);

    const list<shared_ptr<TmssEventEntity>>& GetTmssEvents() const;
    list<shared_ptr<TmssEventEntity>>& GetTmssEvents();
    void SetTmssEvents(const list<shared_ptr<TmssEventEntity>>& tmssEvents);
    void Bind(shared_ptr<TmssEventEntity> tmssEvent);
    void Unbind(shared_ptr<TmssEventEntity> tmssEvent);

    /* the following function is provided just for debug */
    string ToString() const;

private:
    weak_ptr<TsEntity> GetTsPtr() const;
    void SetTsPtr(weak_ptr<TsEntity> ts);

    void Bind(shared_ptr<TsEntity> ts);
    void Unbind(shared_ptr<TsEntity>);
    void Bind(weak_ptr<TsEntity> ts);
    void Unbind(weak_ptr<TsEntity>);

    void Bind(weak_ptr<TmssEventEntity> tmssEvent);
    void Unbind(weak_ptr<TmssEventEntity> tmssEvent);

private:
    TableId id;                      /* not null */
    shared_ptr<ServiceId> serviceId; /* not null */
	shared_ptr<Pid> pmtPid;          /* not null */
	shared_ptr<Pid> pcrPid;          /* not null */
	shared_ptr<Pid> audioPid;        /* not null */
	shared_ptr<Pid> videoPid;        /* not null */
	shared_ptr<string> description;  /* null */

    weak_ptr<TsEntity> ts;
    list<shared_ptr<TmssEventEntity>> tmssEvents;
};

/**********************class TmssEventEntity**********************/
class TmssEventEntity : public std::enable_shared_from_this<TmssEventEntity>
{
public:
    friend class MemberHelper;

    TmssEventEntity(TableId id, shared_ptr<EventId> eventId,
        shared_ptr<system_clock::time_point> startTimePoint, shared_ptr<seconds> duration);
    TmssEventEntity();
    TmssEventEntity(const TmssEventEntity& right);
    TmssEventEntity(TableId id, EventId eventId,
        system_clock::time_point startTimePoint, seconds duration);
    virtual ~TmssEventEntity();

    TableId GetId() const;
    void SetId(TableId id);

    EventId GetEventId() const;
    void SetEventId(EventId eventId);

    system_clock::time_point GetStartTimePoint() const;
    void SetStartTimePoint(system_clock::time_point startTimePoint);

    seconds GetDuration() const;
    void SetGetDuration(seconds duration);

    shared_ptr<TmssEntity> GetTmss() const;
    void SetTmss(shared_ptr<TmssEntity> tmss);

    shared_ptr<RefsEventEntity> GetRefsEvent() const;
    void SetRefsEvent(shared_ptr<RefsEventEntity> refsEvent);
    void Bind(shared_ptr<RefsEventEntity> refsEvent);
    void Unbind(shared_ptr<RefsEventEntity>);

    /* the following function is provided just for debug */
    string ToString() const;

private:
    weak_ptr<TmssEntity> GetTmssPtr() const;
    void SetTmssPtr(weak_ptr<TmssEntity> tmss);

    void Bind(shared_ptr<TmssEntity> tmss);
    void Unbind(shared_ptr<TmssEntity>);
    void Bind(weak_ptr<TmssEntity> tmss);
    void Unbind(weak_ptr<TmssEntity>);

    void Bind(weak_ptr<RefsEventEntity> refsEvent);
    void Unbind(weak_ptr<RefsEventEntity>);

private:
    TableId id;                            /* not null */
    shared_ptr<EventId> eventId;           /* not null */
    shared_ptr<system_clock::time_point> startTimePoint;  /* not null */
    shared_ptr<seconds> duration;         /* not null */

    weak_ptr<TmssEntity> tmss;             /* not null */
    shared_ptr<RefsEventEntity> refsEvent; /* not null */
};

/**********************class PstsEntity**********************/
/* Poster Service Entity,  poster service and relative reference service must be in
the same TS.
*/
class PstsEntity : public std::enable_shared_from_this<PstsEntity>
{
public:
	friend class MemberHelper;

	PstsEntity(TableId id, shared_ptr<ServiceId> serviceId, 
		shared_ptr<Pid> pmtPid, shared_ptr<StreamType> streamType, shared_ptr<Pid> posterPid);
	PstsEntity();
	PstsEntity(const PstsEntity& right);
	PstsEntity(TableId id, ServiceId serviceId, 
		Pid pmtPid, StreamType streamType, 
		Pid posterPid);
	~PstsEntity();

	TableId GetId() const;
	void SetId(TableId id);

	ServiceId GetServiceId() const;
	void SetServiceId(ServiceId serviceId);

	Pid GetPmtPid() const;
	void SetPmtPid(Pid pmtPid);

	/* stream_type in TS_program_map_section */
	StreamType GetStreamType() const;
	void SetStreamType(StreamType streamType);

	/* elementary_PID in TS_program_map_section */
	Pid GetPosterPid() const;
	void SetPosterPid(Pid posterPid);

    weak_ptr<RefsEntity> GetRefs() const;
    void SetRefs(weak_ptr<RefsEntity> refs);

private:
	void Bind(shared_ptr<RefsEntity> refs);
	void Unbind(shared_ptr<RefsEntity>);
	void Bind(weak_ptr<RefsEntity> refs);
	void Unbind(weak_ptr<RefsEntity>);

private:
	TableId id;                      /* not null */
	shared_ptr<ServiceId> serviceId; /* not null */
	shared_ptr<Pid> pmtPid;           /* not null */
	shared_ptr<StreamType> streamType;  /* Program Map Table's stream_type, default 0x80 */
	shared_ptr<Pid> posterPid;          /* Program Map Table's elementary_PID */

	weak_ptr<RefsEntity> refs;
};

/**********************class GlobalCfgEntity**********************/
class GlobalCfgEntity : public std::enable_shared_from_this<GlobalCfgEntity>
{
public:
    friend class MemberHelper;

    GlobalCfgEntity(TableId id, shared_ptr<milliseconds> patInterval,
        shared_ptr<milliseconds> pmtInterval, shared_ptr<milliseconds> posterInterval);
    GlobalCfgEntity();
    GlobalCfgEntity(const GlobalCfgEntity& right);
    GlobalCfgEntity(TableId id, milliseconds patInterval,
        milliseconds pmtInterval, milliseconds posterInterval);
    ~GlobalCfgEntity();

    TableId GetId() const;
    void SetId(TableId id);

    milliseconds GetPatInterval() const;
    void SetPatInterval(milliseconds patInterval);

    milliseconds GetPmtInterval() const;
    void SetPmtInterval(milliseconds pmtInterval);

    milliseconds GetPosterInterval() const;
    void SetPosterInterval(milliseconds posterInterval);
    
private:
    TableId id;                  /* not null */
    shared_ptr<milliseconds> patInterval;
    shared_ptr<milliseconds> pmtInterval;
    shared_ptr<milliseconds> posterInterval;
};

#endif