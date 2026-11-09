#ifndef SCU_API_H
#define SCU_API_H

#ifdef SCU_STATIC
    #define SCU_API
#elif defined(_WIN32) || defined(__CYGWIN__)
    #ifdef SCU_BUILD_SHARED
        #define SCU_API __declspec(dllexport)
    #elifdef SCU_SHARED
        #define SCU_API __declspec(dllimport)
    #else
        #error "Define SCU_STATIC, SCU_BUILD_SHARED or SCU_SHARED."
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define SCU_API __attribute__((visibility("default")))
#else
    #define SCU_API
#endif

#endif
