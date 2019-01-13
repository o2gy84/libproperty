#include "settings_storage.hpp"

std::pair<SettingItem&, bool> SettingsStorage::findOptionByLongKey(const std::string &lk) const noexcept
{
    SettingItem tmp;
    if (lk.empty())
    {
        return std::pair<SettingItem&, bool>(tmp, false);
    }

    std::map<std::string, SettingItem>::const_iterator it = m_Items.find(lk);
    if (it == m_Items.end())
    {
        return std::pair<SettingItem&, bool>(tmp, false);
    }

    SettingItem &item = const_cast<SettingItem&>(it->second);
    return std::pair<SettingItem&, bool>(item, true);
}

const SettingItem& SettingsStorage::findOptionByShortKey(const std::string &k) const
{
    if (k.empty())
    {
        throw std::runtime_error("request for empty option");
    }

    for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
    {
        const SettingItem &item = it->second;
        if (item.key() == k)
        {
            return item;
        }
    }

    throw std::runtime_error("no such option: " + k);
}
