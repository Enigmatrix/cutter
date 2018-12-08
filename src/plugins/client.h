#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QUuid>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QObject>
#include <functional>
#include "CutterPlugin.h"
#include "types.h"

class Client : public QObject
{
    Q_OBJECT

    QNetworkAccessManager* networkManager;
    QUrl url;
    QUuid uuid;
    QString token;
    void send(Message*);
    void listen();
    void onReadyRead(QIODevice*);
    void understandMessage(Message*);
public:
    Client(QString);
    void commentsAdded(RVA, QString);
    std::function<void(RVA, QString)> onCommentsAdded;
    void commentsDeleted(RVA);
    std::function<void(RVA)> onCommentsDeleted;
};

#endif // CLIENT_H
