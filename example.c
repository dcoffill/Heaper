#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
	int fd = open("/dev/heaper", O_RDWR);
	if (fd == -1) {
		perror("An error occurred");
		close(fd);
		return -1;
	}

	printf("File descriptor: %d\n", fd);
	pid_t pid = fork();
	if (pid == 0) {
		printf("I'm the child!\n");
		char buf[1024];
		int i;
		for (i = 0; i < 4; ++i) {
			read(fd, &buf, 1024);
			printf("Recieved '%s'\n", &buf);
		}
		close(fd);
		printf("Child exiting\n");
		return 0;
	} else {
		printf("I'm the parent!\n");
		char* strings[] = {"test 123", "very very very long test", "even longer test", "longer test" };
		int i;
		for (i = 0; i < 4; ++i) {
			write(fd, strings[i], strlen(strings[i]) + 1);
			printf("Wrote '%s'\n", strings[i]);
		}
		close(fd);
		int status;
		wait(&status);
		printf("Parent exiting\n");
		return 0;
	}
}
