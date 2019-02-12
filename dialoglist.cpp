#include "dialoglist.h"
#include "ui_dialoglist.h"
#include "QToolButton.h"
#include "widget.h"
#include "QVector.h"
DialogList::DialogList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);

    //设置标题
    setWindowTitle("MySelfQQ2019");
    setWindowIcon(QPixmap(":/images/qq.png"));
    for( int i=0;i<9;++i){
    //设置头像
    QToolButton * qtoolButton =new QToolButton;
    QString qtring;
    switch (i){
    case 0:
        qtring="111111";
        break;
    case 1:
         qtring="222222";
         break;
    case 2:
         qtring="333333";
         break;
    case 3:
         qtring="444444";
         break;
    case 4:
         qtring="555555";
         break;
    case 5:
         qtring="66666";
         break;
    case 6:
         qtring="77777";
         break;
    case 7:
         break;
    case 8:
         qtring="88888";
         break;
    }
    qtoolButton->setText(qtring);
    qtoolButton->setIcon(QPixmap(":/images/Cherry.png"));
    qtoolButton->setIconSize(QPixmap(":/images/Cherry.png").size());
    qtoolButton->setAutoRaise(true);
    qtoolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->vLayout->addWidget(qtoolButton);
    isShow.push_back(false);


    connect(qtoolButton,&QToolButton::clicked,[=](){

        //构造聊天窗口时告诉窗口名字
        if(!isShow[i]){
        Widget * widget =new Widget(0,qtoolButton->text());

        widget->setWindowTitle(qtoolButton->text());
        widget->setWindowIcon(qtoolButton->icon());

        widget->show();
        connect(widget,&Widget::closeWidget,[=](){
            isShow[i]=false;
        });
        }
        isShow[i]=true;



    });
    }
}

DialogList::~DialogList()
{
    delete ui;
}
