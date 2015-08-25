
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <libmacro/assert.h>
#include <libmacro/debug.h>
#include <libbase/short.h>
#include <libbase/uintmax.h>

#include "../array_char.h"
#include "../array_uchar.h"
#include "../array_short.h"
#include "../array_uintmax.h"
#include "../array_ptrm_int.h"


static
void
test_macro_constructor( void )
{
    ArrayC_short const xs = ARRAY_SHORT( 89, 2, 77 );
    ASSERT( xs.length == 3,
            xs.e[ 0 ] == 89, xs.e[ 1 ] == 2, xs.e[ 2 ] == 77 );

    int y = 3;
    ArrayM_ptrm_int const ys = ARRAY_PTRM_INT( &( int ){ 1 }, &y,
                                               &( int ){ 54 } );
    ASSERT( ys.length == 3,
            *( ys.e[ 0 ] ) == 1,
            ys.e[ 1 ] == &y, y == 3,
            *( ys.e[ 2 ] ) == 54 );
}


static
void
test_equal( void )
{
    ArrayC_uintmax const xs = ARRAY_UINTMAX( 12345, 98888, 213 );
    ArrayC_uintmax const ys = ARRAY_UINTMAX( 12345, 98888, 213 );
    ArrayC_uintmax const zs = ARRAY_UINTMAX( 12345, 213 );
    ASSERT( arrayc_uintmax__equal( xs, ys ),
            arrayc_uintmax__equal( ys, xs ),
            !arrayc_uintmax__equal( xs, zs ),
            !arrayc_uintmax__equal( ys, zs ),
            !arrayc_uintmax__equal( zs, xs ),
            !arrayc_uintmax__equal( zs, ys ) );
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
    ArrayC_ptrm_int const xs = ARRAY_PTRM_INT( &( int ){ 1 }, &a,
                                               &( int ){ 4 }, &( int ){ 8 } );
    ArrayC_ptrm_int const ys = ARRAY_PTRM_INT( &( int ){ 1 }, &a,
                                               &( int ){ 4 }, &( int ){ 8 } );
    ASSERT( xs.length == 4, ys.length == 4,
            xs.e[ 1 ] == &a, ys.e[ 1 ] == &a,
            !arrayc_ptrm_int__equal( xs, ys ),
            arrayc_ptrm_int__equal_by( xs, ys, ptrm_int_equal_val ) );
}


static
void
test_realloc( void )
{
    {
        ArrayM_uintmax xs = arraym_uintmax__new_els( 1, 2, 3 );
        ASSERT( xs.length == 3, arraym_uintmax__equal_els( xs, 1, 2, 3 ) );
        arraym_uintmax__reallocz( &xs, 100 );
        ASSERT( xs.length == 100,
                arraym_uintmax__equal_els( arraym_uintmax__take( xs, 3 ),
                                           1, 2, 3 ),
                arraym_uintmax__all_null( arraym_uintmax__drop( xs, 3 ) ) );
    }
}


static
void
test_logic( void )
{
    {
        ArrayC_short const xs = ARRAY_SHORT( 1, 2, 3, -1 );
        ASSERT( arrayc_short__all( xs, short__is_nonzero ) );
        ArrayC_uintmax const ys = ARRAY_UINTMAX( 0, 0, 1, 32 );
        ASSERT( arrayc_uintmax__any( ys, uintmax__is_positive ) );
        ArrayC_short const zs = ARRAY_SHORT( 1, 0, 56, 23 );
        ASSERT( arrayc_short__all_but_one( zs, short__is_positive ) );
    }
    {
        ArrayM_short const xs = ARRAY_SHORT( 1, 2, 3, -1 );
        ASSERT( arraym_short__all( xs, short__is_nonzero ) );
        ArrayM_uintmax const ys = ARRAY_UINTMAX( 0, 0, 1, 32 );
        ASSERT( arraym_uintmax__any( ys, uintmax__is_positive ) );
        ArrayM_short const zs = ARRAY_SHORT( 1, 0, 56, 23 );
        ASSERT( arraym_short__all_but_one( zs, short__is_positive ) );
    }
}


static
void
test_replace( void )
{
    ArrayC_short xs = ARRAY_SHORT( 3, 4, -8, 4, 4, 5, 4, -1 );

    ArrayM_short ys = arrayc_short__replaced( xs, 4, 6 );
    ASSERT( arraym_short__equal_els( ys, 3, 6, -8, 6, 6, 5, 6, -1 ) );

    arraym_short__replace( ys, 5, 0 );
    ASSERT( arraym_short__equal_els( ys, 3, 6, -8, 6, 6, 0, 6, -1 ) );

    arraym_short__replacef( ys, short__is_negative, 7 );
    ASSERT( arraym_short__equal_els( ys, 3, 6, 7, 6, 6, 0, 6, 7 ) );

    ArrayM_short zs = arraym_short__replacedf( ys, short__is_zero, 1 );
    ASSERT( arraym_short__equal_els( zs, 3, 6, 7, 6, 6, 1, 6, 7 ) );

    arraym_short__free( &ys );
    arraym_short__free( &zs );
}


static
void
test_null( void )
{
    ArrayC_short const a = arrayc_short__view_buf(
                                ( short [] ){ 321, 98, 32, 0 } );
    ASSERT( a.length == 3, arrayc_short__equal_els( a, 321, 98, 32 ) );

    ArrayM_short b = arraym_short__copy_buf(
                          ( short [] ){ 11, 22, 0, 33 } );
    ASSERT( b.length == 2, arraym_short__equal_els( b, 11, 22 ) );
    arraym_short__free( &b );

    ArrayC_short const c = arrayc_short__view_buf0( ( short[] ){ 1, 2, 0 } );
    ASSERT( c.length == 3, arrayc_short__equal_els( c, 1, 2, 0 ) );

    ArrayM_short d = arraym_short__copy_buf0(
                          ( short [] ){ 11, 22, 0, 33 } );
    ASSERT( d.length == 3, arraym_short__equal_els( d, 11, 22, 0 ) );
    arraym_short__free( &d );

    ArrayC_char const e = arrayc_char__view_buf0( "something" );
    ASSERT( arrayc_char__last_is_null( e ), !arrayc_char__all_null( e ) );

    ArrayC_char const f = arrayc_char__view_buf( "blah blah" );
    ASSERT( !arrayc_char__last_is_null( f ), !arrayc_char__all_null( f ) );

    ArrayC_short const g = ARRAY_SHORT( 0, 0, 0, 0 );
    ASSERT( arrayc_short__last_is_null( g ), arrayc_short__all_null( g ) );
}


static
void
test_char( void )
{
    // test char type with non-null-terminator functions:
    {
        ArrayC_char const a = arrayc_char__view_buf( "hello, world" );
        ASSERT( arrayc_char__equal_str( a, "hello, world" ) );

        ArrayC_char const b = ARRAY_CHAR( 'x', 'y', 'z' );
        ASSERT( arrayc_char__equal_str( b, "xyz" ) );

        ArrayM_char c = arraym_char__copy_str( "abc" );
        ASSERT( arraym_char__equal_els( c, 'a', 'b', 'c' ),
                arraym_char__equal_str( c, "abc" ) );
        arraym_char__copy_str_into( &c, "test" );
        ASSERT( arraym_char__equal_els( c, 't', 'e', 's', 't' ),
                arraym_char__equal_str( c, "test" ) );
        arraym_char__free( &c );

        ArrayM_char d = arraym_char__copy_str( NULL );
        ASSERT( arraym_char__is_empty( d ) );
        arraym_char__free( &d );

        ArrayM_char e = arraym_char__copy_str( "" );
        ASSERT( arraym_char__is_empty( e ) );
        arraym_char__free( &e );

        ArrayC_char const f = ARRAY_CHAR( 'h', 'a', 'h', 'a' );
        ASSERT( arrayc_char__equal_str( f, "haha" ) );
    }

    // test char with null-terminator functions:
    {
        ArrayC_char const a = arrayc_char__view_buf0( "hello, world" );
        ASSERT( !arrayc_char__equal_str( a, "hello, world" ),
                arrayc_char__equal_str0( a, "hello, world" ) );

        ArrayC_char const b = ARRAY_CHAR( 'x', 'y', 'z', '\0' );
        ASSERT( !arrayc_char__equal_str( b, "xyz" ),
                arrayc_char__equal_str0( b, "xyz" ),
                !arrayc_char__equal_str0( b, "xy" ),
                !arrayc_char__equal_str0( b, "xyzz" ) );

        ArrayM_char c = arraym_char__copy_str0( "abc" );
        ASSERT( arraym_char__equal_els( c, 'a', 'b', 'c', '\0' ),
                arraym_char__equal_str0( c, "abc" ) );
        arraym_char__copy_str0_into( &c, "test" );
        ASSERT( arraym_char__equal_els( c, 't', 'e', 's', 't', '\0' ),
                arraym_char__equal_str0( c, "test" ) );
        arraym_char__free( &c );

        ArrayM_char d = arraym_char__copy_str0( NULL );
        ASSERT( arraym_char__is_empty( d ) );
        arraym_char__free( &d );

        ArrayM_char e = arraym_char__copy_str0( "" );
        ASSERT( arraym_char__equal_els( e, '\0' ),
                !arraym_char__equal_str( e, "" ),
                arraym_char__equal_str0( e, "" ) );
        arraym_char__free( &e );
    }

    // test uchar with non-null-terminator functions:
    {
        ArrayC_uchar const a = arrayc_uchar__view_buf(
                                   ( uchar[] ){ "hello, world" } );
        ASSERT( arrayc_uchar__equal_str( a, "hello, world" ) );

        ArrayC_uchar const b = ARRAY_UCHAR( 'x', 'y', 'z' );
        ASSERT( arrayc_uchar__equal_str( b, "xyz" ) );

        ArrayM_uchar c = arraym_uchar__copy_str( "abc" );
        ASSERT( arraym_uchar__equal_els( c, 'a', 'b', 'c' ),
                arraym_uchar__equal_str( c, "abc" ) );
        arraym_uchar__copy_str_into( &c, "test" );
        ASSERT( arraym_uchar__equal_els( c, 't', 'e', 's', 't' ),
                arraym_uchar__equal_str( c, "test" ) );
        arraym_uchar__free( &c );

        ArrayM_uchar d = arraym_uchar__copy_str( NULL );
        ASSERT( arraym_uchar__is_empty( d ) );
        arraym_uchar__free( &d );

        ArrayM_uchar e = arraym_uchar__copy_str( "" );
        ASSERT( arraym_uchar__is_empty( e ) );
        arraym_uchar__free( &e );

        ArrayC_uchar const f = ARRAY_UCHAR( 'h', 'a', 'h', 'a' );
        ASSERT( arrayc_uchar__equal_str( f, "haha" ) );
    }
}


static
void
test_ord( void )
{
    ASSERT(
        arrayc_uchar__compare( ARRAYC_UCHAR( 1, 2, 3 ),
                               ARRAYC_UCHAR( 1, 2, 3 ) ) == EQ,
        arrayc_uchar__compare( arrayc_uchar__new_empty(),
                               ARRAYC_UCHAR( 1, 2, 3 ) ) == LT,
        arrayc_uchar__compare( ARRAYC_UCHAR( 1, 2, 3 ),
                               arrayc_uchar__new_empty() ) == GT,
        arrayc_uchar__compare( ARRAYC_UCHAR( 1, 2, 3 ),
                               ARRAYC_UCHAR( 1, 2, 4 ) ) == LT,
        arrayc_uchar__compare( arrayc_uchar__new_empty(),
                               arrayc_uchar__new_empty() ) == EQ,
        arrayc_uchar__is_ascending( arrayc_uchar__new_empty() ),
        arrayc_uchar__is_ascending( ARRAYC_UCHAR( 1, 2, 3 ) ),
        !arrayc_uchar__is_ascending( ARRAYC_UCHAR( 1, 3, 2 ) ),
        arrayc_uchar__is_descending( arrayc_uchar__new_empty() ),
        arrayc_uchar__is_descending( ARRAYC_UCHAR( 3, 2, 1) ),
        !arrayc_uchar__is_descending( ARRAYC_UCHAR( 3, 2, 4 ) ),
        arrayc_uchar__minimum( ARRAYC_UCHAR( 8, 25, 99, 31, 99 ) ) == 8,
        arrayc_uchar__minimum( ARRAYC_UCHAR( 0 ) ) == 0,
        arrayc_uchar__maximum( ARRAYC_UCHAR( 8, 25, 99, 31, 99 ) ) == 99,
        arrayc_uchar__maximum( ARRAYC_UCHAR( 0 ) ) == 0
    );
    { // Sort an empty array:
        ArrayM_uchar const xs = arraym_uchar__new_empty();
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__is_empty( xs ) );
    }
    { // Sort a singleton array:
        ArrayM_uchar const xs = ARRAY_UCHAR( 37 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 37 ) );
    }
    { // Sort a two-element array:
        ArrayM_uchar const xs = ARRAY_UCHAR( 32, 15 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 15, 32 ) );
    }
    { // Sort a three-element array:
        ArrayM_uchar const xs = ARRAY_UCHAR( 8, 32, 15 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 8, 15, 32 ) );
    }
    { // Sort a four-element array:
        ArrayM_uchar const xs = ARRAY_UCHAR( 8, 32, 15, 70 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 8, 15, 32, 70 ) );
    }
    { // Sort a five-element array:
        ArrayM_uchar const xs = ARRAY_UCHAR( 8, 32, 1, 15, 70 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 1, 8, 15, 32, 70 ) );
    }
    { // Sort a five-element array, increasing order:
        ArrayM_uchar const xs = ARRAY_UCHAR( 1, 2, 3, 4, 5 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 1, 2, 3, 4, 5 ) );
    }
    { // Sort a five-element array, decreasing order:
        ArrayM_uchar const xs = ARRAYM_UCHAR( 5, 4, 3, 2, 1 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 1, 2, 3, 4, 5 ) );
    }
    { // Sort a five-element array, equal elements:
        ArrayM_uchar const xs = ARRAYM_UCHAR( 8, 8, 8, 8, 8 );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__equal_els( xs, 8, 8, 8, 8, 8 ) );
    }
}


static size_t gen_array_size = 32;


static
void
gen_array_els(
        ArrayM_uchar * const xs )
{
    xs->length = rand() % gen_array_size;
    for ( size_t i = 0; i < xs->length; i++ ) {
        xs->e[ i ] = rand() % UCHAR_MAX;
    }
}


static
void
gentest_sort( size_t const iters )
{
    ArrayM_uchar xs = arraym_uchar__new_alloc( gen_array_size );
    for ( size_t i = 0; i < iters; i++ ) {
        gen_array_els( &xs );
        arraym_uchar__quicksort( xs );
        ASSERT( arraym_uchar__is_ascending( xs ) );
    }
    arraym_uchar__free( &xs );
}


int
main( void )
{
    puts( "Running unit tests..." );
    test_macro_constructor();   puts( "  macro constructor tests passed" );
    test_equal();               puts( "  equality tests passed" );
    test_equal_by();            puts( "  custom equality tests passed" );
    test_realloc();             puts( "  reallocation tests passed" );
    test_logic();               puts( "  logic tests passed" );
    test_replace();             puts( "  replacement tests passed" );
    test_null();                puts( "  NULL typeclass tests passed" );
    test_char();                puts( "  CHAR typeclass tests passed" );
    test_ord();                 puts( "  ORD typeclass tests passed" );
    puts( "Running generative tests..." );
    srand( time( NULL ) );
    gentest_sort( 10000 );      puts( "  sorting generative tests passed" );
}

