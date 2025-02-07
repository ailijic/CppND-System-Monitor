#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process()
    : pid_(LinuxParser::SelfPid()),
      cmd_(LinuxParser::Command(pid_)),
      user_(LinuxParser::User(pid_)) {}
Process::Process(int pid)
    : pid_(pid),
      cmd_(LinuxParser::Command(pid)),
      user_(LinuxParser::User(pid)) {}

/// TODO: Return this process's ID
int Process::Pid() {
  return pid_;
}

/// Return this process's CPU utilization
float Process::CpuUtilization() {
  return (1.0f * LinuxParser::ActiveJiffies(pid_)) /
         (1.0f * LinuxParser::ActiveJiffies());
}

/// Return the command that generated this process
string Process::Command() {
  return cmd_;
}

/// Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(pid_);
}

/// Return the user (name) that generated this process
string Process::User() {
  return user_;
}

/// Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(pid_);
}

/// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (this->pid_ < a.pid_);
}