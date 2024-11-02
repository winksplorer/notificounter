#include <cleanup.h>
#include <stopwatch.h>

#include <bits/time.h>
#include <time.h>
#include <unistd.h>
#include <libnotify/notify.h>
#include <pthread.h>

char* sw_s;

void* swThread(void* vargp) {
    struct timespec start, current;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (;;) {
        clock_gettime(CLOCK_MONOTONIC, &current);
        long e = (current.tv_sec - start.tv_sec) * 1000 + (current.tv_nsec - start.tv_nsec) / 1000000;

        snprintf(sw_s, 128, "%02ld:%02ld:%02ld.%03ld", (e / 3600000), (e / 60000) % 60, (e / 1000) % 60, e % 1000);
        usleep(5000);
    }
    return NULL;
}

void stopwatch(void) {
    sw_s = malloc(128);

    snprintf(sw_s, 128, "00:00:00.000");

    NotifyNotification* n = notify_notification_new("stopwatch", sw_s, "dialog-information");
    notify_notification_show(n, NULL);

    pthread_t t;
    pthread_create(&t, NULL, swThread, NULL);

    while (!cleanup) {
        notify_notification_update(n, "stopwatch", sw_s, "dialog-information");
        notify_notification_show(n, NULL);
        usleep(50000);
    }

    g_object_unref(G_OBJECT(n));
    notify_uninit();
    return;
}