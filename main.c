#include <stdio.h>
#include <X11/Xlib.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include <dirent.h>
#include <regex.h>

struct event_device {
    char *device;
    int fd;
};

#define die(str, args...) do { \
        perror(str); \
        exit(EXIT_FAILURE); \
    } while(0)

int main(int argc, char *argv[]) {
    struct input_event ev[64];
    int numevents;
    int result = 0;
    int size = sizeof(struct input_event);
    int rd;
    char name[256];
    char *device = NULL;
    struct event_device evdev;
    fd_set fds;
    int maxfd;

    DIR *dirp;
    static char *dirName = "/dev/input";
    char fullPath[1024];
    struct dirent *dp;
    regex_t kbd;

    if ((dirp = opendir(dirName)) == NULL) {
        die("couldn't open");
    }

    if (regcomp(&kbd, "event", 0) != 0) {
        printf("regcomp for kbd failed\n");
        die("");
    }

    do {
        if ((dp = readdir(dirp)) != NULL) {
            if (regexec(&kbd, dp->d_name, 0, NULL, 0) == 0) {
//                printf("match for the event = %s\n", dp->d_name);
                sprintf(fullPath, "%s/%s", dirName, dp->d_name);
                int tmp = open(fullPath, O_RDONLY | O_NONBLOCK);
                char tmpName[256];
                memset(tmpName, 0, sizeof(tmpName));
                ioctl(tmp, EVIOCGNAME(sizeof(tmpName)), tmpName);
                printf("%s\n", tmpName);
                if (strcmp(tmpName, "tahometer-keyhandler") == 0) {
                    device = fullPath;
                    break;
                } else
                    die("cant find taho");
            }
        }
    } while (dp != NULL);

    evdev.device = device;
    evdev.fd = open(evdev.device, O_RDONLY | O_NONBLOCK);
    if (evdev.fd == -1) {
        die("Failed to open event device");
    }

    memset(name, 0, sizeof(name));
    ioctl(evdev.fd, EVIOCGNAME(sizeof(name)), name);
    printf("Reading From : %s (%s)\n", evdev.device, name);

    while (1) {
        FD_ZERO(&fds);
        maxfd = -1;

        FD_SET(evdev.fd, &fds);
        if (maxfd < evdev.fd) maxfd = evdev.fd;

        result = select(maxfd + 1, &fds, NULL, NULL, NULL);
        if (result == -1) {
            die("");
        }


        if (!FD_ISSET(evdev.fd, &fds)) {
            die("");
        }

        if ((rd = read(evdev.fd, ev, size * 64)) < size) {
            die("");
        }

        numevents = rd / size;
        for (int j = 0; j < numevents; ++j) {
            if (ev[j].value > 0) {
                printf("%s: Type[%d] Code[%d] Value[%d]\n", evdev.device, ev[j].type, ev[j].code, ev[j].value);
            }
        }

    }
//    while (1) {
//        if (read(evdev.fd, &ev, sizeof(struct input_event)) != -1) {
//            FD_ZERO(&fds);
//            maxfd = -1;
//
//            FD_SET(evdev.fd, &fds);
//            if (maxfd < evdev.fd) maxfd = evdev.fd;
//
//            result = select(maxfd + 1, &fds, NULL, NULL, NULL);
//            if (result == -1) {
//                die("");
//            }
//
//
//            if (!FD_ISSET(evdev.fd, &fds)) {
//                die("");
//            }
//
//            if ((rd = read(evdev.fd, ev, size * 64)) < size) {
//                die("");
//            }
//
//            numevents = rd / size;
//            for (int j = 0; j < numevents; ++j) {
//                printf("%s: Type[%d] Code[%d] Value[%d]\n", evdev.device, ev[j].type, ev[j].code, ev[j].value);
//            }
//        }
//    }

    printf("Exiting.\n");

    ioctl(evdev.fd, EVIOCGRAB, 0);
    close(evdev.fd);

    return 0;
}
