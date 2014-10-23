#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <utility>

#include <QBrush>
#include <QtConcurrent>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFuture>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QMetaType>
#include <QResource>
#include <QString>
#include <QSize>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QValidator>

#include <boost/asio.hpp>

#undef B0

#include "slide/Answer.hpp"
#include "slide/Problem.hpp"
#include "slide/Point.hpp"
#include "slide/DolphinSolver.hpp"
#include "slide/DragonSolver.hpp"
#include "slide/ExactSolver.hpp"
#include "slide/HitodeSolver.hpp"
#include "slide/LizardSolver.hpp"
#include "slide/L2Solver.hpp"
#include "slide/KurageSolver.hpp"
#include "slide/StraightSolver.hpp"
#include "slide/Solver.hpp"
#include "network/ProblemClient.hpp"
#include "network/PostClient.hpp"
#include "util/define.hpp"

#include "getproblemworker.hpp"
#include "graphicsitemslideblock.hpp"
#include "graphicsitemslideboard.hpp"
#include "mainslidewindow.hpp"

#include "ui_mainslidewindow.h"

namespace slide{
namespace ui{

MainSlideWindow::MainSlideWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainSlideWindow),
    scene(new QGraphicsScene),
    problem(nullptr),
    current_solver(SolverType::StraightSolver),
    auto_solve(false),
    auto_post(false),
    get_problem_worker_thread(nullptr),
    get_problem_worker(nullptr),
    post_problem_worker_thread(nullptr),
    post_problem_worker(nullptr)
{
    ui->setupUi(this);
    ui->slideView->setScene(scene);
    ui->slideView->setMouseTracking(true);
    ui->slideView->show();
    ui->selectionCostEdit->setValidator(new QIntValidator(0,1000000,this));
    ui->swappingCostEdit->setValidator(new QIntValidator(0,1000000,this));
    ui->selectionLimitEdit->setValidator(new QIntValidator(0,1000000,this));

    qRegisterMetaType<slide::Answer>();
    connect(ui->animationTimeSpinBox,SIGNAL(valueChanged(int)),ui->animationTimeScrollBar,SLOT(setValue(int)));
    connect(ui->animationTimeScrollBar,SIGNAL(valueChanged(int)),ui->animationTimeSpinBox,SLOT(setValue(int)));
    connect(this,SIGNAL(answerCreated(const slide::Answer)),this,SLOT(slot_solve_finished(const slide::Answer)));

#ifdef DEBUG
    QString resource_path("../../resources/");
#else
    QString resource_path("./resources/");
#endif
    if(QFileInfo::exists(resource_path)){
        QDir::setSearchPaths("resources", QStringList(resource_path));
        debug(QString("add resource path: ") + QFileInfo(resource_path).absoluteFilePath());
    }else{
        debug(QString("could not find resource dir: ") +  QFileInfo(resource_path).absoluteFilePath());
        ui->problemComboBox->setEnabled(false);
        ui->readProblemButton->setEnabled(false);
    }

    setup_new_game();
}

MainSlideWindow::~MainSlideWindow()
{
    if(get_problem_worker){
        get_problem_worker->stop();
        get_problem_worker_thread->quit();
        get_problem_worker_thread->wait();
    }
    if(post_problem_worker){
        post_problem_worker->stop();
        post_problem_worker_thread->quit();
        post_problem_worker_thread->wait();
    }
    delete ui;
    delete scene;
    delete problem;
}

template<class T>
void MainSlideWindow::debug(T mes){
    ui->slideInfoBrowser->append(mes);
    qDebug() << mes;
}

void MainSlideWindow::setup_new_game()
{
    H = ui->HsizeSpinBox->value();
    W = ui->WsizeSpinBox->value();
    swappingCost = ui->swappingCostEdit->text().toInt();
    selectionCost = ui->selectionCostEdit->text().toInt();
    selectionLimit = ui->selectionLimitEdit->text().toInt();

    std::stringstream mes;
    mes << "load: problem(" << H << "," << W << "," << swappingCost << "," << selectionCost << "," << selectionLimit << ")";
    debug(mes.str().c_str());
    delete problem;
    problem = new slide::Problem(H,W,swappingCost,selectionCost,selectionLimit);
    problem->board = Board<Flexible>::finishedState(H, W);

    setup_board();
}

void MainSlideWindow::setup_board()
{
    ui->playButton->setEnabled(false);
    ui->restoreButton->setEnabled(false);

    QSize view_size = ui->slideView->size();
    QPen blue_pen;
    blue_pen.setColor(QColor(0,0,255,100));
    WSIZE = (view_size.width()-20)/W;
    HSIZE = (view_size.height()-20)/H;

    scene->clear();
    board = new GraphicsItemSlideBoard(H,W,HSIZE,WSIZE,&problem->board);
    board->set_animation_time(ui->animationTimeSpinBox->value());
    connect(board,SIGNAL(shuffle_finished()),this,SLOT(slot_shuffle_finished()));
    connect(board,SIGNAL(move_blocks_finished()),this,SLOT(slot_move_finished()));
    connect(board,SIGNAL(change_selected_block_finished()),this,SLOT(slot_change_selected_block_finished()));
    scene->addItem(board);
}

void MainSlideWindow::read_problem_from_file(const QString &filename)
{
    slide::Problem *problem_ = nullptr;
    try{
        problem_ = new slide::Problem(boost::filesystem::path(filename.toStdString()));
    }catch(std::runtime_error &e){
        debug("Read problem failed..");
        debug(e.what());
        delete problem_;
        return;
    }
    delete problem;
    problem = problem_;

    update_problem_info();
    setup_board();
}

void MainSlideWindow::read_problem_from_string(const QString &problem_string)
{
    slide::Problem *problem_ = nullptr;
    std::stringstream ss;
    ss << problem_string.toStdString();
    try{
        problem_ = new slide::Problem(ss);
    }catch(std::runtime_error &e){
        debug("Read problem failed..");
        debug(e.what());
        delete problem_;
        return;
    }
    delete problem;
    problem = problem_;

    update_problem_info();
    setup_board();
}

void MainSlideWindow::update_problem_info()
{
    std::stringstream mes;
    H = problem->board.height();
    W = problem->board.width();
    swappingCost = problem->swappingCost;
    selectionCost = problem->selectionCost;
    selectionLimit = problem->selectionLimit;
    ui->HsizeSpinBox->setValue(H);
    ui->WsizeSpinBox->setValue(W);
    ui->swappingCostEdit->setText(QString::number(swappingCost));
    ui->selectionCostEdit->setText(QString::number(selectionCost));
    ui->selectionLimitEdit->setText(QString::number(selectionLimit));
    mes << "load: problem(" << H << "," << W << "," << swappingCost << "," << selectionCost << "," << selectionLimit << ")";
    debug(mes.str().c_str());
}

void MainSlideWindow::calc_score(const slide::Answer &ans,
                                 int *selection_cnt, int *total_swap_cnt, int *total_cost)
{
    GraphicsItemSlideBlock* block_tmp[MAX_DIVISION_NUM*MAX_DIVISION_NUM];
    int pos_tmp[MAX_DIVISION_NUM*MAX_DIVISION_NUM];
    std::copy(&board->blocks[0],&board->blocks[MAX_DIVISION_NUM*MAX_DIVISION_NUM],&block_tmp[0]);
    for(int y = 0; y < H; y++){
        for(int x = 0; x < W; x++){
            int idx = y*MAX_DIVISION_NUM+x;
            pos_tmp[idx] = board->blocks[idx]->y*MAX_DIVISION_NUM + board->blocks[idx]->x;
            board->blocks[idx]->swapped_count = 0;
        }
    }

    std::cerr << "calc score" << std::endl;
    std::stringstream ss;
    int swap_cnt=-1,swap_max = -1;
    int y,x,idx=-1,next_idx;
    ss << ans.toString();
    ss >> std::dec >> *selection_cnt;
    for(int i = 0; i < *selection_cnt; i++){
        if(swap_max < 0 || swap_cnt >= swap_max){
            int pos;
            ss >> std::hex >> pos;
            ss >> std::dec >> swap_max;
            y = pos % MAX_DIVISION_NUM;
            x = pos / MAX_DIVISION_NUM;
            idx = y*MAX_DIVISION_NUM + x;
            *total_swap_cnt += swap_max;
            *total_cost += current_selection_cost;
            swap_cnt = 0;
        }
        for(int j = 0; j < swap_max; j++){
            char c;
            swap_cnt += 1;
            ss >> c;
            c = toupper(c);
            *total_cost += current_swap_cost;
            BOOST_ASSERT(c == 'U' || c == 'D' || c == 'L' || c == 'R');
            if(c == 'U'){
                next_idx = (board->blocks[idx]->y-1)*MAX_DIVISION_NUM + board->blocks[idx]->x;
            }else if(c == 'D'){
                next_idx = (board->blocks[idx]->y+1)*MAX_DIVISION_NUM + board->blocks[idx]->x;
            }else if(c == 'L'){
                next_idx = (board->blocks[idx]->y)*MAX_DIVISION_NUM + board->blocks[idx]->x-1;
            }else{
                next_idx = (board->blocks[idx]->y)*MAX_DIVISION_NUM + board->blocks[idx]->x+1;
            }
            board->blocks[idx]->swapped_count += 1;
            board->blocks[next_idx]->swapped_count += 1;
            std::swap(board->blocks[idx]->x, board->blocks[next_idx]->x);
            std::swap(board->blocks[idx]->y, board->blocks[next_idx]->y);
            std::swap(board->blocks[idx], board->blocks[next_idx]);
            idx = next_idx;
        }
    }

    std::copy(&block_tmp[0],&block_tmp[MAX_DIVISION_NUM*MAX_DIVISION_NUM],&board->blocks[0]);
    for(int y = 0; y < H; y++){
        for(int x = 0; x < W; x++){
            int idx = y*MAX_DIVISION_NUM+x;
            board->blocks[idx]->y = pos_tmp[idx] / MAX_DIVISION_NUM;
            board->blocks[idx]->x = pos_tmp[idx] % MAX_DIVISION_NUM;
            swapped_count_backup[idx] = board->blocks[idx]->swapped_count;
        }
    }
}

void MainSlideWindow::save_state()
{
    std::copy(&board->blocks[0],&board->blocks[MAX_DIVISION_NUM*MAX_DIVISION_NUM],&block_backup[0]);
    std::fill(&swapped_count_backup[0], &swapped_count_backup[MAX_DIVISION_NUM*MAX_DIVISION_NUM], 0);
    selected_block_pos_backup = board->get_selected_block_pos();
    for(int y = 0; y < H; y++){
        for(int x = 0; x < W; x++){
            int idx = y*MAX_DIVISION_NUM+x;
            board_pos_backup[idx] = board->blocks[idx]->y*MAX_DIVISION_NUM + board->blocks[idx]->x;
            cell_backup[idx] = problem->board(y, x);
        }
    }
}

void MainSlideWindow::restore_state()
{
    std::copy(&block_backup[0],&block_backup[MAX_DIVISION_NUM*MAX_DIVISION_NUM],&board->blocks[0]);
    for(int y = 0; y < H; y++){
        for(int x = 0; x < W; x++){
            int idx = y*MAX_DIVISION_NUM+x;
            int y_,x_;
            y_ = board_pos_backup[idx] / MAX_DIVISION_NUM;
            x_ = board_pos_backup[idx] % MAX_DIVISION_NUM;
            board->blocks[idx]->y = y_;
            board->blocks[idx]->x = x_;
            board->blocks[idx]->setPos(x_*WSIZE,y_*HSIZE);
            board->blocks[idx]->swapped_count = swapped_count_backup[idx];
            problem->board(y, x) = cell_backup[idx];
            board->blocks[idx]->update();
        }
    }
    board->select(selected_block_pos_backup/MAX_DIVISION_NUM, selected_block_pos_backup%MAX_DIVISION_NUM);
}

void MainSlideWindow::on_reloadButton_clicked()
{
    qDebug() << "reload button";
    setup_new_game();
}

void MainSlideWindow::on_shuffleButton_clicked()
{
    qDebug() << "shuffle button";
    disable_all_button();
    board->shuffle();
}

void MainSlideWindow::on_shuffle2Button_clicked()
{
    qDebug() << "shuffle2 button";
    disable_all_button();
    board->shuffle2();
}

static void solve(slide::Solver *solver,
                  std::function<void(const slide::Answer&)> func)
{
    solver->onCreatedAnswer = func;
    solver->solve();
    delete solver;
}

void MainSlideWindow::on_solveButton_clicked()
{
    qDebug() << "solve button";
    if(problem){
        debug("solve start");
        time = QTime();
        time.start();
        disable_all_button();
        board->set_solving_flag(true);
        slide::Solver *solver = nullptr;
        save_state();
        current_selection_cost = ui->selectionCostEdit->text().toInt();
        current_swap_cost = ui->swappingCostEdit->text().toInt();
        if(current_solver == SolverType::StraightSolver){
            debug("solver: StraightSolver");
            solver = new StraightSolver(*problem);
        }else if(current_solver == SolverType::ExactSolver){
            debug("solver: ExactSolver");
            solver = new ExactSolver(*problem);
        }else if(current_solver == SolverType::HitodeSolver){
            debug("solver: HitodeSolver");
            solver = new HitodeSolver(*problem);
        }else if(current_solver == SolverType::KurageSolver){
            debug("solver: KurageSolver");
            solver = new KurageSolver(*problem);
        }else if(current_solver == SolverType::DolphinSolver){
            debug("solver: DolphinSolver");
            solver = new DolphinSolver(*problem);
        }else if(current_solver == SolverType::DragonSolver){
            debug("solver: DragonSolver");
            solver = new DragonSolver(*problem);
        }else if(current_solver == SolverType::LizardSolver){
            debug("solver: LizardSolver");
            solver = new LizardSolver(*problem);
        }else if(current_solver == SolverType::L2Solver){
            debug("solver: L2Solver");
            solver = new L2Solver(*problem);
        }else{
            debug("solver: LizardSolver");
            solver = new LizardSolver(*problem);
        }
        slidesolver_future = QtConcurrent::run(solve,solver,
                                               std::bind(&MainSlideWindow::onCreatedAnswer,this,std::placeholders::_1));
        // solver is deletede in solve()
    }
}

void MainSlideWindow::onCreatedAnswer(const slide::Answer& ans)
{
    emit answerCreated(ans);
    // NOTE: this function is called by solve thread
}

void MainSlideWindow::on_animationTimeSpinBox_valueChanged(int t)
{
    if(board){
        qDebug() << "set animation time:" << t;
        board->set_animation_time(t);
    }
}

void MainSlideWindow::slot_solve_finished(const Answer ans)
{
    int selection_cnt, swap_cnt, cost;
    qDebug() << ans.toString().c_str();
    debug(ans.toString().c_str());
    debug("solve done");
    debug(QString("time: %1s").arg(time.elapsed()/1000.0));
    selection_cnt = swap_cnt = cost = 0;
    calc_score(ans, &selection_cnt, &swap_cnt, &cost);
    debug(QString("selection cnt: %1").arg(selection_cnt));
    debug(QString("swap cnt: %1").arg(swap_cnt));
    debug(QString("total cost: %1").arg(cost));
    current_answer = ans;
    if(auto_post){
        on_postAnswerButton_clicked();
    }
    ui->playButton->setEnabled(true);
}

void MainSlideWindow::slot_shuffle_finished()
{
    qDebug() << "shuffle done";
    enable_all_button();
}

void MainSlideWindow::slot_move_finished()
{
    qDebug() << "move done";
    enable_all_button();
    ui->restoreButton->setEnabled(true);
    ui->playButton->setEnabled(false);
}

void MainSlideWindow::slot_change_selected_block_finished()
{
    qDebug() << "change selected block done";
    enable_all_button();
}

void MainSlideWindow::on_changeSelectedBlockButton_clicked()
{
    qDebug() << "change selected block";
    disable_all_button();
    board->change_selected_block();
}

void MainSlideWindow::enable_all_button()
{
    ui->shuffleButton->setEnabled(true);
    ui->shuffle2Button->setEnabled(true);
    ui->reloadButton->setEnabled(true);
    ui->solveButton->setEnabled(true);
    ui->changeSelectedBlockButton->setEnabled(true);
    ui->readProblemButton->setEnabled(true);
}

void MainSlideWindow::disable_all_button()
{
    ui->shuffleButton->setEnabled(false);
    ui->shuffle2Button->setEnabled(false);
    ui->reloadButton->setEnabled(false);
    ui->solveButton->setEnabled(false);
    ui->playButton->setEnabled(false);
    ui->restoreButton->setEnabled(false);
    ui->changeSelectedBlockButton->setEnabled(false);
    ui->readProblemButton->setEnabled(false);
}



void MainSlideWindow::on_solverComboBox_currentIndexChanged(int index)
{
    BOOST_ASSERT(index >= 0 && index < static_cast<int>(SolverType::Last));
    enum SolverType solver = static_cast<SolverType>(index);
    current_solver = solver;
}

void MainSlideWindow::on_playButton_clicked()
{
    disable_all_button();
    board->set_solving_flag(false);
    board->move_blocks(current_answer);
}

void MainSlideWindow::on_restoreButton_clicked()
{
    restore_state();
    ui->restoreButton->setEnabled(false);
    ui->playButton->setEnabled(true);
}

void MainSlideWindow::on_fileOpen_triggered()
{
    static QString previous_open_dir = QDir::currentPath();
    QString fileName = QFileDialog::getOpenFileName(this,
                        tr("問題を選択"), previous_open_dir, tr("問題ファイル (*)"));
    previous_open_dir = QFileInfo(fileName).dir().path();
    debug(QString("select file: " + fileName));
    read_problem_from_file(fileName);
}


void MainSlideWindow::on_readProblemButton_clicked()
{
    int idx = ui->problemComboBox->currentIndex();
    QString resourceName;
    const int PRACTICE_NUM = 17;
    if(idx < PRACTICE_NUM){
        resourceName = QString("resources:/answer/practice/%1.txt").arg(idx+1,2,10,QChar('0'));
    }else /*if(idx < (PRACTICE_NUM + 20))*/{
        resourceName = QString("resources:/answer/imData/48x36-%1.txt").arg(idx+1-PRACTICE_NUM);
    }

    read_problem_from_file(QDir(resourceName).absolutePath());
}

void MainSlideWindow::on_colorDistanceButton_clicked()
{
    qDebug() << "color: distance";
    board->change_color(true);
    board->update();
}

void MainSlideWindow::on_colorAccordanceButton_clicked()
{
    qDebug() << "color: accordance";
    board->change_color(false);
    board->update();
}

void MainSlideWindow::slot_got_problem(const QString problem_msg)
{
    //debug(problem);
    debug("Get Problem from server");
    read_problem_from_string(problem_msg);
    ui->solveButton->setEnabled(true);
    if(problem && auto_solve){
        debug("auto solve");
        on_solveButton_clicked();
    }
}

void MainSlideWindow::on_readProblemFromServerButton_clicked()
{
    debug("GET problem from server");
    std::string addr = ui->problemServerAddrEdit->text().toStdString();
    std::string port = ui->problemServerPortEdit->text().toStdString();

    if(get_problem_worker_thread){
        get_problem_worker->stop();
        get_problem_worker_thread->quit();
        get_problem_worker_thread->wait();
    }

    bool err;
    get_problem_worker = std::move(std::unique_ptr<GetProblemWorker>(
                                       new GetProblemWorker(addr, port, err)));
    if(err){
        debug("Error: cannot create GetProblemWorker");
        return;
    }
    debug("waiting...");
    get_problem_worker_thread = std::move(std::unique_ptr<QThread>(new QThread()));
    get_problem_worker->moveToThread(get_problem_worker_thread.get());
    connect(get_problem_worker.get(), &GetProblemWorker::gotProblem,
            this, &MainSlideWindow::slot_got_problem);
    connect(get_problem_worker_thread.get(), &QThread::started,
            get_problem_worker.get(), &GetProblemWorker::start);
    get_problem_worker_thread->start();
    ui->getCancelButton->setEnabled(true);
    ui->readProblemFromServerButton->setEnabled(false);
    disable_all_button();
}

void MainSlideWindow::on_getCancelButton_clicked()
{
    if(get_problem_worker_thread){
        debug("cancel GET problem");
        get_problem_worker->stop();
        get_problem_worker_thread->quit();
        get_problem_worker_thread->wait();
        ui->getCancelButton->setEnabled(false);
        ui->readProblemFromServerButton->setEnabled(true);
        enable_all_button();
    }
}

void MainSlideWindow::on_startPostWorkerButton_clicked()
{
    debug("POST answer to server");
    std::string addr = ui->postServerAddrEdit->text().toStdString();
    std::string port = ui->postServerPortEdit->text().toStdString();

    if(post_problem_worker_thread){
        post_problem_worker->stop();
        post_problem_worker_thread->quit();
        post_problem_worker_thread->wait();
    }

    bool err;
    post_problem_worker = std::move(std::unique_ptr<PostProblemWorker>(
                                       new PostProblemWorker(addr, port, err)));
    if(err){
        debug("Error: cannot create GetProblemWorker");
        return;
    }
    debug("[MainSlideWindow] Start Post Server Worker");
    post_problem_worker_thread = std::move(std::unique_ptr<QThread>(new QThread()));
    //connect(this, &MainSlideWindow::sendAnswer,
    //        post_problem_worker.get(), &PostProblemWorker::send_answer);
    connect(this, SIGNAL(sendAnswer(const slide::Answer)),
            post_problem_worker.get(), SLOT(send_answer(const slide::Answer)));
    connect(post_problem_worker_thread.get(), &QThread::started,
            post_problem_worker.get(), &PostProblemWorker::start);
    post_problem_worker->moveToThread(post_problem_worker_thread.get());
    post_problem_worker_thread->start();
    ui->startPostWorkerButton->setEnabled(false);
    ui->postCancelButton->setEnabled(true);
    ui->postAnswerButton->setEnabled(true);
}

void MainSlideWindow::on_postAnswerButton_clicked()
{
    if(post_problem_worker){
        debug("[MainSlideWindow]Post Answer");
        post_problem_worker->send_answer(current_answer);
        //emit sendAnswer(current_answer);
    }
}

void MainSlideWindow::on_postCancelButton_clicked()
{
    if(post_problem_worker_thread){
        debug("stop POST worker thread");
        post_problem_worker->stop();
        post_problem_worker_thread->quit();
        post_problem_worker_thread->wait();
        ui->startPostWorkerButton->setEnabled(true);
        ui->postCancelButton->setEnabled(false);
        ui->postAnswerButton->setEnabled(false);
    }
}

void MainSlideWindow::on_autoSolveCheckBox_clicked(bool f)
{
    auto_solve = f;
}

void MainSlideWindow::on_autoPostCheckBox_clicked(bool f)
{
    auto_post = f;
}

} // end of namespace ui
} // end of namespace slide



