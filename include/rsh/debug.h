#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#if defined( RSHDEBUG )

#define debugf( ... ) \
    do { \
        fprintf( stderr, "%s/%s():%d: ", __FILE__, __func__, __LINE__ ); \
        fprintf( stderr, __VA_ARGS__ ); \
    } while(0)

#else
#define debugf( ... )

#endif /* defined( RSHDEBUG ) */

// useful macros
#define stringify( x ) #x


#endif /* DEBUG_H */
