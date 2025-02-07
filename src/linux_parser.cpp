#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;
using std::cout;
using std::cerr;

// Column IDs for /proc/[pid]/stat
enum {
  user_jiff_e = 14,
  start_time_e = 22,
};

typedef enum Jiff_Types {
  user_e,
  nice_e,
  sys_e,
  idle_e,
  io_e,
  irq_e,
  s_irq_e,
  steal_e,
  guest_e,
  jiff_types_len_e,
} Jiff_Types;

static vector<long> prev_g = vector<long>(jiff_types_len_e);
static vector<long> curr_g = vector<long>(jiff_types_len_e);

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

/// Return vector of all PIDs for running processes
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO(ai): pull out common logic
float LinuxParser::MemoryUtilization() {
  float ret = -1;
  float mem_total = -1, mem_free = -1;
  ifstream mem = ifstream(kProcDirectory + kMeminfoFilename);
  if (mem.is_open()) {
    string line;
    if (getline(mem, line)) {
      std::istringstream iss(line);
      string field, val, unit;
      iss >> field >> val >> unit;
      try {
        mem_total = std::stof(val);
      } catch (...) {
        cerr << "Can't convert '" << val << "' to a float\n";
      }
    }
    if (getline(mem, line)) {
      std::istringstream iss(line);
      string field, val, unit;
      iss >> field >> val >> unit;
      try {
        mem_free = std::stof(val);
      } catch (...) {
        cerr << "Can't convert '" << val << "' to a float\n";
      }
    }
  }
  if (mem_total != -1 && mem_free != -1) {
    ret = (mem_total - mem_free) / mem_total;
  }
  return ret;
}

/// Read and return the system uptime
long LinuxParser::UpTime() {
  double ret = 0;
  ifstream uptime = ifstream(kProcDirectory + kUptimeFilename);
  if (uptime.is_open()) {
    string line;
    if (getline(uptime, line, ' ')) {
      std::istringstream iss(line);
      string num_str;
      iss >> num_str;
      try {
        ret = std::stod(num_str);
      } catch (...) {
        return -1;
      }
    } else {
      return -2;
    }
    uptime.close();
  } else {
    return -3;
  }
  return ret;
}

/// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return std::accumulate(curr_g.begin(), curr_g.end(), 0L) -
         std::accumulate(prev_g.begin(), prev_g.end(), 0L);
}

/// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    for (int i = 0; i < user_jiff_e; i++) {
      std::getline(stream, line, ' ');
    }
    return std::stol(line);
  }
  return -1;
}

/// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

/// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  return curr_g[idle_e] - prev_g[idle_e];
}

/// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string cpu;
  string line;
  vector<string> ret = vector<string>(jiff_types_len_e);
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);
    iss >> cpu;  // get rid of cpu name so we only have strings of numbers
    for (string& str : ret) {
      iss >> str;
    }
    prev_g = curr_g;
    std::transform(ret.begin(), ret.end(), curr_g.begin(),
                   [](string const& str) { return std::stol(str); });
  }
}

/// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return LinuxParser::Pids().size();
}

/// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int ret = -1;
  string os, kernel, version, field, val;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    for (;;) {
      if (!std::getline(stream, line)) {
        break;
      }
      std::istringstream iss(line);
      iss >> field;
      if (field == "procs_running") {
        iss >> val;
        ret = std::stoi(val);
        break;
      }
    }
  }
  return ret;
}

/// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open() && std::getline(stream, line)) {
    return line;
  } else {
    return string();
  }
}

/// Read and return the memory used by a process (KiB)
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kPidMemMap);
  // std::ifstream filestream(kProcDirectory + "self" + kPidMemMap);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Pss:") {
          return value;
        }
      }
    }
  }
  return value;
}

/// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

/// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line;
  string uname, user_id;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line, ':')) {  // Get user column
      uname = line;
      std::getline(filestream, line, ':');  // Get password column
      std::getline(filestream, line, ':');  // Get uid column
      user_id = line;
      if (user_id ==
          uid) {  // Check to see if we found the user we are looking for
        return uname;
      } else {  // If not the user we are looking for go to the next line
        std::getline(filestream, line);
      }
    }
  }
  return uname;
}

/// Read and return the uptime of a process (seconds)
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    for (int i = 0; i < start_time_e; i++) {
      std::getline(stream, line, ' ');
    }
    return UpTime() - std::stol(line) / sysconf(_SC_CLK_TCK);
  }
  return -1;
}

/// Return PID of this process
int LinuxParser::SelfPid() {
  string line;
  std::ifstream stream(kProcDirectory + "self" + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line, ' ');
    return std::stol(line);
  }
  return -1;
}
