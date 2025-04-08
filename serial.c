#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/ioctl.h>

int setup_serial_port(const char *port_name) {
    int fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        perror("Unable to open serial port");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    options.c_iflag = IGNBRK;
    cfsetispeed(&options, B1152000);
    cfsetospeed(&options, B1152000);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

int write_serial_port(int fd, const char *data, size_t size) {
    return write(fd, data, size);
}

int main(int argc, char **argv) {
    int ret = -1;
    int len = 0;
    int bytes_read = 0;
    struct pollfd fds;
    fds.events = POLLIN;


    if (argc < 2) {
        fprintf(stderr, "Usage: %s <serial_port>\n", argv[0]);
        return 1;
    }

    fds.fd = setup_serial_port(argv[1]);
    if (fds.fd == -1) {
        return 1;
    }

    char buffer[256];
    char buffer_sending[256];
    uint8_t ch;
    while (1) 
    {
        ret = poll(&fds, 1, 1000); // 1-second timeout
        if (ret > 0) 
        {
            if (fds.revents & POLLIN) {
                if(ioctl(fds.fd, FIONREAD, &bytes_read) == -1) {
                    printf("Error getting bytes available");
                    continue;;
                }
                printf("Bytes available: %d\n\r", bytes_read);
                len = read(fds.fd, buffer, bytes_read);

                if (len > 0) {
                    buffer[len] = '\0'; // Null-terminate the string
                    printf("Received: %s %d\r\n", buffer, len);
                    memset(buffer, 0, sizeof(buffer)); // Clear buffer
                    len = 0; // Reset length for next read
                } else if (len == 0) {
                    printf("No data received\n\r");
                    len = 0; // Reset length for next read
                    continue;
                } else {
                    printf("Error reading from serial port\n\r");
                    len = 0; // Reset length for next read
                    continue;
                }
            }
        }else if (ret == 0) {
            printf("Timeout waiting for data...\n\r");
            len = 0; // Reset length for next read
        }
    }

    close(fds.fd);
    return 0;
}
