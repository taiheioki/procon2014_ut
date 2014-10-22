#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <sstream>

#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QGraphicsItemAnimation>
#include <QtConcurrent>
#include <QTimeLine>
#include <QThread>

#include <boost/optional.hpp>

#include "graphicsitemslideblock.hpp"
#include "graphicsitemslideboard.hpp"
#include "slide/Answer.hpp"
#include "slide/Board.hpp"
#include "util/define.hpp"

namespace slide{
namespace ui{

GraphicsItemSlideBoard::GraphicsItemSlideBoard(int H_, int W_, int HSIZE_, int WSIZE_, slide::Board<slide::Flexible> *board_,
                                     int pen_width_, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    H(H_),
    W(W_),
    HSIZE(HSIZE_),
    WSIZE(WSIZE_),
    board(board_),
    pen_width(pen_width_),
    pen(Qt::black),
    brush(Qt::white),
    timer(nullptr),
    is_shuffling(false),
    is_moving(false),
    is_selecting(false),
    is_solving(false),
    is_clicking(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);

    std::fill(&blocks[0],&blocks[MAX_DIVISION_NUM * MAX_DIVISION_NUM],nullptr);
    for(int y = 0; y < H; y++){
        for(int x = 0; x < W; x++){
            uchar p = (*board)(y,x);
            int x_,y_;
            y_ = p / MAX_DIVISION_NUM;
            x_ = p % MAX_DIVISION_NUM;
            GraphicsItemSlideBlock *block = new GraphicsItemSlideBlock(x_,y_,x,y,WSIZE,HSIZE,
                                                QString("%1").arg(p,2,16,QChar('0')).toUpper(),0,this);
            block->setPos(x*WSIZE,y*HSIZE);
            block->setZValue(2);
            if(y == H-1 && x == W-1){
                block->set_selected(true);
                block->setZValue(1);
                selected_block = block;
            }
            blocks[y*MAX_DIVISION_NUM+x] = block;
            //qDebug() << "(" << y << "," << x << "): " << QString("%1").arg(p,2,16,QChar('0'));
        }
    }
    connect(this,SIGNAL(swap_finished()),this,SLOT(slot_swap_finished()));
}

QRectF GraphicsItemSlideBoard::boundingRect() const
{
    return QRectF(-pen_width/2,-pen_width/2,W*WSIZE + pen_width/2,H*HSIZE + pen_width/2);
}

void GraphicsItemSlideBoard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
    //painter->setPen(pen);
    //painter->setBrush(brush);
    //painter->drawRect(0,0,W*WSIZE,H*HSIZE);
}

void GraphicsItemSlideBoard::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void GraphicsItemSlideBoard::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void GraphicsItemSlideBoard::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

void GraphicsItemSlideBoard::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

bool GraphicsItemSlideBoard::boundary_check(int y, int x) const
{
    return x >= 0 && y >= 0 && x < MAX_DIVISION_NUM && y < MAX_DIVISION_NUM;
}

boost::optional<GraphicsItemSlideBlock *> GraphicsItemSlideBoard::find_around_selected_block(int y, int x) const
{
    int me    = y*MAX_DIVISION_NUM + x;
    int up    = (y-1)*MAX_DIVISION_NUM + x;
    int down  = (y+1)*MAX_DIVISION_NUM + x;
    int left  = y*MAX_DIVISION_NUM + x-1;
    int right = y*MAX_DIVISION_NUM + x+1;

    if(blocks[me] == nullptr || blocks[me]->is_selected()){
        return boost::none;
    }
    // up
    if(boundary_check(y-1,x) && blocks[up] != nullptr && blocks[up]->is_selected()){
        return blocks[up];
    }
    // down
    if(boundary_check(y+1,x) && blocks[down] != nullptr && blocks[down]->is_selected()){
        return blocks[down];
    }
    // left
    if(boundary_check(y,x-1) && blocks[left] != nullptr && blocks[left]->is_selected()){
        return blocks[left];
    }
    // right
    if(boundary_check(y,x+1) && blocks[right] != nullptr && blocks[right]->is_selected()){
        return blocks[right];
    }

    return boost::none;
}

// called from GraphicsItemSlideBlock's mousePressEvent
void GraphicsItemSlideBoard::click(int y, int x)
{
    if(is_clicking || is_solving || is_moving || is_shuffling){
        return;
    }
    if(boundary_check(y,x)){
        if(is_selecting){
            is_selecting = false;
            QList<QGraphicsItem *> child = childItems();
            for(int i = 0;i < child.size(); i++){
                GraphicsItemSlideBlock *b = qgraphicsitem_cast<GraphicsItemSlideBlock*>(child.at(i));
                BOOST_ASSERT(b != nullptr);
                b->is_selecting = false;
                b->mouse_hover = false;
            }
            select(y,x);
            emit change_selected_block_finished();
        }else{
            boost::optional<GraphicsItemSlideBlock*> selected_block = find_around_selected_block(y,x);
            if(selected_block != boost::none){
                GraphicsItemSlideBlock *s = selected_block.get();
                is_clicking = true;
                animated_swap(blocks[y*MAX_DIVISION_NUM+x],s);
            }
        }
    }
}

void GraphicsItemSlideBoard::shuffle()
{
    srand(time(nullptr));
    shuffle_count = 0;
    is_shuffling = true;
    prev_shuffle_dir = 0;
    next_shuffle();
}

void GraphicsItemSlideBoard::next_shuffle()
{
    int dir,dy,dx,idx;
    while(true){
        dir = rand()%5;
        switch(dir){
            case 0: // up
                if(prev_shuffle_dir == 1) continue;
                dy = selected_block->y-1;
                dx = selected_block->x;
                break;
            case 1: // down
                if(prev_shuffle_dir == 0) continue;
                dy = selected_block->y+1;
                dx = selected_block->x;
                break;
            case 2: // left
                if(prev_shuffle_dir == 3) continue;
                dy = selected_block->y;
                dx = selected_block->x-1;
                break;
            case 3: // right
                if(prev_shuffle_dir == 2) continue;
                dy = selected_block->y;
                dx = selected_block->x+1;
                break;
            default: // change selected block
                dy = rand()%H;
                dx = rand()%W;
                //qDebug() << "change selected:" << dy << "," << dx;
                select(dy,dx);
                continue;
        }
        idx = dy*MAX_DIVISION_NUM + dx;
        if(boundary_check(dy,dx) && blocks[idx] != nullptr){
            prev_shuffle_dir = dir;
            break;
        }
    }
    swap(blocks[idx],selected_block);
}

// no animation
void GraphicsItemSlideBoard::shuffle2()
{
    srand(time(nullptr));
    const int save_animation_time = animation_time;
    animation_time = 0;
    const int N = H*W;
    for(int i = 0; i < N; i++){
        const int j = rand() % (N-i) + i;
        GraphicsItemSlideBlock *l = blocks[(i/W)*MAX_DIVISION_NUM+i%W];
        GraphicsItemSlideBlock *r = blocks[(j/W)*MAX_DIVISION_NUM+j%W];
        swap(l,r);
    }
    animation_time = save_animation_time;
    emit shuffle_finished();
}


void GraphicsItemSlideBoard::slot_swap_finished()
{
    if(is_shuffling){
        if(shuffle_count < SHUFFLE_MAX){
            shuffle_count += 1;
            next_shuffle();
        }else{
            is_shuffling = false;
            emit shuffle_finished();
        }
    }else if(is_moving){
        if(selection_cnt >= selection_max && swap_cnt >= swap_max){
            is_moving = false;
            emit move_blocks_finished();
        }else{
            if(animation_time <= 0){
                move_blocks_with_no_animation();
            }else{
                next_move_step_with_animated_swap();
            }
        }
    }else if(is_clicking){
        is_clicking = false;
    }
}

void GraphicsItemSlideBoard::swap(GraphicsItemSlideBlock *l, GraphicsItemSlideBlock *r)
{
    BOOST_ASSERT(l != nullptr);
    BOOST_ASSERT(r != nullptr);
    int a = l->y*MAX_DIVISION_NUM + l->x;
    int b = r->y*MAX_DIVISION_NUM + r->x;

    std::swap((*board).at(l->y, l->x),(*board).at(r->y, r->x));
    std::swap(l->x,r->x);
    std::swap(l->y,r->y);
    std::swap(blocks[a],blocks[b]);
    l->setPos(l->x*l->width,l->y*l->height);
    r->setPos(r->x*r->width,r->y*r->height);
}

void GraphicsItemSlideBoard::animated_swap(GraphicsItemSlideBlock *l, GraphicsItemSlideBlock *r)
{
    BOOST_ASSERT(l != nullptr);
    BOOST_ASSERT(r != nullptr);
    int a = l->y*MAX_DIVISION_NUM + l->x;
    int b = r->y*MAX_DIVISION_NUM + r->x;

    if(animation_time > 10){
        if(timer){
            delete timer;
        }
        timer = new QTimeLine(animation_time);
        QGraphicsItemAnimation *animation1 = new QGraphicsItemAnimation;
        QGraphicsItemAnimation *animation2 = new QGraphicsItemAnimation;
        animation1->setItem(l);
        animation2->setItem(r);
        animation1->setTimeLine(timer);
        animation2->setTimeLine(timer);
        const int N = std::max((int)(animation_time / 1000.0 *60),1);
        timer->setFrameRange(0,N);
        for(int i = 0;i <= N;i++){
            animation1->setPosAt(i/(double)N,
                                 QPointF(l->x*WSIZE + WSIZE/(double)N*(r->x-l->x)*i,
                                 l->y*HSIZE + HSIZE/(double)N*(r->y-l->y)*i));
            animation2->setPosAt(i/(double)N,
                                 QPointF(r->x*WSIZE + WSIZE/(double)N*(l->x-r->x)*i,
                                         r->y*HSIZE + HSIZE/(double)N*(l->y-r->y)*i));
        }
        std::swap((*board).at(l->y, l->x),(*board).at(r->y, r->x));
        std::swap(l->x,r->x);
        std::swap(l->y,r->y);
        std::swap(blocks[a],blocks[b]);
        connect(timer,SIGNAL(finished()),this,SLOT(slot_swap_finished()));
        timer->start();
    }else{
        // this cause some kind of animation because of signal / slot delay
        std::swap((*board).at(l->y, l->x),(*board).at(r->y, r->x));
        std::swap(l->x,r->x);
        std::swap(l->y,r->y);
        std::swap(blocks[a],blocks[b]);
        l->setPos(l->x*l->width,l->y*l->height);
        r->setPos(r->x*r->width,r->y*r->height);
        QtConcurrent::run([&](GraphicsItemSlideBoard* b){emit b->swap_finished();},this);
    }
}

void GraphicsItemSlideBoard::set_animation_time(int t)
{
    BOOST_ASSERT(t >= 0);
    animation_time = t;
}

void GraphicsItemSlideBoard::select(int y, int x)
{
    selected_block->set_selected(false);
    blocks[y*MAX_DIVISION_NUM + x]->set_selected(true);
    selected_block = blocks[y*MAX_DIVISION_NUM + x];
}

void GraphicsItemSlideBoard::set_solving_flag(const bool f)
{
    is_solving = f;
}

void GraphicsItemSlideBoard::move_blocks(const slide::Answer &ans)
{
    qDebug() << "move blocks";
    ss.clear();
    ss << ans.toString();
    is_moving = true;
    ss >> std::dec >> selection_max;
    if(selection_max <= 0){
        emit move_blocks_finished();
        return;
    }
    swap_cnt = 0;
    swap_max = -1;
    selection_cnt = 0;
    if(animation_time > 0){
        next_move_step_with_animated_swap();
    }else{
        move_blocks_with_no_animation();
    }
}

void GraphicsItemSlideBoard::move_blocks_with_no_animation()
{
    while(true){
        if(selection_cnt >= selection_max && swap_cnt >= swap_max){
            break;
        }
        int idx = next_move_step();
        blocks[idx]->swapped_count -= 1;
        selected_block->swapped_count -= 1;
        swap(blocks[idx],selected_block);
    }
    emit swap_finished();
}


void GraphicsItemSlideBoard::next_move_step_with_animated_swap()
{
    int idx = next_move_step();
    blocks[idx]->swapped_count -= 1;
    selected_block->swapped_count -= 1;
    animated_swap(blocks[idx],selected_block);
}

int GraphicsItemSlideBoard::next_move_step()
{
    if(swap_max < 0 || swap_cnt >= swap_max){
        int pos,y,x;
        ss >> std::hex >> pos;
        ss >> std::dec >> swap_max;
        swap_cnt = 0;
        selection_cnt += 1;
        y = pos % MAX_DIVISION_NUM;
        x = pos / MAX_DIVISION_NUM;
        //qDebug() << "select " << pos << ": " << y << "," << x;
        //qDebug() << "swap max:" << swap_max;
        select(y,x);
    }
    char c;
    int idx;
    swap_cnt += 1;
    ss >> c;
    switch(toupper(c)){
        case 'U':
            idx = (selected_block->y-1)*MAX_DIVISION_NUM + (selected_block->x);
            break;
        case 'D':
            idx = (selected_block->y+1)*MAX_DIVISION_NUM + (selected_block->x);
            break;
        case 'L':
            idx = (selected_block->y)*MAX_DIVISION_NUM + (selected_block->x-1);
            break;
        case 'R':
            idx = (selected_block->y)*MAX_DIVISION_NUM + (selected_block->x+1);
            break;
        default:
            idx = -1;
            break;
    }
    BOOST_ASSERT(boundary_check(idx/MAX_DIVISION_NUM,idx%MAX_DIVISION_NUM) == true);
    return idx;
}

void GraphicsItemSlideBoard::change_selected_block()
{
    is_selecting = true;
    selected_block->set_selected(false);
    QList<QGraphicsItem *> child = childItems();
    for(int i = 0;i < child.size(); i++){
        GraphicsItemSlideBlock *b = qgraphicsitem_cast<GraphicsItemSlideBlock*>(child.at(i));
        BOOST_ASSERT(b != nullptr);
        b->is_selecting = true;
        b->mouse_hover = false;
    }
    update(boundingRect());
}

void GraphicsItemSlideBoard::change_color(bool color)
{
    QList<QGraphicsItem *> child = childItems();
    for(int i = 0;i < child.size(); i++){
        GraphicsItemSlideBlock *b = qgraphicsitem_cast<GraphicsItemSlideBlock*>(child.at(i));
        BOOST_ASSERT(b != nullptr);
        b->distance_color = color;
    }
}

} // end of namespace ui
} // end of namespace slide
