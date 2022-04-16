#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  int p[2];
  pipe(p);
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork\n");
    exit(1);
  }
  char c;
  if (pid == 0) {  // child
    printf("%d: received ping\n", getpid());
    write(p[1], "\n", 1);
    read(p[0], &c, 1);
  } else {  // parent
    read(p[0], &c, 1);
    printf("%d: received pong\n", getpid());
    write(p[1], "\n", 1);
    wait(0);
  }
  exit(0);
}
