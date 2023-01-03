// Dependency: https://github.com/nickgammon/Regexp

#ifndef LILDUINO_REGEXP_FILE
#define LILDUINO_REGEXP_FILE

#include <Arduino.h>
#include <Regexp.h>
#include <stdlib.h>

extern "C" {
    #include "lil.h"
    #include "lil_helpers.h"
}

lil_value_t fnc_re_match(lil_t lil, int argc, lil_value_t* argv) {
    LIL_CHECKARGS(lil, "re::match", argc, 2, MAXCAPTURES + 2);
    const char* re = lil_to_string(argv[0]);
    const char* str = lil_to_string(argv[1]);
    MatchState matcher(str);
    char result = matcher.Match(re);
    if (result == REGEXP_NOMATCH) {
        return NULL;
    }
    else if (result == REGEXP_MATCHED) {
        // Assign any variables provided
        int numvars = argc - 2;
        int msz;
        char* mbuf = NULL;
        for (int i = 0; i < numvars; i++) {
            msz = matcher.capture[i].len;
            mbuf = (char*)realloc((void*)mbuf, msz + 1);
            matcher.GetCapture(mbuf, i);
            lil_set_var(lil, lil_to_string(argv[i + 2]), lil_alloc_string(mbuf), LIL_SETVAR_LOCAL_ONLY);
        }
        msz = matcher.MatchLength;
        mbuf = (char*)realloc((void*)mbuf, msz + 1);
        matcher.GetMatch(mbuf);
        lil_value_t r = lil_alloc_string(mbuf);
        free(mbuf);
        return r; // Match successful
    }
    else {
        LIL_FAILED(lil, "Regular expression error %i", result);
        return NULL;
    }
}

lil_value_t fnc_re_replace(lil_t lil, int argc, lil_value_t* argv) {
    LIL_CHECKARGS(lil, "re::replace", argc, 3, 4);
    const char* re = lil_to_string(argv[0]);
    const char* str = lil_to_string(argv[1]);
    const char* repl = lil_to_string(argv[2]);
    int maxrepl = 0;
    if (argc == 4) maxrepl = lil_to_integer(argv[4]);
    MatchState matcher(str);
    matcher.GlobalReplace(re, repl, maxrepl);
    if (matcher.result < 0) {
        LIL_FAILED(lil, "Regular expression error %i", matcher.result);
        return NULL;
    }
    return lil_alloc_string(matcher.src);
}

void lilduino_regexp_init(lil_t lil) {
    lil_register(lil, "re::match", (lil_func_proc_t)fnc_re_match);
    lil_register(lil, "re::replace", (lil_func_proc_t)fnc_re_replace);
}

#endif
