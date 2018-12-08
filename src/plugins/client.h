#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QUuid>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QObject>
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
    void (*onCommentsAdded)(RVA, QString);
    void commentsDeleted(RVA);
    void (*onCommentsDeleted)(RVA);
};

#endif // CLIENT_H
