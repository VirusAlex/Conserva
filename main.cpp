#include <QCoreApplication>
#include "conserva.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    conserva *Server;
    QString name = "VirusAlex";
    Server = new conserva(2323, name);

    return a.exec();
}
