#ifndef _TxAddrCfg_h_
#define _TxAddrCfg_h_

#include "SystemInclude.h"
#include "Type.h"
#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/transaction.hxx>

class TxAddrCfg
{
public:
	TxAddrCfg(Ipv4 srcIp, Ipv4 dstIp, UdpPort dstUdpPort)
		: srcIp(srcIp), dstIp(dstIp), dstUdpPort(dstUdpPort)
	{}
	
	Ipv4 GetSrcIp() {return srcIp;}
	void SetSrcIp(Ipv4 srcIp) {this->srcIp = srcIp;}

	Ipv4 GetDstIp() {return dstIp;}
	void SetDstIp(Ipv4 dstIp) {this->dstIp = dstIp;}

	UdpPort GetDstUdpPort() {return dstUdpPort;}
	void SetDstUdpPort(UdpPort dstUdpPort) {this->dstUdpPort = dstUdpPort;}

private:
	Ipv4 srcIp;
	Ipv4 dstIp;
	UdpPort dstUdpPort;
};

#pragma db object(TxAddrCfg) table("TxAddrCfg")
#pragma db member(TxAddrCfg::srcIp) id column("SrcIp") get(GetSrcIp) set(SetSrcIp)
#pragma db member(TxAddrCfg::dstIp) column("DstIp") get(GetDstIp) set(SetDstIp)
#pragma db member(TxAddrCfg::dstUdpPort) column("DstUdpPort") get(GetDstUdpPort) set(SetDstUdpPort)
#endif