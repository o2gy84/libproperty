#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE OptionsTest

#include <boost/test/unit_test.hpp>
#include <iostream>


BOOST_AUTO_TEST_CASE(test1)
{
    BOOST_CHECK_EQUAL("1", "1");
}
