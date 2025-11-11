#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QDir>

class ConfigManager
{
public:
    // 获取单例实例
    static ConfigManager& instance();

    // 读取配置值，如果不存在则返回默认值
    QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant()) const
    {
        return m_settings.value(key, defaultValue);
    }

    // 设置配置值
    void setValue(const QString& key, const QVariant& value)
    {
        m_settings.setValue(key, value);
    }

    // 同步配置到文件
    void sync()
    {
        m_settings.sync();
    }

    // 检查键是否存在
    bool contains(const QString& key) const
    {
        return m_settings.contains(key);
    }

    // 移除配置项
    void remove(const QString& key)
    {
        m_settings.remove(key);
    }

    // 获取所有子键
    QStringList allKeys() const
    {
        return m_settings.allKeys();
    }

    // 获取所有子组
    QStringList childGroups() const
    {
        return m_settings.childGroups();
    }

private:
    ConfigManager()
        : m_settings("config.ini", QSettings::IniFormat)
    {

    }

    ~ConfigManager()
    {
        m_settings.sync();
    }

    QSettings m_settings;
};

inline ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

#endif // CONFIGMANAGER_H
