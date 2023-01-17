#include "format.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>

using namespace std::chrono_literals;
using std::string;

// DONE: Complete this helper function
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  long minutes = (seconds % 3600) / 60;
  long secs = seconds % 60;
  char time[8];
  sprintf(time, "%02d:%02d:%02d", hours, minutes, secs);
  return time;
}
