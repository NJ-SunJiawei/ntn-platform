import './style.css';
import {Map, View, Feature} from 'ol';
import TileLayer from 'ol/layer/Tile';
import VectorLayer from 'ol/layer/Vector';
import XYZSource from 'ol/source/XYZ';
import VectorSource from 'ol/source/Vector';
import {Point } from 'ol/geom';
import {fromLonLat, transform } from 'ol/proj';
import {Style, Icon} from 'ol/style';
import proj4 from 'proj4';

// Tiles信息
var minZoom=1;
var maxZoom=12;
var centX=-101;
var centY=39;
var epsgCode='3857'; //  OpenLayers 3默认使用的是mercator(EPSG:3857)，而不是wgs84(EPSG:4326)，输入的经纬度可直接映射到WGS84，否则需要转换
var tileSize=256;

var epsg = 'EPSG:' + epsgCode;

var view = new View({
  center: transform([centX, centY], 'EPSG:4326', epsg),
  projection: 'EPSG:3857',
  zoom: 7,
  minZoom: minZoom,
  maxZoom: maxZoom
});

// arcgis 卫星
var arcgissource = new XYZSource({
  tileSize: tileSize,
  url:"https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}"
});

var tileLayer = new TileLayer({
  source: arcgissource
});

const map = new Map({
  target: 'map',
  layers: [tileLayer],
  view: view,
});

var features = [];
var testlayer = new VectorLayer({
	source: new VectorSource()
});
map.addLayer(testlayer);

function addImage(json){
  var obj = JSON.parse(json);
    // 添加图标
    var p = new Point([obj.longitude, obj.latitude]);
    var f = new Feature({
      geometry: p
    });
    var s = new Style({
      image: new Icon({
        src: 'images/Font.png',     // 只支持白底
        //src: 'http://192.168.56.1:5080/1.png'
        crossOrigin: 'anonymous',
        size: [50, 50],             // 图片大小
        color: 'red',             // 图片颜色，只能对白底图片设置，否则显示未黑色
        anchor: [0.5, 0.5],         // 图片中心位置
        offset: [1 * 50, 0 * 50],  // 截取整图的部分，[0,0]表示左上角
        rotation: obj.heading,             // 弧度制,正北为0，顺时针，π/2 -> 90°
        opacity: 0.8,               // 透明度
        scale: 0.5,                 // 缩放
      })
    });
    // 设置样式，其中可设置图标
    f.setStyle(s);
    // 添加到图标Layer中
    testlayer.getSource().addFeature(f);
    return f;
}

// 将ECEF 坐标转为WGS84经纬高
function ecefToLonLat(X, Y, Z) {
  const a = 6378137.0; // WGS84 长半轴
  const e2 = 6.69437999014e-3; // WGS84 第一偏心率平方

  // 1. 计算经度 lon
  const lon = Math.atan2(Y, X);

  // 2. 计算纬度 lat（迭代法）
  let lat = Math.atan2(Z, Math.sqrt(X * X + Y * Y)); // 初始估计
  let N, h, prevLat;
  do {
      prevLat = lat;
      N = a / Math.sqrt(1 - e2 * Math.sin(lat) * Math.sin(lat));
      lat = Math.atan2(Z + e2 * N * Math.sin(lat), Math.sqrt(X * X + Y * Y));
  } while (Math.abs(lat - prevLat) > 1e-9); // 收敛条件

  // 3. 计算高度 h
  h = Math.sqrt(X * X + Y * Y) / Math.cos(lat) - N;

  return [lon * (180 / Math.PI), lat * (180 / Math.PI), h]; // 转为度
}

// 将WGS84经纬，转换为墨卡托投影
function wgs84ToMercator(lon, lat) {
  const R = 6378137.0; // 地球半径（米）

  // 1. 经度直接缩放
  const x = R * lon * (Math.PI / 180);

  // 2. 纬度转换（注意边界条件）
  const y = R * Math.log(Math.tan(Math.PI / 4 + (lat * Math.PI / 180) / 2));

  return [x, y];
}

// 将 ECEF 航向转换为 WGS84 方位角​​
function ecefYawToWgs84Bearing(yaw, lon, lat) {
  // 1. 将 ECEF 航向向量转换为 ENU（东-北-天）坐标系
  const sinYaw = Math.sin(yaw);
  const cosYaw = Math.cos(yaw);

  // 2. 计算 ENU 坐标系下的方向向量
  const sinLon = Math.sin(lon * Math.PI / 180);
  const cosLon = Math.cos(lon * Math.PI / 180);
  const sinLat = Math.sin(lat * Math.PI / 180);
  const cosLat = Math.cos(lat * Math.PI / 180);

  // 3. ECEF → ENU 的旋转矩阵
  const east  = -sinLon * cosYaw + cosLon * sinYaw;
  const north = -cosLon * sinLat * cosYaw - sinLon * sinLat * sinYaw + cosLat * 0; // Z 分量不影响水平航向

  // 4. 计算 WGS84 方位角（正北为0°）
  const bearing = Math.atan2(east, north); // 弧度
  return (bearing * (180 / Math.PI) + 360) % 360; // 转为 0°~360°
}

// 将 WGS84 航向转换为墨卡托航向​​
function wgs84BearingToMercator(bearing, lon, lat, distance = 1) {
  const R = 6371000; // 地球半径（米）
  const α = bearing * (Math.PI / 180); // 弧度

  // 1. 沿 WGS84 航向计算目标点
  const lat2 = lat + (distance / R) * Math.cos(α) * (180 / Math.PI);
  const lon2 = lon + (distance / R) * Math.sin(α) / Math.cos(lat * Math.PI / 180) * (180 / Math.PI);

  // 2. 将两点转为墨卡托坐标
  const point1 = fromLonLat([lon, lat]);
  const point2 = fromLonLat([lon2, lat2]);

  // 3. 计算墨卡托平面上的角度
  const dx = point2[0] - point1[0];
  const dy = point2[1] - point1[1];
  const mercatorBearing = Math.atan2(dx, dy) * (180 / Math.PI);
  return (mercatorBearing + 360) % 360;
}

// websocket客户端
console.log("Attempting to connect to WebSocket server...");
var ws = new WebSocket('ws://localhost:30001'); //ws这个是由浏览器发起的
ws.onopen = function(e){
  console.log("连接服务器成功");
}
ws.onclose = function(e){
  console.log("服务器关闭", e);
}
ws.onerror = function(error){
  console.log("连接出错", error);
}
// 接收服务器的消息
ws.onmessage = function(e){
  console.log("收到服务器消息:", e.data);
  const disMessage = JSON.parse(e.data);
  // 判断消息类型
  if (disMessage.pduType === 1) {
    // 处理实体状态消息，在地图上进行渲染
    // 获取实体ID
    const entityID = disMessage.entityID.application + ":" + disMessage.entityID.site + ":" + disMessage.entityID.entity;
    // wgs84经纬高
    const [lon, lat, h] = ecefToLonLat(disMessage.entityLocation.x, disMessage.entityLocation.y, disMessage.entityLocation.z);
    // 墨卡托投影
    const mercatorll = fromLonLat([lon, lat]);

    const wgs84Bearing = ecefYawToWgs84Bearing(disMessage.entityOrientation.psi, lon, lat);
    const mercatorBearing = wgs84BearingToMercator(wgs84Bearing, lon, lat);
    const heading = mercatorBearing * (Math.PI / 180); // 弧度
    // console.log(disMessage.entityOrientation.psi)

    var obj = {}
    obj['longitude'] = mercatorll.longitude;
    obj['latitude'] = mercatorll.latitude;
    obj['heading'] =  heading;

    // 判断是否已经绘制
    let imagF = features[entityID];
    if (imagF){
      imagF.setGeometry(new Point(mercatorll))
      var s = new Style({
        image: new Icon({
          src: 'images/Font.png',     // 只支持白底
          //src: 'http://192.168.56.1:5080/1.png'
          crossOrigin: 'anonymous',
          size: [50, 50],             // 图片大小
          color: 'red',             // 图片颜色，只能对白底图片设置，否则显示未黑色
          anchor: [0.5, 0.5],         // 图片中心位置
          offset: [1 * 50, 0 * 50],  // 截取整图的部分，[0,0]表示左上角
          rotation: obj.heading,             // 弧度制,正北为0，顺时针，π/2 -> 90°
          opacity: 0.8,               // 透明度
          scale: 0.5,                 // 缩放
        })
      });
      imagF.setStyle(s)
    }
    else{
      
      let imagF = addImage(JSON.stringify(obj))
      features[entityID]=imagF
    }
  }
}