#include <gmssl/sm2.h>
#include <gmssl/sm3.h>
#include <gmssl/sm4.h>
#include <QFile>
#include <QSqlQuery>
#include <QDebug>


class LogUpload {
public:
    static bool uploadLog(const QString &filePath, const QString &privateKey, const QString &publicKey) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开文件：" << filePath;
            return false;
        }

        QByteArray fileData = file.readAll();

        // 使用SM3生成文件的哈希值
        unsigned char hash[32];
        sm3(fileData.constData(), fileData.size(), hash);

        // 使用SM2对哈希值进行签名
        unsigned char signature[256];
        int signature_len = sm2_sign(hash, 32, privateKey.toUtf8().constData(), signature);

        // 使用SM4对日志内容进行加密
        unsigned char key[16] = {0};  // 16字节的密钥
        unsigned char iv[16] = {0};   // 16字节的初始化向量
        unsigned char encryptedData[1024];
        int encryptedDataLen = sm4_crypt_cbc(1, key, iv, fileData.constData(), fileData.size(), encryptedData);

        // 将加密后的数据存储到数据库
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("logsys.db");
        if (!db.open()) {
            qDebug() << "数据库连接失败！";
            return false;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO logs (filename, content, signature) VALUES (:filename, :content, :signature)");
        query.bindValue(":filename", file.fileName());
        query.bindValue(":content", QByteArray(reinterpret_cast<char*>(encryptedData), encryptedDataLen).toHex());
        query.bindValue(":signature", QByteArray(reinterpret_cast<char*>(signature), signature_len).toHex());

        if (!query.exec()) {
            qDebug() << "数据库插入失败：" << query.lastError().text();
            return false;
        }

        qDebug() << "日志上传成功！";
        return true;
    }
};
=======
#include <QSqlError>

bool LogUpload::uploadLog(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件：" << filePath;
        return false;
    }

    QByteArray fileData = file.readAll();
    qDebug() << "文件内容：" << fileData;

    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) {
        qDebug() << "数据库连接失败！";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO logs (filename, content) VALUES (:filename, :content)");
    query.bindValue(":filename", file.fileName());
    query.bindValue(":content", fileData.toHex());

    if (!query.exec()) {
        qDebug() << "数据库插入失败：" << query.lastError().text();
        return false;
    }

    qDebug() << "日志上传成功！";
    return true;
}
>>>>>>> dc25f83 (完成)
