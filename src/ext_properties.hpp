#pragma once

#include <string>
#include <vector>

namespace settings
{

struct address_t
{
#if defined __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif
    uint16_t port = 0;
    std::string host;
#if defined __clang__
#pragma clang diagnostic pop
#endif
    bool operator<(const address_t &rhs) const
    {
        if (host == rhs.host) return port < rhs.port;
        return host < rhs.host;
    }
};

struct file_t
{
    std::string name;
    std::string content;
    bool operator<(const file_t &rhs) const
    {
        if (name == rhs.name) return content < rhs.content;
        return name < rhs.name;
    }
};

struct shard_t
{
    std::vector<size_t> shards;
    bool operator<(const shard_t &rhs) const { return shards < rhs.shards; }
};

}   // namespace settings
