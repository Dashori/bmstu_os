#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void print_page(uint64_t address, uint64_t data, FILE *out);
void get_pagemap_info(const char *proc, FILE *out);
void get_maps_info(const char *proc, FILE *out);
void get_stat_info(const char *proc, FILE *out);
void get_task_info(const char *proc, FILE *out);

#endif
