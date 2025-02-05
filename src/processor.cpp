#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  // Call CpuUtilization to cause the jiffie values to update
  LinuxParser::CpuUtilization();

  float jiffies = LinuxParser::Jiffies();
  if (jiffies == 0.0) {
    return 0.0f;
  } else {
    return 1.0f - LinuxParser::IdleJiffies() / jiffies;
  }
}