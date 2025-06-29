# QChat—2025程序设计实习课程大作业
## 演示视频及相关模型请于北大网盘下载：

https://disk.pku.edu.cn/link/AA1BD702504FDF4F1981E0B7CC62983863

文件夹名：QChat大作业

有效期限：2026-07-29 21:05

### 一、语音转文字模型配置
请将于北大网盘下载的model、ffmpeg文件夹均置于QChatServer文件夹与QChatClient文件夹内（即与main.cpp同级）

### 二、外部库配置
请下载requirements.txt  
安装外部库（命令行运行）：

**pip install -r “requirements.txt的绝对地址”**

### 三、网络配置
#### 1.使用热点
可使用个人热点进行测试，两台电脑均需连接同一热点（默认该热点无端与端间的通信隔离），并修改QChatClient主函数第19行的第一个传入参数为QChatServer所在电脑的IP地址

*注：可命令行运行 **ipconfig** 查看IP地址*

#### 2.使用内网穿透工具
*注：该方式无需两电脑连接同一局域网*

请下载natapp.exe，命令行运行：

**“natapp.exe的绝对地址” -authtoken=950695515e358df3**

并保证控制台始终开启

修改QChatClient主函数第19行的第一个传入参数为：

"f3168b1198c3a661.natapp.cc"

*注：开发者目前仅购买了10GB的流量限额，请小心使用*

### 四、运行
请先运行QChatServer，再运行QChatClient
