#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <stdio.h>

typedef enum {
    CONTAINER_INIT,
    CONTAINER_ADD,
    CONTAINER_PLAY,
    CONTAINER_STOP,
    CONTAINER_SEEK,
    CONTAINER_SEEK_ABS,
    CONTAINER_LENGTH,
    CONTAINER_DEL,
    CONTAINER_SWITCH_AUDIO,
    CONTAINER_SWITCH_SUBTITLE,
    CONTAINER_SWITCH_TELETEXT,
    CONTAINER_METADATA,
} ContainerCmd_t;

struct Player;

typedef struct Container_s {
    const char *Name;
    int (*Command) (Player *, ContainerCmd_t, const char *);
    const char **Capabilities;
} Container_t;


extern Container_t FFMPEGContainer;

typedef struct ContainerHandler_s {
    const char *Name;
    Container_t *selectedContainer;
    int (*Command) (Player *, ContainerCmd_t, const char *);
} ContainerHandler_t;

#endif
