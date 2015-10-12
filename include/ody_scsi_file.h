#ifndef ODY_SCSI_FILE_H
#define ODY_SCSI_FILE_H
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INITLIB_RETRY		3

#define SCSI_FILE_OPEN_READONLY 	1
#define SCSI_FILE_OPEN_WRITE		2

#define SCSI_FILE_MAX_LEN	0x0000ffffffffffff

#define SCSI_FILE_SEEK_SET		0
#define SCSI_FILE_SEEK_CUR		1
#define SCSI_FILE_SEEK_END		2

#define FC_ERR_INITLIB_OPENFILE 	65537	/*cannot open the device file*/
#define FC_ERR_NO_MEMORY 		65538 	/*memory not enough*/
#define FC_ERR_GET_TASKID 		65539 	/*get taskid error*/
#define FC_ERR_OPENFILE 		65540 	/*open file error*/
#define FC_ERR_SEEK 			65541 	/*seek file error*/
#define FC_ERR_NULLFILE 		65542 	/*null file error*/
#define FC_ERR_READ 			65543 	/*read file error*/
#define FC_ERR_FILELEN_TOOLARGE 	65544 	/*file length too large*/
#define FC_ERR_WRITE 			65545 	/*write file error*/

int fc_errno ;

typedef unsigned char scsi_handle_t;

struct fc_file {
	int open_flag;
	scsi_handle_t scsi_handle;
	off64_t file_length;
	off64_t pos;
} ;
typedef struct fc_file fc_file_t;

extern int ody_scsi_timeout;
/*
//初始化库，读取target设备配置
//successful:return 0 
//failed: <0,set fc_errno
*/
extern int initlib(char * filename);
extern void fclibclose();
/*
 * set timeout seconds
 */
extern int fc_set_timeout(int timeout);
/*
//打开目标文件，返回一个目标文件的结构指针
//successful: return fc_file_t *
//failed :return NULL ,and set fc_errno
*/
extern fc_file_t * fc_open(const char *pathname, int flags, mode_t mode);

/*
//关闭文件
//*/
extern int fc_close(fc_file_t *);

/*
//文件读写定位
//return the current pos
//failed :return -1, and set fc_errno
//*/
off64_t fc_lseek(fc_file_t *, int64_t offset, int whence);
/*
//return read count
//or return -1 and set fc_errno
//*/
int fc_read(fc_file_t *, void *buf, size_t count);
/*
//return write count
//or return -1 and set fc_errno
//*/
int fc_write(fc_file_t *, const void *buf, size_t count);
/*
//return read count
//or return -1 and set fc_errno
//*/
int fc_pread(fc_file_t *, void *buf, size_t count, int64_t offset);
/*
//return write count
//or return -1 and set fc_errno
*/
int fc_pwrite(fc_file_t *, const void *buf, size_t count, int64_t offset);
/*
//return 0 
//or return -1 when failed
//*/
int fc_truncate(const char *pathname, int64_t length);
int fc_fsync(fc_file_t *);
int fc_remove(const char *pathname);



#ifdef __cplusplus
}
#endif
#endif
