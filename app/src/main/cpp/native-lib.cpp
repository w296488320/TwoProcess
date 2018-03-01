#include <jni.h>
#include <android/log.h>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <sys/un.h>
#include <errno.h>

void child_do_work();

bool child_create_channel();

void child_listen_msg();



#define TAG "JNI" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型


const char *PATH="/data/data/com.mindx.doubleprocess/my.sock";


int m_child;

const  char *pid;


void child_do_work() {
    //开启sorcket   服务端
    if(child_create_channel()){
        child_listen_msg();
    }

}
/*
 *
 * */


void child_listen_msg() {
    fd_set rfds;
    struct timeval timeout={3,0};
    while(1){
        //清空内容 端口号 数据信息
        FD_ZERO(&rfds);
        FD_SET(m_child,&rfds);
        //rfds 读的哪一个客户端
        int r = select(m_child + 1, &rfds, NULL, NULL, &timeout);
        LOGE("读取消息前 %d",r);
        if(r>0){
            char pkg[256]={0};
            //保证所独到的信息是 指定apk客户端
            if(FD_ISSET(m_child,&rfds)){
                //阻塞式函数
                ssize_t resut = read(m_child, pkg, sizeof(pkg));
                //开启服务
                execlp("am", "am", "startservice", "--user", pid,"com.mindx.doubleprocess/com.mindx.doubleprocess.ProcessService",(char*)NULL);
                break;
            }
        }

    }
}

bool child_create_channel() {
    //ip  端口   文件
    int listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    struct sockaddr_un  addr;
    unlink(PATH);
    memset(&addr,0, sizeof(sockaddr_un));
    addr.sun_family=AF_LOCAL;
    strcpy(addr.sun_path,PATH);
    int i = bind(listenfd, (const sockaddr *) &addr, sizeof(sockaddr_un));
    if(i<0){
        LOGE("绑定错误");
        return 0;
    }
    int connfd;
    //最大监听个数
    listen(listenfd,5);
    //保证宿主 进程 连接成功 
    while (1){
        //返回值 客户端的地址 ，阻塞试函数
        connfd = accept(listenfd, NULL, NULL);
        if(connfd<0){
            if(errno==EINTR){
                continue;
            } else{
                LOGE("读取错误");
                return 0;
            }
        }
        m_child=connfd;
        break;
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mindx_doubleprocess_DoubleProcess_createDoubleProcess(JNIEnv *env, jobject instance,
                                                               jstring pid_) {
    pid = env->GetStringUTFChars(pid_,NULL);
    pid_t i = fork();//如果小于0 fork失败 如果大于0进是父进程
    if(i<0){

    } else if(i==0){
        //子线程守护  守护进程
        child_do_work();

    } else if(i>0){
        //父进程
    }

    env->ReleaseStringUTFChars(pid_, pid);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_mindx_doubleprocess_DoubleProcess_connectMonitor(JNIEnv *env, jobject instance) {
    int socked;
    while (1){
        LOGE("客户端  父进程开始连接" );
        socked=socket(AF_LOCAL,SOL_SOCKET,0);
        if(socked<0){
            LOGE("连接失败 111" );
            return;
        }
        struct sockaddr_un  addr;
        //清空内存
        memset(&addr,0, sizeof(sockaddr));
        addr.sun_family=AF_LOCAL;
        strcpy(addr.sun_path,PATH);

        if(connect(socked, (const sockaddr *) &addr, sizeof(addr)) < 0){
            LOGE("连接失败 222");
            close(socked);
            sleep(1);
            //再来下一次尝试连接
            continue;
        }
        LOGE("连接成功");
        break;
    }

}