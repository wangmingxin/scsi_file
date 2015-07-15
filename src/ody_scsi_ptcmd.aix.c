#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "gscdds.h"
#include "ody_scsi_file.h"
#include "ody_scsi_pt.h"

#define DEF_TIMEOUT 	20		/* 20000 millisecs == 20 seconds */

int ody_scsi_get_taskid(int fd)
{
	unsigned char CmdBlk [16] ;	
	unsigned char buff [4] ;	
	unsigned char sense_buffer[32];
	int sbyte=0;
	MK_CMD_GETTASKID16(CmdBlk);

	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	scmd.data_buf = buff;
	scmd.datalen = sizeof(buff);
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 1;
	scmd.timeval = DEF_TIMEOUT;

	if (ioctl(fd, GSC_CMD, (caddr_t) &scmd) < 0) {
		perror("GSC_CMD bad command");
		return  -1;
	}

	 /* now for the error processing */
	if (sbyte != 0) {
		fprintf(stderr, "error: get taskid sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return -1;
	}
	return buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3];
}
int ody_scsi_get_taskret(int fd,int taskid, void * buff, int buflen)
{
	unsigned char CmdBlk [16] ;	
	unsigned char sense_buffer[32];
	int sbyte=0;

	MK_CMD_GETTASKRES16(CmdBlk, (taskid&0xff));
	memset(sense_buffer, 0, sizeof(sense_buffer));

	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	scmd.data_buf = buff;
	scmd.datalen = buflen;
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 1;
	scmd.timeval = DEF_TIMEOUT;

	if (ioctl(fd, GSC_CMD, (caddr_t) &scmd) < 0) {
		perror("ody_scsi_open_file get task ret error");
		return -1;
	}
	if (sbyte != 0) {
		fprintf(stderr, "get taskret error: sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return -1;
	}
	return 0;
}
scsi_handle_t ody_scsi_open_file(int fd, const char * filename, int taskid)
{

	unsigned char CmdBlk [16] ;	
	unsigned char buff [512] ;	
	unsigned char sense_buffer[32];
	int sbyte;

	printf ("ody_scsi_open_file open %s, taskid=%d\n",filename, taskid);
	MK_CMD_SETFILENAME16(CmdBlk, taskid);

	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	memset(buff, 0, sizeof(buff));
	memset(sense_buffer, 0, sizeof(sense_buffer));
	snprintf(buff, sizeof(buff)-1,"%s", filename);

	scmd.data_buf = buff;
	scmd.datalen = sizeof(buff);
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 0;
	scmd.timeval = DEF_TIMEOUT;

	if (ioctl(fd, GSC_CMD, (caddr_t) &scmd) < 0) {
		perror("ody_scsi_open_file setfilename error");
		return 0;
	}
	if (sbyte != 0) {
		fprintf(stderr, "error: setfilename sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return 0;
	}

	MK_CMD_GETTASKRES16(CmdBlk, (taskid & 0xff));
	scmd.data_buf = buff;
	scmd.datalen = sizeof(buff);
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 1;
	scmd.timeval = DEF_TIMEOUT;
	if (ioctl(fd, GSC_CMD, (caddr_t) &scmd) < 0) {
		perror("ody_scsi_open_file get taskret error");
		return 0;
	}
	if (sbyte != 0) {
		fprintf(stderr, "error: get taskret sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return 0;
	}

	return buff[0];	
}

int ody_scsi_read_cmd(int fd, scsi_handle_t handle, void * buf, off64_t pos, int size)
{
	unsigned char CmdBlk [16] ;	
	unsigned char sense_buffer[32];
	int res;
	int sbyte;
	MK_CMD_READ16(CmdBlk, handle, pos, size);

	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	scmd.data_buf = buf;
	scmd.datalen = size;
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 1;
	scmd.timeval = DEF_TIMEOUT;

	while (((res = ioctl(fd, GSC_CMD, (caddr_t)&scmd)) < 0) && (EINTR == errno));
	if (res < 0) {
		perror("reading (SG_IO) on sg device, error");
		return -1;
	}
	if (sbyte != 0) {
		fprintf(stderr, "error: read sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return -1;
	}
	return 0;
}
int ody_scsi_write_cmd(int fd, scsi_handle_t handle,off64_t pos, const void * buf, int writesize)
{
	unsigned char CmdBlk [16] ;	
	unsigned char sense_buffer[32];
	int res,sbyte;
	MK_CMD_WRITE16(CmdBlk, handle, pos, writesize);

	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	scmd.data_buf = buf;
	scmd.datalen = writesize;
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 0;
	scmd.timeval = DEF_TIMEOUT;

	while (((res = ioctl(fd, GSC_CMD, (caddr_t)&scmd)) < 0) && (EINTR == errno));
	if (res < 0) {
		perror("writeing (SG_IO) on sg device, error");
		return -1;
	}
	if (sbyte != 0) {
		fprintf(stderr, "error: write sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return -1;
	}
}
unsigned long long  ody_scsi_getsize_cmd(int fd, scsi_handle_t handle)
{
	unsigned char CmdBlk [16] ;	
	unsigned char buff [8] ;	
	unsigned char sense_buffer[32];
	int res, sbyte;
	MK_CMD_GETSIZE16(CmdBlk, handle);
	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	scmd.data_buf = buff;
	scmd.datalen = sizeof(buff);
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 1;
	scmd.timeval = DEF_TIMEOUT;

	if (ioctl(fd, GSC_CMD, (caddr_t) &scmd) < 0) {
		perror("ody_scsi_getsize_cmd taskret error");
		return -1;
	}
	if (sbyte != 0) {
		fprintf(stderr, "error: getsize sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return -1;
	}
	PRINTCMD8(buff);

	return (uint64_t) buff[0] << 56 | (uint64_t) buff[1] << 48 | (uint64_t) buff[2] << 40 | (uint64_t) buff[3]<<32 | buff[4]<<24 |buff[5]<<16 |buff[6]<<8 |buff[7];
}
int ody_scsi_truncate_cmd(int fd, char* filename, unsigned long long length)
{

	unsigned char CmdBlk [16] ;	
	unsigned char buff [512] ;	
	unsigned char sense_buffer[32];
	int sbyte;
	MK_CMD_TRUNCATE16(CmdBlk, length);

	memset(buff, 0, sizeof(buff));
	memset(sense_buffer, 0, sizeof(sense_buffer));
	snprintf(buff, sizeof(buff)-1,"%s", filename);

	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	scmd.data_buf = buff;
	scmd.datalen = sizeof(buff);
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 0;
	scmd.timeval = DEF_TIMEOUT;

	if (ioctl(fd, GSC_CMD, (caddr_t) &scmd) < 0) {
		perror("ody_scsi_truncate_cmd  error");
		return -1;
	}
	if (sbyte != 0) {
		fprintf(stderr, "error: truncate sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return -1;
	}
	return 0;	
}

int ody_scsi_close_cmd(int fd, scsi_handle_t handle)
{
	unsigned char CmdBlk [16] ;	
	unsigned char sense_buffer[32];
	int res,sbyte;
	MK_CMD_CLOSEFILE16(CmdBlk, handle);

	scmd_t scmd;
	scmd.cdb = CmdBlk;
	scmd.cdblen = 16;

	scmd.data_buf = NULL;
	scmd.datalen = 0;
	scmd.sense_buf = sense_buffer;
	scmd.senselen = sizeof (sense_buffer);
	scmd.statusp = &sbyte;
	scmd.rw = 1;
	scmd.timeval = DEF_TIMEOUT;

	if (ioctl(fd, GSC_CMD, (caddr_t) &scmd) < 0) {
		perror("ody_scsi_close_cmd  error");
		return -1;
	}
	if (sbyte != 0) {
		fprintf(stderr, "error: closecmd sense/ASC/ASCQ = %d/%02x/%02x\n", sbyte, sense_buffer[12], sense_buffer[13]);
		return -1;
	}

	return 0;
}
