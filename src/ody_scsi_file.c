#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "ody_scsi_file.h"
#include "ody_scsi_pt.h"

int initlib(char * devfilename)
{
	if( ody_scsi_dev_fd >0){
		return 0;
	}
	ody_scsi_dev_fd = open (devfilename, O_RDWR);
	if(ody_scsi_dev_fd < 0){
		perror("open error");
		return -1;
	}

	return ody_scsi_dev_fd;
}
fc_file_t * fc_open(const char *pathname, int flag, mode_t mode)
{
		
	fc_file_t *file = calloc(sizeof (fc_file_t),1);
	if(file == NULL){
		perror("calloc error");
		return NULL;
	}
	
	int taskid = ody_scsi_get_taskid(ody_scsi_dev_fd);
	if(taskid <=0){
		printf("get taskid error, taskid=%d\n",taskid);
		free(file);
		return NULL;
	}
	
	file->scsi_handle = ody_scsi_open_file(ody_scsi_dev_fd,pathname, taskid);
	if(file->scsi_handle <= 0){
		perror("ody_scsi_open_file error");
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
	int read_count=count;
	int ret = 0;
	if(file == NULL){
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
		return -1;
	}
	
}
int fc_write(fc_file_t *file, const void *buf, size_t count)
{
	int write_count=count;
	int ret = 0;
	if(file == NULL){
		return -1;
	}

	if(file->pos  >= SCSI_FILE_MAX_LEN ){
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
		return -1;
	}
	
}
int fc_pread(fc_file_t *file, void *buf, size_t count, int64_t offset)
{
	int read_count=count;
	int ret = 0;
	if(file == NULL){
		return -1;
	}

	if(offset + count > file->file_length ){
		read_count = file->file_length - offset;
	} 
	
	ret = ody_scsi_read_cmd(ody_scsi_dev_fd,file->scsi_handle,buf, offset, read_count );

	if(ret == 0){
		file->pos = offset+read_count;	
		return read_count;
	}else{
		return -1;
	}
}
int fc_pwrite(fc_file_t *file, const void *buf, size_t count, int64_t offset)
{
	int write_count=count;
	int ret = 0;
	if(file == NULL){
		return -1;
	}

	if(offset  >= SCSI_FILE_MAX_LEN ){
		perror("fc_write exceed the SCSI_FILE_MAX_LEN");
		return -1;
	} 
	
	ret = ody_scsi_write_cmd(ody_scsi_dev_fd,file->scsi_handle,offset,buf, write_count );

	if(ret == 0){
		file->pos = offset+write_count;	
		if(file->pos > file->file_length){
			file->file_length=file->pos;
		}
		return write_count;
	}else{
		return -1;
	}
}
int fc_truncate(const char *pathname, int64_t length)
{
	return ody_scsi_truncate_cmd(ody_scsi_dev_fd, pathname, length);		
}
int fc_fsync(fc_file_t * file)
{
	return 0;
}
