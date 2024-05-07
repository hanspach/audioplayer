#ifndef INITVALUESMODEL_H
#define INITVALUESMODEL_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QtNetwork>

struct data {
    int volume;
    int itemsize;
    QString defaultcountry;
    QJsonArray jsFavorites;
    QString favoriteString;
};

Q_DECLARE_METATYPE(data);

class AsyncFileReader : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString &param);

signals:
    void resultReady(const struct data& result);
};

class InitvaluesModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString defaultcountry READ defaultCountry NOTIFY defaultCountryChanged FINAL)
    Q_PROPERTY(int vlm READ volume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(int itemsize READ itemSize WRITE setItemSize NOTIFY itemsizeChanged FINAL)
    Q_PROPERTY(QString favoritestring READ favoriteString NOTIFY favoritestringChanged FINAL)
    Q_PROPERTY(QString poolurlstring READ poolUrlString NOTIFY urlChanged FINAL)
    Q_PROPERTY(QString statusrectcolor READ statusRectColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(QString playbtnIconsrc READ playbtnIconSrc  NOTIFY iconsrcChanged FINAL)
    Q_PROPERTY(QString durationtxt READ durationTxt NOTIFY durationChanged FINAL)
    Q_PROPERTY(QString msg READ message NOTIFY msgChanged FINAL)
public:
    static InitvaluesModel* instance();
    virtual ~InitvaluesModel();

    QString defaultCountry();
    int volume();
    int itemSize();
    void setItemSize(int size);
    QString favoriteString();
    QString statusRectColor();
    void setStatusRectColor(QString color);
    QString playbtnIconSrc();
    void setPlaybtnIconSrc(QString src);
    QString durationTxt();
    QString message();
    QString poolUrlString();
    void icyMetaIntRequest(const QUrl);
    void icyMetaDataRequest();

    Q_INVOKABLE void writeJsonFile(QString fileName = "init.json");
    Q_INVOKABLE void setDefaultCountry(QString country);
    Q_INVOKABLE void setVolume(int volume);
    Q_INVOKABLE void setFavoriteString(QString favorite = QString());
    Q_INVOKABLE void addFavorite(QJsonObject);
    Q_INVOKABLE void removeFavorite(QVariantMap);

    Q_INVOKABLE void changeMessage(QString lblmsg, int delay=0, QString color=QString());
    Q_INVOKABLE void startStopTimer(int status);
    Q_INVOKABLE void setMsgBarWidth(int width);
    Q_INVOKABLE void changeStatusRectColor(QString color);
    Q_INVOKABLE void changePlayIcon(QString src);
    Q_INVOKABLE void locationRequest(QString url);
    Q_INVOKABLE void nextEntry();


private slots:
    void handleResults(const struct data& result);
    void locationFinished();
    void icyMetaIntRead();
    void icyMetaDataRead();
    void secElapsed();
    void handleMessage();

signals:
    void operate(const QString&);
    void tagListChanged();
    void defaultCountryChanged();
    void volumeChanged();
    void itemsizeChanged();
    void favoritestringChanged();
    void colorChanged();
    void iconsrcChanged();
    void durationChanged();
    void msgChanged();
    void urlChanged();
    void entryChanged();

private:
    InitvaluesModel(QObject *parent = nullptr);
    static InitvaluesModel* that;

    int itemsize;
    int vlm;
    QString defaultcountry;
    QString statusrectcolor;
    QString playbtnIconsrc;
    QJsonArray favorites;

    QThread workerThread;
    QTimer* secTimer;

    QTimer* msgTimer;
    int delay;
    int pos;
    int cnt;
    QString orgmsg;
    int msgbarwidth;



    QTime  secTime;
    int timeout;
    QString favoritestring;
    QString durationtxt;
    QString msg;
    QNetworkAccessManager qnam;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    QString uri;
    QString poolurlstring;
    int bufferSize;
    QString title;
};

#endif // INITVALUESMODEL_H
