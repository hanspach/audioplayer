#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H
#include <QtCore>


struct Info
{
    QString name   = QString();
    QString folder = QString();
    QString url    = QString();
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
    Q_INVOKABLE void findFiles(const QString& = QString());
    Q_INVOKABLE void setIndex(int index);

signals:
    void indexChanged();

private slots:
    void searchFinished();
    void changeEntry();
private:
    static void searchMusicFolder(const QDir& dir, QFileInfoList& list);
    static QFileInfoList* searching(QFileInfoList& info, QFileInfoList* files, const QStringList& filters);
    static QFileInfoList* search(QFileInfoList* list);

    QList<Info> list;
    QHash<int, QByteArray> rolenames;
    QFuture<QFileInfoList*> future;
    QFutureWatcher<QFileInfoList*> watcher;
    static QString startFolder;
    int idx;
};
#endif
