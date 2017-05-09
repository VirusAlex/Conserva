#ifndef CONSERVA_H
#define CONSERVA_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>

struct User{
    QTcpSocket * userSocket;
    QString userName;

    User(QTcpSocket * userSocket, QString userName){
        this->userName = userName;
        this->userSocket = userSocket;
    }
};

class conserva: public QObject
{
    Q_OBJECT

public:
    //explicit
    conserva(int nPort, QString &uName);
    ~conserva();

public slots:

    virtual void slotNewConnection();

    virtual void slotDisconnect();

    virtual void slotExit();

    void slotReadClient();


private:
    QTcpServer *m_tcpServer;
    quint32 m_nNextBlockSize;
    //QList<QTcpSocket*> pClientSockets;
    quint32 wait_for_size = 0;
    char wait_for_type = 'u';
    QString myName;
    QList<User*> Users;

private:
    void sendToClient(QTcpSocket* pSocket,  QByteArray data, char t);

    User *getUserBySocket(QTcpSocket * value);
    User *getUserByName(QString value);
};

#endif // CONSERVA_H
