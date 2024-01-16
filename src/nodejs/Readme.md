Linux Ubuntu环境
1: 编译并安装node到指定目录

2: 将node核心功能融入自己的可执行程序
进入 install 目录
创建testnode.cpp文件:
#include "node.h"

int main(int argc, char* argv[])
{
        int node_result = node::Start(argc-1, argv+1);

        return 0;
}
3: 编译
g++ -o test -pthread -Wl,-rpath=\$ORIGIN/lib testnode.cpp  ./lib/libnode.so.112 -I include/ -I include/node/ -I./ -std=gnu++17

4: 运行，通过nodejs 语法启动一个http服务器
	   第二个参数node不能省，但是可以通过C代码隐藏。
./test node -e "var http = require('http'); var versions_server = http.createServer( (request, response) => { response.end('Versions: ' + JSON.stringify(process.versions)); }); versions_server.listen(3000);"

5: 参考
https://code.janeasystems.com/nodejs-mobile
https://nodejs.org/dist/latest-v20.x/docs/api/embedding.html
