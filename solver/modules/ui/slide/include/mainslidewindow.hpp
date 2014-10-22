#ifndef MAINSLIDEWINDOW_HPP
#define MAINSLIDEWINDOW_HPP

#include <QFuture>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QString>
#include <QTime>
#include <QThread>

#include <memory>
#include <utility>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "getproblemworker.hpp"
#include "postproblemworker.hpp"
#include "graphicsitemslideblock.hpp"
#include "graphicsitemslideboard.hpp"
#include "slide/Answer.hpp"
#include "slide/Problem.hpp"
#include "network/ProblemClient.hpp"
#include "util/define.hpp"

namespace Ui {
class MainSlideWindow;
}

namespace slide{
namespace ui{

enum class SolverType{
    StraightSolver = 0,
    ExactSolver,
    HitodeSolver,
    KurageSolver,
    DolphinSolver,
    DragonSolver,
    LizardSolver,
    L2Solver,
    Last
};

class MainSlideWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainSlideWindow(QWidget *parent = 0);
    ~MainSlideWindow();

private slots:
    void on_reloadButton_clicked();
    void on_shuffleButton_clicked();
    void on_shuffle2Button_clicked();
    void on_solveButton_clicked();
    void on_animationTimeSpinBox_valueChanged(int t);
    void on_changeSelectedBlockButton_clicked();
    void on_solverComboBox_currentIndexChanged(int index);
    void on_playButton_clicked();
    void on_restoreButton_clicked();
    void on_fileOpen_triggered();
    void on_readProblemButton_clicked();
    void on_colorDistanceButton_clicked();
    void on_colorAccordanceButton_clicked();
    void on_readProblemFromServerButton_clicked();
    void on_getCancelButton_clicked();
    void on_startPostWorkerButton_clicked();
    void on_postAnswerButton_clicked();
    void on_postCancelButton_clicked();
    void on_autoSolveCheckBox_clicked(bool f);
    void on_autoPostCheckBox_clicked(bool f);

    void slot_shuffle_finished();
    void slot_move_finished();
    void slot_solve_finished(const slide::Answer ans);
    void slot_change_selected_block_finished();
    void slot_got_problem(const QString);

signals:
    void answerCreated(const slide::Answer ans);
    void sendAnswer(const slide::Answer answer);

private:
    Ui::MainSlideWindow *ui;
    QGraphicsScene *scene;
    slide::Problem *problem;
    GraphicsItemSlideBoard *board;
    int H,W,HSIZE,WSIZE;
    int swappingCost,selectionCost,selectionLimit;
    int current_selection_cost,current_swap_cost;
    slide::Answer current_answer;
    SolverType current_solver;
    QTime time;
    QFuture<void> slidesolver_future;
    int selected_block_pos_backup;
    int swapped_count_backup[MAX_DIVISION_NUM*MAX_DIVISION_NUM];
    int board_pos_backup[MAX_DIVISION_NUM*MAX_DIVISION_NUM];
    unsigned char cell_backup[MAX_DIVISION_NUM*MAX_DIVISION_NUM];
    GraphicsItemSlideBlock *block_backup[MAX_DIVISION_NUM*MAX_DIVISION_NUM];
    bool auto_solve;
    bool auto_post;
    std::unique_ptr<QThread> get_problem_worker_thread;
    std::unique_ptr<GetProblemWorker> get_problem_worker;
    std::unique_ptr<QThread> post_problem_worker_thread;
    std::unique_ptr<PostProblemWorker> post_problem_worker;

    void setup_new_game();
    void setup_board();
    void read_problem_from_file(const QString& filename);
    void read_problem_from_string(const QString& problem);
    void update_problem_info();
    void shuffle();
    void onCreatedAnswer(const slide::Answer &ans);
    void disable_all_button();
    void enable_all_button();
    void calc_score(const slide::Answer &ans, int *selection_cnt, int *swap_cnt, int *total_cost);
    void save_state();
    void restore_state();
    template<class T> void debug(T mes);
};

} // end of namespace ui
} // end of namespace slide

Q_DECLARE_METATYPE(slide::Answer);

#endif // MAINSLIDEWINDOW_HPP
