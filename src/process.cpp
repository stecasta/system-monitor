#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, float cpu_usage)
    : pid_(pid), cpu_utilization_(cpu_usage){};

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  cpu_utilization_ = LinuxParser::CpuUtilization(pid_);
  return cpu_utilization_;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() {
  ram_ = LinuxParser::Ram(pid_);
  return ram_;
}

string Process::User() { return LinuxParser::User(LinuxParser::Uid(pid_)); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return cpu_utilization_ > a.cpu_utilization_;
}