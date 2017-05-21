//+++/+±/
#include <QTime>
#include <QDebug>
#include <QDataStream>
#include "conserva.h"
#include <QtGui/qtextdocument.h>


//Some types of messages by key;
//'i' = message content some file;
//'s' = common text message;
//'a' = message content UserName;
//'z' = server message to the client;
//'x' = server set client's name. Soon;

conserva::conserva(int nPort, QString &uName): m_nNextBlockSize(0)
{
    m_tcpServer = new QTcpServer();
    if(!m_tcpServer->listen(QHostAddress::Any, nPort)) {
        qDebug() << "Server Error: Unable to start the server";
        qDebug() << m_tcpServer->errorString();
        m_tcpServer->close();
        return;
    }
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    myName = uName;
}

conserva::~conserva(){
    foreach (User *user, Users){
        user->userSocket->close();
    }
    m_tcpServer->close();
}

void conserva:: slotNewConnection() {
    QTcpSocket* clientSocket = m_tcpServer->nextPendingConnection();
    //qDebug() << clientSocket->peerAddress().toString();
    //qDebug() << clientSocket->peerAddress().toIPv4Address();
    //qDebug() << clientSocket->peerName();
    User * newUser = new User(clientSocket,"");
    Users.push_back(newUser);
    connect(Users.last()->userSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnect()));
    connect(Users.last()->userSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    QByteArray hello;
    //hello.append("<b>Сервер: "+myName+"</b>");
    //sendToClient(Users.last()->userSocket, hello, 'z');

    hello = myName.toUtf8();
    sendToClient(Users.last()->userSocket, hello, 'a');
}

void conserva::slotDisconnect(){
    User * disconnectedUser = getUserBySocket((QTcpSocket*)sender());
    disconnectedUser->userSocket->close();
    disconnectedUser->userSocket->deleteLater();

    QByteArray byebye;
    byebye.append("<b><t style = \"color: red;\">Сервер: </t> User \""
                  +disconnectedUser->userName
                  +"\" disconnected.</b>");
    Users.removeOne(disconnectedUser);
    sendToClient(0, byebye, 'z');
}

void conserva::slotExit(){
    foreach (User *user, Users){
        user->userSocket->close();
    }
    m_tcpServer->close();
}

User * conserva::getUserBySocket(QTcpSocket * value){
    foreach (User * user, Users)
        if(user->userSocket == value){
            return user;
        }
    return 0;
}

User * conserva::getUserByName(QString value){
    foreach (User * user, Users)
        if(user->userName == value){
            return user;
        }
    return 0;
}

void conserva::slotReadClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    bool a = false;
    do{
        a = false;
        if (wait_for_size > 0) {
            if (clientSocket->bytesAvailable() >= wait_for_size) {
                QByteArray data = clientSocket->read(wait_for_size);
                if(wait_for_type=='s'){
                    QString utext = "<b style = \"color: green;\">"
                            +getUserBySocket(clientSocket)->userName
                            +": </b>"
                            //+QString(data).toHtmlEscaped(); //Qt 5. required
                            +Qt::escape(data); //Qt 4. required
                    QByteArray datas = utext.toUtf8();
                    sendToClient(clientSocket,datas,'s');
                }
                else if (wait_for_type=='i'){
                    sendToClient(clientSocket,data,'i');
                }
                else if (wait_for_type=='a'){
                    foreach(User * user, Users){
                        if(user->userSocket==clientSocket){
                            user->userName = QString(data);
                            break;
                        }
                    }

                }
                wait_for_size = 0;
                a = true;
            }
        } else
            if(clientSocket->bytesAvailable() >= 5)
            {
                QByteArray info = clientSocket->read(4);
                memcpy(&wait_for_size, info.data(), 4);
                info = clientSocket->read(1);
                memcpy(&wait_for_type, info.data(), 1);
                a = true;
            }
    }while (a);
}

void conserva::sendToClient(QTcpSocket* pSocket, QByteArray data, char t) {
    if (t == 'a'){ //if (t == 'z'||t == 'a'){
        quint32 s = data.size();
        pSocket->write((char *)&s, sizeof(s));
        pSocket->write((char *)&t, sizeof(t));
        pSocket->write(data);

    }
    else
        foreach (User * user, Users){
            if (user->userSocket!=pSocket){
                quint32 s = data.size();
                user->userSocket->write((char *)&s, sizeof(s));
                user->userSocket->write((char *)&t, sizeof(t));
                user->userSocket->write(data);
            }
        }
}
