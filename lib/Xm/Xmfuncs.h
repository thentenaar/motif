#include <X11/Xfuncs.h>

/**
 * This is just a copy of Xfuncs that (for some reason) gets installed
 * alongside Motif.
 *
 * Since this was a public header, complain until we nuke it.
 */
#if defined(__GNUC__) || defined(__clang__)
#warn "X11/Xfuncs.h should be used instead of Xmfuncs.h (will be removed in Motif 3.0)"
#endif

