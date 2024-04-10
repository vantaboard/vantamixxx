#include "track/steminfo.h"

#include <QDebugStateSaver>

std::ostream& operator<<(std::ostream& os, const StemInfo& stemInfo) {
    return os
            << "StemInfo{"
            << stemInfo.getLabel()
            << ','
            << stemInfo.getColor().name()
            << '}';
}

QDebug operator<<(QDebug dbg, const StemInfo& stemInfo) {
    const QDebugStateSaver saver(dbg);
    dbg = dbg.maybeSpace() << "StemInfo";
    return dbg.nospace()
            << '{'
            << stemInfo.getLabel()
            << ','
            << stemInfo.getColor().name()
            << '}';
}
