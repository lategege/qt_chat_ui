#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    explicit Widget(QWidget *parent = nullptr,QString name="default");
     void closeEvent(QCloseEvent *);
    ~Widget();

private:
    Ui::Widget *ui;
signals:
    //关闭窗口发送信号
    void closeWidget();

public:
   enum MsgType{
    Msg,Enter,Leave
   };
   void sendMsg(MsgType type);
   void userEnter(QString& name);
   void userLeave(QString name);
   QString getUser();
   QString getMsg();


private:
    QUdpSocket * udpSocket;
    qint16 port;
    QString name;
    qint16 peopleNum;
    void receiveMessage();
};

#endif // WIDGET_H
