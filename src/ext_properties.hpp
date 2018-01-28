#pragma once

#include <string>
#include <vector>

namespace settings
{

struct address_t
{
    uint16_t port;
    std::string host;
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
    std::vector<int> shards;
    bool operator<(const shard_t &rhs) const { return shards < rhs.shards; }
};

}   // namespace settings
