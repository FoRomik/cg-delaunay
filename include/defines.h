#define DEBUG 0
// if debug set to 1, then not_bull enables exiting
#if DEBUG
#define NOT_NULL(X) assert(X != 0)
#else
#define NOT_NULL(X)
#endif //DEBUG
