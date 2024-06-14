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

void Worker::searching(QFileInfoList& infos, int cnt) {
    foreach(const QFileInfo& fi, infos) {
        if(fi.isDir()) {
            QDir dir(fi.absoluteFilePath());
            QFileInfoList il(dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot| QDir::Files));
            ++cnt;
            searching(il, cnt);
        }
        else {
            if(filters.contains(fi.suffix())) {
                if(cnt == 0) {
                    files->append(fi);
                }
                else {
                    files->append(QFileInfo(fi.dir().filesystemAbsolutePath()));
                    break;
                }
            }
        }
    }
}

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    rolenames[NameRole] = "name";
    rolenames[IconRole] = "img";
    connect(InitvaluesModel::instance(),&InitvaluesModel::entryChanged,this,&FileListModel::changeEntry);
    idx = -1;
    enablebackbtn = false;
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
        else if(role == IconRole) {
            if(info.isDirectory) {
                if(visitedPathes.contains(info.url))
                    res = "qrc:/icons/openfolder";
                else
                    res =  "qrc:/icons/musicfolder";
            }
            else {
                res =  "qrc:/icons/audio";
            }
        }
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
        map.insert("isDirectory",info.isDirectory);
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
    if(path.isEmpty()) {
        path = prevPath;
    }
    else {
        if(path.startsWith("file://")) {
            if(path[9] == ':')
                path = path.mid(8);
             else
                path = path.mid(7);
        }
        prevPath = path;
    }

    QThread* thread = new QThread();
    Worker*  worker = new Worker(path);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &Worker::prepareSearching);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(worker, &Worker::searchFinished, this, &FileListModel::searchFinished);
    connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    enablebackbtn = false;
    emit enableChanged();
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
            path = QDir::toNativeSeparators(path);

            if(fi.isDir())
                info.url = path;
            else
                info.url  = s + path;
            info.isDirectory = fi.isDir();
            info.name   = fi.fileName().mid(0, fi.fileName().lastIndexOf('.'));
            push(info);
        }
    }
}

void FileListModel::addFiles(QString path) {
    if(!visitedPathes.contains(path)) {
        visitedPathes.append(path);
    }
    QDir dir(path);
    QFileInfoList* il = new QFileInfoList(dir.entryInfoList(QDir::NoDotAndDotDot| QDir::Files));
    searchFinished(il);
    dir = QDir(prevPath);
    enablebackbtn = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot).size() > 1;
    emit enableChanged();
}

bool FileListModel::enableBackBtn() {
    return enablebackbtn;
}






