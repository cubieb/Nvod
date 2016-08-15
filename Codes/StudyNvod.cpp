#include "SystemInclude.h"

#include "PacketHelper.h"

using namespace std;

/*
Bibliography:
    1. iso13818-1, 2.4.2.2 Input to the Transport Stream system target decoder
    2. iso13818-1, Table 2-6 – Transport Stream adaptation field

    PCR(i) = PCR_base(i) × 300 + PCR_ext(i)                        (2-1)
    PCR_base(i) = ((system_clock_ frequency × t(i)) DIV 300) % 233 (2-2)
    PCR_ext(i) = ((system_clock_ frequency × t(i)) DIV 1) % 300    (2-3)
            
    t(i) = PCR(i'') / system_clock_ frequency + (i − i'') / transport_rate (i)           (2-4)
    transport_rate(i) = ((i' – i'' ) × system_clock_frequency) / (PCR(i') – PCR(i'')) (2-5)
*/
int64_t ReadPcr(uchar_t *ptr)
{
    const uint_t PcrBaseBits = 33;
    const uint_t PcrExtBits = 9;
    const uint_t TotalBits  = PcrBaseBits + PcrExtBits + 6; //6 bits for Reserved
    int64_t value, pcrBase, pcrExt;
    ReadBuffer(ptr, value);
    pcrBase = value >> (64 - PcrBaseBits);
    pcrExt = (value >> (64 - TotalBits)) & (~(ULLONG_MAX << PcrExtBits));
    return (pcrBase * 300 + pcrExt);
}

chrono::milliseconds GetDuration(int64_t pcr1, int64_t pcr2)
{
	const int64_t SystemClockFrequency = 27000000 / milli::den;	
    return chrono::milliseconds(abs((pcr2 - pcr1) / SystemClockFrequency));
}

void RunDriver()
{ 
    uchar_t pkt1[] = {0xa5, 0x35, 0xe2, 0x46, 0x7e, 0x4d};
	uchar_t pkt2[] = {0xa5, 0x35, 0xe8, 0xcf, 0xfe, 0x1b};

    int64_t pcr1 = ReadPcr(pkt1);
	int64_t pcr2 = ReadPcr(pkt2);
	cout << GetDuration(pcr1, pcr2).count();
}
