#ifndef ODY_SCSI_PT_H
#define ODY_SCSI_PT_H
#ifdef __cplusplus
extern "C" {
#endif

#define DSJ_CMD_TYPE_GET_ID			0x01	//read方向,获取命令id
#define DSJ_CMD_TYPE_SET_FILENAME_BYID		0x02	//write方向,设置打开文件名
#define DSJ_CMD_TYPE_GET_FILEHANDLE_BYID	0x03   //read方向,获取文件句柄
#define DSJ_CMD_TYPE_CLOSEFILE_BYHANDLE		0x04	//read方向,关闭文件
#define DSJ_CMD_TYPE_READ_BYHANDLE		0x05	//read方向,读文件
#define DSJ_CMD_TYPE_WRITE_BYHANDLE		0x06	//write方向,写文件
#define DSJ_CMD_TYPE_TRUNCATE_BYFILENAME	0x07	//write方向,截断文件
#define DSJ_CMD_TYPE_GETFILESIZE_BYHANDLEE	0x08	//read方向,获取文件长度

#define MK_CMD_GETTASKID16(cmd) \
cmd[0] = 0x88;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_GET_ID;	\
cmd[3] = 0x00;			\
cmd[4] = 0x00;			\
cmd[5] = 0x00;			\
cmd[6] = 0x00;			\
cmd[7] = 0x00;			\
cmd[8] = 0x00;			\
cmd[9] = 0x00;			\
cmd[10] = 4>>24 & 0xff;		\
cmd[11] = 4>>16 & 0xff;		\
cmd[12] = 4>>8 & 0xff;		\
cmd[13] = 4 & 0xff ;		\
cmd[14] = 0x00;			\
cmd[15] = 0x00;			

#define MK_CMD_SETFILENAME16(cmd, taskid) \
cmd[0] = 0x8a;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_SET_FILENAME_BYID;	\
cmd[3] = taskid;			\
cmd[4] = 0x00;			\
cmd[5] = 0x00;			\
cmd[6] = 0x00;			\
cmd[7] = 0x00;			\
cmd[8] = 0x00;			\
cmd[9] = 0x00;			\
cmd[10] = 512>>24 & 0xff;		\
cmd[11] = 512>>16 & 0xff;		\
cmd[12] = 512>>8 & 0xff;		\
cmd[13] = 512 & 0xff ;		\
cmd[14] = 0x00;			\
cmd[15] = 0x00;			

#define MK_CMD_GETTASKRES16(cmd, taskid) \
cmd[0] = 0x88;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_GET_FILEHANDLE_BYID;	\
cmd[3] = taskid;			\
cmd[4] = 0x00;			\
cmd[5] = 0x00;			\
cmd[6] = 0x00;			\
cmd[7] = 0x00;			\
cmd[8] = 0x00;			\
cmd[9] = 0x00;			\
cmd[10] = 512>>24 & 0xff;		\
cmd[11] = 512>>16 & 0xff;		\
cmd[12] = 512>>8 & 0xff;		\
cmd[13] = 512 & 0xff ;		\
cmd[14] = 0x00;			\
cmd[15] = 0x00;			

#define MK_CMD_CLOSEFILE16(cmd, handle) \
cmd[0] = 0x88;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_CLOSEFILE_BYHANDLE;	\
cmd[3] = handle;			\
cmd[4] = 0x00;			\
cmd[5] = 0x00;			\
cmd[6] = 0x00;			\
cmd[7] = 0x00;			\
cmd[8] = 0x00;			\
cmd[9] = 0x00;			\
cmd[10] = 0x00;		\
cmd[11] = 0x00;		\
cmd[12] = 0x00;		\
cmd[13] = 0x00;		\
cmd[14] = 0x00;			\
cmd[15] = 0x00;			

#define MK_CMD_READ16(cmd, handle, offset, size) \
cmd[0] = 0x88;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_READ_BYHANDLE;	\
cmd[3] = handle;			\
cmd[4] = offset >>40 & 0xff ;		\
cmd[5] = offset >>32 & 0xff ;		\
cmd[6] = offset >>24 & 0xff ;		\
cmd[7] = offset >>16 & 0xff ;		\
cmd[8] = offset >>8 & 0xff ;		\
cmd[9] = offset  & 0xff ;		\
cmd[10] = size >> 24 &0xff;		\
cmd[11] = size >> 16 &0xff;		\
cmd[12] = size >> 8 &0xff;		\
cmd[13] = size  &0xff;			\
cmd[14] = 0x00;		\
cmd[15] = 0x00;			

#define MK_CMD_WRITE16(cmd, handle, offset, size) \
cmd[0] = 0x8a;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_WRITE_BYHANDLE;	\
cmd[3] = handle;			\
cmd[4] = offset >>40 & 0xff ;		\
cmd[5] = offset >>32 & 0xff ;		\
cmd[6] = offset >>24 & 0xff ;		\
cmd[7] = offset >>16 & 0xff ;		\
cmd[8] = offset >>8 & 0xff ;		\
cmd[9] = offset  & 0xff ;		\
cmd[10] = size >> 24 &0xff;		\
cmd[11] = size >> 16 &0xff;		\
cmd[12] = size >> 8 &0xff;		\
cmd[13] = size  &0xff;			\
cmd[14] = 0x00;		\
cmd[15] = 0x00;			


#define MK_CMD_GETSIZE16(cmd, handle) \
cmd[0] = 0x88;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_GETFILESIZE_BYHANDLEE;	\
cmd[3] = handle;			\
cmd[4] = 0x00;			\
cmd[5] = 0x00;			\
cmd[6] = 0x00;			\
cmd[7] = 0x00;			\
cmd[8] = 0x00;			\
cmd[9] = 0x00;			\
cmd[10] = 0x00;		\
cmd[11] = 0x00;		\
cmd[12] = 0x00;		\
cmd[13] = 8;		\
cmd[14] = 0x00;			\
cmd[15] = 0x00;			

#define MK_CMD_TRUNCATE16(cmd, offset) \
cmd[0] = 0x8a;			\
cmd[1] = 0x00;			\
cmd[2] = DSJ_CMD_TYPE_TRUNCATE_BYFILENAME;	\
cmd[3] = 0;			\
cmd[4] = offset >>40 & 0xff ;		\
cmd[5] = offset >>32 & 0xff ;		\
cmd[6] = offset >>24 & 0xff ;		\
cmd[7] = offset >>16 & 0xff ;		\
cmd[8] = offset >>8 & 0xff ;		\
cmd[9] = offset  & 0xff ;		\
cmd[10] = 512 >> 24 &0xff;		\
cmd[11] = 512 >> 16 &0xff;		\
cmd[12] = 512 >> 8 &0xff;		\
cmd[13] = 512  &0xff;			\
cmd[14] = 0x00;		\
cmd[15] = 0x00;			

#ifdef DEBUG
#define PRINTCMD16(cmd) \
printf("%02x",cmd[0]); \
printf("%02x",cmd[1]); \
printf("%02x",cmd[2]); \
printf("%02x",cmd[3]); \
printf("%02x",cmd[4]); \
printf("%02x",cmd[5]); \
printf("%02x",cmd[6]); \
printf("%02x",cmd[7]); \
printf("%02x",cmd[8]); \
printf("%02x",cmd[9]); \
printf("%02x",cmd[10]); \
printf("%02x",cmd[11]); \
printf("%02x",cmd[12]); \
printf("%02x",cmd[13]); \
printf("%02x",cmd[14]); \
printf("%02x\n",cmd[15]); 

#define PRINTCMD8(cmd) \
printf("%02x",cmd[0]); \
printf("%02x",cmd[1]); \
printf("%02x",cmd[2]); \
printf("%02x",cmd[3]); \
printf("%02x",cmd[4]); \
printf("%02x",cmd[5]); \
printf("%02x",cmd[6]); \
printf("%02x",cmd[7]); 

#else
#define PRINTCMD16(cmd)
#define PRINTCMD8(cmd)
#endif


int ody_scsi_get_taskid(int fd);
int ody_scsi_get_taskret(int fd,int taskid, void * buff, int buflen);
scsi_handle_t ody_scsi_open_file(int fd,const char *, int taskid);
int ody_scsi_read_cmd(int fd, scsi_handle_t handle,void * buf, off64_t pos, int size);
int ody_scsi_write_cmd(int fd, scsi_handle_t handle,off64_t pos, const void * buf, int writesize);
unsigned long long  ody_scsi_getsize_cmd(int fd, scsi_handle_t handle);
int ody_scsi_truncate_cmd(int fd, char* filename, unsigned long long length);
int ody_scsi_close_cmd(int fd, scsi_handle_t handle);

#ifdef __cplusplus
}
#endif
#endif
