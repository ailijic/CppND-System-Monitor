#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;
using std::cout;
using std::cerr;

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

// BONUS: Update this to use std::filesystem
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
  float mem_total=-1, mem_free=-1;
  ifstream mem = ifstream(kProcDirectory + kMeminfoFilename);
  if (mem.is_open()) {
    string line;
    // cerr << "line: " << line << "\n";
   if (getline(mem, line)) {
     // cerr << "line: " << line << "\n";
     std::istringstream iss(line);
     string field, val, unit;
     iss >> field >> val >> unit;
     // cerr << val << "\n";
     try {
        mem_total = std::stof(val);
      } catch (...) {
        cerr << "Can't convert '" << val << "' to a float\n";
      }
   }
     if (getline(mem, line)) {
     // cerr << "line: " << line << "\n";
     std::istringstream iss(line);
     string field, val, unit;
     iss >> field >> val >> unit;
     // cerr <<field << val << "\n";
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

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  double ret = -1;
  ifstream uptime = ifstream(kProcDirectory + kUptimeFilename);
  if (uptime.is_open()) {
    string line;
    if (getline(uptime, line, ' ')){
      std::istringstream iss(line);
      string num_str;
      iss >> num_str;
      try {
        ret = std::stod(num_str);
      } catch (...) {
        cerr << "Can't convert '" << num_str << "' to a double\n";
      }
    } else {
      cerr << "'getline' error\n";
    }
    uptime.close();
  } else {
    cerr << "File not open\n";
  }
  return ret;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string os, kernel, version;
  string line;
  vector<string> ret;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  ret.push_back(os);
  ret.push_back(version);
  ret.push_back(kernel);
  return ret; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
