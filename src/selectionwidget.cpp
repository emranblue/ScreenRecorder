#include "selectionwidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QGuiApplication>

SelectionWidget::SelectionWidget(QWidget *parent)
    : QWidget(parent), m_isSelecting(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::CrossCursor);

    QScreen *screen = QGuiApplication::primaryScreen();
    setGeometry(screen->geometry());
}

void SelectionWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isSelecting = true;
        m_startPoint = event->pos();
        m_selectionRect = QRect(m_startPoint, QSize());
        update();
    }
}

void SelectionWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isSelecting) {
        m_selectionRect = QRect(m_startPoint, event->pos()).normalized();
        update();
    }
}

void SelectionWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_isSelecting) {
        m_isSelecting = false;
        emit areaSelected(m_selectionRect);
        close();
    }
}

void SelectionWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Semi-transparent overlay
    painter.fillRect(rect(), QColor(0, 0, 0, 120));

    if (m_isSelecting) {
        // Clear the selected area
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(m_selectionRect, Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        // Draw the border of the selection
        painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
        painter.drawRect(m_selectionRect);
    }
}
