
#include <stdio.h>

#include <libmacro/assert.h>
#include <libbase/short.h>
#include <libbase/uintmax.h>

#include "../array-short.h"
#include "../array-uintmax.h"
#include "../array-ptrm-int.h"


static
void
test_macro_constructor( void )
{
    Array_short const xs = ARRAY_SHORT( 89, 2, 77 );
    ASSERT( xs.length == 3, xs.e[ 0 ] == 89, xs.e[ 1 ] == 2, xs.e[ 2 ] == 77 );
    int y = 3;
    Array_ptrm_int const ys = ARRAY_PTRM_INT( &( int ){ 1 }, &y, &( int ){ 54 } );
    ASSERT( ys.length == 3,
            *( ys.e[ 0 ] ) == 1, ys.e[ 1 ] == &y, *( ys.e[ 2 ] ) == 54 );
}


static
void
test_equal( void )
{
    Array_uintmax const xs = ARRAY_UINTMAX( 12345, 98888, 213 );
    Array_uintmax const ys = ARRAY_UINTMAX( 12345, 98888, 213 );
    Array_uintmax const zs = ARRAY_UINTMAX( 12345, 213 );
    ASSERT( array_uintmax__equal( xs, ys ),
            array_uintmax__equal( ys, xs ),
            !array_uintmax__equal( xs, zs ),
            !array_uintmax__equal( ys, zs ),
            !array_uintmax__equal( zs, xs ),
            !array_uintmax__equal( zs, ys ) );
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
test_equal_by( void )
{
    int a = 2;
    Array_ptrm_int const xs = ARRAY_PTRM_INT( &( int ){ 1 }, &a,
                                              &( int ){ 4 }, &( int ){ 8 } );
    Array_ptrm_int const ys = ARRAY_PTRM_INT( &( int ){ 1 }, &a,
                                              &( int ){ 4 }, &( int ){ 8 } );
    ASSERT( xs.length == 4, ys.length == 4,
            xs.e[ 1 ] == &a, ys.e[ 1 ] == &a,
            !array_ptrm_int__equal( xs, ys ),
            array_ptrm_int__equal_by( xs, ys, ptrm_int_equal_val ) );
}


static
void
test_logic( void )
{
    Array_short const xs = ARRAY_SHORT( 1, 2, 3, -1 );
    ASSERT( array_short__all( xs, short__is_nonzero ) );
    Array_uintmax const ys = ARRAY_UINTMAX( 0, 0, 1, 32 );
    ASSERT( array_uintmax__any( ys, uintmax__is_positive ) );
    Array_short const zs = ARRAY_SHORT( 1, 0, 56, 23 );
    ASSERT( array_short__all_but_one( zs, short__is_positive ) );
}


static
void
test_replace( void )
{
    Array_short xs = ARRAY_SHORT( 3, 4, -8, 4, 4, 5, 4, -1 );

    Array_short ys = array_short__replaced( xs, 4, 6 );
    ASSERT( array_short__equal( ys,
                ( Array_short ) ARRAY_SHORT( 3, 6, -8, 6, 6, 5, 6, -1 ) ) );

    array_short__replace( ys, 5, 0 );
    ASSERT( array_short__equal( ys,
                ( Array_short ) ARRAY_SHORT( 3, 6, -8, 6, 6, 0, 6, -1 ) ) );

    array_short__replacef( ys, short__is_negative, 7 );
    ASSERT( array_short__equal( ys,
                ( Array_short ) ARRAY_SHORT( 3, 6, 7, 6, 6, 0, 6, 7 ) ) );

    Array_short zs = array_short__replacedf( ys, short__is_zero, 1 );
    ASSERT( array_short__equal( zs,
                ( Array_short ) ARRAY_SHORT( 3, 6, 7, 6, 6, 1, 6, 7 ) ) );

    array_short__free( &ys );
    array_short__free( &zs );
}


int
main( void )
{
    puts( "Running unit tests..." );
    test_macro_constructor();   puts( "  macro constructor tests passed" );
    test_equal();               puts( "  equality tests passed" );
    test_equal_by();            puts( "  custom equality tests passed" );
    test_logic();               puts( "  logic tests passed" );
    test_replace();             puts( "  replacement tests passed" );
    puts( "All unit tests passed!" );
}

