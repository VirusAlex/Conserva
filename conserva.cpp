#include <QTime>
#include <QDebug>
#include <QDataStream>
#include "conserva.h"

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
    foreach (QTcpSocket *sock, pClientSockets){
        sock->close();
    }
    m_tcpServer->close();
}

void conserva:: slotNewConnection() {
    QTcpSocket* clientSocket = m_tcpServer->nextPendingConnection();
    pClientSockets.push_back(clientSocket);
    connect(pClientSockets.last(), SIGNAL(disconnected()), this, SLOT(slotDisconnect()));
    connect(pClientSockets.last(), SIGNAL(readyRead()), this, SLOT(slotReadClient()));
    QByteArray hello;
    hello.append("<b>Сервер: "+myName+"</b>");
    sendToClient(pClientSockets.last(), hello, 'z');
}

void conserva::slotDisconnect(){
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    clientSocket->deleteLater();
    clientSocket->close();
    pClientSockets.removeOne(clientSocket);
    QByteArray byebye;
    byebye.append("<b>Сервер: one disconnected.</b>");
    sendToClient(0, byebye, 's');
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
                    sendToClient(clientSocket,data,'s');
                }
                else if (wait_for_type=='i'){
                    sendToClient(clientSocket,data,'i');
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
    if (t == 'z'){
        quint32 s = data.size();
        pSocket->write((char *)&s, sizeof(s));
        char z = 's';
        pSocket->write((char *)&z, sizeof(z));
        pSocket->write(data);

    }
    else
        foreach (QTcpSocket *sock, pClientSockets){
            if (sock!=pSocket){
                quint32 s = data.size();
                sock->write((char *)&s, sizeof(s));
                sock->write((char *)&t, sizeof(t));
                sock->write(data);
            }
        }
}
