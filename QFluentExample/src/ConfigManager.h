#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QDir>
#include <QOperatingSystemVersion>

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


    bool isWin11()
    {
#if defined(Q_OS_WIN)
        // 方法1：通过注册表读取 CurrentBuild（推荐，简单可靠）
        QSettings reg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion)",
                      QSettings::NativeFormat);
        bool ok = false;
        int buildNumber = reg.value("CurrentBuild").toInt(&ok);

        if (ok && buildNumber >= 22000) {
            return true;
        }

        // 备用：尝试用 QOperatingSystemVersion（Qt5.9+ 支持，但 Win11 可能被报告为 Win10）
        // 注意：Qt5 中 QOperatingSystemVersion::Windows10 实际包含 Win11
        auto current = QOperatingSystemVersion::current();
        if (current.type() == QOperatingSystemVersion::Windows) {
            // Qt6 开始提供 build()，Qt5 没有
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            return current.microVersion() >= 22000; // microVersion() = build number in Qt6
#else
            // Qt5 无法直接获取 build，只能依赖注册表（上面已处理）
            return false;
#endif
        }
#endif

        // 非 Windows 系统
        return false;
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
