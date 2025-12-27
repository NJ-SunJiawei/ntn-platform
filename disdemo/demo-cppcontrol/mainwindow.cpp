#include "mainwindow.h"
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QTableWidgetItem>
#include <QtMath>

#include "dis6/utils/IncomingMessage.h"
#include "dis6/EntityStatePdu.h"
#include "dis6/EntityID.h"
#include "dis6/ActionRequestPdu.h"
#include "dis6/FixedDatum.h"

#include "CustomPdu.h"

struct WGS84LLH
{
    double longitude;
    double latitude;
    double height;
};

// 将ECEF 坐标转为WGS84经纬高（EPSG:4326）
WGS84LLH ecefToLonLat(double X, double Y, double Z) {
    double a = 6378137.0; // WGS84 长半轴
    double e2 = 6.69437999014e-3; // WGS84 第一偏心率平方

    // 1. 计算经度 lon
    double lon = qAtan2(Y, X);

    // 2. 计算纬度 lat（迭代法）
    double lat = qAtan2(Z, qSqrt(X * X + Y * Y)); // 初始估计
    double N, h, prevLat;
    do {
        prevLat = lat;
        N = a / qSqrt(1 - e2 * qSin(lat) * qSin(lat));
        lat = qAtan2(Z + e2 * N * qSin(lat), qSqrt(X * X + Y * Y));
    } while (qAbs(lat - prevLat) > 1e-9); // 收敛条件

    // 3. 计算高度 h
    h = qSqrt(X * X + Y * Y) / qCos(lat) - N;

    WGS84LLH ret;
    ret.longitude = lon * (180 / M_PI); // 转为度
    ret.latitude = lat * (180 / M_PI);
    ret.height = h;

    return ret;
}

struct MercatorPos
{
    double x;
    double y;
};

// WGS84 → 墨卡托（EPSG:3857）​​
MercatorPos wgs84ToMercator(double lon, double lat) 
{
    double R = 6378137.0; // 地球半径（米）

    // 1. 经度直接缩放
    double x = R * lon * (M_PI / 180);

    // 2. 纬度转换（注意边界条件）
    double y = R * qLn(qTan(M_PI / 4 + (lat * M_PI / 180) / 2));

    MercatorPos ret = { x, y };
    return ret;
}

// 将 ECEF 航向（Yaw）转换为 WGS84 方位角
double ecefYawToWgs84Bearing(double psi, double lon, double lat) 
{
    // 1. 将 ECEF 航向向量转换为 ENU（东-北-天）坐标系
    double sinYaw = qSin(psi);
    double cosYaw = qCos(psi);

    // 2. 计算 ENU 坐标系下的方向向量
    double sinLon = qSin(lon * M_PI / 180);
    double cosLon = qCos(lon * M_PI / 180);
    double sinLat = qSin(lat * M_PI / 180);
    double cosLat = qCos(lat * M_PI / 180);

    // 3. ECEF → ENU 的旋转矩阵
    double east = -sinLon * cosYaw + cosLon * sinYaw;
    double north = -cosLon * sinLat * cosYaw - sinLon * sinLat * sinYaw + cosLat * 0; // Z 分量不影响水平航向

    // 4. 计算 WGS84 方位角（正北为0°）
    double bearing = qAtan2(east, north); // 弧度
    double bearingAngle = bearing * (180 / M_PI) + 360; // 角度
    return std::fmod(bearingAngle, 360); // 转为 0°~360°
}

// 将 WGS84 航向转换为墨卡托航向​​
double wgs84BearingToMercator(double bearing, double lon, double lat, double distance) {
    double R = 6371000; // 地球半径（米）
    double a = bearing * (M_PI / 180); // 弧度

    // 1. 沿 WGS84 航向计算目标点
    double lat2 = lat + (distance / R) * qCos(a) * (180 / M_PI);
    double lon2 = lon + (distance / R) * qSin(a) / qCos(lat * M_PI / 180) * (180 / M_PI);

    // 2. 将两点转为墨卡托坐标
    MercatorPos point1 = wgs84ToMercator(lon, lat);
    MercatorPos point2 = wgs84ToMercator(lon2, lat2);

    // 3. 计算墨卡托平面上的角度
    double dx = point2.x - point1.x;
    double dy = point2.y - point1.y;
    double mercatorBearing = qAtan2(dx, dy) * (180 / M_PI);
    return std::fmod((mercatorBearing + 360), 360);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClass())
{
    ui->setupUi(this);

    auto isBind = m_udpSocket.bind(QHostAddress::Any, 7654, QAbstractSocket::ReuseAddressHint);
    if (isBind)
    {
        connect(&m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonClear_clicked()
{
    ui->tableWidgetEntityState->setRowCount(0);
}

void MainWindow::on_pushButtonLeft_clicked()
{
    // 获取界面上选择的行
    int row = getSelectedRow();
    if (row == -1) return;

    // 获取实体标识
    auto entityIdItem = ui->tableWidgetEntityState->item(row, 0);
    auto entityIdStr = entityIdItem->text();
    auto strList = entityIdStr.split("-", QString::SkipEmptyParts);

    DIS::ActionRequestPdu controlAction;
    controlAction.setProtocolVersion(5);
    controlAction.setExerciseID(1);

    // 设置自定义协议数据
    controlAction.setRequestID(1); // 与接收端协商的requestId, 1 表示旋转
    controlAction.setActionID(0);  // 与接收端协商的actionId， 0 表示自定义数据

    // 下面的代码是将实体标识通过自定义协议发送到AFSim用于定位平台，与接收端协商的数据
    std::vector<DIS::FixedDatum> fixedDatums;
    DIS::FixedDatum applicationData;
    applicationData.setFixedDatumID(1);
    applicationData.setFixedDatumValue(strList[0].toInt());	// application
    fixedDatums.push_back(applicationData);
    DIS::FixedDatum siteData;
    siteData.setFixedDatumID(2);
    siteData.setFixedDatumValue(strList[1].toInt());	// site
    fixedDatums.push_back(siteData);
    DIS::FixedDatum entityData;
    entityData.setFixedDatumID(3);
    entityData.setFixedDatumValue(strList[2].toInt());  // entity
    fixedDatums.push_back(entityData);

    controlAction.setFixedDatums(fixedDatums);

    // 打包到数据流
    DIS::DataStream buffer(DIS::BIG);
    controlAction.marshal(buffer);

    // 发送到DIS总线
    int size = m_udpSocket.writeDatagram(&buffer[0], buffer.size(), QHostAddress::Broadcast, 7654);
}


void MainWindow::on_pushButtonPause_clicked()
{
    CustomPdu cutomPdu;
    cutomPdu.setProtocolVersion(5);
    cutomPdu.setExerciseID(1);

    cutomPdu.setType(0); // 暂停

    DIS::DataStream buffer(DIS::BIG);
    cutomPdu.marshal(buffer);

    // 发送
    int size = m_udpSocket.writeDatagram(&buffer[0], buffer.size(), QHostAddress::Broadcast, 7654);
}

void MainWindow::on_pushButtonResume_clicked()
{
    CustomPdu cutomPdu;
    cutomPdu.setProtocolVersion(5);
    cutomPdu.setExerciseID(1);

    cutomPdu.setType(1); // 暂停

    DIS::DataStream buffer(DIS::BIG);
    cutomPdu.marshal(buffer);

    // 发送
    int size = m_udpSocket.writeDatagram(&buffer[0], buffer.size(), QHostAddress::Broadcast, 7654);
}

void MainWindow::readPendingDatagrams()
{
    QByteArray data;
    while (m_udpSocket.hasPendingDatagrams()) {
        data.append(m_udpSocket.receiveDatagram().data());
    }

    DIS::IncomingMessage incoming;
    incoming.AddProcessor(DIS::PDUType::PDU_ENTITY_STATE, this);
    incoming.AddProcessor(DIS::PDUType::PDU_ACTION_REQUEST, this);
    incoming.Process(data.data(), data.size(), DIS::BIG);
}


void MainWindow::Process(const DIS::Pdu& packet)
{
    switch (packet.getPduType())
    {
    case DIS::PDUType::PDU_ENTITY_STATE:
        processEntityState(packet);
        break;
    case DIS::PDUType::PDU_ACTION_REQUEST:
        processActionRequest(packet);
        break;
    default:
        break;
    }
}

int MainWindow::getSelectedRow()
{
    auto selectedItems = ui->tableWidgetEntityState->selectedItems();
    if (selectedItems.isEmpty()) {
        return -1;
    }
    else {
        return selectedItems[0]->row();
    }
}

void MainWindow::processEntityState(const DIS::Pdu& packet)
{
    const DIS::EntityStatePdu& espdu = static_cast<const DIS::EntityStatePdu&>(packet);

    auto entityId = espdu.getEntityID();
    auto entityIdStr = QString("%1-%2-%3")
        .arg(entityId.getApplication())
        .arg(entityId.getSite())
        .arg(entityId.getEntity());
    int row = 0;
    bool isExist = false;
    for (; row < ui->tableWidgetEntityState->rowCount(); ++row)
    {
        auto idItem = ui->tableWidgetEntityState->item(row, 0);
        auto idStr = idItem->text();
        if (idStr == entityIdStr)
        {
            isExist = true;
            break;
        }
    }

    if (!isExist) {
        ui->tableWidgetEntityState->setRowCount(row + 1);
    }

    ui->tableWidgetEntityState->setItem(row, 0, new QTableWidgetItem(entityIdStr));

    const DIS::Vector3Double& position = espdu.getEntityLocation();
    auto wgs84llh = ecefToLonLat(position.getX(), position.getY(), position.getZ());
    auto mercatorPos = wgs84ToMercator(wgs84llh.longitude, wgs84llh.latitude);

    const DIS::Orientation& orientation = espdu.getEntityOrientation();
    auto wgs84bearing = ecefYawToWgs84Bearing(orientation.getPsi(), wgs84llh.longitude, wgs84llh.latitude);
    auto mercatorBearing = wgs84BearingToMercator(wgs84bearing, wgs84llh.longitude, wgs84llh.latitude, 1);

    ui->tableWidgetEntityState->setItem(row, 1, new QTableWidgetItem(QString::number(wgs84llh.longitude)));
    ui->tableWidgetEntityState->setItem(row, 2, new QTableWidgetItem(QString::number(wgs84llh.latitude)));
    ui->tableWidgetEntityState->setItem(row, 3, new QTableWidgetItem(QString::number(wgs84llh.height)));
    ui->tableWidgetEntityState->setItem(row, 4, new QTableWidgetItem(QString::number(wgs84bearing)));

    auto protocolVersion = espdu.getProtocolVersion();
    auto exerciseID = espdu.getExerciseID();
    ui->tableWidgetEntityState->setItem(row, 5, new QTableWidgetItem(QString::number(protocolVersion)));
    ui->tableWidgetEntityState->setItem(row, 6, new QTableWidgetItem(QString::number(exerciseID)));
}

void MainWindow::processActionRequest(const DIS::Pdu& packet)
{
    const DIS::ActionRequestPdu& actionRequestPdu = static_cast<const DIS::ActionRequestPdu&>(packet);
    switch (actionRequestPdu.getActionID())
    {
    case 47: // AdvanceTime
    {
        double advanceTime = 0;
        for (auto fixedDatum : actionRequestPdu.getFixedDatums()) {
            advanceTime = fixedDatum.getFixedDatumValue() / 1000.0;
        }
        ui->labelSimTime->setText(QString::number(advanceTime));
    }
        break;
    default:
        break;
    }
}