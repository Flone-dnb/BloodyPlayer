#define CATCH_CONFIG_RUNNER
#include <QApplication>
#include <iostream>
#include "../ext/Catch2/catch.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const int res = Catch::Session().run(argc, argv);
    std::system("pause");

    return (res < 0xff ? res : 0xff);
}
