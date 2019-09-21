一个简单的聊天室服务器（测试版）
通过epoll实现的聊天室服务器,服务器采用tcp连接（未来将会改为udp）
默认本地端口为9215
默认客户端口为8215
数据包格式为
struct Packet
{
    int flag;
    int onlineCount;//当前在线人数，会由服务器发送给客户端
    char IDBUFF[MAXIDLEN];  //昵称buff，请在发送信息和登陆时附带昵称，最长为0x20
    char msgBUFF[MAXMSGLEN];//消息buff，最长为0x400
};
说明：
flag为标志位 包括以下状态
#define MF_ONLINE   0x1 //上线，仅当上线时由客户端发送给服务器
#define MF_OFFLINE  0x2//下线，仅当下线时由客户端发送给服务器
#define MF_MSG      0X3//消息，客户端发送信息时请使用此标志位，服务器会将此数据包转发给其它所有在线客户端
#define MF_FULL     0X4//服务器已满，当服务器在线人数达到上限时，由服务器回复发送上线数据包的客户端
#define MF_HEART    0x5//心跳包，请每分钟至少向服务器发送一次心跳包，否则服务器将会断开连接