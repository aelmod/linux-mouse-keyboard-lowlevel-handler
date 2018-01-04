#include <stdio.h>
#include <X11/Xlib.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#define MOUSEFILE "/dev/input/mouse0\0"

int main() {
    int fd;
    struct input_event ie;
    unsigned char *ptr = (unsigned char *) &ie;

    unsigned char button, bLeft, bMiddle, bRight, vScroll, hScroll;

    if ((fd = open(MOUSEFILE, O_RDONLY | O_NONBLOCK)) == -1) {
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (read(fd, &ie, sizeof(struct input_event)) != -1) {
            button = ptr[0];
            bLeft = button & 0x1;
            bMiddle = (button & 0x4) > 0;
            bRight = (button & 0x2) > 0;

            if (bLeft) printf("\nleft\n");
            if (bRight) printf("\nRight \n");
            if (bMiddle) printf("\nMiddle \n");

            fflush(stdout);
        }
    }

    close(fd);
    return 0;
}