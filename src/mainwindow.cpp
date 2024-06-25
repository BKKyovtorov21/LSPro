#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QSql>
#include <QSqlQuery>
MainWindow::MainWindow(QString& username, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_username = username;
    accountInit();

    ui->stackedWidget_2->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getBookInfo(QString bookName)
{
    QByteArray val;
    QFile fileWindows(QDir::toNativeSeparators("C:/Users/boyan/Desktop/LSPro/books.json"));
    QFile fileMac(QDir::toNativeSeparators("/Users/boyankiovtorov/Desktop/LSPro/books.json"));

    if (!fileMac.exists()) {
        qDebug() << "JSON file does not exist at path:" << fileMac.fileName();
        return;
    }
    if (!fileMac.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file at path:" << fileMac.fileName();
        return;
    } else {
        val = fileMac.readAll();
        fileMac.close();
    }

    // Parse JSON data
    QJsonDocument document = QJsonDocument::fromJson(val);
    QJsonObject object = document.object();
    const auto settingsObject = object[bookName].toObject();

    // Extract database connection parameters from JSON
    author = settingsObject["Author"].toString();
    title = settingsObject["Title"].toString();
    description = settingsObject["Description"].toString();
    fileName = settingsObject["filename"].toString();
    genre = settingsObject["Genre"].toString();
    releaseYear = settingsObject["Year"].toDouble();
    releaseDate = settingsObject["Date"].toString();
    language = settingsObject["Language"].toString();
    length = settingsObject["Length"].toDouble();
    rating = settingsObject["Rating"].toDouble();

    QString style = "image: url(:/Resources/Images/books/" + fileName + ".png);";
    ui->bookCoverInfo_LA->setStyleSheet(style);
    ui->bookCover_LA->setStyleSheet(style);
    ui->bookCover_LA_2->setStyleSheet(style);
    ui->bookTitle_LA->setText(title);
    ui->bookAuthor_LA->setText(author);
    ui->bookDescription_LA->setText(description);
    ui->bookAuthour_LA->setText(author);
    ui->bookRating_LA->setText(QString::number(rating));
    ui->bookDesc_LA->setText(description);
    ui->bookName_LA->setText(title);
    ui->bookGenre->setText(genre);
    ui->bookReleaseYear_LA->setText(QString::number(releaseYear));
    ui->bookReleaseDate_LA->setText(releaseDate);
    ui->bookLanguage->setText(language);
    ui->bookPages_LA->setText(QString::number(length));

}

void MainWindow::on_book1_PB_clicked()
{
    getBookInfo("The Psychology of Money");

}

void MainWindow::on_readNow_PB_clicked()
{
    QString filePath = "/Users/boyankiovtorov/Desktop/LSPro/books/" + fileName +".epub";
    QUrl url = QUrl::fromLocalFile(filePath);
    QDesktopServices::openUrl(url);
}


void MainWindow::accountInit()
{
    qDebug() << m_username;
    QSqlQuery qry;
    qry.prepare("SELECT * FROM users WHERE username = :username");
    qry.bindValue(":username", m_username);
    if(qry.exec())
    {
        if(qry.next())
        {
            QString username = qry.value("Username").toString();
            QString firstName = qry.value("First Name").toString();
            QString role = qry.value("Role").toString();
            ui->name_LA->setText(firstName);

            if(role == "Librarian")
            {
                ui->stackedWidget->setCurrentIndex(1);
            }
            else
            {
                ui->stackedWidget->setCurrentIndex(0);

            }
        }
    }
}

void MainWindow::on_readMore_PB_clicked()
{
    ui->stackedWidget_2->move(389, 180);
    ui->stackedWidget_2->show();
}


void MainWindow::on_Back_PB_clicked()
{
    ui->stackedWidget_2->hide();
}


void MainWindow::on_search_LE_returnPressed()
{
    QString info = ui->search_LE->text();
    searchBook(info);
}

void MainWindow::searchBook(QString info)
{
    QByteArray val;
    QFile fileWindows(QDir::toNativeSeparators("C:/Users/boyan/Desktop/LSPro/books.json"));
    QFile fileMac(QDir::toNativeSeparators("/Users/boyankiovtorov/Desktop/LSPro/books.json"));

    QFile *file;
#ifdef Q_OS_WIN
    file = &fileWindows;
#else
    file = &fileMac;
#endif

    if (!file->exists()) {
        qDebug() << "JSON file does not exist at path:" << file->fileName();
        return;
    }
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file at path:" << file->fileName();
        return;
    } else {
        val = file->readAll();
        file->close();
    }

    // Parse JSON data
    QJsonDocument document = QJsonDocument::fromJson(val);
    QJsonObject object = document.object();

    bool matchesFound = false;
    QJsonArray matchingBooks;

    // Iterate over all keys (book names) in the JSON object
    for (const QString& key : object.keys()) {
        const QJsonObject bookObject = object[key].toObject();
        QString author = bookObject["Author"].toString();
        QString title = bookObject["Title"].toString();
        QString genre = bookObject["Genre"].toString();

        if (title.contains(info, Qt::CaseInsensitive) || author.contains(info, Qt::CaseInsensitive) || genre.contains(info, Qt::CaseSensitive)) {
            matchingBooks.append(bookObject);
            matchesFound = true;
        }
    }

    if (matchesFound) {
        for (const QJsonValue &value : matchingBooks) {
            QJsonObject bookObject = value.toObject();
            QString author = bookObject["Author"].toString();
            QString title = bookObject["Title"].toString();
            QString description = bookObject["Description"].toString();
            QString fileName = bookObject["filename"].toString();
            QString genre = bookObject["Genre"].toString();
            double releaseYear = bookObject["Year"].toDouble();
            QString releaseDate = bookObject["Date"].toString();
            QString language = bookObject["Language"].toString();
            double length = bookObject["Length"].toDouble();
            double rating = bookObject["Rating"].toDouble();

            // Print out the details for each match
            qDebug() << "Match found:";
            qDebug() << "Author:" << author;
            qDebug() << "Title:" << title;
            qDebug() << "Description:" << description;
            qDebug() << "Filename:" << fileName;
            qDebug() << "Genre:" << genre;
            qDebug() << "Release Year:" << releaseYear;
            qDebug() << "Release Date:" << releaseDate;
            qDebug() << "Language:" << language;
            qDebug() << "Length:" << length;
            qDebug() << "Rating:" << rating;

            // Optionally, you can update the UI here to display the matches.
            // For example, you could add the matches to a list widget.
        }
    } else {
        qDebug() << "No matching book or author found for:" << info;
        // Optionally clear the UI or display a message to the user
    }
}
