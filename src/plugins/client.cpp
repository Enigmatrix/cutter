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
    auto res = networkManager->get(req);
    connect(res, SIGNAL(readyRead()), SLOT(onReadyRead(&res)));
}
void Client::onReadyRead(QIODevice* in) {
    if (in->bytesAvailable() < sizeof(Message)) {
        return;
    }
    auto bytes = in->read(sizeof(Message));
}

void Client::send(Message *m) {
    auto req = QNetworkRequest(url);
    auto body = QByteArray(reinterpret_cast<char*>(m), sizeof(Message));
    networkManager->post(req, body);
}
void Client::commentsAdded(RVA addr, QString cmt) {
    CommentAdded c;
    c.addr = addr;
    c.cmt = cmt;
    Message m;
    m.type = MessageCommentAdded;
    m.commentAdded = c;
    send(&m);
}

void Client::commentsDeleted(RVA addr) {
    CommentDeleted c;
    c.addr = addr;
    Message m;
    m.type = MessageCommentDeleted;
    m.commentDeleted = c;
    send(&m);
}
