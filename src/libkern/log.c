/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/


#include <libkern/log.h>
#include <libkern/string.h>
#include <libkern/panic.h>
#include <limine.h>

static volatile struct limine_terminal_request term_req = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};


static struct limine_terminal* terminal = NULL;


static void puts(const char* str) 
{
    if (terminal == NULL) {
        terminal = term_req.response->terminals[0];
    }

    term_req.response->write(terminal, str, strlen(str));
}


void printf(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char* ptr;

    if (memcmp(fmt, KINFO, strlen(KINFO))) {
        puts(KINFO);
        fmt += strlen(KINFO);
    }

    for (ptr = fmt; *ptr != '\0'; ++ptr) {
        if (*ptr == '%') {
            ++ptr;
            switch (*ptr) {
                case 's':
                    puts(va_arg(ap, char*));
                    break;
                case 'd':
                	puts(dec2str(va_arg(ap, uint64_t)));
                	break;
                case 'x':
                	puts(hex2str(va_arg(ap, uint64_t)));
                	break;
            }
        } else {
            char terminated[2] = {*ptr, 0};
            puts(terminated);
        }
    }
}
