#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "login.h"
MainMenu::MainMenu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainMenu)
{
    ui->setupUi(this);

    logIn = std::make_shared<LogIn>();

    int id = QFontDatabase::addApplicationFont(":/Resources/Fonts/Outfit-Bold.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont OutfitFont(family);
    OutfitFont.setPointSize(50);

    ui->label->setFont(OutfitFont);
}

MainMenu::~MainMenu()
{
    delete ui;
}

void MainMenu::on_login_PB_clicked()
{
    this->hide();
    logIn->show();
}


void MainMenu::on_register_PB_clicked()
{

}


void MainMenu::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QGradient Gradient(QGradient::Preset::SmartIndigo);

    QRect RectLiner(this->rect());
    painter.fillRect(RectLiner, Gradient);
}
