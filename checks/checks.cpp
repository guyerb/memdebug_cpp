#include <random>
#include <thread>
#include <array>
#include <cstdio>

std::mt19937 engine;

void spinner()
{
  std::uniform_int_distribution<int> dist1(0, 8192);
  std::uniform_int_distribution<int> dist2(0, 1024);
  std::uniform_int_distribution<int> dist3(0, 512);
  std::uniform_int_distribution<int> dist4(0, 64);
  void *p = nullptr;
  void *p1 = nullptr;
  void *p2 = nullptr;
  void *p3 = nullptr;
  int i = 0;

  do {
    if (i % 100000 == 0) putc('~', stderr);

    if (!p) p  = malloc(dist1(engine));
    if (p3) { free(p3); p3 = nullptr;}

    if (!p1) p1 = malloc(dist2(engine));
    if (p) { free(p); p = nullptr;}

    if (!p2) p2 = malloc(dist3(engine));
    if (p1) { free(p1); p1 = nullptr;}

    if (!p3) p3 = malloc(dist4(engine));
    if (p2) { free(p2); p2 = nullptr;}

    //    usleep(rand() % 2000000);

  } while (i++  < 10000000);
}

int main()
{
  std::random_device rd;
  std::array<int, std::mt19937::state_size> seed_data;
  //  std::generate_n(seed_data.data(), seed_data.size(), std::ref(rd));
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

  printf("160 million threaded mallocs and frees completed\n");

  return 0;
}
