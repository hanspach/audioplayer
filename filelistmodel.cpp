#include "filelistmodel.h"
#include "initvaluesmodel.h"
#include <QtConcurrent/QtConcurrent>

QString FileListModel::startFolder = QString();

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    rolenames[NameRole] = "name";
    rolenames[IconRole] = "img";
    rolenames[FolderRole] = "folder";

#ifdef Q_OS_LINUX
    startFolder = "/home";
#elif Q_OS_WIN
    startFolder = "C:\\Users";
#endif

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

void FileListModel::findFiles(const QString& folderName) {
    if(!folderName.isEmpty()) {
        startFolder = folderName;
    }
    connect(&watcher,&QFutureWatcher<QFileInfoList*>::finished,this,&FileListModel::searchFinished);
    QFileInfoList* files = new QFileInfoList();
    future = QtConcurrent::run(FileListModel::search, files);
    watcher.setFuture(future);
}

void FileListModel::searchFinished() {
    QFileInfoList* fl = future.result();

    if(fl && fl->size() > 0) {
        if(!list.empty()) {
            remove(0, list.size());
        }
        for(int i=0; i < fl->size(); i++) {
            const QFileInfo fi = fl->at(i);
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

QFileInfoList* FileListModel::searching(QFileInfoList& infos, QFileInfoList* files, const QStringList& filters) {
    foreach(const QFileInfo& fi, infos) {
        if(fi.isDir()) {
            QDir dir(fi.absoluteFilePath());
            QFileInfoList iList(dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot| QDir::Files));
            searching(iList,files,filters);
        }
        else {
            if(filters.contains(fi.suffix())) {
                files->append(fi);
            }
        }
    }
    return files;
}

QFileInfoList* FileListModel::search(QFileInfoList* files) {
    QStringList filters = QStringList() << "mp3";
    QDir d(startFolder);
    QFileInfoList list;
    FileListModel::searchMusicFolder(d, list);
    return FileListModel::searching(list,files,filters);
}

void FileListModel::searchMusicFolder(const QDir& dir, QFileInfoList& list) {
    foreach(const QFileInfo& fi, dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot)) {
        if(fi.isDir()) {
            QDir d(fi.absoluteFilePath());
            foreach(const QFileInfo& fi, d.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot)) {
                if(fi.absoluteFilePath().endsWith("Music")) {
                    list.append(fi);
                }
            }
        }
    }
}





