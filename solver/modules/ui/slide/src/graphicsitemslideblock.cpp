#include <QColor>
#include <QGraphicsItem>
#include <QRectF>
#include <QPen>
#include <QPainter>
#include <QString>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QDebug>
#include <QEvent>

#include <boost/assert.hpp>
#include <boost/optional.hpp>

#include "graphicsitemslideblock.hpp"
#include "graphicsitemslideboard.hpp"
#include "util/define.hpp"

namespace slide{
namespace ui{

GraphicsItemSlideBlock::GraphicsItemSlideBlock(qreal orig_x_, qreal orig_y_, qreal x_, qreal y_,
                                               qreal width_, qreal height_,
                                               QString text_, QPen pen_,
                                               qreal pen_width_, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    orig_x(orig_x_),
    orig_y(orig_y_),
    x(x_),
    y(y_),
    width(width_),
    height(height_),
    text(text_),
    pen(pen_),
    pen_width(pen_width_),
    is_selecting(false),
    mouse_hover(false),
    swapped_count(0),
    selected(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}

GraphicsItemSlideBlock::GraphicsItemSlideBlock(qreal orig_x_, qreal orig_y_, qreal x_, qreal y_,
                                               qreal width_, qreal height_, QString text_,
                                               qreal pen_width_, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    orig_x(orig_x_),
    orig_y(orig_y_),
    x(x_),
    y(y_),
    width(width_),
    height(height_),
    text(text_),
    pen_width(pen_width_),
    is_selecting(false),
    mouse_hover(false),
    distance_color(true),
    swapped_count(0),
    selected(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    pen.setColor(QColor(0,0,255,100));
}

QRectF GraphicsItemSlideBlock::boundingRect() const
{
    return QRectF(-pen_width/2,-pen_width/2,width + pen_width/2,height + pen_width/2);
}

void GraphicsItemSlideBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(pen);
    if(distance_color){
        int manhattan = std::abs(orig_x-x) + std::abs(orig_y-y);
        GraphicsItemSlideBoard *p = qgraphicsitem_cast<GraphicsItemSlideBoard*>(parentItem());
        BOOST_ASSERT(p != nullptr);
        int max_manhattan = p->W+ p->H - 2;
        double rate = manhattan / (double)max_manhattan;
        if(selected){
            painter->setBrush(QColor(0,255,0,100));
        }else
        if(manhattan == 0){
            painter->setBrush(QColor(255, 183, 76,200));
        }else{
            if(rate > 0.50){
                painter->setBrush(QColor(std::cbrt((1-rate)*2) * 255, std::cbrt((1-rate)*2) * 255, 255,100));
            }else{
                painter->setBrush(QColor(255, std::cbrt(rate*2) * 255, std::cbrt(rate*2) * 255,100));
            }
        }
    }else{
        if(mouse_hover){
          painter->setBrush(QColor(0,255,255,100));
        }else if(!selected && orig_x == x && orig_y == y){
            if(swapped_count == 0){
                painter->setBrush(QColor(255,0,0,70));
            }else{
                painter->setBrush(QColor(000,255,0,70));
            }
        }else if(selected){
          painter->setBrush(QColor(0,255,255,100));
        }else{
          painter->setBrush(Qt::white);
        }
    }
    painter->drawRect(0,0,width,height);
    painter->setPen(QColor(0,0,0,255));
    painter->drawText(width/2-text.length()*2,height/2,text);
}

void GraphicsItemSlideBlock::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    //qDebug() << "click: " << y << "," << x;
    GraphicsItemSlideBoard *p = qgraphicsitem_cast<GraphicsItemSlideBoard*>(parentItem());
    BOOST_ASSERT(p != nullptr);
    p->click(y,x);
}

void GraphicsItemSlideBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void GraphicsItemSlideBlock::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if(is_selecting){
        mouse_hover = true;
        update(boundingRect());
    }
}

void GraphicsItemSlideBlock::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if(is_selecting){
        mouse_hover = false;
        update(boundingRect());
    }
}

void GraphicsItemSlideBlock::set_selected(bool f)
{
    selected = f;
    update(boundingRect());
}

} // end of namespace ui
} // end of namespace slide
