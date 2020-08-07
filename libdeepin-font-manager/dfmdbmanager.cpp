#include "dfmdbmanager.h"
#include "dfontinfomanager.h"

#include <QDir>


static DFMDBManager *INSTANCE = nullptr;

DFMDBManager::DFMDBManager(QObject *parent)
    : QObject(parent)
    , m_sqlUtil(new DSqliteUtil(QDir::homePath() + "/.local/share/deepin/deepin-font-manager/.font_manager.db"))
{
}

DFMDBManager::~DFMDBManager()
{
    delete m_sqlUtil;
}

DFMDBManager *DFMDBManager::instance()
{
    if (!INSTANCE) {
        INSTANCE = new DFMDBManager;
    }

    return INSTANCE;
}

bool DFMDBManager::isSystemFont(const QString &filePath)
{
    return filePath.contains("/usr/share/fonts/");
}

bool DFMDBManager::isUserFont(const QString &filePath)
{
    return filePath.contains(QDir::homePath() + "/.local/share/fonts");
}

DFontPreviewItemData DFMDBManager::parseRecordToItemData(const QMap<QString, QString> &record)
{
    DFontPreviewItemData itemData;

    itemData.strFontId = record.value("fontId");
    QString filePath = record.value("filePath");
    itemData.fontData = FontData(record.value("fontName"), record.value("isEnabled").toInt()
                                 , record.value("isCollected").toInt()
                                 , record.value("isChineseFont").toInt()
                                 , record.value("isMonoSpace").toInt(), TTF);
    QFileInfo filePathInfo(filePath);
    filePathInfo.setCaching(false);

    itemData.fontInfo = getDFontInfo(record);
    itemData.fontData.setFontType(itemData.fontInfo.type);
    if (!itemData.fontData.strFontName.endsWith(itemData.fontInfo.styleName) && !itemData.fontInfo.styleName.isEmpty())
        itemData.fontData.strFontName += QString("-%1").arg(itemData.fontInfo.styleName);

    return itemData;
}

DFontInfo DFMDBManager::getDFontInfo(const QMap<QString, QString> &record)
{
    DFontInfo fontInfo;

    fontInfo.filePath = record.value("filePath");
    fontInfo.familyName = record.value("familyName");
    fontInfo.styleName = record.value("styleName");
    fontInfo.type = record.value("type");
    fontInfo.version = record.value("version");
    fontInfo.copyright = record.value("copyright");
    fontInfo.description = record.value("description");
    fontInfo.sysVersion = record.value("sysVersion");
    fontInfo.isInstalled = record.value("isInstalled").toInt();
    fontInfo.isError = record.value("isError").toInt();
    fontInfo.isSystemFont = isSystemFont(fontInfo.filePath);
    //add
    fontInfo.fullname = record.value("fullname");
    fontInfo.psname = record.value("psname");
    fontInfo.trademark = record.value("trademark");
    DFontInfoManager::instance()->getDefaultPreview(fontInfo);

    return fontInfo;
}

void DFMDBManager::appendAllKeys(QList<QString> &keyList)
{
    keyList.append("fontId");
    keyList.append("fontName");
    keyList.append("isEnabled");
    keyList.append("isCollected");
    keyList.append("filePath");
    keyList.append("familyName");
    keyList.append("styleName");
    keyList.append("type");
    keyList.append("version");
    keyList.append("copyright");
    keyList.append("description");
    keyList.append("sysVersion");
    keyList.append("isInstalled");
    keyList.append("isError");
    keyList.append("isChineseFont");
    keyList.append("isMonoSpace");
    //add
    keyList.append("fullname");
    keyList.append("psname");
    keyList.append("trademark");
}

QList<DFontPreviewItemData> DFMDBManager::getAllFontInfo()
{
    QList<DFontPreviewItemData> fontItemDataList;

    QList<QMap<QString, QString>> recordList;

    QList<QString> keyList;
    appendAllKeys(keyList);

    m_sqlUtil->findAllRecords(keyList, recordList);
    for (QMap<QString, QString> &record : recordList) {
        if (record.size() > 0) {
            DFontPreviewItemData itemData = parseRecordToItemData(record);
            fontItemDataList.push_back(itemData);
        }
    }

    return fontItemDataList;
}

int DFMDBManager::getRecordCount()
{
    return m_sqlUtil->getRecordCount();
}

int DFMDBManager::getCurrMaxFontId()
{
    return m_sqlUtil->getMaxFontId();
}
//获取已安装字体路径
QStringList DFMDBManager::getInstalledFontsPath()
{
    return m_sqlUtil->getInstalledFontsPath();
}

int DFMDBManager::isFontExist(const QString &familyName, const QString &styleName, QStringList &result)
{
    QList<QMap<QString, QString>> recordList;

    QList<QString> keyList;
    appendAllKeys(keyList);

    QMap<QString, QString> whereMap;
    whereMap.insert("familyName", familyName);
    whereMap.insert("styleName", styleName);
    m_sqlUtil->findRecords(keyList, whereMap, &recordList);

    result.clear();
    for (QMap<QString, QString> &map : recordList) {
        result << map.value("filePath");
        result << map.value("fullname");
    }

    return recordList.size();
}

QString DFMDBManager::isFontInfoExist(const DFontInfo &newFileFontInfo)
{
    QList<QMap<QString, QString>> recordList;

    QList<QString> keyList;
    keyList.append("filePath");

    QMap<QString, QString> whereMap;
    whereMap.insert("familyName", newFileFontInfo.familyName);
    whereMap.insert("styleName", newFileFontInfo.styleName);
    whereMap.insert("fullname", newFileFontInfo.fullname);
    m_sqlUtil->findRecords(keyList, whereMap, &recordList);

    if (recordList.size() > 0) {
        QString result = recordList.first().value("filePath");
        return result;
    }

    return QString();
}

QMap<QString, QString> DFMDBManager::mapItemData(DFontPreviewItemData itemData)
{
    QMap<QString, QString> mapData;
    //mapData.insert("fontId", itemData.strFontId);   //auto increament ,Don't need supply
    mapData.insert("fontName", itemData.fontData.strFontName);
    mapData.insert("isEnabled", QString::number(itemData.fontData.isEnabled()));
    mapData.insert("isCollected", QString::number(itemData.fontData.isCollected()));
    mapData.insert("isChineseFont", QString::number(itemData.fontData.isChinese()));
    mapData.insert("isMonoSpace", QString::number(itemData.fontData.isMonoSpace()));
    mapData.insert("filePath", itemData.fontInfo.filePath);
    mapData.insert("familyName", itemData.fontInfo.familyName);
    mapData.insert("styleName", itemData.fontInfo.styleName);
    mapData.insert("type", itemData.fontInfo.type);
    mapData.insert("version", itemData.fontInfo.version);
    mapData.insert("copyright", itemData.fontInfo.copyright);
    mapData.insert("description", itemData.fontInfo.description);
    mapData.insert("sysVersion", itemData.fontInfo.sysVersion);
    mapData.insert("isInstalled", QString::number(itemData.fontInfo.isInstalled));
    mapData.insert("isError", QString::number(itemData.fontInfo.isError));
    //add
    mapData.insert("fullname", itemData.fontInfo.fullname);
    mapData.insert("psname", itemData.fontInfo.psname);
    mapData.insert("trademark", itemData.fontInfo.trademark);

    return mapData;
}

bool DFMDBManager::addFontInfo(const DFontPreviewItemData &itemData)
{
//    qDebug() << __FUNCTION__ << itemData.fontInfo.toString();
    if (!m_addFontList.contains(itemData) || itemData.fontInfo.isSystemFont)
//        m_addFontList << itemData;
        m_addFontList.append(itemData);
    return true;
//    return m_sqlUtil->addRecord(mapItemData(itemData));
}

bool DFMDBManager::deleteFontInfoByFontMap(const QMap<QString, QString> &fontDelMap)
{
    return m_sqlUtil->delRecord(fontDelMap);
}

bool DFMDBManager::updateFontInfo(const QMap<QString, QString> &whereMap, const QMap<QString, QString> &dataMap)
{
    return m_sqlUtil->updateRecord(whereMap, dataMap);
}

bool DFMDBManager::updateFontInfoByFontId(const QString &strFontId, const QMap<QString, QString> &dataMap)
{
    QMap<QString, QString> where;
    where.insert("fontId", strFontId);

    return m_sqlUtil->updateRecord(where, dataMap);
}

bool DFMDBManager::updateFontInfoByFontId(const QString &strFontId, const QString &strKey, const QString &strValue)
{
    QMap<QString, QString> where;
    where.insert("fontId", strFontId);

    QMap<QString, QString> dataMap;
    dataMap.insert(strKey, strValue);

    return m_sqlUtil->updateRecord(where, dataMap);
}

bool DFMDBManager::updateFontInfoByFontFilePath(const QString &strFontFilePath, const QString &strKey, const QString &strValue)
{
    QMap<QString, QString> where;
    where.insert("filePath", strFontFilePath);

    QMap<QString, QString> dataMap;
    dataMap.insert(strKey, strValue);

    return m_sqlUtil->updateRecord(where, dataMap);
}

void DFMDBManager::commitAddFontInfo()
{
    if (m_addFontList.isEmpty())
        return;

//    QMutexLocker locker(&m_mutex);
    beginTransaction();
    addFontInfo(m_addFontList);
    endTransaction();
    m_addFontList.clear();
}

void DFMDBManager::addFontInfo(const QList<DFontPreviewItemData> &fontList)
{
    return m_sqlUtil->addFontInfo(fontList);
}

void DFMDBManager::deleteFontInfo(const DFontPreviewItemData &itemData)
{
    if (!m_delFontList.contains(itemData))
        m_delFontList << itemData;
}

void DFMDBManager::deleteFontInfo(const QList<DFontPreviewItemData> &fontList)
{
    m_sqlUtil->deleteFontInfo(fontList);
}

void DFMDBManager::commitDeleteFontInfo()
{
    if (m_delFontList.isEmpty())
        return;

    beginTransaction();
    m_sqlUtil->deleteFontInfo(m_delFontList);
    endTransaction();
    m_delFontList.clear();
}

void DFMDBManager::updateFontInfo(const DFontPreviewItemData &itemData, const QString &strKey)
{
    if (!m_updateFontList.contains(itemData) || itemData.fontInfo.isSystemFont) {
        m_updateFontList << itemData;
        if (m_strKey != strKey)
            m_strKey = strKey;
    }
}

void DFMDBManager::updateFontInfo(const QList<DFontPreviewItemData> &fontList, const QString &strKey)
{
    return m_sqlUtil->updateFontInfo(fontList, strKey);
}

void DFMDBManager::commitUpdateFontInfo()
{
    if (m_updateFontList.isEmpty())
        return;

    beginTransaction();
    m_sqlUtil->updateFontInfo(m_updateFontList, m_strKey);
    endTransaction();
    m_updateFontList.clear();
}

void DFMDBManager::beginTransaction()
{
    m_sqlUtil->m_db.transaction();
}

void DFMDBManager::endTransaction()
{
    m_sqlUtil->m_db.commit();
}
