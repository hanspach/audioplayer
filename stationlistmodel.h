#ifndef STATIONLISTMODEL_H
#define STATIONLISTMODEL_H
#include <QtCore>
#include <QtNetwork>

struct ProgramProps
{
    QString name;
    QString country;
    QString url;
    QString favicon;
    QString codec;

    bool read(const QJsonObject& json);
    QString toString() const;
};

class StationListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PropRoles {
        IconRole = Qt::UserRole + 1,
        NameRole
    };

    explicit StationListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override { return rolenames; }
    void push(const ProgramProps &props);
    void remove(int index, int count = 1);

    Q_INVOKABLE QVariant item(int idx);
    Q_INVOKABLE void stationRequest(QString country, QString tag, QString url="http://de1.api.radio-browser.info/json/stations");

private slots:
    void stationsFinished(QNetworkReply*);

private:
    QList<ProgramProps> list;
    QHash<int, QByteArray> rolenames;
    bool checkCountry;
    QString mCountry;
    int listsize;
};
#endif
