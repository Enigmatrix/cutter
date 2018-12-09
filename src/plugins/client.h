#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QUuid>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QObject>
#include <functional>
#include "CutterPlugin.h"
#include "model_generated.h"

class Client : public QObject
{
    Q_OBJECT

    QNetworkAccessManager* networkManager;
    QUrl url;
    QUuid uuid;
    QString token;
    QString nick;
    QNetworkReply* res;
    void send(flatbuffers::FlatBufferBuilder*);
    void listen();
    void understandMessage(model::Message*);
public:
    Client(QString, QString);
    void commentsAdded(RVA, QString);
    std::function<void(RVA, QString)> onCommentsAdded;
    void commentsDeleted(RVA);
    std::function<void(RVA)> onCommentsDeleted;
public slots:
    void onReadyRead();
};

#endif // CLIENT_H
