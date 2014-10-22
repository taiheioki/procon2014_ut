#ifndef GRAPHICSITEMSLIDEBOARD_HPP
#define GRAPHICSITEMSLIDEBOARD_HPP

#include <sstream>

#include <QObject>
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QTimeLine>

#include <boost/optional.hpp>

#include "graphicsitemslideblock.hpp"
#include "slide/Board.hpp"
#include "util/define.hpp"

namespace slide{
namespace ui{

class GraphicsItemSlideBoard : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit GraphicsItemSlideBoard(int H, int W, int HSIZE, int WSIZE, slide::Board<slide::Flexible> *board,
                               int pen_width = 1, QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *paniter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    boost::optional<GraphicsItemSlideBlock *> find_around_selected_block(int y, int x) const;
    void click(int y, int x);
    void shuffle();
    void shuffle2();
    void animated_swap(GraphicsItemSlideBlock *l, GraphicsItemSlideBlock *r);
    void swap(GraphicsItemSlideBlock *l, GraphicsItemSlideBlock *r);
    bool boundary_check(int y, int x) const;
    void set_animation_time(int t);
    void select(int y, int x);
    int get_selected_block_pos() const {
        return selected_block->y * MAX_DIVISION_NUM + selected_block->x;
    }
    void move_blocks(const slide::Answer &ans);
    void move_blocks_with_no_animation();
    void change_selected_block();
    void set_solving_flag(const bool f);
    void change_color(bool color);

    // ブロックの数
    int H,W;
    // ブロックのサイズ
    int HSIZE,WSIZE;

    slide::Board<slide::Flexible> *board; // swapに必要
    int pen_width;
    int animation_time;
    int shuffle_count;
    QPen pen;
    QBrush brush;
    GraphicsItemSlideBlock *blocks[MAX_DIVISION_NUM * MAX_DIVISION_NUM];
private:
    void next_shuffle();
    int next_move_step();
    void next_move_step_with_animated_swap();
    GraphicsItemSlideBlock *selected_block;
    QTimeLine *timer;
    const int SHUFFLE_MAX = 100;
    bool is_shuffling;
    bool is_moving;
    bool is_selecting;
    bool is_solving;
    bool is_clicking;
    std::stringstream ss;
    int selection_cnt,selection_max;
    int swap_cnt,swap_max;
    int prev_shuffle_dir;

signals:
    void shuffle_finished();
    void move_blocks_finished();
    void change_selected_block_finished();
    void swap_finished();

private slots:
    void slot_swap_finished();
};

} // end of namespace ui
} // end of namespace slide

#endif // GRAPHICSITEMSLIDEBOARD_HPP
