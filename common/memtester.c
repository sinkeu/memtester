/*
 * memtester version 4
 *
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2012 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#include <common.h>
#include <command.h>

#include <memtester/types.h>
#include <memtester/sizes.h>
#include <memtester/tests.h>

static struct test tests[] = {
    { "Random Value", test_random_value },
    { "Compare XOR", test_xor_comparison },
    { "Compare SUB", test_sub_comparison },
    { "Compare MUL", test_mul_comparison },
    { "Compare DIV",test_div_comparison },
    { "Compare OR", test_or_comparison },
    { "Compare AND", test_and_comparison },
    { "Sequential Increment", test_seqinc_comparison },
    { "Solid Bits", test_solidbits_comparison },
    { "Block Sequential", test_blockseq_comparison },
    { "Checkerboard", test_checkerboard_comparison },
    { "Bit Spread", test_bitspread_comparison },
    { "Bit Flip", test_bitflip_comparison },
    { "Walking Ones", test_walkbits1_comparison },
    { "Walking Zeroes", test_walkbits0_comparison },
    { "8-bit Writes", test_8bit_wide_random },
    { "16-bit Writes", test_16bit_wide_random },
    { NULL, NULL }
};

/* Function declarations */
static int do_memtester(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    ul loops, loop, i;
    size_t bufsize, wantraw, wantbytes, wantmb, halflen, count;
	char *memsuffix;
	int memshift;
    ulv *bufa, *bufb;

	if(argc != 4) {
		printf("Usage: <address> <size> <loops>\n");
		return 0;
	}

	wantraw = (size_t) simple_strtoul(argv[2], &memsuffix, 0);
    switch (*memsuffix) {
        case 'G':
        case 'g':
            memshift = 30; /* gigabytes */
            break;
        case 'M':
        case 'm':
            memshift = 20; /* megabytes */
            break;
        case 'K':
        case 'k':
            memshift = 10; /* kilobytes */
            break;
        case 'B':
        case 'b':
            memshift = 0; /* bytes*/
            break;
        case '\0':  /* no suffix */
            memshift = 20; /* megabytes */
            break;
        default:
			printf("Usage: <address> <size> <loops>\n");
    }

	wantbytes = ((size_t) wantraw << memshift);
	wantmb = (wantbytes >> 20);
	halflen = wantbytes / 2;
	count = halflen / sizeof(ul);
	bufa = simple_strtoul(argv[1], NULL, 16);
    bufb = (ulv *) ((size_t) bufa + halflen);
	loops = simple_strtoul(argv[3], NULL, 0);

	printf("want %lluMB (%llu bytes)\n", (ull) wantmb, (ull) wantbytes);
	printf("bufa 0x%x, bufb 0x%x, loops %d, count %d\n", bufa, bufb, loops, count);

    for(loop=1; loop <= loops; loop++) {
        printf("Loop %lu", loop);
        if (loops) {
            printf("/%lu", loops);
        }
        printf(":\n");
        printf("  %-20s: ", "Stuck Address");
        if (!test_stuck_address(bufa, wantbytes/sizeof(ul))) {
             printf("ok\n");
        } else {
             printf("bad\n");
        }
        for (i=0;;i++) {
            if (!tests[i].name) break;

            printf("  %-20s: ", tests[i].name);

            if (!tests[i].fp(bufa, bufb, count)) {
                printf("ok\n");
            } else {
            	printf("bad\n");
            }
        }
        printf("\n");
    }
    printf("Done.\n");
	return 0;
}

U_BOOT_CMD(
	mt,	4,	1,	do_memtester,
		"memory tester",
    	"usage: <address> <size> <loops>"
);
