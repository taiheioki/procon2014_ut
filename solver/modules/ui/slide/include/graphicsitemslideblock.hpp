#ifndef GLAPHICSITEMSLIDEBLOCK_HPP
#define GLAPHICSITEMSLIDEBLOCK_HPP

#include <QGraphicsItem>
#include <QString>
#include <QPen>

#include <algorithm>
#include <utility>
#include <boost/optional.hpp>

#include "util/define.hpp"

namespace slide{
namespace ui{

class GraphicsItemSlideBlock : public QGraphicsItem
{
public:
    explicit GraphicsItemSlideBlock(qreal orig_x, qreal orig_y, qreal x, qreal y, qreal width, qreal height,
                   QString text, QPen pen,
                   qreal pen_width = 1, QGraphicsItem *parent = 0);
    explicit GraphicsItemSlideBlock(qreal orig_x, qreal orig_y, qreal x, qreal y, qreal width, qreal height,
                   QString text,
                   qreal pen_width = 1, QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *paniter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void set_selected(bool f);
    void swap(int dx, int dy);
    bool is_selected() const { return selected; }
    boost::optional<GraphicsItemSlideBlock *> find_around_selected_block(int x,int y) const;

    qreal orig_x,orig_y;
    qreal x,y;
    qreal width,height;
    QString text;
    QPen pen;
    qreal pen_width;
    bool is_selecting;
    bool mouse_hover;
    bool distance_color;
    int swapped_count;

private:
    bool selected;
};

} // end of namespace ui
} // end of namespace slide

#endif // GRAPHICSITEMSLIDEBLOCK_HPP
