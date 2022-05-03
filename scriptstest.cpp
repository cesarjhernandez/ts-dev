#include "scriptstest.h"
#include "ui_scriptstest.h"
#include <QApplication>
#include <QFile>
#include <QString>
#include <QFileDialog>
#include <QProcess>
#include <QDebug>
#include <QCloseEvent>
#include <QtMqtt/QMqttClient>
#include <QTimer>
#include <QMessageBox>
#include <QByteArray>
#include <QSpinBox>
#include <QFileInfo>

#include <iostream>
#include <cstdio>
#include <memory>
#include <string>
#include <array>
#include <fstream>
#include <chrono>
#include <thread>
#include <sstream>

#include <QtMqtt>
#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

using namespace std;

bool gpioOne = true;
QString shellGpioAuto = "/home/root/gpio 0 1";
QString shellGpioOffAuto = "/home/root/gpio 0 0";
QString shellGpioOffHeat = "/home/root/gpio 1 0";
QString shellGpioHeat = "/home/root/gpio 1 1";
QString shellGpioOffCool = "/home/root/gpio 2 0";
QString shellGpioCool = "/home/root/gpio 2 1";
QString shellGpioOffLight = "/home/root/gpio 3 0";
QString shellGpioOnLight = "/home/root/gpio 3 1";
QString shellTemp = "/home/root/am2322.py";
QString alexaStart = "/home/root/alexa-start.sh";
QProcess process;
QProcess process2;
QProcess process3;
QProcess process4;
QProcess processAlexa;
QProcess processTemp;
int setClientPort = 1884;
QString hostName = "localhost";
QString localShadow = "/home/root/local.json";


ScriptsTest::ScriptsTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptsTest)
{
    ui->setupUi(this);

    QPixmap pix("/home/root/heating.svg");
    ui->labelHeating->setPixmap(pix);
    QPixmap pix2("/home/root/temperature.svg");
    ui->labelTemperature->setPixmap(pix2);

    QPixmap textPix("/home/root/text.svg");
    ui->labelCoolPin->setPixmap(textPix);
    ui->labelFanPin->setPixmap(textPix);
    ui->labelHeatPin->setPixmap(textPix);

    QPixmap fanOnPix("/home/root/flow.svg");
    ui->labelFanOn->setPixmap(fanOnPix);

    QPixmap switchPix("/home/root/switch.svg");
    ui->labelSwitch->setPixmap(switchPix);

    QPixmap switchAuto("/home/root/temperature.svg");
    ui->labelSwitchAuto->setPixmap(switchAuto);
    ui->labelSwitchAuto->setVisible(false);

    QPixmap switchAuto_2("/home/root/temperature.svg");
    ui->labelSwitchAuto_2->setPixmap(switchAuto_2);
    ui->labelSwitchAuto_2->setVisible(false);

    QPixmap switchOff("/home/root/switch2.svg");
    ui->labelSwitchOff->setPixmap(switchOff);
    ui->labelSwitchOff->setVisible(true);

    QPixmap lightOff("/home/root/lightoff.png");
    ui->labelLightOff->setPixmap(lightOff);
    ui->labelLightOff->setVisible(true);

    QPixmap lightOn("/home/root/lighton.png");
    ui->labelLightOn->setPixmap(lightOn);
    ui->labelLightOn->setVisible(false);

    ui->pushButtonLight->setIcon(QIcon("/home/root/off_icon.png"));
    ui->pushButtonLight->setCheckable(true);

    ui->tempSlider->setVisible(0);
    ui->tempSlider_2->setVisible(0);
    ui->labelTempSlider->setVisible(false);

    ui->tempSpinBox->setVisible(false);

    ui->labelTextTemp_2->setVisible(false);

    ui->tempSpinBox->setRange(60, 80);
    //connect(ui->tempSpinBox, SIGNAL(valueChanged(double)), ui->tempSpinBox, SLOT(setValue(double))));

    ui->tempSpinBox_2->setVisible(false);
    ui->tempSpinBox_3->setVisible(false);

    ui->tempSpinBox_2->setRange(60, 80);
    ui->tempSpinBox_3->setRange(60, 80);

    ui->tempSlider->setRange(60, 80);
    ui->tempSlider_2->setRange(60, 80);


    ui->tempSpinBox_2->setRange(60, 80);

    QObject::connect(ui->tempSlider, SIGNAL(valueChanged(int)), ui->tempSpinBox_2, SLOT(setValue(int)));
    QObject::connect(ui->tempSpinBox_2, SIGNAL(valueChanged(int)), ui->tempSlider, SLOT(setValue(int)));

    QObject::connect(ui->tempSlider_2, SIGNAL(valueChanged(int)), ui->tempSpinBox_3, SLOT(setValue(int)));
    QObject::connect(ui->tempSpinBox_3, SIGNAL(valueChanged(int)), ui->tempSlider_2, SLOT(setValue(int)));

    //--- Mac Address Show
    QProcess macShow;
    macShow.start("python /home/root/mac1_read.py");
    macShow.waitForFinished(-1); // will wait forever until finished

    QString stdout = macShow.readAllStandardOutput();
    QString stderr = macShow.readAllStandardError();

    ui->labelMac->setText(QString("Mac: ") + stdout);
    QFont font = ui->labelMac->font();
    font.setPointSize(6);
    font.setBold(true);
    ui->labelMac->setFont(font);
    qDebug() << ui->labelMac->text();


    //--- Device Name Show Label
    QProcess devShow;
    devShow.start("/home/root/d_read.py");
    devShow.waitForFinished(-1);

    QString devOut = devShow.readAllStandardOutput();
    QString devErr = devShow.readAllStandardError();

    devOut = devOut.trimmed();
    ui->deviceShowLabel->setText(devOut);
    QFont snFont = ui->deviceShowLabel->font();
    snFont.setPointSize(6);
    snFont.setBold(true);
    ui->deviceShowLabel->setFont(snFont);


    // Temperature Sensor
    ScriptsTest::tempSensor();



    //get IP address
    QProcess ipShow;
    ipShow.start("/home/root/getIP.sh");
    ipShow.waitForFinished(-1);

    QString ipOut = ipShow.readAllStandardOutput();
    QString ipErr = ipShow.readAllStandardError();
    qDebug() << ipOut;


    //local shadow
    if (QFileInfo("/home/root/local.json").exists())
    {

    }
    else
    {
        QJsonObject status, reported, mainObject;

        mainObject.insert("temperature", int(60));
        mainObject.insert("light", QString("OFF"));
        mainObject["mode"] = "OFF";
        reported["reported"] = mainObject;
        status["status"] = reported;

        //qDebug() << reported["reported"].toObject()["status"].toObject()["mode"];

        QJsonDocument jsonDoc;
        jsonDoc.setObject(status);

        QFile file("/home/root/local.json");
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Can't open, error!";
            return;
        }

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        //write file
        stream << jsonDoc.toJson();
        file.close();
    }


    // MQTT Connection
    m_client = new QMqttClient(this);
    m_client->setHostname(hostName);
    m_client->setPort(setClientPort);
    m_client->connectToHost();
    connect(m_client, &QMqttClient::connected, this,&ScriptsTest::isConnected);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message,const QMqttTopicName &topic) {
        const QString content = //QDateTime::currentDateTime().toString()
                //+ QLatin1String(" Received Topic: ")
                topic.name()
                //+ QLatin1String(" Message: ")
                +message;
        //+ QLatin1Char('\n');

        qDebug() << message << " " << topic;



        // Alexa-AVS mode : OFF
        if (message == "OFF" && topic.name() == "hub/sta/mode")
        {
            //update shadow
            QByteArray modeOff = message;
            QMqttTopicName topicOff = topic;
            ScriptsTest::update_shadow(modeOff, topicOff);


            connect(ui->pushButtonOff, SIGNAL(clicked(bool)), this,SLOT(on_pushButtonOff_clicked()));
            ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");


            //Button Styles, visibility
            ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

            ui->labelTextCool_2->setText("OFF");
            ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
            ui->labelTextFan_2->setText("OFF");
            ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: normal;}");
            ui->labelTextHeat_2->setText("OFF");
            ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

            ui->tempSlider->setVisible(0);
            ui->tempSlider_2->setVisible(0);

            ui->labelTempSlider->setVisible(0);

            ui->labelSwitchAuto->setVisible(false);
            ui->labelSwitchAuto_2->setVisible(false);

            ui->labelTempSlider->setVisible(false);

            ui->tempSpinBox->setVisible(false);

            ui->labelTextTemp_2->setVisible(false);

            ui->tempSpinBox_2->setVisible(false);
            ui->tempSpinBox_3->setVisible(false);

            QPixmap pix("/home/root/heating.svg");
            ui->labelHeating->setPixmap(pix);


            // GPIO process, start/stop
            process2.start(shellGpioOffCool);
            process3.start(shellGpioOffHeat);
            process4.start(shellGpioOffAuto);
            process2.execute("gpio 2 0");
            process3.execute("gpio 1 0");
            process4.execute("gpio 0 0");
            process2.waitForFinished(-1);
            process3.waitForFinished(-1);
            process4.waitForFinished(-1);
        }
        // Alexa-AVS mode : AUTO
        else if (message == "AUTO" && topic.name() == "hub/sta/mode")
        {
            //update shadow
            QByteArray modeAuto = message;
            QMqttTopicName topicAuto = topic;
            ScriptsTest::update_shadow(modeAuto, topicAuto);


            ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
            ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
            process4.start(shellGpioAuto);


            //button style, visibility
            ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

            ui->labelTextFan_2->setText("ON");
            ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: bold;}");
            ui->labelTextCool_2->setText("OFF");
            ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
            ui->labelTextHeat_2->setText("OFF");
            ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

            ui->labelTempSlider->setText("AUTO");
            ui->labelTempSlider->setVisible(100);

            ui->tempSlider->setVisible(true);
            ui->tempSlider_2->setVisible(true);

            ui->labelSwitchAuto->setVisible(true);
            ui->labelSwitchAuto_2->setVisible(true);

            ui->tempSpinBox->setVisible(false);

            ui->labelTextTemp_2->setVisible(false);

            ui->tempSpinBox_2->setVisible(true);
            ui->tempSpinBox_3->setVisible(true);



            QPixmap switchAuto("/home/root/temperature.svg");
            ui->labelSwitchAuto->setPixmap(switchAuto);

            QPixmap pix("/home/root/heating2.svg");
            ui->labelHeating->setPixmap(pix);

            process2.start(shellGpioOffCool);
            process3.start(shellGpioOffHeat);
            process2.execute("gpio 2 0");
            process3.execute("gpio 1 0");
            process2.waitForFinished(-1);
            process3.waitForFinished(-1);
        }
        // Alexa-AVS mode : COOL
        else if ( message == "COOL" && topic.name() == "hub/sta/mode")
        {
            //update shadow
            QByteArray modeCool = message;
            QMqttTopicName topicCool = topic;
            ScriptsTest::update_shadow(modeCool, topicCool);


            ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(100, 56, 233); border: none; color: white;}");
            process2.start(shellGpioCool);


            ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

            ui->labelTextCool_2->setText("ON");
            ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: bold;}");
            ui->labelTextFan_2->setText("OFF");
            ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: normal;}");
            ui->labelTextHeat_2->setText("OFF");
            ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

            ui->tempSlider->setVisible(0);
            ui->tempSlider_2->setVisible(0);

            ui->labelTempSlider->setText("COOL");
            ui->labelTempSlider->setVisible(100);

            ui->labelSwitchAuto->setVisible(true);
            ui->labelSwitchAuto_2->setVisible(false);

            QPixmap switchAuto("/home/root/snow.svg");
            ui->labelSwitchAuto->setPixmap(switchAuto);

            QPixmap pix("/home/root/heating2.svg");
            ui->labelHeating->setPixmap(pix);

            ui->labelTextTemp_2->setVisible(true);

            ui->tempSpinBox->setVisible(true);

            ui->tempSpinBox_2->setVisible(false);
            ui->tempSpinBox_3->setVisible(false);

            process3.start(shellGpioOffHeat);
            process4.start(shellGpioOffAuto);
            process3.execute("gpio 1 0");
            process4.execute("gpio 0 0");
            process3.waitForFinished(-1);
            process4.waitForFinished(-1);
        }
        // Alexa-AVS mode : Heat
        else if ((message == "HEAT") || ((message == "{\"mode\": \"HEAT\"}") && (topic.name() == "hub/sta/mode")))
        {
            //update shadow
            QByteArray modeHeat = message;
            QMqttTopicName topicHeat = topic;
            ScriptsTest::update_shadow(modeHeat, topicHeat);


            ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
            process3.start(shellGpioHeat);

            ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
            ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");



            ui->labelTextHeat_2->setText("ON");
            ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: bold;}");
            ui->labelTextCool_2->setText("OFF");
            ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
            ui->labelTextFan_2->setText("OFF");
            ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: normal;}");

            ui->tempSlider->setVisible(0);
            ui->tempSlider_2->setVisible(0);

            ui->labelSwitchAuto->setVisible(true);
            ui->labelSwitchAuto_2->setVisible(false);

            QPixmap switchAuto("/home/root/fire.svg");
            ui->labelSwitchAuto->setPixmap(switchAuto);

            ui->labelTempSlider->setText("HEAT");
            ui->labelTempSlider->setVisible(100);

            QPixmap pix("/home/root/heating2.svg");
            ui->labelHeating->setPixmap(pix);

            ui->labelTextTemp_2->setVisible(true);

            ui->tempSpinBox->setVisible(true);

            ui->tempSpinBox_2->setVisible(false);
            ui->tempSpinBox_3->setVisible(false);

            process2.start(shellGpioOffCool);
            process4.start(shellGpioOffAuto);
            process2.execute("gpio 2 0");
            process4.execute("gpio 0 0");
            process2.waitForFinished(-1);
            process4.waitForFinished(-1);
        }


        if (message >= "60" && message <= "80" && topic.name() == "hub/sta/tempset")
        {

            QByteArray buffer = message;

            QByteArray sizeq = buffer;
            int size = sizeq.toInt();

            //update shadow
            QMqttTopicName topicHeat = topic;
            ScriptsTest::update_shadow(buffer, topicHeat);

            ui->tempSpinBox->setRange(60, 80);
            ui->tempSpinBox->setValue(size);

            ui->tempSpinBox_2->setRange(60, 80);
            ui->tempSpinBox_2->setValue(size - 1);

            ui->tempSpinBox_3->setRange(60, 80);
            ui->tempSpinBox_3->setValue(size + 1);

            ui->tempSlider->setRange(60, 80);
            ui->tempSlider->setValue(size - 1);

            ui->tempSlider_2->setRange(60, 80);
            ui->tempSlider_2->setValue(size + 1);

        }


        if (message == "ON" && topic.name() == "hub/sta/toggle")
        {
            QByteArray newMessage = message;
            QMqttTopicName topicToggleOn = topic;
            ScriptsTest::update_shadow(newMessage, topicToggleOn);
            ui->pushButtonLight->setIcon(QIcon("/home/root/on_icon.png"));
            ui->labelLightOn->setVisible(true);
            ui->labelLightOff->setVisible(false);

            process.execute(shellGpioOnLight);

        }
        else if (message == "OFF" && topic.name() == "hub/sta/toggle")
        {

            QByteArray newMessage = message;
            QMqttTopicName topicToggleOff = topic;
            ScriptsTest::update_shadow(newMessage, topicToggleOff);
            ui->pushButtonLight->setIcon(QIcon("/home/root/off_icon.png"));
            ui->labelLightOff->setVisible(true);
            ui->labelLightOn->setVisible(false);

            process.execute(shellGpioOffLight);


        }
    });
}

ScriptsTest::~ScriptsTest()
{
    delete ui;
}


void ScriptsTest::on_exitSlot_clicked()
{

    process.start(shellGpioOffLight);
    process2.start(shellGpioOffCool);
    process3.start(shellGpioOffHeat);
    process4.start(shellGpioOffAuto);
    process.execute("gpio 3 0");
    process2.execute("gpio 2 0");
    process3.execute("gpio 1 0");
    process4.execute("gpio 0 0");
    process.waitForFinished(-1);
    process2.waitForFinished(-1);
    process3.waitForFinished(-1);
    process4.waitForFinished(-1);

    m_client->disconnectFromHost();

    ScriptsTest::close();
    QApplication::quit();
}

void ScriptsTest::on_pushButtonOff_clicked()
{

    connect(ui->pushButtonOff, SIGNAL(clicked(bool)), this,SLOT(on_pushButtonOff_clicked()));
    ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");


    ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

    ui->labelTextCool_2->setText("OFF");
    ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
    ui->labelTextFan_2->setText("OFF");
    ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: normal;}");
    ui->labelTextHeat_2->setText("OFF");
    ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

    ui->tempSlider->setVisible(0);
    ui->tempSlider_2->setVisible(0);

    ui->labelTempSlider->setVisible(0);

    ui->labelSwitchAuto->setVisible(false);
    ui->labelSwitchAuto_2->setVisible(false);

    ui->labelTempSlider->setVisible(false);

    ui->tempSpinBox->setVisible(false);

    ui->labelTextTemp_2->setVisible(false);

    ui->tempSpinBox_2->setVisible(false);
    ui->tempSpinBox_3->setVisible(false);

    QPixmap pix("/home/root/heating.svg");
    ui->labelHeating->setPixmap(pix);


    process2.start(shellGpioOffCool);
    process3.start(shellGpioOffHeat);
    process4.start(shellGpioOffAuto);
    process2.execute("gpio 2 0");
    process3.execute("gpio 1 0");
    process4.execute("gpio 0 0");
    process2.waitForFinished(-1);
    process3.waitForFinished(-1);
    process4.waitForFinished(-1);


}

void ScriptsTest::on_pushButtonAuto_clicked()
{

    ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
    ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
    process4.start(shellGpioAuto);


    ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

    ui->labelTextFan_2->setText("ON");
    ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: bold;}");
    ui->labelTextCool_2->setText("OFF");
    ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
    ui->labelTextHeat_2->setText("OFF");
    ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

    ui->labelTempSlider->setText("AUTO");
    ui->labelTempSlider->setVisible(100);

    ui->labelSwitchAuto->setVisible(true);
    ui->labelSwitchAuto_2->setVisible(true);

    ui->tempSpinBox->setVisible(false);

    ui->labelTextTemp_2->setVisible(false);

    ui->tempSpinBox_2->setVisible(true);
    ui->tempSpinBox_3->setVisible(true);

    QPixmap switchAuto("/home/root/temperature.svg");
    ui->labelSwitchAuto->setPixmap(switchAuto);

    QPixmap pix("/home/root/heating2.svg");
    ui->labelHeating->setPixmap(pix);

    process2.start(shellGpioOffCool);
    process3.start(shellGpioOffHeat);
    process2.execute("gpio 2 0");
    process3.execute("gpio 1 0");
    process2.waitForFinished(-1);
    process3.waitForFinished(-1);


}


void ScriptsTest::on_pushButtonHeat_clicked()
{

    ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
    process3.start(shellGpioHeat);

    ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");



    ui->labelTextHeat_2->setText("ON");
    ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: bold;}");
    ui->labelTextCool_2->setText("OFF");
    ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
    ui->labelTextFan_2->setText("OFF");
    ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: normal;}");

    ui->tempSlider->setVisible(0);
    ui->tempSlider_2->setVisible(0);

    ui->labelSwitchAuto->setVisible(true);
    ui->labelSwitchAuto_2->setVisible(false);

    QPixmap switchAuto("/home/root/fire.svg");
    ui->labelSwitchAuto->setPixmap(switchAuto);

    ui->labelTempSlider->setText("HEAT");
    ui->labelTempSlider->setVisible(100);

    QPixmap pix("/home/root/heating2.svg");
    ui->labelHeating->setPixmap(pix);

    ui->labelTextTemp_2->setVisible(true);

    ui->tempSpinBox->setVisible(true);

    ui->tempSpinBox_2->setVisible(false);
    ui->tempSpinBox_3->setVisible(false);

    process2.start(shellGpioOffCool);
    process4.start(shellGpioOffAuto);
    process2.execute("gpio 2 0");
    process4.execute("gpio 0 0");
    process2.waitForFinished(-1);
    process4.waitForFinished(-1);

}


void ScriptsTest::on_pushButtonCool_clicked()
{

    ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(100, 56, 233); border: none; color: white;}");
    process2.start(shellGpioCool);

    ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

    ui->labelTextCool_2->setText("ON");
    ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: bold;}");
    ui->labelTextFan_2->setText("OFF");
    ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: normal;}");
    ui->labelTextHeat_2->setText("OFF");
    ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

    ui->tempSlider->setVisible(0);
    ui->tempSlider_2->setVisible(0);

    ui->labelTempSlider->setText("COOL");
    ui->labelTempSlider->setVisible(100);

    ui->labelSwitchAuto->setVisible(true);
    ui->labelSwitchAuto_2->setVisible(false);

    QPixmap switchAuto("/home/root/snow.svg");
    ui->labelSwitchAuto->setPixmap(switchAuto);

    QPixmap pix("/home/root/heating2.svg");
    ui->labelHeating->setPixmap(pix);

    ui->labelTextTemp_2->setVisible(true);

    ui->tempSpinBox->setVisible(true);

    ui->tempSpinBox_2->setVisible(false);
    ui->tempSpinBox_3->setVisible(false);

    process3.start(shellGpioOffHeat);
    process4.start(shellGpioOffAuto);
    process3.execute("gpio 1 0");
    process4.execute("gpio 0 0");
    process3.waitForFinished(-1);
    process4.waitForFinished(-1);

}


void ScriptsTest::on_pushButtonFanAuto_clicked()
{
    ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
    ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
    process4.start(shellGpioAuto);

    ui->labelTextFan_2->setText("ON");
    ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: bold;}");
    ui->labelTextCool_2->setText("OFF");
    ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
    ui->labelTextHeat_2->setText("OFF");
    ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

    ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

    ui->tempSlider->setVisible(100);
    ui->tempSlider_2->setVisible(100);

    ui->labelTempSlider->setText("AUTO");
    ui->labelTempSlider->setVisible(100);

    ui->labelSwitchAuto->setVisible(true);
    ui->labelSwitchAuto_2->setVisible(true);

    ui->tempSpinBox->setVisible(false);

    QPixmap switchAuto("/home/root/temperature.svg");
    ui->labelSwitchAuto->setPixmap(switchAuto);

    QPixmap pix("/home/root/heating2.svg");
    ui->labelHeating->setPixmap(pix);

    ui->labelTextTemp_2->setVisible(false);

    ui->tempSpinBox_2->setVisible(true);
    ui->tempSpinBox_3->setVisible(true);

    process.start(shellGpioOffLight);
    process2.start(shellGpioOffCool);
    process3.start(shellGpioOffHeat);
    process.execute("gpio 3 0");
    process2.execute("gpio 2 0");
    process3.execute("gpio 1 0");
    process.waitForFinished(-1);
    process2.waitForFinished(-1);
    process3.waitForFinished(-1);

}


void ScriptsTest::on_pushButtonFanOn_clicked()
{
    ui->pushButtonFanOn->setStyleSheet("QPushButton {background-color: rgb(100,56,233); border: none; color: white;}");
    process4.start(shellGpioAuto);

    ui->labelTextFan_2->setText("ON");
    ui->labelTextFan_2->setStyleSheet("QLabel {font-weight: bold;}");
    ui->labelTextCool_2->setText("OFF");
    ui->labelTextCool_2->setStyleSheet("QLabel {font-weight: normal;}");
    ui->labelTextHeat_2->setText("OFF");
    ui->labelTextHeat_2->setStyleSheet("QLabel {font-weight: normal;}");

    ui->pushButtonOff->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonCool->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonHeat->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonFanAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");
    ui->pushButtonAuto->setStyleSheet("QPushButton {background-color: rgb(224, 224, 224); border: none;}");

    ui->tempSlider->setVisible(100);
    ui->tempSlider_2->setVisible(100);

    ui->labelTempSlider->setText("AUTO");
    ui->labelTempSlider->setVisible(100);

    ui->labelSwitchAuto->setVisible(true);
    ui->labelSwitchAuto_2->setVisible(true);

    ui->tempSpinBox->setVisible(false);

    QPixmap switchAuto("/home/root/temperature.svg");
    ui->labelSwitchAuto->setPixmap(switchAuto);

    QPixmap pix("/home/root/heating2.svg");
    ui->labelHeating->setPixmap(pix);

    ui->labelTextTemp_2->setVisible(false);

    ui->tempSpinBox_2->setVisible(true);
    ui->tempSpinBox_3->setVisible(true);

    process2.start(shellGpioOffCool);
    process3.start(shellGpioOffHeat);
    process2.execute("gpio 2 0");
    process3.execute("gpio 1 0");
    process2.waitForFinished(-1);
    process3.waitForFinished(-1);

}

void ScriptsTest::on_pushButtonLight_toggled(bool checked)
{
    if(checked)
    {
        ui->pushButtonLight->setIcon(QIcon("/home/root/on_icon.png"));
        ui->labelLightOn->setVisible(true);
        ui->labelLightOff->setVisible(false);

        process.start(shellGpioOnLight);
    }
    else
    {
        ui->pushButtonLight->setIcon(QIcon("/home/root/off_icon.png"));
        ui->labelLightOff->setVisible(true);
        ui->labelLightOn->setVisible(false);

        process.start(shellGpioOffLight);
    }
}

void ScriptsTest::tempSensor()
{
    QFont snFont = ui->tempLabel->font();
    snFont.setPointSize(7);
    snFont.setBold(true);
    ui->tempLabel->setFont(snFont);

    QTimer* timer = new QTimer();
    timer->setInterval(1000); //Time in milliseconds
    //timer->setSingleShot(true); //Setting this to true makes the timer run only once
    connect(timer, &QTimer::timeout, this, [=](){
        //Do your stuff in here, gets called every interval time
        QProcess temp;
        temp.start("/home/root/gettemp.sh");
        temp.waitForFinished(-1);

        QString tempOut = temp.readAllStandardOutput();
        QString tempErr = temp.readAllStandardError();

        //QString newTemp = tempOut + QString::fromUtf8("°C");

        ui->tempLabel->setText(QString(tempOut));
        ui->tempLabel->setFont(snFont);
        ui->tempLabel->setAlignment(Qt::AlignCenter);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        //qDebug() << tempOut;
        //temp.close();
        temp.close();
    });
    timer->start(); //Call start() AFTER connect
    //Do not call start(0) since this will change interval
}


void ScriptsTest::on_pushButton_clicked()
{

    //get IP address
    QProcess ipShow;
    ipShow.start("/home/root/getIP.sh");
    ipShow.waitForFinished(-1);

    QString ipOut = ipShow.readAllStandardOutput();
    QString ipErr = ipShow.readAllStandardError();

    if (m_client->state() == QMqttClient::Disconnected) {
        ui->pushButton->setText(tr("Disconnect"));
        m_client->setHostname(hostName);
        m_client->setPort(setClientPort);

        m_client->connectToHost();
        qDebug()<<"client is connected";
        ui->pushButton->setStyleSheet("QPushButton {background-color: rgb(0, 204, 0); border: none; padding: 0 8px; border-radius: 10px; color: white}");

        QPixmap switchOn("/home/root/switch.svg");
        ui->labelSwitch->setPixmap(switchOn);
        ui->labelSwitch->setVisible(true);

        QPixmap switchOff("/home/root/switch2.svg");
        ui->labelSwitchOff->setPixmap(switchOff);
        ui->labelSwitchOff->setVisible(false);
    }
    else {
        ui->pushButton->setText(tr("Connect"));
        m_client->setHostname("");
        m_client->setPort(false);
        m_client->disconnectFromHost();
        qDebug() << "client is disconnected";
        ui->pushButton->setStyleSheet("QPushButton {background-color: #FF2E2E; border: none; none; padding: 0 8px; border-radius: 10px;}");

        QPixmap switchOff("/home/root/switch2.svg");
        ui->labelSwitchOff->setPixmap(switchOff);
        ui->labelSwitchOff->setVisible(true);

        QPixmap switchOn("/home/root/switch.svg");
        ui->labelSwitch->setPixmap(switchOn);
        ui->labelSwitch->setVisible(false);
    }

    ipShow.close();

}

void ScriptsTest::isConnected()
{
        QMqttTopicFilter topic;
        QMqttTopicFilter topic2;
        QMqttTopicFilter topic3;
        QMqttTopicFilter topic4;
        topic.setFilter("hub/sta/mode");
        topic2.setFilter("hub/sta/tempset");
        topic3.setFilter("hub/sta/toggle");

        auto subscription = m_client->subscribe(topic);
        qDebug() << "subscribed to topic: " << topic;
        auto subscription2 = m_client->subscribe(topic2);
        qDebug() << "subscribed to topic: " << topic2;
        auto subscription3 = m_client->subscribe(topic3);
        qDebug() << "subscribed to topic: " << topic3;

        if(!subscription || !subscription2 || !subscription3){
           qDebug()<<"Could not subscribe. Is there a valid connection?";
            return;
        }
}



void ScriptsTest::on_tempSpinBox_2_valueChanged(int arg1)
{
    ui->tempSpinBox_3->setValue(arg1 + 2);
    ui->tempSlider_2->setValue(arg1 + 2);

    ui->tempSpinBox->setValue(arg1 + 1);
}

void ScriptsTest::on_tempSpinBox_3_valueChanged(int arg1)
{
    ui->tempSpinBox_2->setValue(arg1 - 2);
    ui->tempSlider->setValue(arg1 - 2);

    ui->tempSpinBox->setValue(arg1 - 1);
}

void ScriptsTest::on_tempSlider_valueChanged(int value)
{
    ui->tempSpinBox_3->setValue(value + 2);
    ui->tempSlider_2->setValue(value + 2);

    ui->tempSpinBox->setValue(value + 1);
}


void ScriptsTest::on_tempSlider_2_valueChanged(int value)
{
    ui->tempSpinBox_2->setValue(value - 2);
    ui->tempSlider->setValue(value - 2);

    ui->tempSpinBox->setValue(value - 1);
}


void ScriptsTest::on_tempSpinBox_valueChanged(int arg1)
{
    ui->tempSpinBox_2->setValue(arg1 - 1);
    ui->tempSlider->setValue(arg1 - 1);

    ui->tempSpinBox_3->setValue(arg1 + 1);
    ui->tempSlider_2->setValue(arg1 + 1);
}



void ScriptsTest::update_shadow(QByteArray &newMessage, QMqttTopicName &newTopic)
{

    QByteArray message = newMessage;
    QMqttTopicName tsDevTopic = newTopic;

    // Update Local Shadow Document
    QFile file(localShadow);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QJsonParseError JsonParseError;
    QJsonDocument JsonDocument = QJsonDocument::fromJson(file.readAll(), &JsonParseError);
    file.close();

    QString topic = tsDevTopic.name();
    QString mqttMessage = message;
    QJsonObject rootObject = JsonDocument.object();
    QJsonObject status_obj = rootObject.value("status").toObject();
    QJsonObject reported_obj = status_obj.value("reported").toObject();
    QJsonObject light_object = reported_obj.value("light").toObject();
    QJsonObject mode_object = reported_obj.value("mode").toObject();
    QJsonObject temp_object = reported_obj.value("temperature").toObject();
    QJsonValue mode_value = reported_obj.value("mode");
    QJsonValue light_value = reported_obj.value("light");
    QJsonValue temp_value = reported_obj.value("temperature");

    if (topic == "hub/sta/mode")
    {
        mode_value = mqttMessage;
    }
    else if (topic == "hub/sta/toggle")
    {
        light_value = mqttMessage;
    }
    else if (topic == "hub/sta/tempset" && message >= "60" && message <= "80")
    {

        QByteArray buffer = message;

        QByteArray sizeq = buffer;
        int size = sizeq.toInt();
        temp_value = size;
    }

    QJsonObject newRoot, reported;

    QJsonValueRef ref = status_obj.find("reported").value();
    QJsonObject m_addValue = ref.toObject();
    m_addValue.insert("light", light_value);
    m_addValue.insert("mode", mode_value);
    m_addValue.insert("temperature", temp_value);
    ref=m_addValue;

    qDebug() << newRoot;

    qDebug() << light_value;

    reported["reported"] = ref;
    newRoot["status"] = reported;

    JsonDocument.setObject(newRoot);
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(JsonDocument.toJson());

    file.close();
}

