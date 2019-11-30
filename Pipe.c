#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>

#define K 1024

// Create a pipe between parent and child process

void parent(int p[2], int fd, pid_t pid) {
  int r;
  char buf[K];
  int status;

  close(p[0]);
  while ((r = read(fd, buf, K)) > 0) {
   write(p[1], buf, r);
  }
  
  close(p[1]);
  while (waitpid(pid, &status, 0) < 0)
	  perror("wait");
  printf("Exit status of child: %d\n", WEXITSTATUS(status));
 

}

void child(int p[2]) {

 close(p[1]);

 int f = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

 dup2(f, STDOUT_FILENO);
 dup2(p[0], STDIN_FILENO);

 
 close(f);

 execve("/usr/games/ppt", (char *[]){"ppt", NULL}, NULL);
 
 fflush(stdout);
 
 perror("execute");
 close(p[0]);

 exit(1);
}


int main(int argc, char *argv[]) {

  int pfd[2];

  int fd = open(argv[1], O_RDONLY);

  if (pipe(pfd) < 0) {
    perror("pipe");
    return -1;
  }

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    return -1;
  }

  if (pid > 0) parent(pfd, fd, pid);
  else child(pfd);
	  
  return 0;


}
