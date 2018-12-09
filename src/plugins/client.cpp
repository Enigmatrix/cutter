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
    uuid = QUuid::createUuid();
    networkManager = new QNetworkAccessManager();
    url = QUrl("http://makerforce.io:8080");
    //do smth with nick here
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
    if (res->bytesAvailable() < sizeof(model::Message)) {
        return;
    }
    auto bytes = res->read(sizeof(model::Message));
    auto m = reinterpret_cast<model::Message*>(bytes.data());
    understandMessage(m);
}

void Client::understandMessage(model::Message* m) {

}

void Client::send(flatbuffers::FlatBufferBuilder *fbb) {
    qDebug() << "making req..";
    auto req = QNetworkRequest(url);
    qDebug() << &req;
    qDebug() << "making byte array...";
    auto body = QByteArray(reinterpret_cast<char*>(fbb->GetBufferPointer()), fbb->GetSize());
    qDebug() << &body;
    qDebug() << "posting...";
    networkManager->post(req, body);
    qDebug() << "posted";
}
void Client::commentsAdded(RVA addr, QString cmt) {
    qDebug() << "cmts add";
    flatbuffers::FlatBufferBuilder fbb(1024);
    auto msg = model::CreateMessage(fbb, fbb.CreateString("username"), model::MessageContent_CommentAdded,
                         model::CreateCommentAdded(fbb, addr, fbb.CreateString(cmt.toStdString())).Union());

    send(&fbb);
}

void Client::commentsDeleted(RVA addr) {
    qDebug() << "cmts rmved";
    flatbuffers::FlatBufferBuilder fbb(1024);
    auto msg = model::CreateMessage(fbb, fbb.CreateString("username"), model::MessageContent_CommentDeleted,
                         model::CreateCommentDeleted(fbb, addr).Union());
    send(&fbb);
}
