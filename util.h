// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __UTIL_H_
#define __UTIL_H_


template <typename T>
static void dealloc(T& p) {
    if (p) {
        free(p);
        p = NULL;
    }
}


template <typename T, typename F>
static void dealloc(T& p, F callee) {
    if (p) {
        callee(&p);
        p = NULL;
    }
}


#endif // __UTIL_H_
