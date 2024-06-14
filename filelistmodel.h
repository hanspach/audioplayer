#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H
#include <QtCore>


struct Info
{
    QString name   = QString();
    bool isDirectory = false;
    QString url    = QString();
};

class Worker : public QObject {
    Q_OBJECT

public:
    explicit Worker(const QString& path, const QStringList& list = QStringList() << "mp3");

public slots:
    void prepareSearching();

signals:
    void finished();
    void searchFinished(QFileInfoList*);

private:
    void searching(QFileInfoList&, int cnt=0);

    QString startPath;
    QStringList filters;
    QFileInfoList* files;
};

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PropRoles {
        IconRole = Qt::UserRole + 1,
        NameRole,
    };

    Q_PROPERTY(int idx  READ index  NOTIFY indexChanged FINAL)
    Q_PROPERTY(bool enableBackBtn READ enableBackBtn NOTIFY enableChanged)

    explicit FileListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override { return rolenames; }
    void push(const Info& info);
    void remove(int index, int count);
    bool enableBackBtn();
    int index();

    Q_INVOKABLE QVariant item(int idx);
    Q_INVOKABLE QString standardPath(int location = 4); // QStandardPath::MusicLocation
    Q_INVOKABLE void findFiles(QString = QString());
    Q_INVOKABLE void addFiles(QString path);
    Q_INVOKABLE void setIndex(int index);

signals:
    void indexChanged();
    void enableChanged();

private slots:
    void searchFinished(QFileInfoList*);
    void changeEntry();
private:
    QHash<int, QByteArray> rolenames;
    QList<Info> list;

    QStringList visitedPathes;
    QString prevPath;
    int idx;
    bool enablebackbtn;
};

#endif
