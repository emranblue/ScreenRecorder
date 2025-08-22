#ifndef SELECTIONWIDGET_H
#define SELECTIONWIDGET_H

#include <QWidget>
#include <QRect>

class SelectionWidget : public QWidget {
    Q_OBJECT

public:
    explicit SelectionWidget(QWidget *parent = nullptr);

signals:
    void areaSelected(const QRect &rect);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QRect m_selectionRect;
    QPoint m_startPoint;
    bool m_isSelecting;
};

#endif // SELECTIONWIDGET_H
