// Comment

#pragma once

#include <stdbool.h>

typedef enum {
    APOGEE,
    APOGEE_BACKUP,
    MAIN,
    MAIN_BACKUP
} PyroChannel;

void init_pyro();

bool check_continuity(PyroChannel channel);

void fire_pyro(PyroChannel channel);