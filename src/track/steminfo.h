#pragma once

#include <QColor>
#include <QObject>
#include <QString>

class StemInfo {
  public:
    StemInfo(const QString& label = QString(), const QColor& color = QColor())
            : m_label(label), m_color(color) {
    }
    StemInfo(const StemInfo&) = default;

    const QString& getLabel() const {
        return m_label;
    }
    void setLabel(const QString& label) {
        m_label = label;
    }

    const QColor& getColor() const {
        return m_color;
    }
    void setColor(const QColor& color) {
        m_color = color;
    }

    bool isValid() const {
        return !m_label.isEmpty() && m_color.isValid();
    }

  private:
    QString m_label;
    QColor m_color;
};
Q_DECLARE_METATYPE(StemInfo);

std::ostream& operator<<(std::ostream& os, const StemInfo& stemInfo);

QDebug operator<<(QDebug debug, const StemInfo& stemInfo);
