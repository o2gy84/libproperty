#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE OptionsTest

#include <boost/test/unit_test.hpp>
#include <iostream>

#include "options.hpp"
#include "utils.hpp"


using libproperty::Options;

char **make_argv(const std::string &str, int &n)
{
    std::vector<std::string> v = utils::split(str, " ");
    char **argv = new char*[v.size()];

    for (size_t i = 0; i < v.size(); ++i)
    {
        argv[i] = strdup(v[i].c_str());
    }

    n = v.size();
    return argv;
}

BOOST_AUTO_TEST_CASE(test1)
{
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin --help", n);

    opt->add("help", "h", "print help", false);
    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
}

BOOST_AUTO_TEST_CASE(test2)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h", n);

    opt->add("help", "h", "print help", false);

    bool it_throws = false;
    try
    {
        opt->parse(n, argv);
    }
    catch (...)
    {
        it_throws = true;
    }
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(it_throws, false);
}

BOOST_AUTO_TEST_CASE(test3)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin", n);

    opt->add("help", "h", "print help", false);
    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), false);
}

BOOST_AUTO_TEST_CASE(test4)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -help", n);

    opt->add("help", "h", "print help", false);

    bool it_throws = false;

    try
    {
        opt->parse(n, argv);
    }
    catch (...)
    {
        it_throws = true;
    }

    BOOST_CHECK_EQUAL(opt->get<bool>("help"), false);
    BOOST_CHECK_EQUAL(it_throws, true);
}

BOOST_AUTO_TEST_CASE(test5)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin --hel", n);

    opt->add("help", "h", "print help", false);

    try
    {
        opt->parse(n, argv);
    }
    catch (...)
    {}
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), false);
}

BOOST_AUTO_TEST_CASE(test7)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin --h", n);

    opt->add("help", "h", "print help", false);

    try
    {
        opt->parse(n, argv);
    }
    catch (...)
    {}
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), false);
}

BOOST_AUTO_TEST_CASE(test8)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h -l 5", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
}

BOOST_AUTO_TEST_CASE(test9)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h -l=5", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
}

BOOST_AUTO_TEST_CASE(test10)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h --loglevel 5", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
}

BOOST_AUTO_TEST_CASE(test11)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h --loglevel=5", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
}


BOOST_AUTO_TEST_CASE(test12)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h --logleve 5", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);

    try
    {
        opt->parse(n, argv);
    }
    catch (...)
    {
    }
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 0);
}

BOOST_AUTO_TEST_CASE(test13)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h -loglevel 5", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);

    try
    {
        opt->parse(n, argv);
    }
    catch (...)
    {
    }
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 0);
}

BOOST_AUTO_TEST_CASE(test14)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h --l 5", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);

    try
    {
        opt->parse(n, argv);
    }
    catch (...)
    {
    }
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 0);
}


BOOST_AUTO_TEST_CASE(test15)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h -l 5 --port=7777", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
}

BOOST_AUTO_TEST_CASE(test16)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -h -l 5 --port=7777 --path=/etc/wow.conf", n);

    opt->add("help", "h", "print help", false);
    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);
    opt->add("path", "", "path to config", "");

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<bool>("help"), true);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
    BOOST_CHECK_EQUAL(opt->get<std::string>("path"), "/etc/wow.conf");
}

BOOST_AUTO_TEST_CASE(test17)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin --port=7777 -l=1", n);

    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);
    opt->add("path", "", "path to config", "/some/path");

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 1);
    BOOST_CHECK_EQUAL(opt->get<std::string>("path"), "/some/path");
}

BOOST_AUTO_TEST_CASE(test18)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -c=/etc/wow.conf -l 5 --port=7777 ", n);

    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);
    opt->add("config", "c", "path to config", "");

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
    BOOST_CHECK_EQUAL(opt->get<std::string>("config"), "/etc/wow.conf");
}

BOOST_AUTO_TEST_CASE(test19)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin -c /etc/wow.conf -l 5 --port=7777 ", n);

    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);
    opt->add("config", "c", "path to config", "");

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
    BOOST_CHECK_EQUAL(opt->get<std::string>("config"), "/etc/wow.conf");
}

BOOST_AUTO_TEST_CASE(test20)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin --config=/etc/wow.conf -l 5 --port=7777 ", n);

    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);
    opt->add("config", "c", "path to config", "");

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
    BOOST_CHECK_EQUAL(opt->get<std::string>("config"), "/etc/wow.conf");
}

BOOST_AUTO_TEST_CASE(test21)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin --config /etc/wow.conf -l 5 --port=7777 ", n);

    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);
    opt->add("config", "c", "path to config", "");

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
    BOOST_CHECK_EQUAL(opt->get<std::string>("config"), "/etc/wow.conf");
}

BOOST_AUTO_TEST_CASE(test22)
{
    Options::impl()->destroy();
    Options *opt = Options::impl();

    int n = 0;
    char **argv = make_argv("./bin --config /etc/wow.conf -l 5 --port=7777 ", n);

    opt->add("loglevel", "l", "loglevel", 0);
    opt->add("port", "p", "port", 0);
    opt->add("config", "c", "path to config", "");

    opt->parse(n, argv);
    BOOST_CHECK_EQUAL(opt->get<int>("port"), 7777);
    BOOST_CHECK_EQUAL(opt->get<int>("loglevel"), 5);
    BOOST_CHECK_EQUAL(opt->get<std::string>("config"), "/etc/wow.conf");

    bool it_throws = false;
    try
    {
        opt->get<bool>("not_registered");
    }
    catch (...)
    {
        it_throws = true;
    }
    BOOST_CHECK_EQUAL(it_throws, true);
}
