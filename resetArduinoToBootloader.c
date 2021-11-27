#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

void usage(char **argv)
{
	printf("usage: %s <PORT>\n", argv[0]);
	exit(1);
}

int setBaudrate(int fd, int baud)
{
	struct termios tty;
	if (tcgetattr (fd, &tty) != 0)
	{
		printf("error %d from tcgetattr", errno);
		return -1;
	}

	int ret = cfsetospeed (&tty, baud);
	if (ret) {
		printf("Error setting ospeed: %s\n", strerror(errno));
		return -1;
	}

	ret = cfsetispeed (&tty, baud);
	if (ret) {
		printf("Error setting ispeed: %s\n", strerror(errno));
		return -1;
	}

	printf("setting baudrate to %i\n", baud);
	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		printf("error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}


int main(int argc, char **argv)
{
	if (argc != 2) {
		usage(argv);
	}
	char *port = argv[1];
	int fd, ret, serial;

	printf("Opening port %s\n", port);
	fd = open(port, O_RDWR | O_NOCTTY );
	if (fd == -1) {
		printf("Failure opening port %s. Error: %s\n", port, strerror(errno));
		exit(-1);
	}

	ret = setBaudrate(fd, B1200);
	if (ret) {
		printf("Error setting baudrate\n");
		exit(-1);
	}

	ret = ioctl(fd, TIOCMGET, &serial);
	if (ret == 1) {
		printf("Failure getting port information on %s: %s\n", port, strerror(errno));
		exit(-1);
	}


	// Clear DTR pin
	serial = serial & ~(TIOCM_DTR);
	ret = ioctl(fd, TIOCMSET, serial);
	if (ret == 1) {
		printf("Failure to clear DTR bit on %s: %s\n", port, strerror(errno));
		exit(-1);
	}
	close(fd);

	// Wait for 500 ms
	printf("Wait for %s to become available again...\n", port);
	usleep(500 * 1000);

	struct stat buffer;
	/* counter = 100 -> 100ms * 100 = 10s max */
	int status, found = 0, counter = 100;
	errno = 0;
	do {
		status = stat(port, &buffer);
		if (status) {
			if (errno != ENOENT) {
				printf("Error accessing %s: %s %i\n", port, strerror(errno), errno);
				exit(-1);
			} else {
				usleep(100 * 1000);
			}
		} else {
			found = 1;
			// Wait for udev to change the uid if a udev rule exists
			// This avoids root privileges for flashing
			if ( buffer.st_uid != getuid()) {
				usleep(100 * 1000);
			} else {
				break;
			}
		}
		counter--;
	} while (errno == ENOENT && counter);
	
	if (!counter) {
		// we ran in a timeout
		if (found) {
			printf("%s found again, but the user is buffer.st_uid. " \
					"This will most probably require root privleges for " \
					"flashing then.\n", port);
		} else {
			printf("%s not found after reset with uid == %u\n", port, getuid());
			exit(-1);
		}
	}


	printf("%s is available in bootloader mode!\n", port);
	return 0;
}
