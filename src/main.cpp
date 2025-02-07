#include "ncurses_display.h"
#include "system.h"

#include "linux_parser.h"
#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <dirent.h>
#include <cctype>

using std::cout;

int main() {
  {
    // std::cout << "Starting...\n";
    // long n = LinuxParser::UpTime();
    // std::cout << "n: " << n << "\n";
    // float n = LinuxParser::MemoryUtilization();
    // std::cout << "n: " << n << "\n";
    // LinuxParser::CpuUtilization();
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // LinuxParser::CpuUtilization();
  }
  /*
  std::vector<int> vec;
  vec.resize(10);
  std::cout << vec.size() << "\n";
  */

  /*
  const fs::path proc{LinuxParser::kProcDirectory};
  for (auto const& dir_entry : fs::directory_iterator{proc}) {
    if (fs::is_directory(dir_entry)) {
      std::cout << dir_entry << "\n";
    }
  }
  */
  /*
    DIR* proc = opendir("/proc");
    struct dirent* entry;
    int num_proc = 0;
    while ((entry = readdir(proc))) {
      char* fname = entry->d_name;
      char c;
      bool b = true;
      while ((c = *fname++) != '\0') {
        b &= isdigit(c); // This is okay because we only care if `isdigit()`
    returns a zero
        if (!b) { // If `b` is ever false bail out early.
          break;
        }
      }
      if (b) {
        num_proc++;
        cout << entry->d_name << '\n';
      }
    }
    cout << "procs: " << num_proc << "\n";
    */
  
  /// show cmd parser is working
  // int pid = 481;
  // cout << "PID " << pid << " CMD: " << LinuxParser::Command(10) << "\n";
  // cout << "PID " << "self" << " Pss(Mem): " << LinuxParser::Ram(59) << "\n";
  // cout << "Pid: " << pid << "\tUid: " << LinuxParser::Uid(pid) << "\n";
  // cout << "Pid: " << pid << "\tUser: " << LinuxParser::User(pid) << "\n";
  // cout << "Pid: " << pid << "\t" "Up Time: " << LinuxParser::UpTime(pid) << "\t""Ticks/sec: " << sysconf(_SC_CLK_TCK) << "\n";
  // cout << "Pid: " << pid << "\t" "Up Time: " << LinuxParser::ActiveJiffies(pid) << "\n";
  
  
  System system;
  NCursesDisplay::Display(system);
}