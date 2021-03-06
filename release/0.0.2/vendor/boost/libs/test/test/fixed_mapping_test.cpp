//  (C) Copyright Gennadiy Rozental 2001-2004.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: fixed_mapping_test.cpp,v $
//
//  Version     : $Revision: 1.2 $
//
//  Description : basic_cstring unit test
// *****************************************************************************

// Boost.Test
#include <boost/test/unit_test.hpp>

#include <boost/test/detail/basic_cstring/basic_cstring.hpp>
#include <boost/test/detail/basic_cstring/compare.hpp>
#include <boost/test/detail/fixed_mapping.hpp>

namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;
using utf::const_string;

//____________________________________________________________________________//

void test_default_compare()
{
    utf::fixed_mapping<const_string,int> test_mapping( 
        "Key1", 1,
        "Key2", 2,
        "QWE" , 3,
        "ASD" , 4,
        "aws" , 5,
        "dfg" , 6,
        "dgt" , 7,
        "ght" , 8,

        0
        );

    BOOST_CHECK_EQUAL( test_mapping[ "Key1" ], 1 );
    BOOST_CHECK_EQUAL( test_mapping[ "Key2" ], 2 );
    BOOST_CHECK_EQUAL( test_mapping[ "QWE" ] , 3 );
    BOOST_CHECK_EQUAL( test_mapping[ "ASD" ] , 4 );
    BOOST_CHECK_EQUAL( test_mapping[ "aws" ] , 5 );
    BOOST_CHECK_EQUAL( test_mapping[ "dfg" ] , 6 );
    BOOST_CHECK_EQUAL( test_mapping[ "dgt" ] , 7 );
    BOOST_CHECK_EQUAL( test_mapping[ "ght" ] , 8 );
    BOOST_CHECK_EQUAL( test_mapping[ "bla" ] , 0 );
}

//____________________________________________________________________________//

void test_custom_compare()
{
    utf::fixed_mapping<const_string,int,utf::case_ins_less<char const> > test_mapping( 
        "Key1", 1,
        "Key2", 2,
        "QWE" , 3,
        "ASD" , 4,

        0
    );

    BOOST_CHECK_EQUAL( test_mapping[ "Key1" ], 1 );
    BOOST_CHECK_EQUAL( test_mapping[ "Key2" ], 2 );
    BOOST_CHECK_EQUAL( test_mapping[ "QWE" ] , 3 );
    BOOST_CHECK_EQUAL( test_mapping[ "ASD" ] , 4 );
    BOOST_CHECK_EQUAL( test_mapping[ "kEy1" ], 1 );
    BOOST_CHECK_EQUAL( test_mapping[ "key2" ], 2 );
    BOOST_CHECK_EQUAL( test_mapping[ "qwE" ] , 3 );
    BOOST_CHECK_EQUAL( test_mapping[ "aSd" ] , 4 );
    BOOST_CHECK_EQUAL( test_mapping[ "bla" ] , 0 );
}

//____________________________________________________________________________//

utf::test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    utf::test_suite* test= BOOST_TEST_SUITE( "Mappings test" );

    test->add( BOOST_TEST_CASE( &test_default_compare ) );
    test->add( BOOST_TEST_CASE( &test_custom_compare ) );

    return test;
}

// *****************************************************************************
// History :
//
// $Log: fixed_mapping_test.cpp,v $
// Revision 1.2  2004/05/21 06:26:10  rogeeff
// licence update
//
// Revision 1.1  2004/05/13 09:04:44  rogeeff
// added fixed_mapping
//
// *****************************************************************************

// EOF
