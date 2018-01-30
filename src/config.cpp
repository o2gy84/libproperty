#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "utils.hpp"
#include "config.hpp"

namespace config
{

struct token_t
{
    token_t() {}
    std::string key;
    std::string value;
    void reset() { key = value = ""; }
};

std::string next_line(const std::string &config, std::string::size_type &prev_pos)
{
    while (prev_pos < config.size() && config[prev_pos] == '\n')
    {
        ++prev_pos;
    }

    std::string::size_type cr_pos = config.find("\n", prev_pos);
    if (cr_pos == std::string::npos)
    {
        cr_pos = config.size();
    }

    if (cr_pos <= prev_pos)
    {
        // parse is over
        return "";
    }

    std::string line = config.substr(prev_pos, cr_pos - prev_pos);
    prev_pos = cr_pos + 1;
    return line;
}

int get_next_token(const std::string &config,
                   std::string::size_type &prev_pos,
                   const std::string &delimiter,
                   token_t &token)
/*
 *   values are:
 *   1. usual keys, like:
 *      key value
 *   2. maps, like:
 *      key {
 *          key value
 *      }
 */
{
    std::string line = next_line(config, prev_pos);
    if (line.empty())
    {
        // parse is over
        return 0;
    }

    line = utils::trimmed(line);

    std::vector<std::string> parts = utils::split(line, delimiter, 2);
    if (parts.size() == 0)
    {
        throw std::runtime_error("invalid config: " + line);
    }

    token.key = utils::trimmed(parts[0]);

    std::string value;
    if (parts.size() == 2)
    {
        value = utils::trimmed(parts[1]);
        if (value[0] != '{')
        {
            token.value = value;
            return 1;
        }
    }

    // is map?
    if (value.empty())
    {
        value = next_line(config, prev_pos);
        if (value.empty())
        {
            throw std::runtime_error("invalid config: " + line);
        }
        value = utils::trimmed(value);
    }

    if (value[0] != '{')
    {
        throw std::runtime_error("invalid config (expected '{') in line: " + line);
    }

    // is map! need find closed '}'

    std::string::size_type close_map_pos = value.find("}");
    if (close_map_pos != std::string::npos)
    {
        token.value = value.substr(0, close_map_pos + 1);
        return 1;
    }

    close_map_pos = config.find("}", prev_pos);
    if (close_map_pos == std::string::npos)
    {
         throw std::runtime_error("invalid config: " + line);
    }

    token.value = value + config.substr(prev_pos, close_map_pos + 1 - prev_pos);

    prev_pos = close_map_pos + 1;
    return 1;
}

std::string skip_comments(const std::string &config)
{
    std::string ret;
    std::vector<std::string> lines = utils::split(config, "\n");
    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string line = utils::trimmed(lines[i]);
        if (line.empty())
        {
            continue;
        }

        if (line[0] == '#')
        {
            continue;
        }

        ret += line;
        ret += "\n";
    }

    return ret;
}

}   // namespace config

namespace libproperty
{

Config* Config::m_Self = nullptr;

Config* Config::impl()
{
    if (m_Self == NULL)
    {
        m_Self = new Config();
    }
    return m_Self;
}

void Config::destroy()
{
    delete m_Self;
    m_Self = nullptr;
}

void Config::load(const std::string &path)
{
    std::string real_path = path;
    if (real_path.empty())
    {
        // TODO: what can we do in this case?
    }

    read(real_path);
}

void Config::read(const std::string &path)
{
    if (path.empty())
    {
        std::cerr << "path to config is empty" << std::endl;
        return;
    }

    std::ifstream ifs(path.c_str());
    if (!ifs.good())
    {
        std::cerr << "cannot open config file \"" << path << "\"" << std::endl;
        return;
    }

    std::string config((std::istreambuf_iterator<char>(ifs)),
                 std::istreambuf_iterator<char>());

    parse(config);
}

void Config::parse(const std::string &_config)
{
    std::string config = config::skip_comments(_config);

    config::token_t token;
    std::string::size_type prev_pos = 0;
    while (config::get_next_token(config, prev_pos, " ", token))
    {
        std::pair<SettingItem &, bool> item = m_Storage.find_option_by_long_key(token.key);
        if (!item.second)
        {
            // TODO: save all unused keys? show them in dump?
            // unused key in config - token.key
            continue;
        }

        parseFromConfig(item.first.value(), item.first.value().type(), token.value);
        token.reset();
    }
}

void Config::parseFromConfig(AnyItem &item, AnyItem::type_t type, const std::string &text)
{
    if (type == AnyItem::INT)
    {
        int v = std::stoi(text);
        item.store(v);
    }
    else if (type == AnyItem::DOUBLE)
    {
        double v = std::stod(text);
        item.store(v);
    }
    else if (type == AnyItem::STRING)
    {
        item.store(text);
    }
    else if (type == AnyItem::ADDRESS)
    {
        settings::address_t address;
        std::vector<std::string> tmp = utils::split(text, ":");
        if (tmp.size() == 1)
        {
            address.host = tmp[0];
            address.port = 0;
        }
        else if (tmp.size() == 2)
        {
            address.host = tmp[0];
            address.port = std::stoi(tmp[1]);
        }
        else
        {
            throw std::runtime_error("parse config: value is not address - " + text);
        }
        item.store(address);
    }
    else if (type == AnyItem::FILE)
    {
        std::ifstream ifs(text.c_str());
        if (!ifs.good())
        {
            throw std::runtime_error("cannot open file \"" + text + "\"");
        }

        std::string content((std::istreambuf_iterator<char>(ifs)),
                 std::istreambuf_iterator<char>());

        settings::file_t file;
        file.name = text;
        file.content = content;
        item.store(file);
    }
    else if (type == AnyItem::SHARD)
    {
        settings::shard_t shard;
        std::vector<std::string> ranges = utils::split(text, ",");
        for (const std::string &range : ranges)
        {
            if (range.find("-") != std::string::npos)
            {
                std::vector<std::string> range_bounds = utils::split(range, "-");
                if (range_bounds.size() != 2)
                {
                    throw std::runtime_error("Cannot parse shards range: " + range);
                }

                try
                {
                    auto range_begin = std::stoul(range_bounds[0]);
                    auto range_end = std::stoul(range_bounds[1]);
                    for (auto shard_i = range_begin; shard_i <= range_end; ++shard_i)
                    {
                        shard.shards.push_back(shard_i);
                    }
                }
                catch (const std::exception &)
                {
                    throw std::runtime_error("Cannot parse shards range: " + range);
                }
            }
            else
            {
                try
                {
                    shard.shards.push_back(std::stoul(range));
                }
                catch (const std::exception &)
                {
                    throw std::runtime_error("Cannot parse shards range: " + range);
                }
            }
        }

        std::sort(shard.shards.begin(), shard.shards.end());
        item.store(shard);
    }
    else if (type == AnyItem::VECTOR)
    {
        std::vector<std::string> tmp = utils::split(text, ",");
        for (size_t i = 0; i < tmp.size(); ++i)
        {
            tmp[i] = utils::trimmed(tmp[i]);
            AnyItem any;
            parseFromConfig(any, item.vectorType(), tmp[i]);
            item.pushBack(any);
        }
    }
    else if (type == AnyItem::MAP)
    {
        // skip braces {}
        std::string map_val = text.substr(1, text.size() - 2);

        config::token_t token;
        std::string::size_type prev_pos = 0;
        while (config::get_next_token(map_val, prev_pos, " ", token))
        {
            AnyItem any_key;
            parseFromConfig(any_key, item.mapKeyType(), token.key);

            AnyItem any_val;
            parseFromConfig(any_val, item.mapValueType(), token.value);

            item.insertPair(std::pair<AnyItem, AnyItem>(any_key, any_val));
            token.reset();
        }
    }
}

std::string Config::dump() const
{
    std::stringbuf str;
    std::ostream os(&str);

    size_t max_item_len = 16;
    for (const auto &i : m_Storage.items())
    {
        if (i.second.lkey().size() > max_item_len)
        {
            max_item_len = i.second.lkey().size();
        }
    }

    ++max_item_len;

    os << "Config dump: " << std::endl;
    for (const auto &i : m_Storage.items())
    {
        os << "\t" << std::setw(max_item_len) << std::left << i.second.lkey();
        os << ": " << i.second.value() << std::endl;
    }
    return str.str();
}

std::string Config::usage() const
{
    std::stringstream ss;
    ss << "Config options description: " << std::endl;

    for (const auto &i : m_Storage.items())
    {
        const SettingItem &item = i.second;
        std::string param = item.lkey();

        ss << "\t" << std::setw(24) << std::left << param
           << ": " << item.desc() << std::endl;
    }

    return ss.str();
}

}  // namespace libproperty
