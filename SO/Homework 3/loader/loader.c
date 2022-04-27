/*
 * Loader Implementation
 *
 * 2018, Operating Systems
 */

#include "exec_parser.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

static so_exec_t *exec;
static int exec_descriptor;
static int size_page;

static struct so_seg *search_for_segment(uintptr_t address) {
	struct so_seg *seg = NULL;
	int i = 0;

	while (i < exec->segments_no) {
		struct so_seg *curr_segm = &exec->segments[i];
		// find the segment that contains the address
		if (address >= curr_segm->vaddr) {
			// found the segment we want to load
			if (address < curr_segm->vaddr + curr_segm->mem_size) {
				seg = curr_segm;
				break;
			}
		}
		i++;
	}
	return seg;
}

static void sigsegv_check(int signum, siginfo_t *siginfo, void *sigtext) {
	// define variables for the pagefault handler
	struct so_seg *seg = search_for_segment((uintptr_t)siginfo->si_addr);
	struct sigaction handler;
	uintptr_t si_address = (uintptr_t)siginfo->si_addr;
	uintptr_t vaddress = (uintptr_t)seg->vaddr;
	uintptr_t file_size = (uintptr_t)seg->file_size;
	int flag = MAP_PRIVATE | MAP_FIXED;
	int num_page = (si_address - vaddress) / size_page;
	char *data = (char *)seg->data;
	int check_map = 0;

	// check if the page was mapped and deny the access
	if (data[num_page] == 1) {
		handler.sa_sigaction(signum, siginfo, sigtext);
		return;
	}
	// check if the page is in the file
	if (file_size < seg->mem_size) {
		if (file_size < num_page * size_page) {
			// use MAP_ANONYMOUS
			check_map = num_page * size_page - file_size;
			flag |= MAP_ANONYMOUS;
		} else if ((num_page + 1) * size_page > file_size) {
			// file exceeded the size of the segment
			check_map = (num_page + 1) * size_page - file_size;
		}
	}
	// use mmap to map the page
	char *mapp =
			mmap((void *)vaddress + num_page * size_page, size_page, seg->perm,
					 flag, exec_descriptor, seg->offset + num_page * size_page);

	if (check_map != 0) {
		// use memset to fill the page with 0
		uintptr_t diff = vaddress + num_page * size_page + (size_page - check_map);
		memset((char *)diff, 0, check_map);
	} else {
		// set page as mapped and set the data to 1
		data[num_page] = 1;
	}
}

int so_init_loader(void) {
	// get the page size
	struct sigaction handler;
	size_page = getpagesize();
	// open the executable file
	handler.sa_sigaction = sigsegv_check;
	sigemptyset(&handler.sa_mask);
	sigaddset(&handler.sa_mask, SIGSEGV);
	handler.sa_flags = SA_SIGINFO;
	// set the signal handler

	if (sigaction(SIGSEGV, &handler, NULL) == -1)
		return -1;

	return -1;
}

int so_execute(char *path, char *argv[]) {
	// open the file
	exec_descriptor = open(path, O_RDONLY);
	exec = so_parse_exec(path);
	int i = 0;

	while (i < exec->segments_no) {
		// keep the data of the segment
		// we will use it to check if the page was mapped
		exec->segments[i].data =
				calloc(exec->segments[i].mem_size / size_page, sizeof(char));
		if (!exec->segments[i].data)
			exit(-ENOMEM);
		i++;
	}

	/* Start execution: */
	so_start_exec(exec, argv);
	// free the data of the segments
	free(exec);
	free(exec->segments);
	return -1;
}
