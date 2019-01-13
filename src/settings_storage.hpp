#pragma once

#include <map>
#include <set>

#include "settings.hpp"

class SettingsStorage
{
public:
    template <typename T>
    void addValueByKey(const std::string &lk, const std::string &k, const std::string &desc, T&& default_value)
    {
        if (m_Items.find(lk) != m_Items.end())
        {
            throw std::runtime_error("options key already registered: " + lk);
        }

        m_Items[lk] = SettingItem(lk, k, desc, std::forward<T>(default_value));
    }

    std::pair<SettingItem&, bool> findOptionByLongKey(const std::string &lk) const noexcept;
    const SettingItem& findOptionByShortKey(const std::string &k) const;

    const std::map<std::string, SettingItem>& items() const { return  m_Items; }

private:
    std::map<std::string, SettingItem> m_Items;     // long key and Item
};
