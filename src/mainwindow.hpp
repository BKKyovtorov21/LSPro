#pragma once

#include <QWidget>
#include <QFile>
#include <QDebug>
namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QString& username, QWidget *parent = nullptr);
    ~MainWindow();
    void getBookInfo(QString bookName);
    QString getAuthor() const { return author; }
    QString getTitle() const { return title; }
    QString getDescription() const { return description; }
    float getRating() const { return rating; }

private slots:
    void on_book1_PB_clicked();

    void on_readNow_PB_clicked();

    void on_readMore_PB_clicked();

    void on_Back_PB_clicked();
    void on_search_LE_returnPressed();

private:
    Ui::MainWindow *ui;


    QString author;
    QString title;
    QString description;
    QString fileName;
    QString genre;
    float releaseYear;
    QString releaseDate;
    QString language;
    float length;
    float rating;

    QString m_username;

    void accountInit();
    void searchBook(QString info);
};
