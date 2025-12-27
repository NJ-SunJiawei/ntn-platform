const dgram = require('dgram')
const websocket = require('ws')
const dis = require("open-dis")
const DISUtils = require('./DISUtils');

var utils = new DISUtils();

// 创建一个UDP服务器,接收DIS数据源发送的数据
var server = dgram.createSocket({type: 'udp4', reuseAddr: true});
server.on('message', (msg, rinfo) => {
    // 解析二进制的DIS消息，转换为DIS对象
    try{
        var disMessage = utils.DISObjectFromBuffer(msg);
        // 根据pduType确定pdu类型
        switch(disMessage.pduType)
        {
            case 16: break;
            case 1: // EntityState PDU:
                broadcastMessage(JSON.stringify(disMessage));
                console.log("EntityState PDU (Type 1):", JSON.stringify(disMessage));
                break;
            case 23: // Electromagnetic Emission PDU:
                broadcastMessage(JSON.stringify(disMessage));
                console.log("Electromagnetic Emission PDU (Type 23):", JSON.stringify(disMessage));
                break;
            default:
                console.log(`PDU Type ${disMessage.pduType}:`, JSON.stringify(disMessage));
        }
    }
    catch(e)
    {
        // 这是为了截获不支持的PDU类型
        console.log(e);
    }
});

server.bind(7654)
/**
 * 创建一个WebSocket服务
 */
const wss = new websocket.Server({ host: '0.0.0.0',port: 30001 });
wss.on('connection', function connection(ws) {
    console.log('New client connected to WebSocket server');
    ws.on('message', function incoming(message) {
        console.log('Received message from client:', message.toString());
    });
    ws.on('close', function close() {
        console.log('Client disconnected from WebSocket server');
    });
    ws.on('error', function error(err) {
        console.log('WebSocket error:', err);
    });
});
/**
 * 将接收到的DIS数据广播消息到所有连接的WebSocket客户端
 * @param {string} message
 */
function broadcastMessage(message) {
  wss.clients.forEach(function each(client) {
      if (client.readyState === websocket.OPEN) {
          client.send(message);
          //console.log(message)
      }
  });
}

console.log('dis port: 7654');
console.log('websorck port: 30001');