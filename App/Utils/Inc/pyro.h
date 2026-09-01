// Comment

#pragma once

typedef enum {
    APOGEE,
    APOGEE_BACKUP,
    MAIN,
    MAIN_BACKUP
} PyroChannel;

void init_pyro();

void check_continuity(PyroChannel channel);

void fire_pyro(PyroChannel channel);