#include "TextWrap.h"
#include "Private/TextWrapPrivate.h"

// ================ TextWrap 公共接口实现 ================
TextWrap::TextWrap()
    : d_ptr(new TextWrapPrivate(this))
{
}

TextWrap::~TextWrap()
{
}

TextWrap* TextWrap::instance()
{
    static TextWrap inst;
    return &inst;
}

std::pair<QString, bool> TextWrap::wrap(const QString &text, int width, bool once)
{
    return instance()->d_func()->wrapImpl(text, width, once);
}

void TextWrap::setCacheSize(int size)
{
    TextWrap* inst = instance();
    inst->d_func()->widthCache.setMaxCost(size);
    inst->d_func()->charWidthCache.setMaxCost(size * 2);
    inst->d_func()->tokenizeCache.setMaxCost(size / 2);
    inst->d_func()->splitCache.setMaxCost(size / 2);
}

void TextWrap::clearCache()
{
    TextWrap* inst = instance();
    inst->d_func()->widthCache.clear();
    inst->d_func()->charWidthCache.clear();
    inst->d_func()->tokenizeCache.clear();
    inst->d_func()->splitCache.clear();
}
