#pragma once

#include <QMainWindow>
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

#include <QUdpSocket>

#include "dis6/EntityStatePdu.h"           
#include "dis6/utils/IPacketProcessor.h" 

class MainWindow : public QMainWindow, public DIS::IPacketProcessor
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // DIS协议处理实现DIS::IPacketProcessor
    void Process(const DIS::Pdu& packet);

protected slots:
    void on_pushButtonClear_clicked();
    void on_pushButtonLeft_clicked();
    void on_pushButtonPause_clicked();
    void on_pushButtonResume_clicked();
    void readPendingDatagrams();

private:
    Ui::MainWindowClass *ui;

    QUdpSocket m_udpSocket;

    int getSelectedRow();

    void processEntityState(const DIS::Pdu& packet);
    void processActionRequest(const DIS::Pdu& packet);
};