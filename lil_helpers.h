#ifndef LIL_HELPERS_H
#define LIL_HELPERS_H

#define LIL_FAILED(l, p...) do { char* b; asprintf(&b, p); lil_set_error(l, b); } while (0)

#define streq(s1, s2) (strcmp(s1, s2) == 0)

#define LIL_FIXARITY(l, p, a, n) do { if (a != n) { LIL_FAILED(l, "Expected %i args to " p ", got %i", n, a); return NULL; } } while (0)

#define LIL_CHECKARGS(l, p, a, lo, hi) do { if (a < lo || a > hi) { LIL_FAILED(l, "Expected %i-%i args to " p ", got %i", lo, hi, a); return NULL; } } while (0)

#endif
