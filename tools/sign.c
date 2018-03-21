#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
	// USAGE: sign <input_file_name> <output_file_name>
	if(argc != 3) {
		printf("Error Usage: sign <input_file_name> <output_file_name>\r\n");
		return -1;
	}
    // printf("argv[1]:%s, argv[2]:%s \r\n",argv[1],argv[2]);
	// 获取bootloader文件信息
	// stat 结构体
	/*
	struct stat {
		dev_t         st_dev;       //文件的设备编号
		ino_t         st_ino;       //节点
		mode_t        st_mode;      //文件的类型和存取的权限
		nlink_t       st_nlink;     //连到该文件的硬连接数目，刚建立的文件值为1
		uid_t         st_uid;       //用户ID
		gid_t         st_gid;       //组ID
		dev_t         st_rdev;      //(设备类型)若此文件为设备文件，则为其设备编号
		off_t         st_size;      //文件字节数(文件大小)
		unsigned long st_blksize;   //块大小(文件系统的I/O 缓冲区大小)
		unsigned long st_blocks;    //块数
		time_t        st_atime;     //最后一次访问时间
		time_t        st_mtime;     //最后一次修改时间
		time_t        st_ctime;     //最后一次改变时间(指属性)
	};
	 */
	struct stat st;
	// stat失败返回-1,错误信息保存在errno里面
	if(stat(argv[1],&st) != 0) {
		perror("获取文件信息stat失败:");
        return -1;
	}

	if(st.st_size > 510) {
		perror(">> 510 bytes");
		return -1;
	}

	// 申请512字节，将bootloader扩展为合法的扇区
	char buff[512];
	memset(buff,0,sizeof(buff));
	FILE* fp = fopen(argv[1],"rb");
	if(fp == NULL) {
		perror("打开文件失败");
        fclose(fp);
        return -1;
	}
	// 读取其全部内容进内存
	size_t readSize = fread(&buff,1,st.st_size,fp);
	if(readSize < st.st_size) {
		perror("读取文件错误：");
        fclose(fp);
        return -1;
	}
	fclose(fp);

	// buff尾部添加0x55aa，使其变为合法扇区
	buff[510] = (char)0x55;
	buff[511] = (char)0xAA;

	// 写出到文件
	FILE* wfp = fopen(argv[2],"wb+");
	if(wfp == NULL) {
		perror("创建文件失败:");
        fclose(wfp);
        return -1;
	}
	size_t writeSize = fwrite(&buff,1,sizeof(buff),wfp);
	if(writeSize < sizeof(buff)) {
		perror("未完全写入文件:");
        fclose(wfp);
        return -1;
	}
	fclose(wfp);
	// 打印输出成功
	printf("创建bootloader合法扇区成功\r\n");
	return 0;
}
