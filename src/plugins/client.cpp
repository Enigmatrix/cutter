#include "client.h"
#include <QString>
#include <QUuid>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QString>
#include <QString>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>

Client::Client(QString token)
{
    uuid = QUuid::createUuid();
    networkManager = new QNetworkAccessManager();
    url = QUrl("http://makerforce.io:8080");
    url.setPath("/" + token);
    listen();
}

void Client::listen() {
    auto req = QNetworkRequest(url);
    req.setRawHeader("X-Client-UUID", uuid.toByteArray());
    qDebug() << "listening..";
    res = networkManager->get(req);
    qDebug() << res;
    connect(res, &QNetworkReply::readyRead, this, &Client::onReadyRead);
}
void Client::onReadyRead() {
    qDebug() << "ready";
    qDebug() << res;
    if (res->bytesAvailable() < sizeof(Message)) {
        return;
    }
    auto bytes = res->read(sizeof(Message));
    auto m = reinterpret_cast<Message*>(bytes.data());
    understandMessage(m);
}

void Client::understandMessage(Message* m) {
    switch (m->type) {
        case MessageCommentAdded:
            if (onCommentsAdded) onCommentsAdded(m->commentAdded.addr, m->commentAdded.cmt);
            break;
        case MessageCommentDeleted:
            if (onCommentsDeleted) onCommentsDeleted(m->commentDeleted.addr);
            break;
    }
}

void Client::send(Message *m) {
    qDebug() << "making req..";
    auto req = QNetworkRequest(url);
    qDebug() << &req;
    qDebug() << "making byte array...";
    auto body = QByteArray(reinterpret_cast<char*>(m), sizeof(Message));
    qDebug() << &body;
    qDebug() << "posting...";
    networkManager->post(req, body);
    qDebug() << "posted";
}
void Client::commentsAdded(RVA addr, QString cmt) {
    qDebug() << "cmts add";
    CommentAdded c;
    c.addr = addr;
    c.cmt = cmt;
    qDebug() << "cmts added struct created";
    Message m;
    m.type = MessageCommentAdded;
    m.commentAdded = c;
    qDebug() << "cmts sending";
    send(&m);
}

void Client::commentsDeleted(RVA addr) {
    qDebug() << "cmts rmved";
    CommentDeleted c;
    c.addr = addr;
    Message m;
    m.type = MessageCommentDeleted;
    m.commentDeleted = c;
    send(&m);
}
