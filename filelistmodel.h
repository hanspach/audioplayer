#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H
#include <QtCore>


struct Info
{
    QString name   = QString();
    QString folder = QString();
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
    void searching(QFileInfoList&);

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
        FolderRole
    };

    Q_PROPERTY(int idx  READ index  NOTIFY indexChanged FINAL)

    explicit FileListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override { return rolenames; }
    void push(const Info& info);
    void remove(int index, int count);
    int index();

    Q_INVOKABLE QVariant item(int idx);
    Q_INVOKABLE QString standardPath(int location = 4); // QStandardPath::MusicLocation
    Q_INVOKABLE void findFiles(QString = QString());
    Q_INVOKABLE void setIndex(int index);

signals:
    void indexChanged();

private slots:
    void searchFinished(QFileInfoList*);
    void changeEntry();
private:
    QHash<int, QByteArray> rolenames;
    QList<Info> list;
    int idx;
};

#endif
