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
#include "model_generated.h"
#include "flatbuffers/flatbuffers.h"

Client::Client(QString token, QString nick)
{
    qDebug() << "started client";
    uuid = QUuid::createUuid();
    networkManager = new QNetworkAccessManager();
    url = QUrl("http://makerforce.io:8080");
    nick = nick;
    url.setPath("/" + token);
    qDebug() << "started to listen...";
    listen();
    qDebug() << "end listen";
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
    qDebug() << res->bytesAvailable();
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
        auto m = model::GetMessage(bytes);
        understandMessage(const_cast<model::Message*>(m));
        break;
    }
    if (res->bytesAvailable() > 0) {
        onReadyRead();
    }
}

void Client::understandMessage(model::Message* m) {
    switch(m->content_type()){
        case model::MessageContent_CommentAdded:
    {
            if(this->onCommentsAdded) return;
            auto cntAdd = m->content_as_CommentAdded();
            this->onCommentsAdded(QString::fromLocal8Bit(m->username()->c_str()) ,cntAdd->addr(), QString::fromLocal8Bit(cntAdd->cmt()->c_str()));
            break;
    }
        case model::MessageContent_CommentDeleted:
    {
            if(this->onCommentsDeleted) return;
            auto cntDel = m->content_as_CommentDeleted();
            this->onCommentsDeleted(QString::fromLocal8Bit(m->username()->c_str()), cntDel->addr());
            break;
    }
    }
}

void Client::send(flatbuffers::FlatBufferBuilder *fbb) {
    qDebug() << "making req..";
    auto req = QNetworkRequest(url);
    qDebug() << &req;
    qDebug() << "making byte array...";
    qDebug() << "lmao";
    auto body = QByteArray(reinterpret_cast<char*>(fbb->GetBufferPointer()), fbb->GetSize());
    qDebug() << &body;
    qDebug() << "posting...";

    req.setRawHeader("X-Client-UUID", uuid.toByteArray());
    networkManager->post(req, body);
    qDebug() << "posted";
}
void Client::commentsAdded(RVA addr, QString cmt) {
    qDebug() << "cmts add";
    flatbuffers::FlatBufferBuilder fbb(1024);
    auto msg = model::CreateMessage(fbb, fbb.CreateString("username"), model::MessageContent_CommentAdded,
                         model::CreateCommentAdded(fbb, addr, fbb.CreateString(cmt.toStdString())).Union());

    fbb.Finish(msg);
    send(&fbb);
}

void Client::commentsDeleted(RVA addr) {
    qDebug() << "cmts rmved";
    flatbuffers::FlatBufferBuilder fbb(1024);
    auto msg = model::CreateMessage(fbb, fbb.CreateString("username"), model::MessageContent_CommentDeleted,
                         model::CreateCommentDeleted(fbb, addr).Union());
    fbb.Finish(msg);
    send(&fbb);
}
