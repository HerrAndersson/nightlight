#include "Cpu.h"
#include <string>

Cpu::Cpu()
{
	PDH_STATUS status;

	canReadCpu = true;

	status = PdhOpenQuery(NULL, 0, &queryHandle);
	if (status != ERROR_SUCCESS)
		canReadCpu = false;

	status = PdhAddEnglishCounter(queryHandle, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &counterHandle);
	if (status != ERROR_SUCCESS)
		canReadCpu = false;

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
