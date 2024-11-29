#include "cleanup.h"
#include "countdown.h"

#include <bits/time.h>
#include <time.h>
#include <unistd.h>
#include <libnotify/notify.h>
#include <pthread.h>

char* cd_s;
volatile int finished = 0;

void* cdThread(void* vargp) {
    struct timespec start, current;
    clock_gettime(CLOCK_MONOTONIC, &start);

    long ms = (long)vargp * 1000;

    for (;;) {
        clock_gettime(CLOCK_MONOTONIC, &current);
        long e = (current.tv_sec - start.tv_sec) * 1000 + (current.tv_nsec - start.tv_nsec) / 1000000;
        long r = ms - e;

        if (r <= 0) {
            cleanup = 1;
            finished = 1;
            return NULL;
        }

        snprintf(cd_s, 128, "%02ld.%03ld", r / 1000, r % 1000);
        usleep(5000);
    }
    return NULL;
}

void countdown(long seconds) {
    cd_s = malloc(128);

    snprintf(cd_s, 128, "%02ld.000", seconds);

    NotifyNotification* n = notify_notification_new("countdown", cd_s, "dialog-information");
    notify_notification_show(n, NULL);

    pthread_t t;

    pthread_create(&t, NULL, cdThread, (void*)seconds);

    while (!cleanup) {
        notify_notification_update(n, "countdown", cd_s, "dialog-information");
        notify_notification_show(n, NULL);

        usleep(50000);
    }

    if (finished) {
        notify_notification_update(n, "countdown", "00.000", "dialog-information");
        notify_notification_show(n, NULL);
    }

    g_object_unref(G_OBJECT(n));
    notify_uninit();

    return;
}