#include "Cpu.h"
#include <string>

Cpu::Cpu()
{
	PDH_STATUS status;

	canReadCpu = true;

	status = PdhOpenQuery(NULL, 0, &queryHandle);
	if (status != ERROR_SUCCESS)
		canReadCpu = false;

	status = PdhAddCounter(queryHandle, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &counterHandle);
	if (status != ERROR_SUCCESS)
		canReadCpu = false;

	if (status != ERROR_SUCCESS)
	{
		printf("PhdCollectQueryData() ***Error: 0x%X\n", status);
		if (status == PDH_INVALID_HANDLE)
			printf("PDH_INVALID_HANDLE\n");
		else if (status == PDH_NO_DATA)
			printf("PDH_NO_DATA\n");
		else
			printf("Unknown error\n");
	}

	lastSampleTime = GetTickCount();
	cpu = 0;
}


Cpu::~Cpu()
{
	PdhCloseQuery(queryHandle);
}

int Cpu::GetCpuPercentage()
{
	int usage;
	if (canReadCpu)
		usage = (int)cpu;
	else
		usage = 0;
	return usage;
}

void Cpu::Update()
{
	PDH_FMT_COUNTERVALUE value;

	if (canReadCpu)
	{
		if ((lastSampleTime + 1000) < GetTickCount())
		{
			lastSampleTime = GetTickCount();

			PdhCollectQueryData(queryHandle);

			PdhGetFormattedCounterValue(counterHandle, PDH_FMT_LONG, NULL, &value);

			cpu = value.longValue;
		}
	}
}
