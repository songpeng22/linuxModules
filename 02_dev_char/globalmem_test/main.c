#include<fcntl.h>
#include<stdio.h>


int main(void)
{
	char s[] = "Linux Programmer!\n";
	char buffer[80];
	int fd=open("/dev/globalmem",O_RDWR);//打开globalmem设备，fd返回大于2的数则成功，O_RDWR为权限赋予
	write(fd,s,sizeof(s));          //将字符串s写入globalmem字符设备中
	printf("test write %d %s\n",fd,s );  
		close(fd);  //关闭设备
	fd=open("/dev/globalmem",O_RDWR);
	read(fd,buffer,sizeof(buffer));   //读取globalmem设备中存储的数据
	printf("test read %d %s\n",fd,buffer);  //输出结果显示
	return 0;
}
