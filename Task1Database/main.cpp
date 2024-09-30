#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QChar>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


QString generateRandomString(int length)
// Генерує рядок довжини length, що містить випадкові букви з обох регістрів і цифри
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    QString randomString;
    for (int i = 0; i < length; i++) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        randomString.append(possibleCharacters.at(index));
    }
    return randomString;
}

bool startsWithVowel(const QString& str)
{
    // Використовуємо QRegularExpression для пошуку голосної на початку рядка
    QRegularExpression regex("^[AEIOUYaeiouy]");
    QRegularExpressionMatch match = regex.match(str);
    return match.hasMatch();  // Повертає true, якщо знайдено відповідність
}

QString replaceOddDigits(const QString& str) {
    QString result = str;
    for (int i = 0; i < result.length(); i++) {
        if (result[i].isDigit() && (result[i].digitValue() % 2 != 0)) {
            result[i] = '#';
        }
    }
    return result;
}


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QFile file("output.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int i = 0; i < 1024; ++i) {
            QStringList row;
            for (int j = 0; j < 6; ++j) {
                row << generateRandomString(8);
            }
            out << row.join(",") << "\n";
        }
        file.close();
    }

    QFile inputFile("output.csv");
    QFile outputFile("processed_output.csv");

    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text) && outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream in(&inputFile);
        QTextStream out(&outputFile);

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList columns = line.split(',');

            bool removeLine = true;
            for (const QString& col : columns) {
                if (!startsWithVowel(col)) {
                    removeLine = false;
                    break;
                }
            }

            if (!removeLine) {
                for (QString& col : columns) {
                    col = replaceOddDigits(col);
                }
                out << columns.join(",") << "\n";
            }
        }

        inputFile.close();
        outputFile.close();
    }

    //QFile file("output.csv");

    // Створення SQLite бази даних
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");

    if (!db.open()) {
        qDebug() << "Error: Could not open database.";
        return -1;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS records (id INTEGER PRIMARY KEY, col1 TEXT, col2 TEXT, col3 TEXT, col4 TEXT, col5 TEXT, col6 TEXT)");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int id = 1;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList columns = line.split(',');

            query.prepare("INSERT INTO records (id, col1, col2, col3, col4, col5, col6) VALUES (?, ?, ?, ?, ?, ?, ?)");
            query.addBindValue(id++);
            for (const QString& col : columns) {
                query.addBindValue(col);
            }
            query.exec();
        }
        file.close();
    }

    // Видалення записів, де у другому стовпці перший символ цифра
    query.exec("DELETE FROM records WHERE col2 GLOB '[0-9]*'");

    db.close();


    return a.exec();
}
