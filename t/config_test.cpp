#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ConfigTests

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "config.hpp"


using Config = libproperty::Config;

void touch_file(const std::string &name)
{
    bool ok = static_cast<bool>(std::ofstream(name).put('a'));
    if (!ok)
    {
        throw std::runtime_error("can't open file");
    }
}

BOOST_AUTO_TEST_CASE(simple_values)
{
    Config *conf = Config::impl();

    std::string config = R"(

port 9999
engine poll
pi 3.14

address 127.0.0.1:443
address2 127.0.0.2

path unittest.conf

shards 0, 2, 1, 3-5
    )";
    
    std::string unittest_conf = "unittest.conf";

    conf->add("port", "test int", 7788);
    conf->add("pi", "test double", 0.);
    conf->add("engine", "test string", "epoll");
    conf->add("address", "test address", settings::address_t());
    conf->add("address2", "test address2", settings::address_t());
    conf->add("path", "test file", settings::file_t());
    conf->add("shards", "test shards", settings::shard_t());

    touch_file(unittest_conf);
    conf->parse(config);
    std::remove(unittest_conf.c_str());

    BOOST_CHECK_EQUAL(conf->get<int>("port"), 9999);
    BOOST_CHECK_EQUAL(conf->get<std::string>("engine"), "poll");
    BOOST_CHECK_EQUAL(conf->get<double>("pi"), 3.14);

    BOOST_CHECK_EQUAL(conf->get<settings::address_t>("address").host, "127.0.0.1");
    BOOST_CHECK_EQUAL(conf->get<settings::address_t>("address").port, 443);

    BOOST_CHECK_EQUAL(conf->get<settings::address_t>("address2").host, "127.0.0.2");
    BOOST_CHECK_EQUAL(conf->get<settings::address_t>("address2").port, 0);

    BOOST_CHECK_EQUAL(conf->get<settings::file_t>("path").name, unittest_conf);
    BOOST_CHECK_EQUAL(conf->get<settings::file_t>("path").content, "a");
    BOOST_CHECK(conf->get<settings::file_t>("path").content.size() == 1);

    static const int arr[] = {0, 1, 2, 3, 4, 5};
    std::vector<int> conf_shards = conf->get<settings::shard_t>("shards").shards;
    BOOST_CHECK_EQUAL_COLLECTIONS(conf_shards.begin(), conf_shards.end(),
                                    arr, arr + 6);
}

BOOST_AUTO_TEST_CASE(comment)
{
    Config::impl()->destroy();
    Config *conf = Config::impl();
    std::string config = R"(

#port 9999

    )";
    
    conf->add("port", "test int", 7788);
    BOOST_CHECK_EQUAL(conf->get<int>("port"), 7788);
}


BOOST_AUTO_TEST_CASE(vector)
{
    Config::impl()->destroy();
    Config *conf = Config::impl();

    std::string config = R"(

test1 1, 2 ,3  , 4
test2 str1,  str2  , str3
test3 localhost:123, localhost:456

    )";
   
    conf->add("test1", "test 1", std::vector<int>());
    conf->add("test2", "test 2", std::vector<std::string>());
    conf->add("test3", "test 3", std::vector<settings::address_t>());

    conf->parse(config);

    static const int arr1[] = {1, 2, 3, 4};
    std::vector<int> conf_ints = conf->get<std::vector<int>>("test1");
    BOOST_CHECK_EQUAL_COLLECTIONS(conf_ints.begin(), conf_ints.end(),
                                    arr1, arr1 + 4);


    static const char* arr2[] = {"str1", "str2", "str3"};
    std::vector<std::string> conf_strings = conf->get<std::vector<std::string>>("test2");
    BOOST_CHECK_EQUAL_COLLECTIONS(conf_strings.begin(), conf_strings.end(),
                                    arr2, arr2 + 3);

    std::vector<settings::address_t> conf_addr = conf->get<std::vector<settings::address_t>>("test3");
    BOOST_CHECK_EQUAL(conf_addr[0].host, "localhost");
    BOOST_CHECK_EQUAL(conf_addr[0].port, 123);
    BOOST_CHECK_EQUAL(conf_addr[1].host, "localhost");
    BOOST_CHECK_EQUAL(conf_addr[1].port, 456);

}

BOOST_AUTO_TEST_CASE(map)
{
    Config::impl()->destroy();
    Config *conf = Config::impl();

    std::string config = R"(
test1 {
    key1 val1
    # comment
    key2 val2

}

test2
{
    1 line1
    2 one two three
}

test3 {
line1 111
line2 222
}

test4
{
    localhost:111 0,1,2
    localhost 3-5
}
    
    )";
   
    conf->add("test1", "test 1", std::map<std::string, std::string>());
    conf->add("test2", "test 2", std::map<int, std::string>());
    conf->add("test3", "test 3", std::map<std::string, int>());
    conf->add("test4", "test 4", std::map<settings::address_t, settings::shard_t>());

    conf->parse(config);

    std::map<std::string, std::string> test1 = conf->get<std::map<std::string, std::string>>("test1");
    BOOST_CHECK_EQUAL(test1["key1"], "val1");
    BOOST_CHECK_EQUAL(test1["key2"], "val2");

    std::map<int, std::string> test2 = conf->get<std::map<int, std::string>>("test2");
    BOOST_CHECK_EQUAL(test2[1], "line1");
    BOOST_CHECK_EQUAL(test2[2], "one two three");

    std::map<std::string, int> test3 = conf->get<std::map<std::string, int>>("test3");
    BOOST_CHECK_EQUAL(test3["line1"], 111);
    BOOST_CHECK_EQUAL(test3["line2"], 222);

    std::map<settings::address_t, settings::shard_t> test4 = conf->get<std::map<settings::address_t, settings::shard_t>>("test4");
    BOOST_CHECK_EQUAL(test4.size(), 2);
    {
        settings::address_t addr;
        addr.host = "localhost";
        addr.port = 111;
        settings::shard_t shard = test4[addr];
        static const int expected[] = {0, 1, 2};
        BOOST_CHECK_EQUAL_COLLECTIONS(shard.shards.begin(), shard.shards.end(), expected, expected + 3);
    }
    {
        settings::address_t addr;
        addr.host = "localhost";
        addr.port = 0;
        settings::shard_t shard = test4[addr];
        static const int expected[] = {3, 4, 5};
        BOOST_CHECK_EQUAL_COLLECTIONS(shard.shards.begin(), shard.shards.end(), expected, expected + 3);
    }
}

BOOST_AUTO_TEST_CASE(no_key)
{
    Config::impl()->destroy();
    Config *conf = Config::impl();

    bool it_was_exception = false;
    try
    {
        conf->get<int>("no_such_key");
    }
    catch (...)
    {
        it_was_exception = true;
    }

    BOOST_CHECK_EQUAL(it_was_exception, true);
}

BOOST_AUTO_TEST_CASE(double_key)
{
    Config::impl()->destroy();
    Config *conf = Config::impl();

    bool it_was_exception = false;
    try
    {
        conf->add("double_key", "", 0);
        conf->add("double_key", "", 0);
    }
    catch (...)
    {
        // key already registered
        it_was_exception = true;
    }

    BOOST_CHECK_EQUAL(it_was_exception, true);
}

BOOST_AUTO_TEST_CASE(invalid1)
{
    Config::impl()->destroy();
    Config *conf = Config::impl();

    std::string config = R"(

key_without_val

    )";

    bool it_was_exception = false;
    try
    {
        conf->parse(config);
    }
    catch (...)
    {
        it_was_exception = true;
    }

    BOOST_CHECK_EQUAL(it_was_exception, true);
}

BOOST_AUTO_TEST_CASE(invalid2)
{
    Config::impl()->destroy();
    Config *conf = Config::impl();

    std::string config = R"(

map {
    key_without_val
}

    )";

    conf->add("map", "invalid map", std::map<std::string, std::string>());

    bool it_was_exception = false;
    try
    {
        conf->parse(config);
    }
    catch (...)
    {
        it_was_exception = true;
    }

    BOOST_CHECK_EQUAL(it_was_exception, true);
}

BOOST_AUTO_TEST_CASE(not_invalid2)
{
    // in previous case it is not bad, if something into map ill formed,
    // when this map will not be parsed

    Config::impl()->destroy();
    Config *conf = Config::impl();

    std::string config = R"(

map {
    key_without_val
}

    )";

    bool it_was_exception = false;
    try
    {
        conf->parse(config);
    }
    catch (...)
    {
        it_was_exception = true;
    }

    BOOST_CHECK_EQUAL(it_was_exception, false);
}


