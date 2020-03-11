#include "controllers/colormapper.h"

#include <cmath>
#include <cstdint>

#include "util/logger.h"

namespace {

const mixxx::Logger kLogger("ColorMapper");

double colorDistance(QRgb a, QRgb b) {
    // This algorithm calculates the distance between two colors. In
    // contrast to the L2 norm, this also tries take the human perception
    // of colors into account. More accurate algorithms like the CIELAB2000
    // Delta-E rely on sophisticated color space conversions and need a lot
    // of costly computations. In contrast, this is a low-cost
    // approximation and should be sufficently accurate.
    // More details: https://www.compuphase.com/cmetric.htm
    long mean_red = (static_cast<long>(qRed(a)) + static_cast<long>(qRed(b))) / 2;
    long delta_red = static_cast<long>(qRed(a)) - static_cast<long>(qRed(b));
    long delta_green = static_cast<long>(qGreen(a)) - static_cast<long>(qGreen(b));
    long delta_blue = static_cast<long>(qBlue(a)) - static_cast<long>(qBlue(b));
    return sqrt(
            (((512 + mean_red) * delta_red * delta_red) >> 8) +
            (4 * delta_green * delta_green) +
            (((767 - mean_red) * delta_blue * delta_blue) >> 8));
}

} // namespace

QRgb ColorMapper::getNearestColor(QRgb desiredColor) {
    // If desired color is already in cache, use cache entry
    QMap<QRgb, QRgb>::const_iterator i = m_cache.constFind(desiredColor);
    QMap<QRgb, QVariant>::const_iterator j;
    if (i != m_cache.constEnd()) {
        j = m_availableColors.find(i.value());
        DEBUG_ASSERT(j != m_availableColors.constEnd());
        kLogger.trace()
                << "ColorMapper cache hit for" << desiredColor << ":"
                << "Color =" << j.key();
        return j.key();
    }

    // Color is not cached
    QMap<QRgb, QVariant>::const_iterator nearestColorIterator;
    double nearestColorDistance = qInf();
    for (j = m_availableColors.constBegin(); j != m_availableColors.constEnd(); j++) {
        QRgb availableColor = j.key();
        double distance = colorDistance(desiredColor, availableColor);
        if (distance < nearestColorDistance) {
            nearestColorDistance = distance;
            nearestColorIterator = j;
        }
    }

    kLogger.trace()
            << "ColorMapper found matching color for" << desiredColor << ":"
            << "Color =" << nearestColorIterator.key() << ","
            << "Value =" << nearestColorIterator.value();
    m_cache.insert(desiredColor, nearestColorIterator.key());
    return nearestColorIterator.key();
}

QVariant ColorMapper::getValueForNearestColor(QRgb desiredColor) {
    return m_availableColors.value(getNearestColor(desiredColor));
}
