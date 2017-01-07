/*
 * Copyright (c) Siemens AG, 2016
 *
 * Authors:
 *  Sascha Weisenberger <sascha.weisenberger@siemens.com>.
 *
 * This file is subject to the terms and conditions of the MIT License.  See
 * COPYING.MIT file in the top-level directory.
 */

#include <linux/serial.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void printUsage(char * name)
{
	printf("Usage:\n");
	printf("%s device mode\n", name);
	printf("device\t\t\tThe device you for which you want to switch the mode (e.g. /dev/ttyS2)\n");
	printf("mode\t\t\tThe mode you want to use (rs232, rs485, rs422)\n");
	printf("\n");
	printf("example: %s /dev/ttyS2 rs232", name);

}

int main(int argc, char** argv)
{
	char* device;
	char* mode;
	struct serial_rs485 rs485conf;
	int file;

	device = argv[1];
	mode = argv[2];
	rs485conf.flags = 0;

	if(3 != argc)
	{
		printUsage(argv[0]);
		return -EINVAL;
	}

	file = open(device, O_RDWR);
	if (file < 0)
	{
		perror("Error");
		return -errno;
	}

	if(0 == strcasecmp("rs232", mode))
		rs485conf.flags |= SER_RS485_RX_DURING_TX;
	if(0 == strcasecmp("rs485", mode))
		rs485conf.flags |= SER_RS485_ENABLED;
	if(0 == strcasecmp("rs422", mode))
		rs485conf.flags |= (SER_RS485_ENABLED | SER_RS485_RX_DURING_TX);

	if(0 == rs485conf.flags)
	{
		fprintf(stderr, "No valid mode \"%s\"", mode);
		close(file);
		return -EINVAL;
	}

	if (ioctl (file, TIOCSRS485, &rs485conf) < 0)
	{
		perror("Error");
		close(file);
		return -errno;
	}

	if (close (file) < 0)
	{
		perror("Error");
		return -errno;
	}

	printf("Successfully set %s to %s\n", device, mode);
	return 0;
}
