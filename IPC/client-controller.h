#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#ifdef __cplusplus
extern "C" {
    #endif
    #if defined (__STDC__) || defined(__cplusplus)
        extern void relay(const char*);
    #else
        extern void relay();
    #endif

    #ifdef __cplusplus
}
#endif

#endif