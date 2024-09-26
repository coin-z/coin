#include <QApplication>
#include "mainwindow.hpp"

#include <thread>

#include <coin-data/coin-data.hpp>
#include <coin-data/communicator.hpp>

int main(int argc, char *argv[])
{
    coin::init(argc, argv);
    QApplication app(argc, argv);

    auto work = std::thread([]{
        while(coin::ok())
        {
            coin::spin_once();
            usleep(1);
        }
        QApplication::quit();
    });

    MainWindow window;
    window.show();
    int ret = app.exec();

    coin::exit();
    work.join();

    return ret;
}
