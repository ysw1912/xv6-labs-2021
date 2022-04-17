#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int N = 35;

void child_process(int* left_channel) {
  int n;  // buffer for pipe read/write.
  int prime;  // first int read from pipe, used as base prime.
  if (left_channel) {
    if (read(left_channel[0], &n, 4)) {
      prime = n;
      printf("prime %d\n", prime);
    }
    if (!read(left_channel[0], &n, 4)) {
      // if there is no more int to read, then no need to create pipe and fork,
      // could just exit.
      exit(0);
    }
  }
  int right_channel[2];
  pipe(right_channel);
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork\n");
    exit(1);
  }
  if (pid > 0) {  // parent
    close(right_channel[0]);
    if (left_channel) {
      do {
        if (n % prime) {  // have read the next n in Line 15, so use do-while.
          write(right_channel[1], &n, 4);
        }
      } while (read(left_channel[0], &n, 4));
    } else {
      for (n = 2; n <= N; n++) {
        write(right_channel[1], &n, 4);
      }
    }
    // close right_channel write end, making the read call of child process
    // returns 0.
    close(right_channel[1]);
    wait(0);
  } else {  // child
    close(right_channel[1]);
    child_process(right_channel);
  }
}

int main(int argc, char* argv[]) {
  child_process(0);
  exit(0);
}
