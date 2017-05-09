#include <QCoreApplication>
#include <QTextStream>
#include "conserva.h"

QTextStream out(stdout); //for console output
QTextStream in(stdin); //for input


int main(int argc, char* argv[])
{
    QCoreApplication a(argc,argv);
    conserva *Server;
    QString name = "VirusAlex";
    Server = new conserva(2323, name);

    //Someone called me a killer after that:
    //signal(SIGINT, SIG_DFL);

    return a.exec();
}
