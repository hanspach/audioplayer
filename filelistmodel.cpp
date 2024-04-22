#include "filelistmodel.h"
#include "initvaluesmodel.h"

Worker::Worker(const QString& path, const QStringList& list) {
    startPath = path;
    filters = list;
    files = new QFileInfoList();
}

void Worker::prepareSearching() {
    QDir dir(startPath);
    QFileInfoList infos = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot| QDir::Files);
    searching(infos);
    emit searchFinished(files);
    emit finished();
}

void Worker::searching(QFileInfoList& infos) {
    foreach(const QFileInfo& fi, infos) {
        if(fi.isDir()) {
            QDir dir(fi.absoluteFilePath());
            QFileInfoList il(dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot| QDir::Files));
            searching(il);
        }
        else {
            if(filters.contains(fi.suffix())) {
                files->append(fi);
            }
        }
    }
}

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    rolenames[NameRole] = "name";
    rolenames[IconRole] = "img";
    rolenames[FolderRole] = "folder";

    connect(InitvaluesModel::instance(),&InitvaluesModel::entryChanged,this,&FileListModel::changeEntry);
    idx = -1;
}

int FileListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return list.count();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const {
    QVariant res = QVariant();

    if(index.row() >= 0 && index.row() < list.count()) {
        const Info &info = list.at(index.row());
        if(role == NameRole)
            res = info.name;
        else if(role == IconRole)
            res =  "qrc:/icons/audio";
        else if(role == FolderRole)
            res = info.folder;
    }
    return res;
}

void FileListModel::push(const Info& info) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    list << info;
    endInsertRows();
}

void FileListModel::remove(int index, int count) {
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    for (int row = 0; row < count; ++row) {
        list.removeAt(index);
    }
    endRemoveRows();
}

QVariant FileListModel::item(int idx) {
    if(idx >= 0 && idx < list.size()) {
        const Info& info = list.at(idx);
        QVariantMap map;
        map.insert("name",  info.name);
        map.insert("folder",info.folder);
        map.insert("url",  info.url);
        return QVariant(map);
    }
    return QVariant();
}

void FileListModel::changeEntry() {
    if(idx + 1 < list.size()) {
        ++idx;
        emit indexChanged();
    }
}

int FileListModel::index() {
   return idx;
}

void FileListModel::setIndex(int index) {
    idx = index;
}

QString FileListModel::standardPath(int location) {
    QStandardPaths::StandardLocation loc = static_cast<QStandardPaths::StandardLocation>(location);
    QStringList list = QStandardPaths::standardLocations(loc);
    QString res = QString();

    if(!list.empty()) {
        res = list[0];
        if(!res.endsWith("Music")) {
            res += "/Music";
        }
    }
    return res;
}

void FileListModel::findFiles(QString path) {
    if(path.startsWith("file://")) {
        if(path[9] == ':')
            path = path.mid(8);
         else
            path = path.mid(7);
    }

    QThread* thread = new QThread();
    Worker*  worker = new Worker(path);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &Worker::prepareSearching);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(worker, &Worker::searchFinished, this, &FileListModel::searchFinished);
    connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}



void FileListModel::searchFinished(QFileInfoList* files) {
    if(files && files->size() > 0) {
        if(!list.empty()) {
            remove(0, list.size());
        }
        for(int i=0; i < files->size(); i++) {
            const QFileInfo fi = files->at(i);
            Info info;
            QString s = "file:/";
            QString path = fi.absoluteFilePath();

#ifdef Q_OS_WIN
            static QRegularExpression regex("\\[m]");
            path = path.replace(regex, "/");
#endif
            info.url  = s + path;
            info.folder = fi.absoluteDir().dirName();
            info.name   = fi.fileName().mid(0, fi.fileName().lastIndexOf('.'));
            push(info);
        }
    }
}






