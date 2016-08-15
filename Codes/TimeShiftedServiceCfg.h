#ifndef _TimeShiftedServiceCfg_h_
#define _TimeShiftedServiceCfg_h_

#include "SystemInclude.h"
#include "Type.h"
#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/transaction.hxx>

class TimeShiftedServiceCfg
{
public:
    TimeShiftedServiceCfg(TsId tsId, ServiceId	serviceId, Pid pmtPid,
        Pid pcrPid, Pid audioPid, Pid videoPid)
        : tsId(tsId), serviceId(serviceId), pmtPid(pmtPid),
        pcrPid(pcrPid), audioPid(audioPid), videoPid(videoPid)
    {}

	Ipv4 GetTsId() {return tsId;}
	void SetTsId(Ipv4 tsId) {this->tsId = tsId;}

	Ipv4 GetServiceId() {return serviceId;}
	void SetServiceId(Ipv4 serviceId) {this->serviceId = serviceId;}

	UdpPort GetPmtPid() {return pmtPid;}
	void SetPmtPid(UdpPort pmtPid) {this->pmtPid = pmtPid;}

	Ipv4 GetPcrPid() {return pcrPid;}
	void SetPcrPid(Ipv4 pcrPid) {this->pcrPid = pcrPid;}

	Ipv4 GetAudioPid() {return audioPid;}
	void SetAudioPid(Ipv4 audioPid) {this->audioPid = audioPid;}

	UdpPort GetVideoPid() {return videoPid;}
	void SetVideoPid(UdpPort videoPid) {this->videoPid = videoPid;}

private:
	TsId		tsId;
	ServiceId	serviceId;
	Pid         pmtPid;
	Pid			pcrPid;
	Pid         audioPid;
	Pid         videoPid;
};

#pragma db object(TimeShiftedServiceCfg) table("TimeShiftedServiceCfg")
#pragma db member(TimeShiftedServiceCfg::tsId) id column("TsId") get(GetTsId) set(SetTsId)
#pragma db member(TimeShiftedServiceCfg::serviceId) id column("ServiceId") get(serviceId) set(serviceId)
#pragma db member(TimeShiftedServiceCfg::pmtPid) column("PmtPid") get(pmtPid) set(pmtPid)
#pragma db member(TimeShiftedServiceCfg::pcrPid) column("PcrPid") get(pcrPid) set(pcrPid)
#pragma db member(TimeShiftedServiceCfg::audioPid) column("AudioPid") get(audioPid) set(audioPid)
#pragma db member(TimeShiftedServiceCfg::videoPid) column("VideoPid") get(videoPid) set(videoPid)

#endif