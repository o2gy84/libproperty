#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "options.hpp"
#include "utils.hpp"


namespace libproperty
{

Options* Options::m_Self = nullptr;

Options* Options::impl()
{
    if (m_Self == NULL)
    {
        m_Self = new Options();
    }
    return m_Self;
}

void Options::destroy()
{
    delete m_Self;
    m_Self = nullptr;
}


std::string Options::usage(const std::string &progname) const
{
    std::stringstream ss;
    ss << "Usage: " << progname;

    std::set<std::string> long_keys;

    for (const auto &i : m_Storage.items())
    {
        const SettingItem &item = i.second;
        if (item.key().empty())
        {
            long_keys.insert(item.lkey());
        }
        else
        {
            ss << " -" << item.key();
        }
    }

    for (const auto &lkey : long_keys)
    {
        ss << " --" << lkey;
    }

    ss << std::endl << std::endl;

    size_t max_item_size = 24;
    std::map<std::string, std::string> short_keys_desc;
    std::map<std::string, std::string> long_keys_desc;

    for (const auto &i : m_Storage.items())
    {
        const SettingItem &item = i.second;
        std::string desc;
        if (item.key().empty())
        {
            desc = "--" + item.lkey();
            long_keys_desc[desc] = item.desc();
        }
        else
        {
            desc = "-" + item.key() + " [ --" + item.lkey() + " ] ";
            short_keys_desc[desc] = item.desc();
        }

        if (desc.size() > max_item_size)
        {
            max_item_size = desc.size();
        }
    }

    max_item_size += 1;
    for (const auto &pair : short_keys_desc)
    {
        ss << "\t" << std::setw(max_item_size) << std::left << pair.first
           << ": " << pair.second << std::endl;
    }

    for (const auto &pair : long_keys_desc)
    {
        ss << "\t" << std::setw(max_item_size) << std::left << pair.first
           << ": " << pair.second << std::endl;
    }

    return ss.str();
}

std::string Options::dump() const
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

    os << "Options dump: " << std::endl;
    for (const auto &i : m_Storage.items())
    {
        os << "\t" << std::setw(max_item_len) << std::left << i.second.lkey();
        os << ": " << i.second.value() << std::endl;
    }
    return str.str();
}

void Options::parse(int count, const char *const *args)
{
    int counter = 1;

    while (true)
    {
        if (counter >= count) break;

        std::string tmp(args[counter]);
        int hyphen_counter = 0;
        while (tmp.size() && tmp[hyphen_counter] == '-')
        {
            ++hyphen_counter;
        }

        if (hyphen_counter == 0 || hyphen_counter > 2)
        {
            std::string wk = "wrong key: " + std::string(args[counter]);
            std::cerr << usage(args[0]) << std::endl;
            throw std::runtime_error(wk);
        }

        std::string key = tmp.substr(hyphen_counter, std::string::npos);
        std::string::size_type eq_pos = key.find("=");
        if (eq_pos != std::string::npos)
        {
            key = key.substr(0, eq_pos);
        }

        if (hyphen_counter == 1)    // short key
        {
            try
            {
                const SettingItem &item = m_Storage.find_option_by_short_key(key);
                key = item.lkey();
            }
            catch (...)
            {
                std::string wk = "wrong key (not registered): " + std::string(args[counter]);
                std::cerr << usage(args[0]) << std::endl;
                throw std::runtime_error(wk);
            }
        }

        std::pair<SettingItem &, bool> item = m_Storage.find_option_by_long_key(key);
        if (!item.second)
        {
            std::string wk = "wrong key (not registered): " + std::string(args[counter]);
            std::cerr << usage(args[0]) << std::endl;
            throw std::runtime_error(wk);
        }

        counter = parseFromProgrammOptions(item.first, counter, count, args);
    }
}

int Options::parseFromProgrammOptions(SettingItem &item, int cur_counter, int total_opts, const char *const *args)
{
    int next_args_counter = cur_counter + 1;

    if (item.value().type() == AnyItem::BOOL)
    {
        // positional option - just yes or no
        item.value().store(true);
        return next_args_counter;
    }

    std::string value;

    std::string tmp(args[cur_counter]);
    std::string::size_type eq_pos = tmp.find("=");
    if (eq_pos != std::string::npos)
    {
        value = tmp.substr(eq_pos + 1, std::string::npos);
        if (value.empty())
        {
            throw std::runtime_error("bad options");
        }
    }
    else
    {
        // TODO: format without space? like " -l5 -I/usr/local/include"
        if (cur_counter + 1 >= total_opts)
        {
            throw std::runtime_error("bad options");
        }

        value = std::string(args[cur_counter + 1]);
        next_args_counter = cur_counter + 2;
    }

    if (item.value().type() == AnyItem::INT)
    {
        int v = std::stoi(value);
        item.value().store(v);
        return next_args_counter;
    }
    else if (item.value().type() == AnyItem::STRING)
    {
        item.value().store(value);
        return next_args_counter;
    }

    throw std::runtime_error("bad options");
    return -1;
}

}  // namespace libproperty
