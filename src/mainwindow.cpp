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
#include <QMessageBox>
#include <QSqlError>
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
    QJsonObject object = openJSONDoc();
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
    copies = settingsObject["Copies"].toInt();
    status = settingsObject["Status"].toString();

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
    if (copies == 0 && status == "Borrowed")
    {
        QMessageBox::information(this, "No more book copies", "All copies of this book are currently borrowed. \n \nPlease try again later");
    }
    else
    {
        QJsonObject object = openJSONDoc();
        QJsonObject settingsObject = object[title].toObject(); // Non-const QJsonObject for modification

        // Modify the JSON data
        if (copies == 1)
        {
            settingsObject["Status"] = "Borrowed";
        }
        settingsObject["Copies"] = copies - 1;
        object[title] = settingsObject;

        // Write the updated JSON data back to the file
        QByteArray val;

        QJsonDocument document = QJsonDocument::fromJson(val);
        document.setObject(object);
        val = document.toJson(QJsonDocument::Indented);

        QFile fileWindows(QDir::toNativeSeparators("C:/Users/boyan/Desktop/LSPro/books.json"));
        QFile fileMac(QDir::toNativeSeparators("/Users/boyankiovtorov/Desktop/LSPro/books.json"));

        QFile *file;
#ifdef Q_OS_WIN
        file = &fileWindows;
#else
        file = &fileMac;
#endif

        if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Failed to open JSON file for writing at path:" << file->fileName();
            return;
        }

        file->write(val);
        file->close();



        QDateTime startTime = QDateTime::currentDateTime();
        QDateTime endTime = startTime.addMonths(1);
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT * FROM bookBorrowing WHERE username = :username AND bookBorrowed = :bookTitle");
        checkQuery.bindValue(":username", m_username);
        checkQuery.bindValue(":bookTitle", title); // Assuming 'title' is the title of the book being borrowed

        if (checkQuery.exec() && checkQuery.next()) {
            #ifdef Q_OS_WIN
            QString filePath = "C:/Users/boyan/Desktop/LSPro/books/" + fileName + ".epub";
            #else
            QString filePath = "/Users/boyankiovtorov/Desktop/LSPro/books/" + fileName + ".epub";
            #endif

            QUrl url = QUrl::fromLocalFile(filePath);
            QDesktopServices::openUrl(url);
        }
        else{
            QSqlQuery qry;


            qry.prepare("INSERT INTO bookBorrowing(username, bookBorrowed, startTime, endTime) "
                        "VALUES (:username, :bookBorrowed, :startTime, :endTime)");
            qry.bindValue(":username", m_username);
            qry.bindValue(":bookBorrowed", title);
            qry.bindValue(":startTime", startTime.toString(Qt::ISODate));
            qry.bindValue(":endTime", endTime.toString(Qt::ISODate));

            qDebug() << startTime.toString(Qt::ISODate);
            qDebug() << endTime.toString(Qt::ISODate);

            if(!qry.exec()) {
                qDebug() << "Error inserting into bookBorrowing:";
            } else {
                qDebug() << "Insertion successful!";
            }
        }
    }
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
                ui->role_LA->setText("Librarian");
            }
            else
            {
                ui->role_LA->setText("Reader");

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
    QJsonObject object = openJSONDoc();
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
                connect(readNow, &QPushButton::clicked, this, [=]() {getBookInfo(title);on_readNow_PB_clicked();});

                QPushButton* readMore = new QPushButton("Read More!", lessonItemWidget);
                readMore->setGeometry(770,150,111,41);
                readMore->setStyleSheet("background-color:white;font: 700 12pt ""Segoe UI"";color:black;border: 0px;border-radius:10px;");
                connect(readMore, &QPushButton::clicked, this, [=]() {getBookInfo(title);on_readMore_PB_clicked();});

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



void MainWindow::on_search_LE_2_returnPressed()
{
    on_search_LE_returnPressed();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_borrowedBooks_PB_clicked()
{
     showBorrowedBooks();
}

void MainWindow::showBorrowedBooks()
{
    ui->stackedWidget_3->setCurrentIndex(2);
    QJsonObject object = openJSONDoc();
    bookTitles.clear();
    QSqlQuery qry;
    qry.prepare("SELECT * FROM bookBorrowing WHERE username = :username");
    qry.bindValue(":username", m_username);

    if (!qry.exec()) {
        qDebug() << "Error executing query:";
        return;
    }
    while(qry.next())
    {
        bookTitles.push_back(qry.value("bookBorrowed").toString());
    }

    QWidget* booksWidget = new QWidget(ui->borrowedBooks_WG); // Create a new widget to hold all the lessons
    QVBoxLayout *layout = new QVBoxLayout(booksWidget); // Create a vertical layout for the widget
    layout->setAlignment(Qt::AlignTop);

    for (const QString& bookTitle : bookTitles) {
        const QJsonObject bookObject = object[bookTitle].toObject();
        QString author = bookObject["Author"].toString();
        QString title = bookObject["Title"].toString();
        QString fileName = bookObject["filename"].toString();
        QString genre = bookObject["Genre"].toString();
        QString releaseYear = bookObject["Year"].toString();
        QString releaseDate = bookObject["Date"].toString();
        QString language = bookObject["Language"].toString();
        double length = bookObject["Length"].toDouble();
        double rating = bookObject["Rating"].toDouble();

        QWidget *borrowedBookWidget = new QWidget; // Create a widget for each lesson
        borrowedBookWidget->setFixedSize(890,220);

        QLabel *bookCover = new QLabel(borrowedBookWidget);
        bookCover->setObjectName(title + "heading_LA"); // Set object name
        QString style = "image: url(:/Resources/Images/books/" + fileName + ".png); border: 0px;";
        bookCover->setStyleSheet(style);
        qDebug() << style;
        bookCover->setGeometry(20, 0, 140, 210);

        QLabel *heading = new QLabel(title, borrowedBookWidget);
        heading->setObjectName(title + "_LA"); // Set object name
        heading->setStyleSheet("font: 30pt ""Apple Braille""; border: 0px;");
        heading->setGeometry(190, 20, 670, 30);

        QLabel *authorLA = new QLabel(author, borrowedBookWidget);
        authorLA->setObjectName(author + "_LA"); // Set object name
        authorLA->setStyleSheet("font: 20pt ""Apple Braille""; border: 0px;");
        authorLA->setGeometry(200, 70, 330, 40);

        QLabel *ratingLA = new QLabel(borrowedBookWidget);
        ratingLA->setText("Rating: " + QString::number(rating));
        ratingLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
        ratingLA->setGeometry(190, 110, 111, 21);

        QLabel *genreLA = new QLabel(borrowedBookWidget);
        genreLA->setText("Genre: " + genre);
        genreLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
        genreLA->setGeometry(190, 170, 380, 21);

        QLabel *dateLA = new QLabel(borrowedBookWidget);
        dateLA->setText(releaseDate + " " + releaseYear);
        dateLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
        dateLA->setGeometry(750, 10, 200, 16);

        QLabel *lengthLA = new QLabel(borrowedBookWidget);
        lengthLA->setText(QString::number(length) + " " + "pages");
        lengthLA->setStyleSheet("font: 15pt ""Apple Braille""; border: 0px;");
        lengthLA->setGeometry(750, 35, 100, 16);

        QPushButton* readNow = new QPushButton("Continue reading", borrowedBookWidget);
        readNow->setGeometry(640,150,111,41);
        readNow->setStyleSheet("background-color:#2254F5;font: 700 12pt ""Segoe UI"";color:white;border:0px;border-radius:10px;");
        connect(readNow, &QPushButton::clicked, this, [=]() {getBookInfo(title);on_readNow_PB_clicked();});

        QPushButton* readMore = new QPushButton("Return book", borrowedBookWidget);
        readMore->setGeometry(770,150,111,41);
        readMore->setStyleSheet("background-color:white;font: 700 12pt ""Segoe UI"";color:black;border: 0px;border-radius:10px;");
        connect(readMore, &QPushButton::clicked, this, [=]() {getBookInfo(title); returnBook(title);});

        layout->addWidget(borrowedBookWidget); // Add the lesson widget to the layout
    }

    ui->scrollArea_2->setWidget(booksWidget); // Set the container widget with all borrowed books as the scroll area widget
}

void MainWindow::returnBook(QString title)
{
    QJsonObject object = openJSONDoc();
    const auto settingsObject = object[title].toObject();

    // Extract database connection parameters from JSON
    QString author = settingsObject["Author"].toString();

    QSqlQuery qry;
    QDateTime returnTime = QDateTime::currentDateTime();
    qry.prepare("INSERT INTO returnedBooks(username, returnedBook, dateOfReturn) "
                "VALUES(:username, :returnedBook, :dateOfReturn)");
    qry.bindValue(":username", m_username);
    qry.bindValue(":returnedBook", title);
    qry.bindValue(":dateOfReturn", returnTime);

    if(qry.exec())
    {
        qry.prepare("DELETE FROM bookBorrowing WHERE username = :username AND bookBorrowed = :bookBorrowed");
        qry.bindValue(":username", m_username);
        qry.bindValue(":bookBorrowed", title);
        if(qry.exec())
        {
            QMessageBox::information(this, "Book returned", "You have returned " + title);
        }
        else
        {
            qDebug() << "error when returning: " << qry.lastError();

        }
    }
    else
    {
        qDebug() << "error when returning: " << qry.lastError();
    }
}


QJsonObject MainWindow::openJSONDoc()
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
    }
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file at path:" << file->fileName();
    } else {
        val = file->readAll();
        file->close();
    }

    // Parse JSON data
    QJsonDocument document = QJsonDocument::fromJson(val);
    QJsonObject object = document.object();

    return object;
}

void MainWindow::on_homepage_PB_clicked()
{
    ui->stackedWidget_3->setCurrentIndex(0);
}

