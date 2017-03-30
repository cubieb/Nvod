#include "Entities.h"

/* Foundation */
#include "SystemInclude.h"

/* Entity */
#include "MemberHelper.h"

using namespace std;
using namespace placeholders;

/**********************class TsEntity**********************/
TsEntity::TsEntity(TableId id, shared_ptr<TsId> tsId,
    shared_ptr<in_addr> srcAddr, shared_ptr<sockaddr_in> dstAddr)
    : id(id), tsId(tsId), srcAddr(srcAddr), dstAddr(dstAddr)
{}

TsEntity::TsEntity()
    : TsEntity(InvalidTableId, nullptr, nullptr, nullptr)
{}

TsEntity::TsEntity(const TsEntity& right)
    : TsEntity(right.id, make_shared<TsId>(*right.tsId),
    make_shared<in_addr>(*right.srcAddr), make_shared<sockaddr_in>(*right.dstAddr))
{}

TsEntity::TsEntity(TableId id, TsId tsId,
    const in_addr& srcAddr, const sockaddr_in& dstAddr)
    : TsEntity(id, make_shared<TsId>(tsId),
    make_shared<in_addr>(srcAddr), make_shared<sockaddr_in>(dstAddr))
{}

TsEntity::~TsEntity()
{}

TableId TsEntity::GetId() const
{
    return id;
}

void TsEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<TsId> TsEntity::GetTsId() const
{
    return tsId;
}

void TsEntity::SetTsId(shared_ptr<TsId> tsId)
{
    this->tsId = tsId;
}

void TsEntity::SetTsId(TsId tsId)
{
    MemberHelper::SetMember(this->tsId, tsId, nullptr);
}

shared_ptr<in_addr> TsEntity::GetSrcAddr() const
{
    return srcAddr;
}

void TsEntity::SetSrcAddr(shared_ptr<in_addr> srcAddr)
{
    this->srcAddr = srcAddr;
}

void TsEntity::SetSrcAddr(in_addr srcAddr)
{
    MemberHelper::SetMember(this->srcAddr, srcAddr, nullptr);
}

shared_ptr<sockaddr_in> TsEntity::GetDstAddr() const
{
    return dstAddr;
}

void TsEntity::SetDstAddr(shared_ptr<sockaddr_in> dstAddr)
{
    this->dstAddr = dstAddr;
}

void TsEntity::SetDstAddr(const sockaddr_in& dstAddr)
{
    MemberHelper::SetMember(this->dstAddr, dstAddr, nullptr);
}

const list<shared_ptr<RefsEntity>>& TsEntity::GetRefses() const
{
    return refses;
}

list<shared_ptr<RefsEntity>>& TsEntity::GetRefses()
{
    return refses;
}

void TsEntity::SetRefses(list<shared_ptr<RefsEntity>>& refses)
{
    MemberHelper::SetMember(this->refses, refses, shared_from_this());
}

void TsEntity::Bind(shared_ptr<RefsEntity> refs)
{
    MemberHelper::BindMember(this->refses, refs, shared_from_this());
}

void TsEntity::Unbind(shared_ptr<RefsEntity> refs)
{
    MemberHelper::UnbindMember(this->refses, refs, shared_from_this());
}

const list<shared_ptr<TmssEntity>>& TsEntity::GetTmsses() const
{
    return tmsses;
}

list<shared_ptr<TmssEntity>>& TsEntity::GetTmsses()
{
    return tmsses;
}

void TsEntity::SetTmsses(list<shared_ptr<TmssEntity>>& tmsses)
{
    MemberHelper::SetMember(this->tmsses, tmsses, shared_from_this());
}

void TsEntity::Bind(shared_ptr<TmssEntity> tmss)
{
    MemberHelper::BindMember(this->tmsses, tmss, shared_from_this());
}

void TsEntity::Unbind(shared_ptr<TmssEntity> tmss)
{
    MemberHelper::UnbindMember(this->tmsses, tmss, shared_from_this());
}

/* private functions */
void TsEntity::Bind(weak_ptr<RefsEntity> refs)
{
    MemberHelper::BindMember(this->refses, refs, shared_from_this());
}

void TsEntity::Unbind(weak_ptr<RefsEntity> refs)
{
    MemberHelper::UnbindMember(this->refses, refs, shared_from_this());
}

void TsEntity::Bind(weak_ptr<TmssEntity> tmss)
{
    MemberHelper::BindMember(this->tmsses, tmss, shared_from_this());
}

void TsEntity::Unbind(weak_ptr<TmssEntity> tmss)
{
    MemberHelper::UnbindMember(this->tmsses, tmss, shared_from_this());
}

/**********************class RefsEntity**********************/
RefsEntity::RefsEntity(TableId id, shared_ptr<ServiceId> serviceId, shared_ptr<string> description)
    : id(id), serviceId(serviceId), description(description)
{}

RefsEntity::RefsEntity()
: RefsEntity(InvalidTableId, nullptr, nullptr)
{}

RefsEntity::RefsEntity(const RefsEntity& right)
    : RefsEntity(right.id, make_shared<ServiceId>(*right.serviceId), nullptr)
{
    if (right.description != nullptr)
    {
        description = make_shared<string>(*right.description);
    }
}

RefsEntity::RefsEntity(TableId id, ServiceId serviceId)
	: RefsEntity(id, make_shared<ServiceId>(serviceId), nullptr)
{}

RefsEntity::~RefsEntity()
{}

TableId RefsEntity::GetId() const
{
    return id;
}

void RefsEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<ServiceId> RefsEntity::GetServiceId() const
{
    return serviceId;
}

void RefsEntity::SetServiceId(shared_ptr<ServiceId> serviceId)
{
    this->serviceId = serviceId;
}

void RefsEntity::SetServiceId(ServiceId serviceId)
{
    MemberHelper::SetMember(this->serviceId, serviceId, nullptr);
}

shared_ptr<string> RefsEntity::GetDescription() const
{
    return description;
}

void RefsEntity::SetDescription(shared_ptr<string> description)
{
    this->description = description;
}

void RefsEntity::SetDescription(const string& description)
{
    MemberHelper::SetMember(this->description, description, nullptr);
}

weak_ptr<TsEntity> RefsEntity::GetTs() const
{
	return ts;
}

void RefsEntity::SetTs(weak_ptr<TsEntity> ts)
{
	MemberHelper::SetMember(this->ts, ts, shared_from_this());
}

shared_ptr<PstsEntity> RefsEntity::GetPsts() const
{
	return psts;
}

void RefsEntity::SetPsts(shared_ptr<PstsEntity> psts)
{
	MemberHelper::SetMember(this->psts, psts, shared_from_this());
}

const list<shared_ptr<RefsEventEntity>>& RefsEntity::GetRefsEvents() const
{
    return refsEvents;
}

list<shared_ptr<RefsEventEntity>>& RefsEntity::GetRefsEvents()
{
    return refsEvents;
}

void RefsEntity::SetRefsEvents(list<shared_ptr<RefsEventEntity>>& refsEvents)
{
    MemberHelper::SetMember(this->refsEvents, refsEvents, shared_from_this());
}

void RefsEntity::Bind(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(refsEvents, refsEvent, shared_from_this());
}

void RefsEntity::Unbind(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::UnbindMember(refsEvents, refsEvent, shared_from_this());
}


string RefsEntity::ToString() const
{
    return "";
}

/* private functions */
void RefsEntity::Bind(shared_ptr<PstsEntity> psts)
{
	MemberHelper::BindMember(this->psts, psts, shared_from_this());
}

void RefsEntity::Unbind(shared_ptr<PstsEntity>)
{
	MemberHelper::UnbindMember(this->psts, NullSharedPtr(PstsEntity), shared_from_this());
}

void RefsEntity::Bind(weak_ptr<PstsEntity> psts)
{
	MemberHelper::BindMember(this->psts, psts, shared_from_this());
}

void RefsEntity::Unbind(weak_ptr<PstsEntity>)
{
	MemberHelper::UnbindMember(this->psts, NullWeakPtr(PstsEntity), shared_from_this());
}

void RefsEntity::Bind(shared_ptr<TsEntity> ts)
{
	MemberHelper::BindMember(this->psts, psts, shared_from_this());
}

void RefsEntity::Unbind(shared_ptr<TsEntity>)
{
	MemberHelper::UnbindMember(this->ts, NullSharedPtr(TsEntity), shared_from_this());
}

void RefsEntity::Bind(weak_ptr<TsEntity> ts)
{
	MemberHelper::BindMember(this->ts, ts, shared_from_this());
}

void RefsEntity::Unbind(weak_ptr<TsEntity>)
{
	MemberHelper::UnbindMember(this->ts, NullWeakPtr(TsEntity), shared_from_this());
}

void RefsEntity::Bind(weak_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(refsEvents, refsEvent, shared_from_this());
}

void RefsEntity::Unbind(weak_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(refsEvents, refsEvent, shared_from_this());
}

/**********************class RefsEventEntity**********************/
RefsEventEntity::RefsEventEntity(TableId id, shared_ptr<EventId> eventId,
    shared_ptr<TimePoint> startTimePoint, shared_ptr<Duration> duration)
    : id(id), eventId(eventId), startTimePoint(startTimePoint), duration(duration)
{}

RefsEventEntity::RefsEventEntity()
    : RefsEventEntity(InvalidTableId, nullptr, nullptr, nullptr)
{}

RefsEventEntity::RefsEventEntity(const RefsEventEntity& right)
    : RefsEventEntity(right.id, make_shared<EventId>(*right.eventId),
    make_shared<TimePoint>(*right.startTimePoint), make_shared<Duration>(*right.duration))
{}

RefsEventEntity::RefsEventEntity(TableId id, EventId eventId,
    TimePoint startTimePoint, Duration duration)
    : RefsEventEntity(id, make_shared<EventId>(eventId),
    make_shared<TimePoint>(startTimePoint), make_shared<Duration>(duration))
{}

RefsEventEntity::~RefsEventEntity()
{}

TableId RefsEventEntity::GetId() const
{
    return id;
}

void RefsEventEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<EventId> RefsEventEntity::GetEventId() const
{
    return eventId;
}

void RefsEventEntity::SetEventId(shared_ptr<EventId> eventId)
{
    this->eventId = eventId;
}

void RefsEventEntity::SetEventId(EventId eventId)
{
    MemberHelper::SetMember(this->eventId, eventId, nullptr);
}

shared_ptr<TimePoint> RefsEventEntity::GetStartTimePoint() const
{
    return startTimePoint;
}

void RefsEventEntity::SetStartTimePoint(shared_ptr<TimePoint> startTimePoint)
{
    this->startTimePoint = startTimePoint;
}

void RefsEventEntity::SetStartTimePoint(TimePoint startTimePoint)
{
    MemberHelper::SetMember(this->startTimePoint, startTimePoint, nullptr);
}

shared_ptr<Duration> RefsEventEntity::GetDuration() const
{
    return duration;
}

void RefsEventEntity::SetDuration(shared_ptr<Duration> duration)
{
    this->duration = duration;
}

void RefsEventEntity::SetDuration(Duration duration)
{
    MemberHelper::SetMember(this->duration, duration, nullptr);
}

weak_ptr<RefsEntity> RefsEventEntity::GetRefs() const
{
    return refs;
}

void RefsEventEntity::SetRefs(weak_ptr<RefsEntity> refs)
{
    MemberHelper::SetMember(this->refs, refs, shared_from_this());
}

const list<shared_ptr<MovieEntity>>& RefsEventEntity::GetMovies() const
{
    return movies;
}

list<shared_ptr<MovieEntity>>& RefsEventEntity::GetMovies()
{
    return movies;
}

void RefsEventEntity::SetMovies(const list<shared_ptr<MovieEntity>>& movies)
{
    MemberHelper::SetMember(this->movies, movies, shared_from_this());
}

void RefsEventEntity::Bind(shared_ptr<MovieEntity> movie)
{
    MemberHelper::BindMember(movies, movie, shared_from_this());
}

void RefsEventEntity::Unbind(shared_ptr<MovieEntity> movie)
{
    MemberHelper::UnbindMember(movies, movie, shared_from_this());
}

const list<shared_ptr<PosterEntity>>& RefsEventEntity::GetPosters() const
{
	return posters;
}

list<shared_ptr<PosterEntity>>& RefsEventEntity::GetPosters()
{
	return posters;
}

void RefsEventEntity::SetPosters(const list<shared_ptr<PosterEntity>>& posters)
{
	MemberHelper::SetMember(this->posters, posters, shared_from_this());
}

void RefsEventEntity::Bind(shared_ptr<PosterEntity> poster)
{
	MemberHelper::BindMember(this->posters, poster, shared_from_this());
}

void RefsEventEntity::Unbind(shared_ptr<PosterEntity> poster)
{
	MemberHelper::UnbindMember(this->posters, poster, shared_from_this());
}

const list<weak_ptr<TmssEventEntity>>& RefsEventEntity::GetTmssEvents() const
{
    return tmssEvents;
}

list<weak_ptr<TmssEventEntity>>& RefsEventEntity::GetTmssEvents()
{
    return tmssEvents;
}

void RefsEventEntity::SetTmssEvents(const list<weak_ptr<TmssEventEntity>>& tmssEvents)
{
    MemberHelper::SetMember(this->tmssEvents, tmssEvents, shared_from_this());
}

void RefsEventEntity::Bind(shared_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::BindMember(tmssEvents, tmssEvent, shared_from_this());
}

void RefsEventEntity::Unbind(shared_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::UnbindMember(tmssEvents, tmssEvent, shared_from_this());
}

string RefsEventEntity::ToString() const
{
    return "";
}

/* private functions */
void RefsEventEntity::Bind(shared_ptr<RefsEntity> refs)
{
    MemberHelper::BindMember(this->refs, refs, shared_from_this());
}

void RefsEventEntity::Unbind(shared_ptr<RefsEntity>)
{
    MemberHelper::UnbindMember(refs, NullSharedPtr(RefsEntity), shared_from_this());
}

void RefsEventEntity::Bind(weak_ptr<RefsEntity> refs)
{
    MemberHelper::BindMember(this->refs, refs, shared_from_this());
}

void RefsEventEntity::Unbind(weak_ptr<RefsEntity>)
{
    MemberHelper::UnbindMember(refs, NullWeakPtr(RefsEntity), shared_from_this());
}

void RefsEventEntity::Bind(weak_ptr<MovieEntity> movie)
{
    MemberHelper::BindMember(movies, movie, shared_from_this());
}

void RefsEventEntity::Unbind(weak_ptr<MovieEntity> movie)
{
    MemberHelper::UnbindMember(movies, movie, shared_from_this());
}

void RefsEventEntity::Bind(weak_ptr<PosterEntity> poster)
{
	MemberHelper::BindMember(this->posters, poster, shared_from_this());
}

void RefsEventEntity::Unbind(weak_ptr<PosterEntity> poster)
{
	MemberHelper::UnbindMember(this->posters, poster, shared_from_this());
}

void RefsEventEntity::Bind(weak_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::BindMember(tmssEvents, tmssEvent, shared_from_this());
}

void RefsEventEntity::Unbind(weak_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::UnbindMember(tmssEvents, tmssEvent, shared_from_this());
}

/**********************class MovieEntity**********************/
/* public functions */
MovieEntity::MovieEntity(TableId id, shared_ptr<MovieId> movieId, 
    shared_ptr<string> remotePath, shared_ptr<string> localPath)
    : id(id), movieId(movieId), remotePath(remotePath), localPath(localPath)
{}

MovieEntity::MovieEntity()
    : MovieEntity(InvalidMovieId, nullptr, nullptr, nullptr)
{}

MovieEntity::MovieEntity(const MovieEntity& right)
    : MovieEntity(right.id, make_shared<MovieId>(*right.movieId),
    make_shared<string>(*right.remotePath), nullptr)
{
    if (right.localPath != nullptr)
    {
        this->localPath = make_shared<string>(*right.localPath);
    }
}

MovieEntity::MovieEntity(TableId id, MovieId movieId, const char *remotePath)
    : MovieEntity(id, make_shared<MovieId>(movieId), make_shared<string>(remotePath), nullptr)
{}

MovieEntity::~MovieEntity()
{}

TableId MovieEntity::GetId() const
{
    return id;
}

void MovieEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<MovieId> MovieEntity::GetMovieId() const
{
    return movieId;
}

void MovieEntity::SetMovieId(shared_ptr<MovieId> movieId)
{
    this->movieId = movieId;
}

void MovieEntity::SetMovieId(MovieId movieId)
{
    MemberHelper::SetMember(this->movieId, movieId, nullptr);
}

shared_ptr<string> MovieEntity::GetRemotePath() const
{
    return remotePath;
}

void MovieEntity::SetRemotePath(shared_ptr<string> remotePath)
{
    this->remotePath = remotePath;
}

void MovieEntity::SetRemotePath(const string& remotePath)
{
    MemberHelper::SetMember(this->remotePath, remotePath, nullptr);
}

shared_ptr<string> MovieEntity::GetLocalPath() const
{
    return localPath;
}

void MovieEntity::SetLocalPath(shared_ptr<string> localPath)
{
    this->localPath = localPath;
}

void MovieEntity::SetLocalPath(const string& localPath)
{
    MemberHelper::SetMember(this->localPath, localPath, nullptr);
}

const list<weak_ptr<RefsEventEntity>>& MovieEntity::GetRefsEvents() const
{
    return refsEvents;
}

list<weak_ptr<RefsEventEntity>>& MovieEntity::GetRefsEvents()
{
    return refsEvents;
}

void MovieEntity::SetRefsEvents(const list<weak_ptr<RefsEventEntity>>& refsEvents)
{
    MemberHelper::SetMember(this->refsEvents, refsEvents, shared_from_this());
}

void MovieEntity::Bind(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(refsEvents, refsEvent, shared_from_this());
}

void MovieEntity::Unbind(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::UnbindMember(refsEvents, refsEvent, shared_from_this());
}

string MovieEntity::ToString() const
{
    return "";
}

/* private functions */
void MovieEntity::Bind(weak_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(refsEvents, refsEvent, shared_from_this());
}

void MovieEntity::Unbind(weak_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::UnbindMember(refsEvents, refsEvent, shared_from_this());
}

/**********************class PosterEntity**********************/
/* public functions */
PosterEntity::PosterEntity(TableId id, shared_ptr<PosterId> posterId,
	shared_ptr<string> remotePath, shared_ptr<string> localPath)
	: id(id), posterId(posterId), remotePath(remotePath), localPath(localPath)
{}

PosterEntity::PosterEntity()
	: PosterEntity(InvalidMovieId, nullptr, nullptr, nullptr)
{}

PosterEntity::PosterEntity(const PosterEntity& right)
	: PosterEntity(right.id, make_shared<PosterId>(*right.posterId),
	make_shared<string>(*right.remotePath), nullptr)
{
	if (right.localPath != nullptr)
	{
		this->localPath = make_shared<string>(*right.localPath);
	}
}

PosterEntity::PosterEntity(TableId id, PosterId movieId, const char *remotePath)
	: PosterEntity(id, make_shared<PosterId>(posterId), make_shared<string>(remotePath), nullptr)
{}

PosterEntity::~PosterEntity()
{}

TableId PosterEntity::GetId() const
{
	return id;
}

void PosterEntity::SetId(TableId id)
{
	this->id = id;
}

shared_ptr<PosterId> PosterEntity::GetPosterId() const
{
	return posterId;
}

void PosterEntity::SetPosterId(shared_ptr<PosterId> posterId)
{
	this->posterId = posterId;
}

void PosterEntity::SetPosterId(PosterId posterId)
{
	MemberHelper::SetMember(this->posterId, posterId, nullptr);
}

shared_ptr<string> PosterEntity::GetRemotePath() const
{
	return remotePath;
}

void PosterEntity::SetRemotePath(shared_ptr<string> remotePath)
{
	this->remotePath = remotePath;
}

void PosterEntity::SetRemotePath(const string& remotePath)
{
	MemberHelper::SetMember(this->remotePath, remotePath, nullptr);
}

shared_ptr<string> PosterEntity::GetLocalPath() const
{
	return localPath;
}

void PosterEntity::SetLocalPath(shared_ptr<string> localPath)
{
	this->localPath = localPath;
}

void PosterEntity::SetLocalPath(const string& localPath)
{
	MemberHelper::SetMember(this->localPath, localPath, nullptr);
}

const list<weak_ptr<RefsEventEntity>>& PosterEntity::GetRefsEvents() const
{
    return refsEvents;
}

list<weak_ptr<RefsEventEntity>>& PosterEntity::GetRefsEvents()
{
    return refsEvents;
}

void PosterEntity::SetRefsEvents(const list<weak_ptr<RefsEventEntity>>& refsEvents)
{
    MemberHelper::SetMember(this->refsEvents, refsEvents, shared_from_this());
}

void PosterEntity::Bind(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(refsEvents, refsEvent, shared_from_this());
}

void PosterEntity::Unbind(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::UnbindMember(refsEvents, refsEvent, shared_from_this());
}

string PosterEntity::ToString() const
{
	return "";
}

/* private functions */
void PosterEntity::Bind(weak_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(refsEvents, refsEvent, shared_from_this());
}

void PosterEntity::Unbind(weak_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::UnbindMember(refsEvents, refsEvent, shared_from_this());
}

/**********************class TmssEntity**********************/
/* public functions */
TmssEntity::TmssEntity(TableId id, shared_ptr<ServiceId> serviceId,
	shared_ptr<Pid> pmtPid, shared_ptr<Pid> pcrPid, shared_ptr<Pid> audioPid, shared_ptr<Pid> videoPid,
	shared_ptr<string> description)
    : id(id), serviceId(serviceId), 
	pmtPid(pmtPid), pcrPid(pcrPid), audioPid(audioPid), videoPid(videoPid),
	description(description)
{}

TmssEntity::TmssEntity()
	: TmssEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{}

TmssEntity::TmssEntity(const TmssEntity& right)
	: TmssEntity(right.id, make_shared<ServiceId>(*right.serviceId),
	make_shared<Pid>(*right.pmtPid), make_shared<Pid>(*right.pcrPid), 
	make_shared<Pid>(*right.audioPid), make_shared<Pid>(*right.videoPid),
    nullptr)
{
    if (right.description != nullptr)
    {
        description = make_shared<string>(*right.description);
    }
}

TmssEntity::TmssEntity(TableId id, ServiceId serviceId, Pid pmtPid, Pid pcrPid, Pid audioPid, Pid videoPid)
: TmssEntity(id, make_shared<ServiceId>(serviceId), 
make_shared<Pid>(pmtPid), make_shared<Pid>(pcrPid), make_shared<Pid>(audioPid), make_shared<Pid>(videoPid), 
nullptr)
{}

TmssEntity::~TmssEntity()
{}

TableId TmssEntity::GetId() const
{
    return id;
}

void TmssEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<ServiceId> TmssEntity::GetServiceId() const
{
    return serviceId;
}

void TmssEntity::SetServiceId(shared_ptr<ServiceId> serviceId)
{
    this->serviceId = serviceId;
}

void TmssEntity::SetServiceId(ServiceId serviceId)
{
    MemberHelper::SetMember(this->serviceId, serviceId, nullptr);
}

shared_ptr<Pid> TmssEntity::GetPmtPid() const
{
	return pmtPid;
}

void TmssEntity::SetPmtPid(shared_ptr<Pid> pmtPid)
{
	this->pmtPid = pmtPid;
}

void TmssEntity::SetPmtPid(Pid pmtPid)
{
	MemberHelper::SetMember(this->pmtPid, pmtPid, nullptr);
}

shared_ptr<Pid> TmssEntity::GetPcrPid() const
{
	return pcrPid;
}

void TmssEntity::SetPcrPid(shared_ptr<Pid> pcrPid)
{
	this->pcrPid = pcrPid;
}

void TmssEntity::SetPcrPid(Pid pcrPid)
{
	MemberHelper::SetMember(this->pcrPid, pcrPid, nullptr);
}

shared_ptr<Pid> TmssEntity::GetAudioPid() const
{
	return audioPid;
}

void TmssEntity::SetAudioPid(shared_ptr<Pid> audioPid)
{
	this->audioPid = audioPid;
}

void TmssEntity::SetAudioPid(Pid audioPid)
{
	MemberHelper::SetMember(this->audioPid, audioPid, nullptr);
}

shared_ptr<Pid> TmssEntity::GetVideoPid() const
{
	return videoPid;
}

void TmssEntity::SetVideoPid(shared_ptr<Pid> videoPid)
{
	this->videoPid = videoPid;
}

void TmssEntity::SetVideoPid(Pid videoPid)
{
	MemberHelper::SetMember(this->videoPid, videoPid, nullptr);
}

shared_ptr<string> TmssEntity::GetDescription() const
{
    return description;
}

void TmssEntity::SetDescription(shared_ptr<string> description)
{
    this->description = description;
}

void TmssEntity::SetDescription(const string& description)
{
    MemberHelper::SetMember(this->description, description, nullptr);
}

weak_ptr<TsEntity> TmssEntity::GetTs() const
{
    return ts;
}

void TmssEntity::SetTs(weak_ptr<TsEntity> ts)
{
    MemberHelper::SetMember(this->ts, ts, shared_from_this());
}

const list<shared_ptr<TmssEventEntity>>& TmssEntity::GetTmssEvents() const
{
    return tmssEvents;
}

list<shared_ptr<TmssEventEntity>>& TmssEntity::GetTmssEvents()
{
    return tmssEvents;
}

void TmssEntity::SetTmssEvents(const list<shared_ptr<TmssEventEntity>>& tmssEvents)
{
    MemberHelper::SetMember(this->tmssEvents, tmssEvents, shared_from_this());
}

void TmssEntity::Bind(shared_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::BindMember(tmssEvents, tmssEvent, shared_from_this());
}

void TmssEntity::Unbind(shared_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::UnbindMember(tmssEvents, tmssEvent, shared_from_this());
}

string TmssEntity::ToString() const
{
    return "";
}

/* private functions */
void TmssEntity::Bind(shared_ptr<TsEntity> ts)
{
    MemberHelper::BindMember(this->ts, ts, shared_from_this());
}

void TmssEntity::Unbind(shared_ptr<TsEntity>)
{
    MemberHelper::UnbindMember(this->ts, NullSharedPtr(TsEntity), shared_from_this());
}

void TmssEntity::Bind(weak_ptr<TsEntity> ts)
{
    MemberHelper::BindMember(this->ts, ts, shared_from_this());
}

void TmssEntity::Unbind(weak_ptr<TsEntity>)
{
    MemberHelper::UnbindMember(this->ts, NullWeakPtr(TsEntity), shared_from_this());
}

void TmssEntity::Bind(weak_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::BindMember(tmssEvents, tmssEvent, shared_from_this());
}

void TmssEntity::Unbind(weak_ptr<TmssEventEntity> tmssEvent)
{
    MemberHelper::UnbindMember(tmssEvents, tmssEvent, shared_from_this());
}

/**********************class TmssEventEntity**********************/
/* public functions */
TmssEventEntity::TmssEventEntity(TableId id, shared_ptr<EventId> eventId,
    shared_ptr<TimePoint> startTimePoint, shared_ptr<Duration> duration)
    : id(id), eventId(eventId), startTimePoint(startTimePoint), duration(duration)
{}

TmssEventEntity::TmssEventEntity()
: TmssEventEntity(InvalidTableId, nullptr, nullptr, nullptr)
{}

TmssEventEntity::TmssEventEntity(const TmssEventEntity& right)
    : TmssEventEntity(right.id, make_shared<EventId>(*right.eventId),
    make_shared<TimePoint>(*right.startTimePoint), make_shared<Duration>(*right.duration))
{}

TmssEventEntity::TmssEventEntity(TableId id, EventId eventId,
    TimePoint startTimePoint, Duration duration)
    : TmssEventEntity(id, make_shared<EventId>(eventId),
    make_shared<TimePoint>(startTimePoint), make_shared<Duration>(duration))
{}

TmssEventEntity::~TmssEventEntity()
{}

TableId TmssEventEntity::GetId() const
{
    return id;
}

void TmssEventEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<EventId> TmssEventEntity::GetEventId() const
{
    return eventId;
}

void TmssEventEntity::SetEventId(shared_ptr<EventId> eventId)
{
    this->eventId = eventId;
}

void TmssEventEntity::SetEventId(EventId eventId)
{
    MemberHelper::SetMember(this->eventId, eventId, nullptr);
}

shared_ptr<TimePoint> TmssEventEntity::GetStartTimePoint() const
{
    return startTimePoint;
}

void TmssEventEntity::SetStartTimePoint(shared_ptr<TimePoint> startTimePoint)
{
    this->startTimePoint = startTimePoint;
}

void TmssEventEntity::SetStartTimePoint(TimePoint startTimePoint)
{
    MemberHelper::SetMember(this->startTimePoint, startTimePoint, nullptr);
}

shared_ptr<Duration> TmssEventEntity::GetDuration() const
{
    return duration;
}

void TmssEventEntity::SetGetDuration(shared_ptr<Duration> duration)
{
    this->duration = duration;
}

void TmssEventEntity::SetGetDuration(Duration duration)
{
    MemberHelper::SetMember(this->duration, duration, nullptr);
}

weak_ptr<TmssEntity> TmssEventEntity::GetTmss() const
{
    return tmss;
}

void TmssEventEntity::SetTmss(weak_ptr<TmssEntity> tmss)
{
    MemberHelper::SetMember(this->tmss, tmss, shared_from_this());
}

shared_ptr<RefsEventEntity> TmssEventEntity::GetRefsEvent() const
{
    return refsEvent;
}

void TmssEventEntity::SetRefsEvent(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::SetMember(this->refsEvent, refsEvent, shared_from_this());
}

void TmssEventEntity::Bind(shared_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(this->refsEvent, refsEvent, shared_from_this());
}

void TmssEventEntity::Unbind(shared_ptr<RefsEventEntity>)
{
    MemberHelper::UnbindMember(refsEvent, NullSharedPtr(RefsEventEntity), shared_from_this());
}

string TmssEventEntity::ToString() const
{
    return "";
}

/* private functions */
void TmssEventEntity::Bind(shared_ptr<TmssEntity> tmss)
{
    MemberHelper::BindMember(this->tmss, tmss, shared_from_this());
}

void TmssEventEntity::Unbind(shared_ptr<TmssEntity>)
{
    MemberHelper::UnbindMember(tmss, NullSharedPtr(TmssEntity), shared_from_this());
}

void TmssEventEntity::Bind(weak_ptr<TmssEntity> tmss)
{
    MemberHelper::BindMember(this->tmss, tmss, shared_from_this());
}

void TmssEventEntity::Unbind(weak_ptr<TmssEntity>)
{
    MemberHelper::UnbindMember(tmss, NullWeakPtr(TmssEntity), shared_from_this());
}

void TmssEventEntity::Bind(weak_ptr<RefsEventEntity> refsEvent)
{
    MemberHelper::BindMember(this->refsEvent, refsEvent, shared_from_this());
}

void TmssEventEntity::Unbind(weak_ptr<RefsEventEntity>)
{
    MemberHelper::UnbindMember(refsEvent, NullWeakPtr(RefsEventEntity), shared_from_this());
}

/**********************class PstsEntity**********************/
PstsEntity::PstsEntity(TableId id, shared_ptr<ServiceId> serviceId,
	shared_ptr<Pid> pmtPid, shared_ptr<StreamType> streamType, shared_ptr<Pid> posterPid)
	: id(id), serviceId(serviceId), pmtPid(pmtPid), streamType(streamType), posterPid(posterPid)
{}

PstsEntity::PstsEntity()
	: PstsEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr)
{}

PstsEntity::PstsEntity(const PstsEntity& right)
	: PstsEntity(right.id, make_shared<ServiceId>(*right.serviceId),
	make_shared<Pid>(right.pmtPid), make_shared<StreamType>(*right.streamType), make_shared<Pid>(*right.posterPid))
{}

PstsEntity::PstsEntity(TableId id, ServiceId serviceId,
	Pid pmtPid, StreamType streamType, Pid posterPid)
	: PstsEntity(id, make_shared<ServiceId>(serviceId),
	make_shared<Pid>(pmtPid), make_shared<StreamType>(streamType), make_shared<Pid>(posterPid))
{}

PstsEntity::~PstsEntity()
{}

TableId PstsEntity::GetId() const
{
	return id;
}

void PstsEntity::SetId(TableId id)
{
	this->id = id;
}

shared_ptr<ServiceId> PstsEntity::GetServiceId() const
{
	return serviceId;
}

void PstsEntity::SetServiceId(shared_ptr<ServiceId> serviceId)
{
	this->serviceId = serviceId;
}

void PstsEntity::SetServiceId(ServiceId serviceId)
{
	MemberHelper::SetMember(this->serviceId, serviceId, nullptr);
}

shared_ptr<Pid> PstsEntity::GetPmtPid() const
{
	return pmtPid;
}

void PstsEntity::SetPmtPid(shared_ptr<Pid> pmtPid)
{
	this->pmtPid = pmtPid;
}

void PstsEntity::SetPmtPid(Pid pmtPid)
{
	MemberHelper::SetMember(this->pmtPid, pmtPid, nullptr);
}

shared_ptr<StreamType> PstsEntity::GetStreamType() const
{
	return streamType;
}

void PstsEntity::SetStreamType(shared_ptr<StreamType> streamType)
{
	this->streamType = streamType;
}

void PstsEntity::SetStreamType(StreamType streamType)
{
	MemberHelper::SetMember(this->streamType, streamType, nullptr);
}

shared_ptr<Pid> PstsEntity::GetPosterPid() const
{
	return posterPid;
}

void PstsEntity::SetPosterPid(shared_ptr<Pid> posterPid)
{
	this->posterPid = posterPid;
}

void PstsEntity::SetPosterPid(Pid posterPid)
{
	MemberHelper::SetMember(this->posterPid, posterPid, nullptr);
}

weak_ptr<RefsEntity> PstsEntity::GetRefs() const
{
    return refs;
}

void PstsEntity::SetRefs(weak_ptr<RefsEntity> refs)
{
    MemberHelper::SetMember(this->refs, refs, shared_from_this());
}

/* private functions */
void PstsEntity::Bind(shared_ptr<RefsEntity> refs)
{
	MemberHelper::BindMember(this->refs, refs, shared_from_this());
}

void PstsEntity::Unbind(shared_ptr<RefsEntity>)
{
	MemberHelper::UnbindMember(this->refs, NullSharedPtr(RefsEntity), shared_from_this());
}

void PstsEntity::Bind(weak_ptr<RefsEntity> refs)
{
	MemberHelper::BindMember(this->refs, refs, shared_from_this());
}

void PstsEntity::Unbind(weak_ptr<RefsEntity>)
{
	MemberHelper::UnbindMember(this->refs, NullWeakPtr(RefsEntity), shared_from_this());
}

/**********************class GlobalCfgEntity**********************/
GlobalCfgEntity::GlobalCfgEntity(TableId id, shared_ptr<Duration> patInterval,
    shared_ptr<Duration> pmtInterval, shared_ptr<Duration> posterInterval)
    : id(id), patInterval(patInterval), pmtInterval(pmtInterval), posterInterval(posterInterval)
{}

GlobalCfgEntity::GlobalCfgEntity()
    : GlobalCfgEntity(InvalidTableId, nullptr, nullptr, nullptr)
{}

GlobalCfgEntity::GlobalCfgEntity(const GlobalCfgEntity& right)
    : GlobalCfgEntity(right.id, make_shared<Duration>(*right.patInterval),
    make_shared<Duration>(*right.pmtInterval), make_shared<Duration>(*right.posterInterval))
{}

GlobalCfgEntity::GlobalCfgEntity(TableId id, Duration patInterval,
    Duration pmtInterval, Duration posterInterval)
    : GlobalCfgEntity(id, make_shared<Duration>(patInterval),
    make_shared<Duration>(pmtInterval), make_shared<Duration>(posterInterval))
{}

GlobalCfgEntity::~GlobalCfgEntity()
{}

TableId GlobalCfgEntity::GetId() const
{
    return id;
}

void GlobalCfgEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<Duration> GlobalCfgEntity::GetPatInterval() const
{
    return patInterval;
}

void GlobalCfgEntity::SetPatInterval(shared_ptr<Duration> patInterval)
{
    this->patInterval = patInterval;
}

void GlobalCfgEntity::SetPatInterval(Duration patInterval)
{
    MemberHelper::SetMember(this->patInterval, patInterval, nullptr);
}

shared_ptr<Duration> GlobalCfgEntity::GetPmtInterval() const
{
    return pmtInterval;
}

void GlobalCfgEntity::SetPmtInterval(shared_ptr<Duration> pmtInterval)
{
    this->pmtInterval = pmtInterval;
}

void GlobalCfgEntity::SetPmtInterval(Duration pmtInterval)
{
    MemberHelper::SetMember(this->pmtInterval, pmtInterval, nullptr);
}

shared_ptr<Duration> GlobalCfgEntity::GetPosterInterval() const
{
    return posterInterval;
}

void GlobalCfgEntity::SetPosterInterval(shared_ptr<Duration> posterInterval)
{
    this->posterInterval = posterInterval;
}

void GlobalCfgEntity::SetPosterInterval(Duration posterInterval)
{
    MemberHelper::SetMember(this->posterInterval, posterInterval, nullptr);
}

