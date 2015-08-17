#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "ody_scsi_file.h"
#include "ody_scsi_pt.h"

#define MAX_SCSI_TRANSFER_SIZE	128*1024
#define MIN(a,b) 	((a)>(b))?(b):(a)

static char ody_scsi_dev_conf [1024];
static int ody_scsi_dev_fd =-1;

static int fc_test()
{
	return ody_scsi_test_cmd(ody_scsi_dev_fd);
}

int initlib(char * devfilename)
{
	fc_errno = 0;
	int retry=0;
	if( ody_scsi_dev_fd >=0){
		return 0;
	}
	ody_scsi_dev_fd = open (devfilename, O_RDWR);
	if(ody_scsi_dev_fd < 0){
		perror("open error");
		fc_errno = FC_ERR_INITLIB_OPENFILE;
		return -1;
	}

	while(retry < MAX_INITLIB_RETRY){	
		if(fc_test() < 0){
			retry++;
			sleep(1);
		}else{
			break;
		}
	}

	return ody_scsi_dev_fd;
}
fc_file_t * fc_open(const char *pathname, int flag, mode_t mode)
{
		
	fc_file_t *file = calloc(1,sizeof (fc_file_t));
	if(file == NULL){
		fc_errno = FC_ERR_NO_MEMORY;
		return NULL;
	}
	
	int taskid = ody_scsi_get_taskid(ody_scsi_dev_fd);
	if(taskid <=0){
		fc_errno = FC_ERR_GET_TASKID;
		free(file);
		return NULL;
	}
	
	file->scsi_handle = ody_scsi_open_file(ody_scsi_dev_fd,pathname, taskid);
	if(file->scsi_handle <= 0){
		fc_errno = FC_ERR_OPENFILE;
		free(file);
		return NULL;
	}	
	file->file_length = ody_scsi_getsize_cmd(ody_scsi_dev_fd, file->scsi_handle);
	file->open_flag = flag;
	file->pos = 0;

	return file;
		
}

int fc_close(fc_file_t * fc_file)
{
	int ret = 0;
	if(fc_file != NULL){ 
		if(fc_file ->scsi_handle >0){
			ret = ody_scsi_close_cmd(ody_scsi_dev_fd,fc_file->scsi_handle);	
		}

		if(ret == 0){
			free(fc_file);
		}
		return ret;
	}
	return ret;
}

off64_t fc_lseek(fc_file_t * file, int64_t offset, int whence)
{
	off64_t pos;
	if(file == NULL){
		fc_errno = FC_ERR_NULLFILE;
		return -1;
	}
	switch(whence){
	case SCSI_FILE_SEEK_CUR:
		pos = file->pos + offset;	
		break;
	case SCSI_FILE_SEEK_END:
		pos = file->file_length + offset;
		break;
	case SCSI_FILE_SEEK_SET:
	default:
		pos = offset;
	} 

	if(pos > SCSI_FILE_MAX_LEN){
		fc_errno = FC_ERR_FILELEN_TOOLARGE;
		perror("fc_lseek exceed SCSI_FILE_MAX_LEN");
		return -1;
	}

	file->pos = pos;
	if(pos > file->file_length){
		file->file_length = pos;
	}
	return pos;
}

int fc_read(fc_file_t *file, void *buf, size_t count)
{
	return fc_pread(file,buf,count,file->pos);
/*
	int read_count=count;
	int ret = 0;
	if(file == NULL){
		fc_errno = FC_ERR_NULLFILE;
		return -1;
	}

	if(file->pos >= file->file_length)
		return 0;

	if(file->pos + count > file->file_length ){
		read_count = file->file_length - file->pos;
	} 
	
	ret = ody_scsi_read_cmd(ody_scsi_dev_fd,file->scsi_handle,buf, file->pos, read_count );

	if(ret == 0){
		file->pos += read_count;	
		return read_count;
	}else{
		fc_errno = FC_ERR_READ;
		return -1;
	}
	
*/
}
int fc_write(fc_file_t *file, const void *buf, size_t count)
{
	return fc_pwrite(file,buf,count, file->pos);
/*
	int write_count=count;
	int ret = 0;
	if(file == NULL){
		fc_errno = FC_ERR_NULLFILE;
		return -1;
	}

	if(file->pos  >= SCSI_FILE_MAX_LEN ){
		fc_errno = FC_ERR_FILELEN_TOOLARGE;
		perror("fc_write exceed the SCSI_FILE_MAX_LEN");
		return -1;
	} 
	
	ret = ody_scsi_write_cmd(ody_scsi_dev_fd,file->scsi_handle,file->pos,buf, write_count );

	if(ret == 0){
		file->pos += write_count;	
		if(file->pos > file->file_length){
			file->file_length=file->pos;
		}
		return write_count;
	}else{
		fc_errno = FC_ERR_WRITE;
		return -1;
	}
	
*/
}
int fc_pread(fc_file_t *file, void *buf, size_t count, int64_t offset)
{
	int readed_count=0;
	int read_count=count;
	int ret = 0;
	if(file == NULL){
		fc_errno = FC_ERR_NULLFILE;
		return -1;
	}

	if(offset + count > file->file_length ){
		count = file->file_length - offset;
	} 
	read_count = MIN(count, MAX_SCSI_TRANSFER_SIZE);

	while(read_count>0){

		ret = ody_scsi_read_cmd(ody_scsi_dev_fd,file->scsi_handle,buf+readed_count, offset+readed_count, read_count );

		if(ret == 0){
			readed_count += read_count;
		}else{
			fc_errno = FC_ERR_READ;
			if(readed_count >0){
				file->pos = offset+readed_count;	
				return readed_count;
			}else{
				return -1;
			}
		}
		read_count = MIN(count-readed_count, MAX_SCSI_TRANSFER_SIZE);
	}
	file->pos = offset+readed_count;	
	return readed_count;
}
int fc_pwrite(fc_file_t *file, const void *buf, size_t count, int64_t offset)
{
	int written_size=0;
	int write_count= MIN(count, MAX_SCSI_TRANSFER_SIZE);
	int ret = 0;
	if(file == NULL){
		fc_errno = FC_ERR_NULLFILE;
		return -1;
	}

	if(offset  >= SCSI_FILE_MAX_LEN ){
		fc_errno = FC_ERR_FILELEN_TOOLARGE;
		perror("fc_write exceed the SCSI_FILE_MAX_LEN");
		return -1;
	} 
	
	while(write_count >0){
		ret = ody_scsi_write_cmd(ody_scsi_dev_fd,file->scsi_handle,offset+written_size,buf+written_size, write_count );

		if(ret == 0){
			written_size += write_count;
		}else{
			fc_errno = FC_ERR_WRITE;
			if(written_size >0){
				file->pos = offset+written_size;	
				if(file->pos > file->file_length){
					file->file_length=file->pos;
				}
				return written_size;
			}else{
				return -1;
			}
		}
		write_count= MIN(count-written_size,MAX_SCSI_TRANSFER_SIZE);
	}

	file->pos = offset+written_size;	
	if(file->pos > file->file_length){
		file->file_length=file->pos;
	}
	return written_size;
}
//return 0 
//or return -1 when failed
int fc_truncate(const char *pathname, int64_t length)
{
	return ody_scsi_truncate_cmd(ody_scsi_dev_fd, pathname, length);		
}

int fc_fsync(fc_file_t * file)
{
	return 0;
}
//return 0 
//or return -1 when failed
int fc_remove(const char *pathname)
{
	return ody_scsi_unlink_cmd(ody_scsi_dev_fd, pathname);		
}


