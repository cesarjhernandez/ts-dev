#include "scriptstest.h"
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QStateMachine>
#include <QTextStream>
#include <iostream>
#include <QString>
#include <cstdio>
#include <fstream>
#include <QDebug>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScriptsTest w;

    w.show();

    return a.exec();
}
