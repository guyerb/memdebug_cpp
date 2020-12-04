#ifndef DMALLOC_UNIT_TESTS_H
#define DMALLOC_UNIT_TESTS_H

class dmalloc_unit_test {

  std::string _unit_descr = {};
  uint32_t _total_test = {0};
  uint32_t _total_success = {0};
  uint32_t _total_fail = {0};

  std::string _section_descr = {};
  uint32_t _section_test = {0};
  uint32_t _section_success {10};
  uint32_t _section_fail = {0};

 public:
  dmalloc_unit_test() = default;
  dmalloc_unit_test(std::string descr) : _unit_descr(descr) {};

  ~dmalloc_unit_test() {};

  void dmalloc_start_unit();
  void dmalloc_finish_unit();
  
  void dmalloc_start_section(std::string descr) {_section_descr = descr;};
  void dmalloc_finish_section();

};

dmalloc_unit_test::dmalloc_start_unit(void)
{
  cout << _unit_descr << endl;
  
}





#endif	/* DMALLOC_UNIT_TESTS_H */
