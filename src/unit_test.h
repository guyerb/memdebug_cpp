#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <iostream>
#include <iomanip>
#include <cstdio>

class unit_test {

  const std::string _unit_prefix = "UNITTEST: ";
  std::string _unit_dscr = {};
  uint32_t _unit_test = {0};
  uint32_t _unit_pass = {0};
  uint32_t _unit_fail = {0};

  const std::string _sctn_prefix = "SECTION : ";
  std::string _sctn_dscr = {};
  uint32_t _sctn_test = {0};
  uint32_t _sctn_pass = {0};
  uint32_t _sctn_fail = {0};

 public:
  unit_test() = default;
  ~unit_test() {};

  void ut_start_unit(std::string descr, char delim='#');
  void ut_finish_unit();

  void ut_start_section(std::string descr, char delim='*');
  void ut_finish_section();

  void ut_header(std::string prefix, std::string descr, char delim);

  template <class T>
  void ut_check(std::string descr, T, T);

  template <class T>
  void ut_check_ne(std::string descr, T, T);
};

template <class T>
void unit_test::ut_check(std::string descr, T expected, T actual)
{
  bool passed = (expected == actual);

  _unit_test++; _sctn_test++;
  if (passed) {
    _sctn_pass++; _unit_pass++;
  } else {
    _sctn_fail++; _unit_fail++;
  }
  std::cout << std::setw(30) << std::left << descr << std::setw(8) << \
    (passed ? "PASSED_" : "_FAILED") << std::setw(12) << std::right <<  \
    expected << std::setw(12) << std::right << actual << std::endl;
}

template <class T>
void unit_test::ut_check_ne(std::string descr, T expected, T actual)
{
  bool passed = (expected != actual);

  _unit_test++; _sctn_test++;
  if (passed) {
    _sctn_pass++; _unit_pass++;
  } else {
    _sctn_fail++; _unit_fail++;
  }
  std::cout << std::setw(30) << std::left << descr << std::setw(8) << \
    (passed ? "PASSED_" : "_FAILED") << std::setw(12) << std::right <<  \
    expected << std::setw(12) << std::right << actual << std::endl;
}

void unit_test::ut_header(std::string prefix, std::string descr, char delim)
{
  uint8_t strlen = prefix.length() + descr.length();
  std::cout << std::endl << std::setfill(delim) << std::setw(15) << " " << prefix \
	    << descr << " " << std::setw(80 - strlen - 15) << delim << std::setfill(' ') \
	    << std::endl;
}

void unit_test::ut_start_unit(std::string descr, char delim)
{
  _unit_dscr = descr;
  ut_header(_unit_prefix, _unit_dscr, delim);
}

void unit_test::ut_start_section(std::string descr, char delim)
{
  _sctn_dscr = descr;
  ut_header(_sctn_prefix, _sctn_dscr, delim);
}

void unit_test::ut_finish_unit()
{
  std::cout << std::endl << _unit_prefix << std::setw(30) << std::left << \
    _unit_dscr << " tests " << _unit_test << " passed " << _unit_pass << \
    " failed " << _unit_fail << std::endl << std::endl;
  _unit_test = _unit_pass = _unit_fail = 0;
}

void unit_test::ut_finish_section()
{
  std::cout << std::endl << _sctn_prefix << std::setw(30) << std::left << \
    _sctn_dscr << " tests " << _sctn_test << " passed " << _sctn_pass << \
    " failed " << _sctn_fail << std::endl;
  _sctn_test = _sctn_pass = _sctn_fail = 0;
}
#endif	/* UNIT_TESTS_H */
