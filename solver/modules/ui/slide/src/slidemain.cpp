#include "mainslidewindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    slide::ui::MainSlideWindow w;
    w.show();

    return a.exec();
}
