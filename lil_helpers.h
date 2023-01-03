#ifndef LIL_HELPERS_H
#define LIL_HELPERS_H

#define LIL_FAILED(lil, printf_args...) do { char* temp_buf_; asprintf(&temp_buf_, printf_args); lil_set_error(lil, temp_buf_); } while (0)

#define streq(s1, s2) (strcmp(s1, s2) == 0)

#define LIL_FIXARITY(lil, cmd, argc, numargs) do { if (argc != numargs) { LIL_FAILED(lil, "Expected %i args to " cmd ", got %i", numargs, argc); return NULL; } } while (0)

#define LIL_CHECKARGS(lil, cmd, argc, lo, hi) do { if (argc < lo || argc > hi) { LIL_FAILED(lil, "Expected %i-%i args to " cmd ", got %i", lo, hi, a); return NULL; } } while (0)

#define LIL_NOARGS(lil, cmd, argc) do { if (argc != 0) { LIL_FAILED(l, cmd " takes no arguments (got %i)", argc); return NULL; } } while (0)

#define LIL_HASSUBCOMMANDS(lil, topcmd, argc) do { if (argc == 0) { LIL_FAILED(lil, topcmd " needs a subcommand (got nothing)"); return NULL; } } while (0)

#define LIL_SUBCOMMAND(lil, topcmd, subcmd, argc, argv, subcmd_func) if (streq(lil_to_string(argv[0]), subcmd)) return subcmd_func(lil, argc - 1, &argv[1]);

#endif
