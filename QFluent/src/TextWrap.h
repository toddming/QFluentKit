#pragma once

#include <QString>
#include <QScopedPointer>

#include "FluentGlobal.h"

class TextWrapPrivate;

class QFLUENT_EXPORT TextWrap
{
public:
    static std::pair<QString, bool> wrap(const QString &text, int width, bool once = true);

    // 可选:设置缓存大小(单位:条目数)
    static void setCacheSize(int size);
    static void clearCache(); // 清空缓存

private:
    Q_DISABLE_COPY(TextWrap)
    Q_DECLARE_PRIVATE(TextWrap)

    TextWrap();
    ~TextWrap();

    // 获取单例实例
    static TextWrap* instance();

    QScopedPointer<TextWrapPrivate> d_ptr;
};

