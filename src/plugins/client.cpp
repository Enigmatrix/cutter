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
bool wantsHeader = true;
int consumeBodySize = 0;
void Client::onReadyRead() {
    qDebug() << "ready";
    int consume = 0;
    switch (wantsHeader) {
    case true:
        consume = 2;
        break;
    case false:
        consume = consumeBodySize;
        break;
    }
    if (res->bytesAvailable() < consume) {
        return;
    }
    auto bytes = res->read(consume);
    switch (wantsHeader) {
    case true:
        consumeBodySize = (static_cast<unsigned int>(bytes[0]) & 0xFF) << 8
                        + (static_cast<unsigned int>(bytes[1]) & 0xFF);
        wantsHeader = false;
        break;
    case false:
        wantsHeader = true;
        auto m = Message();
        m.ParseFromArray(bytes, consume);
        understandMessage(m);
        break;
    }
}

void Client::understandMessage(Message m) {
    if (m.has_commentadded()) {
        if (onCommentsAdded) onCommentsAdded(m.commentadded().addr(), QString::fromStdString(m.commentadded().cmt()));
    } else if (m.has_commentdeleted()) {
        if (onCommentsDeleted) onCommentsDeleted(m.commentdeleted().addr());
    }
}

void Client::send(Message m) {
    m.set_username(username.toStdString());
    qDebug() << "making req..";
    auto req = QNetworkRequest(url);
    qDebug() << &req;
    size_t size = m.ByteSize();
    auto body = malloc(size);
    m.SerializeToArray(body, size);
    qDebug() << "posting...";
    networkManager->post(req, QByteArray((const char*)body, size));
    qDebug() << "posted";
}
void Client::commentsAdded(RVA addr, QString cmt) {
    qDebug() << "cmts add";
    auto c = CommentAdded();
    c.set_addr(addr);
    c.set_cmt(cmt.toStdString());
    auto m = Message();
    m.set_allocated_commentadded(&c);
    send(m);
}

void Client::commentsDeleted(RVA addr) {
    qDebug() << "cmts rmved";
    auto c = CommentDeleted();
    c.set_addr(addr);
    auto m = Message();
    m.set_allocated_commentdeleted(&c);
    send(m);
}
