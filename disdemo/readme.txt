port:
    dis:7654
    websokcet:30001
    openlayer-web:5173
    cesium-web:8080
    cppcontorl:

1、wargame-txt 选定的仿真场景
   修改dis相关配置，因为在容器运行，只修改了广播ip

2、制作nodejs base镜像
    docker run --rm --name nodejs-base -it ubuntu:20.04 bash
        >apt update
        >apt install -y curl net-tools vim iputils-ping
        >curl -fsSL https://deb.nodesource.com/setup_20.x | bash -
        >apt install -y nodejs
    版本：node -v ===》v20.19.6
          npm -v ===》10.8.2
    docker commit nodejs-base nodejs-base:v1

    ####docker network create dis-net  只有在“用户自定义 Docker 网络”里，容器名才能互相 ping

2、demo-nodejs 后端框架(WebSocket 服务端)
   docker run --name demo-nodejs -itd -p 30001:30001 nodejs-base:v1 bash
   docker cp ./demo-nodejs demo-nodejs:/home/demo-nodejs
   docker exec -it demo-nodejs bash
        >cd /home/demo-nodejs // 创建项目文件夹 
        ###>npm init // 初始化项目 
        ###>npm install open-dis // 安装open-dis的javascript实现 
        ###>npm install ws // 安装websocket
        >node ./index.js  // 启动后端 

2、demo-openlayers-web 前端框架(WebSocket 客户端) 注意ws的ip替换为nodejs所在容器的ip
   docker run --name demo-openlayers-web -itd -p 5173:5173 nodejs-base:v1 bash
   docker cp ./demo-openlayers-web demo-openlayers-web:/home/demo-openlayers-web
   docker exec -it demo-openlayers-web bash
        >cd /home/demo-openlayers-web // 创建项目文件夹 
        >chmod 777 -R *
        >npm install
        >npm start

        重新创建(未成功，后续再说)：     
        ###>npm init // 初始化项目
        ###>npx create-ol-app // 创建ol环境，并生成index.html和main.js文件 create-ol-app@1.2.0
        ###>npm start // 验证ol环境
        ###>npm install proj4 // 安装proj4，用于坐标转换
        ###>修改main.js文件
    访问：http://localhost:5173/


3、demo-cesium-web 前端框架(WebSocket 客户端) 注意ws的ip替换为nodejs所在容器的ip
   docker run --name demo-cesium-web -itd -p 8080:8080 nodejs-base:v1 bash
   docker cp ./demo-cesium-web demo-cesium-web:/home/demo-cesium-web
   docker exec -it demo-cesium-web bash
        >cd /home/demo-cesium-web // 创建项目文件夹 
        >chmod 777 -R *
        >npm install
        >node ./server.js --public ### --port 8080
    访问：http://localhost:8080/

4、demo-cpp-control c++控制程序

5、启动wsfplugin
    rm -rf /tmp/.docker.xauth*
    touch /tmp/.docker.xauth
    chmod 600 /tmp/.docker.xauth
    chown root:root /tmp/.docker.xauth
    xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f /tmp/.docker.xauth nmerge -

    (同机器上docker之间端口访问不需要映射)
    docker run --name  afsim -itd -p  -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix  -v /tmp/.docker.xauth:/tmp/.docker.xauth -e XAUTHORITY=/tmp/.docker.xauth ubuntu-afsim:20.04 /bin/bash
    docker cp ./demo-dis-wsfplugin afsim:/home/src/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/wsf_plugins/
    docker exec -it afsim bash
        >cd /home/src/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build
        >cmake ..
        >make
        >make install

        >warlock /home/afsim/afsim_bin/demos/wargame/green_team.txt
