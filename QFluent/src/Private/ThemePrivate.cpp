#include "ThemePrivate.h"
#include "Theme.h"

ThemePrivate::ThemePrivate()
{

}

QColor ThemePrivate::calculateThemeColor(Fluent::ThemeColor type) const {
    Q_Q(const Theme);

    QColor color = m_themeColor;
    qreal h, s, v, a;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    float h_f, s_f, v_f, a_f;
    color.getHsvF(&h_f, &s_f, &v_f, &a_f);
    h = h_f; s = s_f; v = v_f; a = a_f;
#else
    color.getHsvF(&h, &s, &v, &a);
#endif

    if (m_currentTheme == Fluent::ThemeMode::DARK) {
        s *= 0.84f;
        v = 1.0f;
        switch (type) {
        case Fluent::ThemeColor::DARK_1: v *= 0.9; break;
        case Fluent::ThemeColor::DARK_2: s *= 0.977; v *= 0.82; break;
        case Fluent::ThemeColor::DARK_3: s *= 0.95; v *= 0.7; break;
        case Fluent::ThemeColor::LIGHT_1: s *= 0.92; break;
        case Fluent::ThemeColor::LIGHT_2: s *= 0.78; break;
        case Fluent::ThemeColor::LIGHT_3: s *= 0.65; break;
        default: break;
        }
    } else {
        switch (type) {
        case Fluent::ThemeColor::DARK_1: v *= 0.75; break;
        case Fluent::ThemeColor::DARK_2: s *= 1.05; v *= 0.5; break;
        case Fluent::ThemeColor::DARK_3: s *= 1.1; v *= 0.4; break;
        case Fluent::ThemeColor::LIGHT_1: v *= 1.05; break;
        case Fluent::ThemeColor::LIGHT_2: s *= 0.75; v *= 1.05; break;
        case Fluent::ThemeColor::LIGHT_3: s *= 0.65; v *= 1.05; break;
        default: break;
        }
    }

    return QColor::fromHsvF(h, qMin(s, 1.0), qMin(v, 1.0));
}
