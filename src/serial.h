//
// Created by rafik on 13. 11. 2019.
//
#include <stdio.h>

#ifndef GARDENER_SERIAL_H
#define GARDENER_SERIAL_H

int init_serial_port();

int write_to_serial_port(const char data);

int close_serial_port();

#endif //GARDENER_SERIAL_H
