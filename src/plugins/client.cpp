#include "client.h"

Client::Client(QString token)
{
    uuid = QUuid::createUuid();
    networkManager = new QNetworkAccessManager();
    url = new QUrl("http://makerforce.io:8080");
    url->setPath("/" + token);
    listen();
}

Client::listen() {
    auto req = new QNetworkRequest(url);
    auto res = networkManager.get(req);
    connect(res, SIGNAL(readyRead()), SLOT(onReadyRead(res)));
}
Client::onReadyRead(QIODevice in) {
    if (in.bytesAvailable() < sizeof(Message)) {
        return;
    }
    auto bytes = in.read(sizeof(Message));
}

Client::send(Message m) {
    auto req = new QNetworkRequest(url);
    auto body = new QByteArray(&m, sizeof(m));
    networkManager.post(req, body);
}

Client::commentsAdded(RVA addr, QString cmt) {
    CommentAdded c;
    c->addr = addr;
    c->cmt = cmt;
    Message m;
    m->type = MessageCommentAdded;
    m->commentAdded = c;
    send(m);
}

Client::commentsDeleted(RVA addr) {
    CommentDeleted c;
    c->addr = addr;
    Message m;
    m->type = MessageCommentDeleted;
    m->commentAdded = c;
    send(m);
}
