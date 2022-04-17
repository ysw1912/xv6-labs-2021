#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(const char* path, const char* filename) {
  int fd = open(path, 0);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }
  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    goto error;
  }
  if (st.type != T_DIR) {
    fprintf(2, "find: %s is not directory\n", path);
    goto error;
  }
  char sub_path[512];
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(sub_path)) {
    fprintf(2, "find: %s too long\n", path);
    goto error;
  }
  strcpy(sub_path, path);
  char* p = sub_path + strlen(sub_path);
  *p++ = '/';
  struct dirent de;
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
      // not recurse into "." and "..".
      continue;
    }
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if (stat(sub_path, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", sub_path);
      continue;
    }
    if (st.type == T_FILE && strcmp(de.name, filename) == 0) {
      printf("%s\n", sub_path);
    } else if (st.type == T_DIR) {
      find(sub_path, filename);
    }
  }
  close(fd);
  return;

error:
  close(fd);
  exit(1);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <dirname> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
