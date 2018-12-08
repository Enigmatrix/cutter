#ifndef CLIENT_H
#define CLIENT_H

#include <types.h>

class Client
{
    QNetworkAccessManager networkManager;
    QUrl url;
    QUuid uuid;
    QString token;
    void send(Message);
public:
    Client(QString);
    void commentsAdded(CommentAdded);
    void commentsDeleted(CommentDeleted);
};

#endif // CLIENT_H
