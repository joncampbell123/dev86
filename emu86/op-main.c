
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "op-class.h"
#include "emu-mem-io.h"
#include "emu-serial.h"
#include "emu-proc.h"
#include "op-exec.h"


static int file_load (addr_t start, char * path)
	{
	int err = -1;
	int fd = -1;

	while (1)
		{
		// Load binary image file

		fd = open (path, O_RDONLY);
		if (fd < 0)
			{
			puts ("fatal: cannot open file");
			break;
			}

		off_t size = lseek (fd, 0, SEEK_END);
		if (size <= 0)
			{
			puts ("fatal: empty file");
			break;
			}

		printf ("info: file size=%lXh\n", size);
		if (start + size > MEM_MAX)
			{
			puts ("fatal: file too big");
			break;
			}

		lseek (fd, 0, SEEK_SET);

		byte_t * buf = mem_get_addr (start);
		off_t count = read (fd, buf, size);
		if (count != size)
			{
			puts ("fatal: incomplete file read");
			break;
			}

		puts ("success: file loaded");
		err = 0;
		break;
		}

	// Cleanup

	if (fd >= 0)
		{
		close (fd);
		fd = -1;
		}

	return err;
	}


// Program main

int main (int argc, char * argv [])
	{
	int exit_code = 0;

	while (1)
		{
		mem_io_reset ();
		proc_reset ();

		// Command line processing

		char * file_path = NULL;
		addr_t file_address = -1;
		int file_loaded = 0;

		op_code_seg = seg_get (SEG_CS);
		op_code_off = reg16_get (REG_IP);

		addr_t breakpoint = -1;

		int flag_trace = 0;
		int flag_prompt = 0;

		char opt;

		while (1)
			{
			opt = getopt (argc, argv, "w:f:x:b:ti");
			if (opt < 0 || opt == '?') break;

			switch (opt)
				{
				case 'w':  // load address
					if (sscanf (optarg, "%lx", &file_address) != 1)
						{
						puts ("error: bad load address");
						}
					else
						{
						printf ("info: load address %.5lXh\n", file_address);
						}

					break;

				case 'f':  // file path
					file_path = optarg;
					printf ("info: load file %s\n", file_path);
					break;

				case 'x':  // execute address
					if (sscanf (optarg, "%hx:%hx", &op_code_seg, &op_code_off) != 2)
						{
						puts ("error: bad execute address");
						}
					else
						{
						printf ("info: execute address %.4hXh:%.4hXh\n", op_code_seg, op_code_off);
						}

					break;

				case 'b':  // breakpoint address
					if (sscanf (optarg, "%lx", &breakpoint) != 1)
						{
						puts ("error: bad breakpoint address");
						}
					else
						{
						printf ("info: breakpoint address %.5lXh\n", breakpoint);
						}

					break;

				case 't':  // trace mode
					flag_trace = 1;
					break;

				case 'i':  // interactive mode
					flag_trace = 1;
					flag_prompt = 1;
					break;

				}

			if (file_address != -1 && file_path)
				{
				if (!file_load (file_address, file_path))
					{
					file_loaded = 1;
					}

				file_path = NULL;
				file_address = -1;
				}
			}

		if (opt == '?' || optind != argc || !file_loaded)
			{
			printf ("usage: %s [options]\n\n", argv [0]);
			puts ("  -w <address>         load address (mandatory)");
			puts ("  -f <path>            file path (mandatory)");
			puts ("  -x <segment:offset>  execute address");
			puts ("  -b <address>         breakpoint address");
			puts ("  -t                   trace mode");
			puts ("  -i                   interactive mode");
			exit_code = 1;
			break;
			}

		// Main loop

		serial_init ();

		op_code_base = mem_get_addr (0);

		seg_set (SEG_CS, op_code_seg);
		reg16_set (REG_IP, op_code_off);

		op_desc_t desc;

		word_t last_seg = 0xFFFF;
		word_t last_off_0 = 0xFFFF;
		word_t last_off_1;

		int flag_exit = 0;

		while (!flag_exit)
			{
			// Decode next instruction

			op_code_seg = seg_get (SEG_CS);
			op_code_off = reg16_get (REG_IP);

			// Breakpoint test

			if (addr_seg_off (op_code_seg, op_code_off) == breakpoint)
				{
				puts ("info: breakpoint hit");
				flag_trace = 1;
				flag_prompt = 1;
				}

			int err;
			int flag_exec = 1;

			// Optimize: no twice decoding of the same instruction
			// Example: REPeated or LOOP on itself

			if (op_code_seg != last_seg || op_code_off != last_off_0)
				{
				last_seg = op_code_seg;
				last_off_0 = op_code_off;

				memset (&desc, 0, sizeof desc);

				err = op_decode (&desc);
				if (err)
					{
					puts ("\nerror: unknown opcode");
					flag_trace = 1;
					flag_prompt = 1;
					flag_exec = 0;
					}

				last_off_1 = op_code_off;
				}
			else
				{
				op_code_off = last_off_1;
				}

			if (flag_trace)
				{
				// Print processor status

				putchar ('\n');
				regs_print ();
				putchar ('\n');

				printf ("%.4hX:%.4hX  ", seg_get (SEG_CS), reg16_get (REG_IP));
				print_column (op_code_str, 3 * OPCODE_MAX + 1);
				op_print (&desc);
				puts ("\n");
				}

			// User prompt

			if (flag_prompt)
				{
				// Get user command
				// Ugly but temporary

				char com [8];
				putchar ('>');
				gets (com);

				switch (com [0])
					{
					// Dump stack

					case 's':
						putchar ('\n');
						stack_print ();
						flag_exec = 0;
						break;

					// Step over

					case 'p':
						breakpoint = addr_seg_off (op_code_seg, op_code_off);
						flag_trace = 0;
						flag_prompt = 0;
						break;

					// Go (keep breakpoint)

					case 'g':
						flag_trace = 0;
						flag_prompt = 0;
						break;

					// Quit

					case 'q':
						flag_exec = 0;
						flag_exit = 1;
						break;
					}
				}

			// Execute operation

			if (flag_exec)
				{
				reg16_set (REG_IP, op_code_off);

				err = op_exec (&desc);
				if (err)
					{
					putchar ('\n');
					puts ("fatal: execute operation");
					break;
					}

				if (rep_stat ())
					{
					reg16_set (REG_IP, last_off_0);
					}
				else
					{
					seg_reset ();
					}
				}
			}

		break;
		}

	// Cleanup

	serial_term ();

	return exit_code;
	}
