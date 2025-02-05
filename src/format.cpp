#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long secs) {
  long hr = secs / (60 * 60);
  long min = (secs - (hr * 60 * 60)) / 60;
  long s = secs - (hr * 60 * 60) - (min * 60);
  // auto new_str = std::string(n_zero - std::min(n_zero, old_str.length()), '0') + old_str;
  string hr_s = to_string(hr);
  string min_s = to_string(min);
  string s_s = to_string(s);
  size_t n_zero = 2;
  string hr_ns =  string(n_zero - std::min(n_zero, hr_s.length()), '0') + hr_s;
  string min_ns =  string(n_zero - std::min(n_zero, min_s.length()), '0') + min_s;
  string s_ns =  string(n_zero - std::min(n_zero, s_s.length()), '0') + s_s;
  return hr_ns + ":" + min_ns + ":" + s_ns;
}