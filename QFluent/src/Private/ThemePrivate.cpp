#include "ThemePrivate.h"

#include "Theme.h"

ThemePrivate::ThemePrivate()
{

}

QColor ThemePrivate::calculateThemeColor(ThemeType::ThemeColor type) const {
    Q_Q_CONST(Theme);

    QColor color = _themeColor;
    float h, s, v, a;
    color.getHsvF(&h, &s, &v, &a);

    if (_currentTheme == ThemeType::ThemeMode::DARK) {
        s *= 0.84f;
        v = 1.0f;
        switch (type) {
        case ThemeType::ThemeColor::DARK_1: v *= 0.9f; break;
        case ThemeType::ThemeColor::DARK_2: s *= 0.977f; v *= 0.82f; break;
        case ThemeType::ThemeColor::DARK_3: s *= 0.95f; v *= 0.7f; break;
        case ThemeType::ThemeColor::LIGHT_1: s *= 0.92f; break;
        case ThemeType::ThemeColor::LIGHT_2: s *= 0.78f; break;
        case ThemeType::ThemeColor::LIGHT_3: s *= 0.65f; break;
        default: break;
        }
    } else {
        switch (type) {
        case ThemeType::ThemeColor::DARK_1: v *= 0.75f; break;
        case ThemeType::ThemeColor::DARK_2: s *= 1.05f; v *= 0.5f; break;
        case ThemeType::ThemeColor::DARK_3: s *= 1.1f; v *= 0.4f; break;
        case ThemeType::ThemeColor::LIGHT_1: v *= 1.05f; break;
        case ThemeType::ThemeColor::LIGHT_2: s *= 0.75f; v *= 1.05f; break;
        case ThemeType::ThemeColor::LIGHT_3: s *= 0.65f; v *= 1.05f; break;
        default: break;
        }
    }

    return QColor::fromHsvF(h, qMin(s, 1.0f), qMin(v, 1.0f));
}
