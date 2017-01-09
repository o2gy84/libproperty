# libproperty
Library allows easy to define set of properties and then parse (from file or memory) and use them.

## Introduction
Base concept - simple idea that in most cases the feature is a key and value pair. No more, no less.

Why need empty keys? (boost.property_tree)  
Why need ```;``` as a comment symbol? (boost.property_tree)  
Why need nesting and file inclusion? (boost.property_tree)  
Why need use own types even for integers, strings? (boost.programm_options)  
Why need use separate variables? (boost.programm_options)  
Why are there several ways to define something? A year later, someone definitely will be confused! (libconfig)  
Why need the json, if the program needs only a few paths and, for example, the port number?  

The main goal of libproperty is simplicity. Algorithm is: 
 - describe needed properties
 - run parser
 - get values

Library does not provide high efficiency, so that the "get()" call in a cycle - a pretty bad idea.

### Example of programm options parser
```
#include "options.hpp" 

Options *opt = Options::impl();
opt->add("help", "h", "print help and exit", false);
opt->add("loglevel", "l", "loglevel (1..5)", 0); 
opt->add("config", "c", "path to config", ""); 

opt->parse(argc, argv);

if (opt->get<bool>("help"))
{
    std::cout << opt->usage(argv[0]) << std::endl;
    return 0;
}

int loglevel = opt->get<int>("loglevel");
std::string config = opt->get<std::string>("config");
```

### Example of config parser

```
#include "config.hpp" 

Config *conf = Config::impl()
conf->add("port", "port to listen to", 7788);
conf->add("pi", "pi value", 3.14);
conf->add("vector", "test vector", std::vector<std::string>());
conf->add("map", "test map", std::map<std::string, int>());

conf->load(path);

int port = conf->get<int>("port");
double pi = conf->get<double>("pi");
std::vector<std::string> v = conf->get<std::vector<std::string>>("vector");
std::map<std::string, int> m = conf->get<std::map<std::string, int>>("map");
```

### What can parse options parser?
 - bool
 - int
 - string
 
Example of options string:
```
./libpropery -p=7788 -l=1 --syslog
```

libpropery automatically generates "usage":
```
./libpropery --help
Usage: ./libpropery -c -h -l -p --syslog

        -c [ --config ]         : path to config
        -h [ --help ]           : print help and exit
        -l [ --loglevel ]       : loglevel (1..5)
        -p [ --port ]           : port to listen to (1025..65536)
        --syslog                : write logs into syslog
```

### What can parse config parser?
 - int
 - double
 - std::string
 - address_t
 - file_t
 - shard_t
 - std::vector (with T = any from above)
 - std::map    (with K and T = any from above)
 
address_t, file_t, shard_t: see in https://github.com/o2gy84/libproperty/blob/master/src/settings.hpp
 
Example of config:
```
#comment
int_val 7788
double_val 3.14789

string_val 127.0.0.1
address_val 127.0.0.1:443

vector_int 1, 10,  100
vector_string      string1,        string2

path_val /home/v.mogilin/pwd
shard_val 0-3,4, 5, 6,  7-9

map1
{
    # comment
    key1_string val1_string
}

map2
{
    localhost:111 0,1,2
    localhost 3-5
}

map3 {a b}
map4 {
    1 xxx
    2 vvv
}
```

So there are:
 - only key and value, divided by first space
 - comments start with ```#```
 

### More examples
See more usage examples in unittests: https://github.com/o2gy84/libproperty/tree/master/t

### Authors
- Victor Mogilin (o2gy84@gmail.com)
