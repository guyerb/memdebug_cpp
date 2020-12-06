#include <random>
#include <thread>
#include <array>
#include <cstdio>


std::mt19937 engine;

void spinner()
{
  std::uniform_int_distribution<int> dist(0, 8192);
  void *p = nullptr;
  void *p1 = nullptr;
  void *p2 = nullptr;
  void *p3 = nullptr;
  int i = 0;

  do {
    if (!p) p  = malloc(dist(engine));
    if (p2) { free(p2); p2 = nullptr;}
    if (!p1) p1 = malloc(dist(engine));
    if (p) { free(p); p = nullptr;}
    if (p3) { free(p3); p3 = nullptr;}
    if (!p2) p2 = malloc(dist(engine));
    if (p1) { free(p1); p1 = nullptr;}
    if (!p3) p3 = malloc(dist(engine));
  } while (i++  < 10000000);
}

int main()
{
  std::random_device rd;
  std::array<int, std::mt19937::state_size> seed_data;
  std::generate_n(seed_data.data(), seed_data.size(), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 gen(seq);

  printf("CTRL-C to exit\n");


  std::thread t1 (spinner);
  std::thread t2 (spinner);
  std::thread t3 (spinner);
  std::thread t4 (spinner);

  t1.join();
  t2.join();
  t3.join();
  t4.join();

  printf("120 million threaded mallocs and frees completed\n");

  return 0;
}
