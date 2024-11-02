#include <cleanup.h>
#include <stopwatch.h>
#include <countdown.h>

#include <unistd.h>
#include <libnotify/notify.h>
#include <stdlib.h>
#include <signal.h>

volatile int cleanup = 0;

void sighandle(int signal) {
    printf("notificounter: exiting\n");
    cleanup = 1;
}

int main(int argc, char* argv[]) {
    long seconds = 0;
    if (argc == 2) {
        seconds = strtol(argv[1], NULL, 10);
    } else if (argc != 1) {
        printf("usage: notificounter [seconds]\ncreates a stopwatch or countdown timer using notifications. powered by libnotify.\nwith no [seconds], it will start a stopwatch.\nif [seconds] is a valid number, it will start a countdown timer.");
        return 0;
    }

    signal(SIGINT, sighandle);
    signal(SIGTERM, sighandle);
    notify_init("notificounter");

    if (seconds == 0) stopwatch();
    else countdown(seconds);

    return 0;
}