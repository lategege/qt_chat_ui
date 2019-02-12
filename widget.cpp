#include "widget.h"
#include "ui_widget.h"
#include "QDataStream"
#include "QMessageBox"
#include "QDateTime"
#include "QColorDialog"
#include "QFileDialog"
#include "QTextStream"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}


Widget::Widget(QWidget *parent,QString name) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    udpSocket=new QUdpSocket(this);
    this->name=name;
    //端口号
    this->port=9999;
    //绑定端口
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);

    //发送新用户进入
    sendMsg(MsgType::Enter);

    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        sendMsg(MsgType::Msg);
    });

    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::receiveMessage);

    connect(ui->exitBtn,&QPushButton::clicked,this,[=](){
      this->close();
    });

    connect(ui->fontCbx,&QFontComboBox::currentFontChanged,[=](const QFont & font){
        ui->msgTxtEdit->setCurrentFont(font);
        ui->msgTxtEdit->setFocus();

    });
    void (QComboBox:: *cbx) (const QString &text) =&QComboBox::currentIndexChanged;
    connect(ui->sizeCbx,cbx,[=](const QString& text){
        ui->msgTxtEdit->setFontPointSize(text.toDouble());
        ui->msgTxtEdit->setFocus();
    });

    connect(ui->boldTBtn,&QToolButton::clicked,[=](bool isChecked){
        if(isChecked){
            ui->msgTxtEdit->setFontWeight(QFont::Bold);
        }else{
            ui->msgTxtEdit->setFontWeight(QFont::Normal);
        }
    });
    connect(ui->italicTBtn,&QToolButton::clicked,[=](bool isChecked){
        if(isChecked){
            ui->msgTxtEdit->setFontItalic(true);
        }else{
            ui->msgTxtEdit->setFontItalic(false);
        }
    });
    connect(ui->underlineTBtn,&QToolButton::clicked,[=](bool isChecked){
        if(isChecked){
            ui->msgTxtEdit->setFontUnderline(true);
        }else{
            ui->msgTxtEdit->setFontUnderline(false);
        }
    });

    connect(ui->colorTbtn,&QToolButton::clicked,[=](){
        QColor color =QColorDialog::getColor();
        ui->msgTxtEdit->setTextColor(color);
    });

    connect(ui->clearTBtn,&QToolButton::clicked,[=](){
        ui->msgBrowser->clear();
    });


    connect(ui->saveTBtn,&QToolButton::clicked,[=](){
        QString path=  QFileDialog::getSaveFileName(this,"保存记录","聊天记录","*.txt");
        if(path.isEmpty()||ui->msgBrowser->document()->isEmpty()){
            QMessageBox::warning(this,"警告","路径或者内容不能为空");
        }else{
            QFile file(path);
            file.open(QIODevice::WriteOnly|QIODevice::Text);
            QTextStream stream(&file);
            stream <<ui->msgBrowser->toPlainText();
            file.close();
        }
    });


}

Widget::~Widget()
{
    delete ui;
}
void Widget::closeEvent(QCloseEvent *)
{
    emit closeWidget();
    sendMsg(MsgType::Leave);
    udpSocket->close();
    udpSocket->destroyed();
}

QString Widget::getMsg(){
    QString str =ui->msgTxtEdit->toHtml();
    //清空输入框
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus();
    return str;
}

QString Widget::getUser(){
    return this->name;
}

void Widget::sendMsg(MsgType type)
{
    //发送的数据 做分段处理 第一段类型 第二段 具体的内容
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);
    stream << type;
    stream << getUser();
    switch (type){
    //普通消息
            case MsgType::Msg:
            if(ui->msgTxtEdit->toPlainText()==""){
             QMessageBox::warning(this,"警告","消息不能为空");
               return;
            }
            stream <<getMsg();
            break;
        //用户进入
        case MsgType::Enter:

            break;
        //用户离开
        case MsgType::Leave:
            break;
    }
    //书写报文
    udpSocket->writeDatagram(array,QHostAddress::Broadcast,port);

}
void Widget::receiveMessage(){
    //解析数据
    qint64 size =udpSocket->pendingDatagramSize();
    QByteArray array =QByteArray(size,0);
    udpSocket->readDatagram(array.data(),size);
    QDataStream stream(&array,QIODevice::ReadOnly);
    int msgType;  //读取类型
    //获取当前时间
   QString time= QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    stream >> msgType;
    QString name;
    QString msg;
    stream >> name;
    switch (msgType){
         case Msg:
              stream >>msg;
              ui->msgBrowser->setTextColor(Qt::blue);
              ui->msgBrowser->append("["+name+"]"+ time);
              ui->msgBrowser->append(msg);
              break;
             //用户离开
         case Leave:
              userLeave(name);
              break;
            //用户进入
          case Enter:
              userEnter(name);
              break;


    }

}
void Widget::userLeave(QString name){
   bool isEmpty= ui->usrTblWidget->findItems(name,Qt::MatchExactly).isEmpty();
   if(!isEmpty){
       int row=ui->usrTblWidget->findItems(name,Qt::MatchExactly).first()->row();
       ui->usrTblWidget->removeRow(row);
       ui->msgBrowser->setTextColor(Qt::gray);
       ui->msgBrowser->append(name+"离线");
       ui->usrNumLb->setText(QString("在线用户:%1人").arg(ui->usrTblWidget->rowCount()));
   }
}

void Widget::userEnter(QString& name){
    bool isEmpty= ui->usrTblWidget->findItems(name,Qt::MatchExactly).isEmpty();
    if(isEmpty){
       QTableWidgetItem * user =new QTableWidgetItem(name);
       ui->usrTblWidget->insertRow(0);
       ui->usrTblWidget->setItem(0,0,user);
       ui->msgBrowser->setTextColor(Qt::gray);
       ui->msgBrowser->append(name+"上线了");
       ui->usrNumLb->setText(QString("在线用户:%1人").arg(ui->usrTblWidget->rowCount()));
       sendMsg(MsgType::Enter);
    }
}
