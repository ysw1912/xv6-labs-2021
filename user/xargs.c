#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void exec_line(char* path, char** argv) {
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork\n");
    exit(1);
  }
  if (pid > 0) {  // parent
    wait(0);
  } else {  // child
    exit(exec(path, argv));
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(2, "Usage: xargs ...\n");
    exit(1);
  }
  int exec_argc = 0;
  char* exec_argv[MAXARG];
  for (int i = 1; i < argc; i++) {
    const char* arg = argv[i];
    uint size = strlen(arg) + 1;
    char* exec_arg = malloc(size);
    memcpy(exec_arg, arg, size - 1);
    exec_arg[size] = '\0';
    exec_argv[exec_argc++] = exec_arg;
  }

  char c;
  char stdin[1024];
  int len = 0;
  while (read(0, &c, 1)) {
    if (c != '\n') {
      stdin[len++] = c;
    } else {
      stdin[len] = '\0';
      exec_argv[exec_argc] = stdin;
      exec_argv[exec_argc + 1] = 0;
      exec_line(argv[1], exec_argv);
      len = 0;
    }
  }
  exit(0);
}
