#ifndef _EpgServerInterface_h_
#define _EpgServerInterface_h_

//EPG: Electronic Program Guide
#include "SystemInclude.h"

/*
<Root>
    <Tx>
        <!--当DstIp为组播时,通过SrcIp来决定通过那张网卡发送数据。-->
        <SrcIp>10.0.0.2</SrcIp>
        <DstIp>224.1.1.1</DstIp>
        <DstUdpPort>5001</DstUdpPort>
    </Tx>
    <DataPipeTransportStream tsid=”1”>
        <Service Id="9">
            <PmtPid>101</PmtPid>
            <PosterPid>102</PosterPid>
        </Service>
    </DataPipeTransportStream>
    <DataPipePid>500</DataPipePid>
    <TimeShiftedTransportStream tsid=”1”>
        <Service Id="1">
            <PmtPid>201</PmtPid>
            <PcrPid>202</PcrPid>
            <AudioPid>203</AudioPid>
            <VideoPid>204</VideoPid>
        </Service>
    </TimeShiftedTransportStream>
    <SendingIntervalMilliseconds>
        <Pat>500</Pat>
        <Pmt>500</Pmt>
        <Poster>2000</Poster>
    </SendingIntervalMilliseconds>
</Root>
*/
class EpgServerInterface
{
public:
    EpgServerInterface() {};
    virtual ~EpgServerInterface() {};

    /* Get Nvod config and save as path. */
    virtual bool GetNvodConfig(const char *path) = 0;
};

#endif