/**
 * Macros for marking functions as deprecated, and suggesting
 * alternatives
 */
#ifndef XM_DEPRECATED_H
#define XM_DEPRECATED_H

/**
 * Try for C23's [[deprecated]], falling back to good ol' __attribute__,
 * assuming we're building with GCC 4.x or newer.
 *
 * FWIW, the original XM_DEPRECATED macro required GCC >= 3.
 */
#if __STDC_VERSION__ >= 202311L || (defined(__cplusplus) && __cplusplus >= 201402L)
#define XM_DEPRECATED       [[deprecated]]
#define XM_ALTERNATIVE(...) [[deprecated(#__VA_ARGS__)]]
#elif defined(__GNUC__) || defined(__clang__)
#define XM_DEPRECATED       __attribute__((deprecated))
#define XM_ALTERNATIVE(...) __attribute__((deprecated(#__VA_ARGS__)))
#else
#define XM_DEPRECATED
#define XM_ALTERNATIVE(...)
#endif

#endif /* XM_DEPRECATED_H */
