
#include <stdio.h>

#include <libmacro/assert.h>
#include <libbase/short.h>
#include <libbase/uintmax.h>

#include "../array-short.h"
#include "../array-uintmax.h"
#include "../array-ptrm-int.h"


static
void
test_array_short( void )
{
    ArrayC_short const xs = ARRAY_SHORT( 89, 2, 77, 5, 123 );
    ArrayC_short const ys = ARRAY_SHORT( 77, 5, 123 );
    ASSERT( xs.length == 5,
            xs.e[ 0 ] == 89,
            xs.e[ 4 ] == 123,
            ys.length == 3,
            arrayc_short__find_index( xs, short__is_even ).value == 1,
            arrayc_short__all( xs, short__is_positive ) );
}


static
void
test_array_uintmax( void )
{
    ArrayC_uintmax const xs = ARRAY_UINTMAX( 12345, 98888, 213 );
    ArrayC_uintmax const ys = ARRAY_UINTMAX( 12345, 98888, 213 );
    ASSERT( xs.length == 3,
            arrayc_uintmax__equal( xs, ys ),
            arrayc_uintmax__all( xs, uintmax__is_positive ) );
}


static
bool
ptrm_int_equal_val( int * const x,
                    int * const y )
{
    ASSERT( x != NULL, y != NULL );

    return *x == *y;
}


static
void
test_array_ptrm_int( void )
{
    ArrayC_ptrm_int const xs = ARRAY_PTRM_INT( &( int ){ 1 }, &( int ){ 2 },
                                               &( int ){ 4 }, &( int ){ 8 } );
    ArrayC_ptrm_int const ys = ARRAY_PTRM_INT( &( int ){ 1 }, &( int ){ 2 },
                                               &( int ){ 4 }, &( int ){ 8 } );
    ASSERT( xs.length == 4,
            !arrayc_ptrm_int__equal( xs, ys ),
            arrayc_ptrm_int__equal_by( xs, ys, ptrm_int_equal_val ) );
}


int
main( void )
{
    test_array_short();
    printf( "All `array-short` assertions passed.\n" );
    test_array_uintmax();
    printf( "All `array-uintmax` assertions passed.\n" );
    test_array_ptrm_int();
    printf( "All `array-ptrm-int` assertions passed.\n" );
}

