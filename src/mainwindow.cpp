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
#include <QVBoxLayout>
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
    ui->bookCoverHP_LA->setStyleSheet(style);
    ui->bookCover_LA_2->setStyleSheet(style);
    ui->bookTitle_LA->setText(title);
    ui->bookAuthor_LA->setText(author);
    ui->bookDescription_LA->setText(description);
    ui->bookAuthourHP_LA->setText(author);
    ui->bookRatingHP_LA->setText(QString::number(rating));
    ui->bookDescHP_LA->setText(description);
    ui->bookNameHP_LA->setText(title);
    ui->bookGenre_LA->setText(genre);
    ui->bookReleaseYear_LA->setText(QString::number(releaseYear));
    ui->bookReleaseDate_LA->setText(releaseDate);
    ui->bookLanguage_LA->setText(language);
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
        ui->stackedWidget_3->setCurrentIndex(1);
        QWidget* lessonWidget = new QWidget(ui->foundBooks); // Create a new widget to hold all the lessons
        QVBoxLayout *layout = new QVBoxLayout(lessonWidget); // Create a vertical layout for the widget
        layout->setAlignment(Qt::AlignTop);
        for (const QJsonValue &value : matchingBooks) {
            QJsonObject bookObject = value.toObject();
            QString author = bookObject["Author"].toString();
            QString title = bookObject["Title"].toString();
            QString fileName = bookObject["filename"].toString();
            QString genre = bookObject["Genre"].toString();
            QString releaseYear = bookObject["Year"].toString();
            QString releaseDate = bookObject["Date"].toString();
            QString language = bookObject["Language"].toString();
            double length = bookObject["Length"].toDouble();
            double rating = bookObject["Rating"].toDouble();

            // Print out the details for each match
            qDebug() << "Match found:";
            qDebug() << fileName;

                QWidget *lessonItemWidget = new QWidget; // Create a widget for each lesson
            lessonItemWidget->setFixedSize(890,220);
//                lessonItemWidget->setStyleSheet("border");

                QLabel *bookCover = new QLabel(lessonItemWidget);
                bookCover->setObjectName(title + "heading_LA"); // Set object name
                QString style = "image: url(:/Resources/Images/books/" + fileName + ".png); border: 0px;";
                bookCover->setStyleSheet(style);
                qDebug() << style;
                bookCover->setGeometry(20, 0, 140, 210);

                QLabel *heading = new QLabel(title, lessonItemWidget);
                heading->setObjectName(title + "_LA"); // Set object name
                heading->setStyleSheet("font: 30pt ""Apple Braille""; border: 0px;");
                heading->setGeometry(190, 20, 670, 30);

                QLabel *authorLA = new QLabel(author, lessonItemWidget);
                authorLA->setObjectName(author + "_LA"); // Set object name
                authorLA->setStyleSheet("font: 20pt ""Apple Braille""; border: 0px;");
                authorLA->setGeometry(200, 70, 330, 40);

                QLabel *ratingLA = new QLabel(lessonItemWidget);
                ratingLA->setText("Rating: " + QString::number(rating));
                ratingLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
                ratingLA->setGeometry(190, 110, 111, 21);

                QLabel *genreLA = new QLabel(lessonItemWidget);
                genreLA->setText("Genre: " + genre);
                genreLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
                genreLA->setGeometry(190, 170, 380, 21);

                QLabel *dateLA = new QLabel(lessonItemWidget);
                dateLA->setText(releaseDate + " " + releaseYear);
                dateLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
                dateLA->setGeometry(750, 10, 150, 16);

                QLabel *lengthLA = new QLabel(lessonItemWidget);
                lengthLA->setText(QString::number(length) + " " + "pages");
                lengthLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
                lengthLA->setGeometry(750, 35, 100, 16);

                QPushButton* readNow = new QPushButton("Read Now!", lessonItemWidget);
                readNow->setGeometry(640,150,111,41);
                readNow->setStyleSheet("background-color:#2254F5;font: 700 12pt ""Segoe UI"";color:white;border:0px;border-radius:10px;");

                QPushButton* readMore = new QPushButton("Read More!", lessonItemWidget);
                readMore->setGeometry(770,150,111,41);
                readMore->setStyleSheet("background-color:white;font: 700 12pt ""Segoe UI"";color:black;border: 0px;border-radius:10px;");

                layout->addWidget(lessonItemWidget); // Add the lesson widget to the layout


            ui->scrollArea->setWidget(lessonWidget); // Set the widget containing all lessons as the content of the scroll area
            // Optionally, you can update the UI here to display the matches.
            // For example, you could add the matches to a list widget.
        }
    } else {
        qDebug() << "No matching book or author found for:" << info;
        // Optionally clear the UI or display a message to the user
    }
}

void MainWindow::on_home_PB_clicked()
{
    ui->stackedWidget_3->setCurrentIndex(0);
}

