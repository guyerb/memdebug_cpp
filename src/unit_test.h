#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <iostream>
#include <iomanip>
#include <cstdio>

using namespace::std;

class unit_test {

  const std::string _unit_prefix = "UNITTEST: ";
  std::string _unit_descr = {};
  uint32_t _unit_test = {0};
  uint32_t _unit_pass = {0};
  uint32_t _unit_fail = {0};

  const std::string _sectn_prefix = "SECTION : ";
  std::string _sectn_descr = {};
  uint32_t _sectn_test = {0};
  uint32_t _sectn_pass = {0};
  uint32_t _sectn_fail = {0};

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

  _unit_test++; _sectn_test++;
  if (passed) {
    _sectn_pass++; _unit_pass++;
  } else {
    _sectn_fail++; _unit_fail++;
  }
  cout << setw(30) << left << descr << setw(8) << (passed ? "PASSED_" : "_FAILED") << setw(12) << right <<  expected << setw(12) << right << actual << endl;
}

template <class T>
void unit_test::ut_check_ne(std::string descr, T expected, T actual)
{
  bool passed = (expected != actual);

  _unit_test++; _sectn_test++;
  if (passed) {
    _sectn_pass++; _unit_pass++;
  } else {
    _sectn_fail++; _unit_fail++;
  }
  cout << setw(30) << left << descr << setw(8) << (passed ? "PASSED_" : "_FAILED") << setw(12) << right <<  expected << setw(12) << right << actual << endl;
}

void unit_test::ut_header(std::string prefix, std::string descr, char delim)
{
  uint8_t strlen = prefix.length() + descr.length();
  cout << endl << setfill(delim) << setw(15) << " " << prefix << descr << " " << setw(80 - strlen - 15) << delim << setfill(' ') << endl;
}

void unit_test::ut_start_unit(std::string descr, char delim)
{
  _unit_descr = descr;
  ut_header(_unit_prefix, _unit_descr, delim);
}

void unit_test::ut_start_section(std::string descr, char delim)
{
  _sectn_descr = descr;
  ut_header(_sectn_prefix, _sectn_descr, delim);
}

void unit_test::ut_finish_unit()
{
  cout << endl << _unit_prefix << setw(30) << left << _unit_descr << " tests " << _unit_test << " passed " << _unit_pass << " failed " << _unit_fail << endl << endl;
  _unit_test = _unit_pass = _unit_fail = 0;
}

void unit_test::ut_finish_section()
{
  cout << endl << _sectn_prefix << setw(30) << left << _sectn_descr << " tests " << _sectn_test << " passed " << _sectn_pass << " failed " << _sectn_fail << endl;
  _sectn_test = _sectn_pass = _sectn_fail = 0;
}

#endif	/* UNIT_TESTS_H */
