#ifndef ODY_SCSI_FILE_H
#define ODY_SCSI_FILE_H
#ifdef __cplusplus
extern "C" {
#endif

static char ody_scsi_dev_conf [1024];
static int ody_scsi_dev_fd =0;

#define SCSI_FILE_OPEN_READONLY 	1
#define SCSI_FILE_OPEN_WRITE		2

#define SCSI_FILE_MAX_LEN	0x0000ffffffffffff

#define SCSI_FILE_SEEK_SET		0
#define SCSI_FILE_SEEK_CUR		1
#define SCSI_FILE_SEEK_END		2

typedef unsigned char scsi_handle_t;

struct fc_file {
	int open_flag;
	scsi_handle_t scsi_handle;
	off_t file_length;
	off_t pos;
} ;
typedef struct fc_file fc_file_t;

extern int initlib(char * filename);
//初始化库，读取target设备配置

extern fc_file_t * fc_open(const char *pathname, int flags, mode_t mode);
//打开目标文件，返回一个目标文件的结构指针

extern int fc_close(fc_file_t *);
//关闭文件
off_t fc_lseek(fc_file_t *, int64_t offset, int whence);
//文件读写定位
int fc_read(fc_file_t *, void *buf, size_t count);

int fc_write(fc_file_t *, const void *buf, size_t count);
int fc_pread(fc_file_t *, void *buf, size_t count, int64_t offset);
int fc_pwrite(fc_file_t *, const void *buf, size_t count, int64_t offset);
//int fc_ftruncate(fc_file *, int64_t length);
int fc_truncate(const char *pathname, int64_t length);
int fc_fsync(fc_file_t *);



#ifdef __cplusplus
}
#endif
#endif
