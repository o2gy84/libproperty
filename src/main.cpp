#include <iostream>

#include "config.hpp"
#include "options.hpp"

void init_config(const std::string &path, Config *conf)
{
    conf->add("port", "port to listen to", 7788);
    conf->add("local_address", "local address", "localhost");
    conf->add("local_address2", "local address2", settings::address_t());
    conf->add("test1", "test 1", std::vector<int>());
    conf->add("test2", "test 2", std::vector<std::string>());
    conf->add("test3", "test 3", 0.);
    conf->add("test4", "test 4 address", settings::address_t());
    conf->add("test5", "test 5 file", settings::file_t());
    conf->add("test6", "test 6 shards", settings::shard_t());
    conf->add("test61", "test 61 address", std::vector<settings::address_t>());
    conf->add("test7", "test 7 map", std::map<std::string, std::string>());
    conf->add("test8", "test 8 map", std::map<settings::address_t, settings::shard_t>());
    conf->add("test9", "test 9 map", std::map<std::string, std::string>());
    conf->add("test10", "test 10 map", std::map<int, std::string>());
    conf->add("test11", "test 11 map", std::map<settings::address_t, settings::shard_t>());

    conf->load(path);
    std::cout << conf->dump() << std::endl;
}

void clear()
{
    Options::impl()->destroy();
    Config::impl()->destroy();
}

int main(int argc, char *argv[])
{
    try
    {
        Options *opt = Options::impl();
        opt->add("help", "h", "print help and exit", false);
        opt->add("port", "p", "port to listen to (1025..65536)", 0);
        opt->add("loglevel", "l", "loglevel (1..5)", 0);
        opt->add("config", "c", "path to config", "../../test.conf");
        opt->add("syslog", "", "write logs into syslog", false);

        opt->parse(argc, argv);
        std::cout << opt->dump() << std::endl;
        if (opt->get<bool>("help"))
        {
            std::cout << opt->usage(argv[0]) << std::endl;
            clear();
            return 0;
        }

        init_config(opt->get<std::string>("config"), Config::impl());
        clear();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "terminated: " << e.what() << std::endl;
    }

    clear();
    return 0;
}
