#ifndef LIL_HELPERS_H
#define LIL_HELPERS_H

#define LIL_FAILED(l, p...) do { char* b; asprintf(&b, p); lil_set_error(l, b); } while (0)
#define streq(s1, s2) (strcmp(s1, s2) == 0)

#endif
