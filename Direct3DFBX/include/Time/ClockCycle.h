#pragma once

// Cycles / Hz = Seconds (Use for profilng or any need for precise timing)
// Cycles (Can be used to calculate percentage)

// The number of clock cycles
long long GetCurrentClockCycles();

// Hz, Get processor frequency in Hz not GHz
long long CalculateProcessorFrequency();

// Get System Ticks
long long GetSystemTicks();

// Get System Frequency
long long GetSystemFrequency();

// Get Internal Ticks
long long GetInternalTicks();
