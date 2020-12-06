#define _DEFAULT_SOURCE		/* usleep */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define UNUSED(x) (void)(x)
void *spinner(void *vargp)
{
  void *p = NULL;
  void *p1 = NULL;

  UNUSED(vargp);
  do {
    p = malloc(rand() % 10000);
    p1 = malloc(rand() % 2048);
    usleep(rand() % 2000000);
    free(p);
    free(p1);
  } while (1);

  return NULL;
}

int main()
{
    pthread_t tid;

    srand(time(NULL));

    printf("CTRL-C to exit\n");

    for (int i = 0; i < 10; i++) {
	pthread_create(&tid, NULL, spinner, NULL);
	pthread_detach(tid);
    }

    pthread_exit(NULL);
    return 0;
}
