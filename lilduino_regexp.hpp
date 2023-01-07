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
    LIL_CHECKARGS(lil, "regmatch", argc, 2, MAXCAPTURES + 2);
    const char* re = lil_to_string(argv[0]);
    const char* str = lil_to_string(argv[1]);
    MatchState matcher((char*)str);
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
            if (msz == CAP_POSITION) {
                lil_set_var(lil, lil_to_string(argv[i + 2]), lil_alloc_integer(matcher.capture[i].init - matcher.src), LIL_SETVAR_LOCAL_ONLY);
                continue;
            }
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
    LIL_CHECKARGS(lil, "regsub", argc, 3, 4);
    const char* re = lil_to_string(argv[0]);
    const char* str = lil_to_string(argv[1]);
    const char* repl = lil_to_string(argv[2]);
    int maxrepl = 0;
    if (argc == 4) maxrepl = lil_to_integer(argv[4]);
    MatchState matcher((char*)str);
    matcher.GlobalReplace(re, repl, maxrepl);
    if (matcher.GetResult() < 0) {
        LIL_FAILED(lil, "Regular expression error %i", matcher.GetResult());
        return NULL;
    }
    return lil_alloc_string(matcher.src);
}

void lilduino_regexp_init(lil_t lil) {
    lil_register(lil, "regmatch", (lil_func_proc_t)fnc_re_match);
    lil_register(lil, "regsub", (lil_func_proc_t)fnc_re_replace);
}

#endif
