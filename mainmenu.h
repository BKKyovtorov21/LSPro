#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QFontDatabase>
class LogIn;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainMenu;
}
QT_END_NAMESPACE

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

private slots:
    void on_login_PB_clicked();

    void on_register_PB_clicked();

private:
    Ui::MainMenu *ui;

    std::shared_ptr<LogIn> logIn;

    virtual void paintEvent(QPaintEvent*) override;
};
#endif // MAINMENU_H
