/*
 *  MIT License
 *
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */


#ifndef MODULE_H
#define MODULE_H

#include <stdint.h>
#include <limine.h>

/*
 *  -- Limine file --
 *
 *  struct limine_file {
 *       uint64_t revision;
 *       void *address;
 *       uint64_t size;
 *       char *path;
 *       char *cmdline;
 *       uint32_t media_type;
 *       uint32_t unused;
 *       uint32_t tftp_ip;
 *       uint32_t tftp_port;
 *       uint32_t partition_index;
 *       uint32_t mbr_disk_id;
 *       struct limine_uuid gpt_disk_uuid;
 *       struct limine_uuid gpt_part_uuid;
 *       struct limine_uuid part_uuid;
 *   };
 *
 *
 */

uint64_t get_module_count(void);

/*
 *  Returns pointer to file if found,
 *  otherwise NULL.
 *
 */

struct limine_file* get_module_by_name(const char* name);

#endif
