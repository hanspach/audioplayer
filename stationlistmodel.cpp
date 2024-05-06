#include "stationlistmodel.h"
#include "initvaluesmodel.h"

bool ProgramProps::read(const QJsonObject& json) {
    bool res = true;

    name    = QString();
    url     = QString();
    favicon = QString();
    country = QString();
    codec   = QString();

    if(json.contains("name"))
        name = json["name"].toString().trimmed().mid(0,40);
    if(json.contains("url"))
        url = json["url"].toString();
    if(json.contains("favicon"))
        favicon = json["favicon"].toString();
    if(json.contains("country"))
        country = json["country"].toString();
    if(json.contains("codec"))
        codec = json["codec"].toString();

    if(name.isEmpty())
        res = false;
    if(url.isEmpty())
        res = false;

    return res;
}

QString ProgramProps::toString() const {
    return QString("%1 %2 %3").arg(name,url,country);
}

StationListModel::StationListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    rolenames[NameRole] = "name";
    rolenames[IconRole] = "img";
}

int StationListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return list.count();
}

QVariant StationListModel::data(const QModelIndex &index, int role) const {
    QVariant res = QVariant();

    if(index.row() >= 0 && index.row() < list.count()) {
        const ProgramProps &props = list[index.row()];
        if(role == NameRole)
            res = props.name;
        else if(role == IconRole)
            res =  "qrc:/icons/audio";
    }
    return res;
}

void StationListModel::push(const ProgramProps &props) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    list << props;
    endInsertRows();
}

void StationListModel::remove(int index, int count) {
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    for (int row = 0; row < count; ++row) {
        list.removeAt(index);
    }
    endRemoveRows();
}

QVariant StationListModel::item(int idx) {
    if(idx >= 0 && idx < list.size()) {
        const ProgramProps& props = list.at(idx);
        QVariantMap map;
        map.insert("name", props.name);
        map.insert("url", props.url);
        map.insert("country", props.country);
        map.insert("favicon", props.favicon);
        map.insert("codec", props.codec);
        return QVariant(map);
    }
    return QVariant();
}

void StationListModel::stationRequest(QString country, QString tag, QString url) {
    mCountry = country;
    checkCountry = !country.isEmpty() && !tag.isEmpty();
    if(!country.isEmpty() && tag.isEmpty()) {
        url += "/bycountry/" + country;
    }
    else if(!tag.isEmpty()) {
        url +=  "/bytag/" + tag;
        mCountry = country;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json");
    manager->get(request);
    connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(stationsFinished(QNetworkReply*)));
}

void StationListModel::stationsFinished(QNetworkReply* reply) {
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(status == 200) {
        QByteArray a = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(a);
        ProgramProps pp;
        if(list.size() > 0) {
            remove(0, list.size());
        }
        if(doc.isArray()) {
            foreach(QJsonValue val, doc.array()) {
                const QJsonObject& jo = val.toObject();
                if(pp.read(jo)) {
                    if(!checkCountry || pp.country.compare(mCountry) == 0) {
                        push(pp);
                    }
                }
            }
            const int size = list.size();
            if(size > 9) {
                InitvaluesModel::instance()->changeMessage(QString::number(size) + " " + tr("radio programs"),2500);
            }
        }
    }
    else {
        InitvaluesModel::instance()->changeMessage("Network-Error",5000,"red");
    }
    reply->deleteLater();
}


