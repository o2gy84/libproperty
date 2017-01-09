#include <iostream>

#include "config.hpp"
#include "options.hpp"

void init_config(const std::string &path, Config *conf)
{
    conf->add("port", "port to listen to", 7788);
    conf->add("engine", "input/output engine: select, poll or epoll", "epoll");
    conf->add("local_address", "local address", "");
    conf->add("local_address2", "local address2", settings::address_t());
    conf->add("test1", "test 1", std::vector<int>());
    conf->add("test2", "test 2", std::vector<std::string>());
    conf->add("test3", "test 3", 0.);
    conf->add("test4", "test 4 address", settings::address_t());
    conf->add("test5", "test 5 file", settings::file_t());
    conf->add("test6", "test 6 shards", settings::shard_t());
    conf->add("test7", "test 7 map", std::map<std::string, std::string>());
    conf->add("test8", "test 8 map", std::map<settings::address_t, settings::shard_t>());
    conf->add("test9", "test 9 map", std::map<std::string, std::string>());
    conf->add("test10", "test 10 map", std::map<int, std::string>());

    conf->load(path);
    conf->dump();
    std::cout << conf->usage() << std::endl;

    /*
    std::map<std::string, std::string> test7 = conf->get<std::map<std::string, std::string>>("test7");
    logd("test7 map size: ", test7.size());

    settings::shard_t test6 = conf->get<settings::shard_t>("test6");
    logd("test6 shards size: ", test6.shards.size());

    settings::file_t test5 = conf->get<settings::file_t>("test5");
    logd("test5 file: {0} {1} bytes", test5.name, test5.content.size());

    settings::address_t test4 = conf->get<settings::address_t>("test4");
    logd("test4 address: {0}:{1}", test4.host, test4.port);

    double test3 = conf->get<double>("test3");
    logd("test3 double: ", test3);

    std::vector<int> test1 = conf->get<std::vector<int>>("test1");
    for (size_t i = 0; i < test1.size(); ++i)
    {
        logi("test[{0}] = {1}", i, test1[i]);
    }
    std::vector<std::string> test2 = conf->get<std::vector<std::string>>("test2");
    for (size_t i = 0; i < test2.size(); ++i)
    {
        logi("test[{0}] = {1}", i, test2[i]);
    }
    */
}

int main(int argc, char *argv[])
{
    try
    {
        Options opt;
        opt.add("help", "h", "print help and exit", false);
        opt.add("port", "p", "port to listen to (1025..65536)", 0);
        opt.add("loglevel", "l", "loglevel (1..5)", 0);
        opt.add("config", "c", "path to config", "");
        opt.add("syslog", "", "write logs into syslog", false);

        opt.parse(argc, argv);

        std::cout << "opt: loglevel: " << opt.get<int>("loglevel") << std::endl;
        std::cout << "opt: syslog: " << opt.get<bool>("syslog") << std::endl;
        std::cout << "opt: config: " << opt.get<std::string>("config") << std::endl;

        std::cout << opt.dump() << std::endl;

        init_config("../../test.conf", Config::impl());
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "terminated: " << e.what() << std::endl;
    }
    return 0;
}
