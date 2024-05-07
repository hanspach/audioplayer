#include "initvaluesmodel.h"
#include <QtCore>

void AsyncFileReader::doWork(const QString& fileName) {
    struct data result;
    result.volume = 30;
    result.defaultcountry = "Germany";
    result.itemsize = 100;
    result.jsFavorites = QJsonArray();
    result.favoriteString = QString();

    QFile file(fileName);
    if(file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray a = file.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(a, &jsonError);
        if(jsonError.error == QJsonParseError::NoError) {
            const QJsonObject& o = doc.object();
            if(!o.isEmpty()) {
                result.volume = o["volume"].toInt();
                result.defaultcountry = o["defaultCountry"].toString();
                result.itemsize = o["itemsize"].toInt();
                result.jsFavorites = o["favorites"].toArray();

                QJsonDocument docb(result.jsFavorites);
                QByteArray bytes = docb.toJson();
                result.favoriteString = QString(bytes);
            }
        }
    }
    emit resultReady(result);
}

InitvaluesModel* InitvaluesModel::that = nullptr;

InitvaluesModel* InitvaluesModel::instance() {
    if(InitvaluesModel::that == nullptr) {
        that = new InitvaluesModel();
    }
    return that;
}

InitvaluesModel::InitvaluesModel(QObject *parent)
    : QObject(parent)  {

    secTimer = new QTimer(parent);
    secTimer->setInterval(1000);
    AsyncFileReader *worker = new AsyncFileReader;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &InitvaluesModel::operate, worker, &AsyncFileReader::doWork);
    connect(worker, &AsyncFileReader::resultReady, this, &InitvaluesModel::handleResults);
    connect(secTimer, &QTimer::timeout, this, &InitvaluesModel::secElapsed);
    workerThread.start();

    msgTimer = new QTimer(parent);
    connect(msgTimer, &QTimer::timeout, this, &InitvaluesModel::handleMessage);

    favoritestring  = QString();
    msg             = QString();
    poolurlstring   =  QString();
    statusrectcolor = "lightgrey";
    durationtxt     = QString();
    playbtnIconsrc  = "qrc:/icons/pause";
    favorites  = QJsonArray();
}

InitvaluesModel::~InitvaluesModel() {
    workerThread.quit();
    workerThread.wait();
}

int InitvaluesModel::volume() {
    return vlm;
}

void InitvaluesModel::setVolume(int volume) {
    vlm = volume;
    emit volumeChanged();
}

QString InitvaluesModel::defaultCountry() {
    return defaultcountry;
}

void InitvaluesModel::setDefaultCountry(QString country) {
    defaultcountry = country;
    emit defaultCountryChanged();
}

int InitvaluesModel::itemSize() {
    return itemsize;
}

void InitvaluesModel::setItemSize(int size) {
    itemsize = size;
    emit itemsizeChanged();
}

QString InitvaluesModel::statusRectColor() {
    return statusrectcolor;
}

void InitvaluesModel::changeStatusRectColor(QString color) {
    statusrectcolor = color;
    emit colorChanged();
}

QString InitvaluesModel::playbtnIconSrc() {
    return playbtnIconsrc;
}

void InitvaluesModel::changePlayIcon(QString src) {
    playbtnIconsrc = src;
    emit iconsrcChanged();
}



QString InitvaluesModel::poolUrlString() {
    return poolurlstring;
}

QString InitvaluesModel::durationTxt() {
    return durationtxt;
}

QString InitvaluesModel::message() {
    return msg;
}

void InitvaluesModel::changeMessage(QString lblmsg, int delay, QString color) {
    if(!lblmsg.isEmpty()) {
        msgTimer->stop();
        msg = lblmsg;
        int charwidth= 8;
        int txtwidth = lblmsg.length() * charwidth;
        cnt = msgbarwidth / charwidth;
        pos = 1;
        this->delay = delay;

        if(delay > 0) {
            msg = msg.mid(0, cnt);
            emit msgChanged();
            msgTimer->start(delay);
        }
        else {
            if(txtwidth > msgbarwidth) {
                for(int i=0; i < cnt; i++) {
                    msg = " " + msg;
                }
                orgmsg = msg;
                msgTimer->start(100);
             }
             else {
                 emit msgChanged();
             }
        }

        if(!color.isEmpty()) {
            changeStatusRectColor(color);
        }
    }
}

void InitvaluesModel::handleMessage() {
    static int times = 0;
    static int ds = 600;

    if(delay > 100) {
        msg = "";
        emit msgChanged();
        msgTimer->stop();
    }
    else {
        ++ds;
        if(pos  <= orgmsg.length()) {
            msg = orgmsg.mid(pos, cnt);
            emit msgChanged();
            ++pos;
        }
        else {
            if(times < 3) {
                pos = 1;
                ++times;
            }
            else {
                if(ds > 600) {
                    times = 0;
                    ds   = 0;
                }
            }
        }
    }
}

void InitvaluesModel::setMsgBarWidth(int width) {
    msgbarwidth = width;
}

void InitvaluesModel::startStopTimer(int status) {
    if(status)
        secTimer->start();
    else
        secTimer->stop();
}

void InitvaluesModel::secElapsed() {
    if(timeout < 5) {
        secTime = secTime.addSecs(1);
        durationtxt = secTime.toString("h:mm:ss");
        emit durationChanged();
    }
    else {
        secTimer->stop();
        changeMessage(tr("connection lost"),2000,"red");
       //locationFinished(); // ?????????????
    }
    ++timeout;
}

void InitvaluesModel::locationRequest(QString url) {
    uri = url;
    timeout= 0;
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    req.setMaximumRedirectsAllowed(3);
    req.setTransferTimeout(50);
    reply.reset(qnam.get(req));
    connect(reply.get(), &QNetworkReply::finished, this, &InitvaluesModel::locationFinished);
}

void InitvaluesModel::locationFinished() {
    if(reply->hasRawHeader("Location")) {
        poolurlstring = QString::fromUtf8(reply->rawHeader("Location"));
    }
    else {
        poolurlstring = uri;
    }
    emit urlChanged();
    icyMetaIntRequest(QUrl(poolurlstring));
    secTime.setHMS(0,0,0);
    durationtxt = secTime.toString("h:mm:ss");
    emit durationChanged();
    secTimer->start();
}

void InitvaluesModel::icyMetaIntRequest(QUrl url) {
    QNetworkRequest req(url);
    req.setRawHeader("userAgent", "icecast-parser");
    req.setRawHeader("Icy-MetaData", "1");
    reply.reset(qnam.get(req));
    connect(reply.get(), &QNetworkReply::readyRead, this, &InitvaluesModel::icyMetaIntRead);
}

void InitvaluesModel::icyMetaIntRead() {
    if(reply->hasRawHeader("icy-metaint")) {
        QString sint = QString::fromUtf8(reply->rawHeader("icy-metaint"));
        bool ok;
        int num = sint.toInt(&ok);
        if(ok) {
            bufferSize = num;
        }
    }
    icyMetaDataRequest();
}

void InitvaluesModel::icyMetaDataRequest() {
    QNetworkRequest req(poolurlstring);
    req.setRawHeader("userAgent", "icecast-parser");
    req.setRawHeader("Icy-MetaData", "1");
    req.setRawHeader("Access-Control-Allow-Origin", "*");
    req.setRawHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
    req.setRawHeader("Access-Control-Allow-Headers", "Content-Type, Icy-Metadata");

    reply.reset(qnam.get(req));
    connect(reply.get(), &QNetworkReply::readyRead, this, &InitvaluesModel::icyMetaDataRead);
}

void InitvaluesModel::icyMetaDataRead() {
    if(reply->isReadable()) {
        QByteArray a = reply->read(bufferSize);
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString s = toUtf16(a);

        int idx = s.indexOf("StreamTitle");
        if(idx != -1) {
            idx += 13;
            s = s.mid(idx, 128);
            idx = s.indexOf("'");
            if(idx != -1) {
                s = s.mid(0, idx);
                if(!s.isEmpty() && s.compare(title) != 0) {
                    title = s;
                    changeMessage(title);
                }
            }
        }
    }
    timeout = 0;
}

void InitvaluesModel::addFavorite(QJsonObject obj) {
    favorites.append(obj);
}

void InitvaluesModel::removeFavorite(QVariantMap map) {
    for(int i=0; i < favorites.size(); i++) {
        if(favorites.at(i)["name"].toString().compare(map["name"].toString()) == 0) {
            favorites.removeAt(i);
            break;
        }
    }
}

QString InitvaluesModel::favoriteString() {
    return favoritestring;
}

void InitvaluesModel::setFavoriteString(QString favorite) {
    if(favorite.isEmpty()) {
        QJsonDocument doc(favorites);
        QByteArray bytes = doc.toJson();
        favorite = QString(bytes);
    }
    favoritestring = favorite;
    emit favoritestringChanged();
}

void InitvaluesModel::nextEntry() {
    emit entryChanged();
}

void InitvaluesModel::handleResults(const struct data& result) {
    setDefaultCountry(result.defaultcountry);
    setVolume(result.volume);
    setItemSize(result.itemsize);
    favorites = result.jsFavorites;
    setFavoriteString(result.favoriteString);
}

void InitvaluesModel::writeJsonFile(QString fileName) {
    QFile file(fileName);
    if(file.exists()) {
        file.remove();
    }
    QJsonObject content;

    content.insert("volume", vlm);
    content.insert("defaultCountry", defaultcountry);
    content.insert("itemsize", itemsize);
    content["favorites"] = favorites;

    QJsonDocument doc(content);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);
    if(file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)) {
        QTextStream is(&file);
        is << bytes;
        file.close();
    }
}


