#ifndef _Entities_h_
#define _Entities_h_

#include <string>  // std::string
#include <list>    // std::list
#include <memory>  // std::std::shared_ptr, std::weak_ptr
#include <Winsock2.h>

#include "BaseType.h"

using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

class TsEntity;
class RefsEntity;
class RefsEventEntity;
class MovieEntity;
class PosterEntity;
class TmssEntity;
class TmssEventEntity;
class GlobalCfgEntity; 
class DataPipeTsEntity;

class PosterViewEntity;
class RefsViewEntity;

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

    shared_ptr<TsId> GetTsId() const;
    void SetTsId(shared_ptr<TsId> tsId);
    void SetTsId(TsId tsId);

    shared_ptr<in_addr> GetSrcAddr() const;
    void SetSrcAddr(shared_ptr<in_addr> srcAddr);
    void SetSrcAddr(in_addr srcAddr);

    shared_ptr<sockaddr_in> GetDstAddr() const;
    void SetDstAddr(shared_ptr<sockaddr_in> dstAddr);
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

    RefsEntity(TableId id, shared_ptr<TsId> tsId, shared_ptr<ServiceId> serviceId,
		shared_ptr<Pid> pmtPid, shared_ptr<StreamType> streamType, 
		shared_ptr<Pid> posterPid, shared_ptr<string> description);
	RefsEntity();
    RefsEntity(const RefsEntity& right);
	RefsEntity(TableId id, TsId tsId, ServiceId serviceId, 
		Pid pmtPid, StreamType streamType, Pid posterPid);
    virtual ~RefsEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<TsId> GetTsId() const;
    void SetTsId(shared_ptr<TsId> tsId);
    void SetTsId(TsId tsId);

    shared_ptr<ServiceId> GetServiceId() const;
    void SetServiceId(shared_ptr<ServiceId> serviceId);
    void SetServiceId(ServiceId serviceId);
    
    shared_ptr<Pid> GetPmtPid() const;
    void SetPmtPid(shared_ptr<Pid> pmtPid);
    void SetPmtPid(Pid pmtPid);

    /* stream_type in TS_program_map_section */
    shared_ptr<StreamType> GetStreamType() const;
    void SetStreamType(shared_ptr<StreamType> streamType);
    void SetStreamType(StreamType streamType);

    /* elementary_PID in TS_program_map_section */
    shared_ptr<Pid> GetPosterPid() const;
    void SetPosterPid(shared_ptr<Pid> posterPid);
    void SetPosterPid(Pid posterPid);

    shared_ptr<string> GetDescription() const;
    void SetDescription(shared_ptr<string> description);
    void SetDescription(const string& description);
    
    weak_ptr<TsEntity> GetTs() const;
    void SetTs(weak_ptr<TsEntity> ts);

    const list<shared_ptr<RefsEventEntity>>& GetRefsEvents() const;
    list<shared_ptr<RefsEventEntity>>& GetRefsEvents();
    void SetRefsEvents(list<shared_ptr<RefsEventEntity>>& refsEvents);
    void Bind(shared_ptr<RefsEventEntity> refsEvent);
    void Unbind(shared_ptr<RefsEventEntity> refsEvent);

	/* the following function is provided just for debug */
    string ToString() const;

private:
    void Bind(shared_ptr<TsEntity> ts);
    void Unbind(shared_ptr<TsEntity>);
    void Bind(weak_ptr<TsEntity> ts);
    void Unbind(weak_ptr<TsEntity>);

    void Bind(weak_ptr<RefsEventEntity> refsEvent);
    void Unbind(weak_ptr<RefsEventEntity> refsEvent);

private:
    TableId id;                       /* not null */
    shared_ptr<TsId> tsId;            /* not null */
    shared_ptr<ServiceId> serviceId;  /* not null */
    shared_ptr<Pid> pmtPid;           /* not null */
    shared_ptr<StreamType> streamType;  /* Program Map Table's stream_type, default 0x80 */
    shared_ptr<Pid> posterPid;          /* Program Map Table's elementary_PID */
    shared_ptr<string> description;   /* null */

    weak_ptr<TsEntity> ts;
    list<shared_ptr<RefsEventEntity>> refsEvents;
};

/**********************class RefsEventEntity**********************/
class RefsEventEntity : public std::enable_shared_from_this<RefsEventEntity>
{
public:
    friend class MemberHelper;

    RefsEventEntity(TableId id, shared_ptr<EventId> eventId,
        shared_ptr<TimePoint> startTimePoint, shared_ptr<Duration> duration);
    RefsEventEntity();
    RefsEventEntity(const RefsEventEntity& right);
    RefsEventEntity(TableId id, EventId eventId,
        TimePoint startTimePoint, Duration duration);
    virtual ~RefsEventEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<EventId> GetEventId() const;
    void SetEventId(shared_ptr<EventId> eventId);
    void SetEventId(EventId eventId);

    shared_ptr<TimePoint> GetStartTimePoint() const;
    void SetStartTimePoint(shared_ptr<TimePoint> startTimePoint);
    void SetStartTimePoint(TimePoint startTimePoint);

    shared_ptr<Duration> GetDuration() const;
    void SetDuration(shared_ptr<Duration> duration);
    void SetDuration(Duration duration);

    weak_ptr<RefsEntity> GetRefs() const;
    void SetRefs(weak_ptr<RefsEntity> refs);

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
    shared_ptr<TimePoint> startTimePoint;         /* not null */
    shared_ptr<Duration> duration;                /* not null */

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

    shared_ptr<MovieId> GetMovieId() const;
    void SetMovieId(shared_ptr<MovieId> movieId);
    void SetMovieId(MovieId movieId);

    shared_ptr<string> GetRemotePath() const;
    void SetRemotePath(shared_ptr<string> remotePath);
    void SetRemotePath(const string& remotePath);

    shared_ptr<string> GetLocalPath() const;
    void SetLocalPath(shared_ptr<string> localPath);
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

	shared_ptr<PosterId> GetPosterId() const;
	void SetPosterId(shared_ptr<PosterId> posterId);
	void SetPosterId(PosterId posterId);

	shared_ptr<string> GetRemotePath() const;
	void SetRemotePath(shared_ptr<string> remotePath);
	void SetRemotePath(const string& remotePath);

	shared_ptr<string> GetLocalPath() const;
	void SetLocalPath(shared_ptr<string> localPath);
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

    TmssEntity(TableId id, shared_ptr<TsId> tsId, shared_ptr<ServiceId> serviceId, 
		shared_ptr<Pid> pmtPid, shared_ptr<Pid> pcrPid, shared_ptr<Pid> audioPid, shared_ptr<Pid> videoPid,
		shared_ptr<string> description);
    TmssEntity();
    TmssEntity(const TmssEntity& right);
	TmssEntity(TableId id, TsId tsId, ServiceId serviceId, Pid pmtPid, Pid pcrPid, Pid audioPid, Pid videoPid);
    virtual ~TmssEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<TsId> GetTsId() const;
    void SetTsId(shared_ptr<TsId> tsId);
    void SetTsId(TsId tsId);

    shared_ptr<ServiceId> GetServiceId() const;
    void SetServiceId(shared_ptr<ServiceId> serviceId);
    void SetServiceId(ServiceId serviceId);

	shared_ptr<Pid> GetPmtPid() const;
	void SetPmtPid(shared_ptr<Pid> pmtPid);
	void SetPmtPid(Pid pmtPid);

	shared_ptr<Pid> GetPcrPid() const;
	void SetPcrPid(shared_ptr<Pid> pcrPid);
	void SetPcrPid(Pid pcrPid);

	shared_ptr<Pid> GetAudioPid() const;
	void SetAudioPid(shared_ptr<Pid> audioPid);
	void SetAudioPid(Pid audioPid);

	shared_ptr<Pid> GetVideoPid() const;
	void SetVideoPid(shared_ptr<Pid> videoPid);
	void SetVideoPid(Pid videoPid);

	shared_ptr<string> GetDescription() const;
	void SetDescription(shared_ptr<string> description);
	void SetDescription(const string& description);

    weak_ptr<TsEntity> GetTs() const;
    void SetTs(weak_ptr<TsEntity> ts);

    const list<shared_ptr<TmssEventEntity>>& GetTmssEvents() const;
    list<shared_ptr<TmssEventEntity>>& GetTmssEvents();
    void SetTmssEvents(const list<shared_ptr<TmssEventEntity>>& tmssEvents);
    void Bind(shared_ptr<TmssEventEntity> tmssEvent);
    void Unbind(shared_ptr<TmssEventEntity> tmssEvent);

    /* the following function is provided just for debug */
    string ToString() const;

private:
    void Bind(shared_ptr<TsEntity> ts);
    void Unbind(shared_ptr<TsEntity>);
    void Bind(weak_ptr<TsEntity> ts);
    void Unbind(weak_ptr<TsEntity>);

    void Bind(weak_ptr<TmssEventEntity> tmssEvent);
    void Unbind(weak_ptr<TmssEventEntity> tmssEvent);

private:
    TableId id;                      /* not null */
    shared_ptr<TsId> tsId;           /* not null */
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
        shared_ptr<TimePoint> startTimePoint, shared_ptr<Duration> duration);
    TmssEventEntity();
    TmssEventEntity(const TmssEventEntity& right);
    TmssEventEntity(TableId id, EventId eventId,
        TimePoint startTimePoint, Duration duration);
    virtual ~TmssEventEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<EventId> GetEventId() const;
    void SetEventId(shared_ptr<EventId> eventId);
    void SetEventId(EventId eventId);

    shared_ptr<TimePoint> GetStartTimePoint() const;
    void SetStartTimePoint(shared_ptr<TimePoint> startTimePoint);
    void SetStartTimePoint(TimePoint startTimePoint);

    shared_ptr<Duration> GetDuration() const;
    void SetGetDuration(shared_ptr<Duration> duration);
    void SetGetDuration(Duration duration);

    weak_ptr<TmssEntity> GetTmss() const;
    void SetTmss(weak_ptr<TmssEntity> tmss);

    shared_ptr<RefsEventEntity> GetRefsEvent() const;
    void SetRefsEvent(shared_ptr<RefsEventEntity> refsEvent);
    void Bind(shared_ptr<RefsEventEntity> refsEvent);
    void Unbind(shared_ptr<RefsEventEntity>);

    /* the following function is provided just for debug */
    string ToString() const;

private:
    void Bind(shared_ptr<TmssEntity> tmss);
    void Unbind(shared_ptr<TmssEntity>);
    void Bind(weak_ptr<TmssEntity> tmss);
    void Unbind(weak_ptr<TmssEntity>);

    void Bind(weak_ptr<RefsEventEntity> refsEvent);
    void Unbind(weak_ptr<RefsEventEntity>);

private:
    TableId id;                            /* not null */
    shared_ptr<EventId> eventId;           /* not null */
    shared_ptr<TimePoint> startTimePoint;  /* not null */
    shared_ptr<Duration> duration;         /* not null */
    weak_ptr<TmssEntity> tmss;             /* not null */
    shared_ptr<RefsEventEntity> refsEvent; /* not null */
};

/**********************class GlobalCfgEntity**********************/
class GlobalCfgEntity : public std::enable_shared_from_this<GlobalCfgEntity>
{
public:
    friend class MemberHelper;

    GlobalCfgEntity(TableId id, shared_ptr<Duration> patInterval,
        shared_ptr<Duration> pmtInterval, shared_ptr<Duration> posterInterval);
    GlobalCfgEntity();
    GlobalCfgEntity(const GlobalCfgEntity& right);
    GlobalCfgEntity(TableId id, Duration patInterval,
        Duration pmtInterval, Duration posterInterval);
    ~GlobalCfgEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<Duration> GetPatInterval() const;
    void SetPatInterval(shared_ptr<Duration> patInterval);
    void SetPatInterval(Duration patInterval);

    shared_ptr<Duration> GetPmtInterval() const;
    void SetPmtInterval(shared_ptr<Duration> pmtInterval);
    void SetPmtInterval(Duration pmtInterval);

    shared_ptr<Duration> GetPosterInterval() const;
    void SetPosterInterval(shared_ptr<Duration> posterInterval);
    void SetPosterInterval(Duration posterInterval);

private:
    void Bind(weak_ptr<DataPipeTsEntity> dataPipeTs);
    void Unbind(weak_ptr<DataPipeTsEntity> dataPipeTs);

private:
    TableId id;                  /* not null */
    shared_ptr<Duration> patInterval;
    shared_ptr<Duration> pmtInterval;
    shared_ptr<Duration> posterInterval;

    //list<weak_ptr<DataPipeTsEntity>> dataPipeTses;
};

/**********************class RefsViewEntity**********************/
class PosterViewEntity
{
public:
    PosterViewEntity(TableId id, shared_ptr<PosterId> posterId, 
        shared_ptr<string> remotePath, shared_ptr<string> localPath);
    PosterViewEntity();
    PosterViewEntity(const PosterViewEntity& right);
    PosterViewEntity(TableId id, PosterId posterId,
        const char *remotePath, const char *localPath);
    ~PosterViewEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<PosterId> GetPosterId() const;
    void SetPosterId(shared_ptr<PosterId> posterId);
    void SetPosterId(PosterId posterId);

    shared_ptr<string> GetRemotePath() const;
    void SetRemotePath(shared_ptr<string> remotePath);
    void SetRemotePath(const string& remotePath);

    shared_ptr<string> GetLocalPath() const;
    void SetLocalPath(shared_ptr<string> localPath);
    void SetLocalPath(const string& localPath);

	const list<shared_ptr<RefsEntity>>& GetRefses() const;
    list<shared_ptr<RefsEntity>>& GetRefses();
    void SetRefses(const list<shared_ptr<RefsEntity>>& refses);
    void Bind(shared_ptr<RefsEntity> refs);
    void Unbind(shared_ptr<RefsEntity> refs);

private:
    TableId id;                    /* not null */
    shared_ptr<PosterId> posterId;
    shared_ptr<string> remotePath;
    shared_ptr<string> localPath;  /* tansient, not null */

    list<shared_ptr<RefsEntity>> refses;
};

#endif