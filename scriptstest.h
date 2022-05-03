#ifndef SCRIPTSTEST_H
#define SCRIPTSTEST_H

#include <QWidget>
#include <QLCDNumber>
#include <QMqttClient>
#include <QSlider>

namespace Ui {
class ScriptsTest;
}

class ScriptsTest : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptsTest(QWidget *parent = 0);
    ~ScriptsTest();


private slots:

    void on_exitSlot_clicked();

    void on_pushButtonOff_clicked();

    void on_pushButtonAuto_clicked();

    void on_pushButtonCool_clicked();

    void on_pushButtonHeat_clicked();

    void on_pushButtonFanAuto_clicked();

    void on_pushButtonFanOn_clicked();

    void tempSensor();

    void on_pushButton_clicked();

    void isConnected();

    void on_pushButtonLight_toggled(bool checked);

    void on_tempSpinBox_2_valueChanged(int arg1);

    void on_tempSpinBox_3_valueChanged(int arg1);

    void on_tempSlider_valueChanged(int value);

    void on_tempSlider_2_valueChanged(int value);

    void on_tempSpinBox_valueChanged(int arg1);

    void update_shadow(QByteArray &newMessage, QMqttTopicName &newTopic);


private:
    Ui::ScriptsTest *ui;
    QMqttClient *m_client;

};

class QDoublePushButton : public QWidget

{
    Q_OBJECT

public:
    explicit QDoublePushButton(QWidget *parent = 0);
    //~QDoublePushButton();

signals:

    void setValue(int);

    void valueChanged(int);

};



#endif // SCRIPTSTEST_H
