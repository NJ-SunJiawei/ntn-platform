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

var viewer = new Cesium.Viewer("cesiumContainer")

var entitylist = [];

function addEntity(location, heading){
  // 添加模型
  console.log(location)
  const entity = viewer.entities.add({
    name: 'My Model',
    position: location, // 经纬度高程
    orientation: heading, // 默认无旋转
    model: {
        uri: './Cesium_Air.glb',      // GLTF/GLB 模型路径
        color: Cesium.Color.RED,
        scale: 400,
    }
  });

  viewer.zoomTo(entity)
  return entity;
}

// websocket客户端
var ws = new WebSocket('ws://localhost:30001');
ws.onopen = function(e){
  console.log("连接服务器成功");
}
ws.onclose = function(e){
  console.log("服务器关闭");
}
ws.onerror = function(){
  console.log("连接出错");
}

// 接收服务器的消息
ws.onmessage = function(e){
  // console.log(e.data)
  const disMessage = JSON.parse(e.data);
  // 判断消息类型
  if (disMessage.pduType === 1) {
    // 处理实体状态消息，在地图上进行渲染
    // 获取实体ID
    const entityID = disMessage.entityID.application + ":" + disMessage.entityID.site + ":" + disMessage.entityID.entity;
    const dislocation = disMessage.entityLocation;
    const disorientation = disMessage.entityOrientation
    // wgs84经纬高
    const [lon, lat, h] = ecefToLonLat(dislocation.x, dislocation.y, dislocation.z);
    const wgs84Bearing = ecefYawToWgs84Bearing(disorientation.psi, lon, lat);

    const position = Cesium.Cartesian3.fromDegrees(lon, lat, h);
    const heading = disorientation.psi; // 北偏东45度
    const pitch = 0 // disorientation.theta;                
    const roll = 0// disorientation.phi;    // 无翻滚
    // 将欧拉角转换为四元数
    const orientation = Cesium.Quaternion.fromHeadingPitchRoll(
       new Cesium.HeadingPitchRoll(heading, pitch, roll)
    );
    
    // 判断是否已经绘制
    let entity = entitylist[entityID];
    if (entity){
      entity.position = position
      entity.orientation = orientation
    }
    else{
      let entity = addEntity(position, orientation)
      entitylist[entityID]=entity
    }
  };
}