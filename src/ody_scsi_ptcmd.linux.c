#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <scsi/sg_linux_inc.h>
#include <scsi/sg_lib.h>
#include "ody_scsi_file.h"
#include "ody_scsi_pt.h"

#define DEF_TIMEOUT 	20000		/* 20000 millisecs == 20 seconds */

static void DUMPBUF(unsigned char *buf, int buflen)
{
	int k;
	for (k = 0; k < buflen; ++k) {
		if ((k > 0) && (0 == (k % 8)))
			fprintf(stderr,"\n  ");
		fprintf(stderr,"0x%02x ", buf[k]);
	}
	fprintf(stderr,"\n ");
}
int ody_scsi_get_taskid(int fd)
{
	unsigned char CmdBlk [16] ;	
	unsigned char buff [4] ;	
	unsigned char sense_buffer[32];
	int res;
	MK_CMD_GETTASKID16(CmdBlk);

	struct sg_io_hdr io_hdr;
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	memset(buff, 0, sizeof(buff));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = sizeof(buff);
	io_hdr.dxferp = buff;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;

	while (((res = ioctl(fd, SG_IO, &io_hdr)) < 0) && (EINTR == errno)) ;
	if (res < 0) {
		perror("reading (SG_IO) on sg device, error");
		return -1;
	}
	 /* now for the error processing */
    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        if (io_hdr.sb_len_wr > 0) {
		fprintf(stderr,"get taskid sense data: ");
		DUMPBUF(sense_buffer, io_hdr.sb_len_wr);

        }
	return -1;
    }else {  /* output result if it is available */
	return (uint32_t)buff[0] << 24 | (uint32_t)buff[1] << 16 | buff[2] << 8 | buff[3];
    }
}
int ody_scsi_get_taskret(int fd,int taskid, void * buff, int buflen)
{
	unsigned char CmdBlk [16] ;	
	unsigned char sense_buffer[32];

	struct sg_io_hdr io_hdr;
	MK_CMD_GETTASKRES16(CmdBlk, (taskid&0xff));
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	memset(sense_buffer, 0, sizeof(sense_buffer));

	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = buflen;
	io_hdr.dxferp = buff;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;
	if (ioctl(fd, SG_IO, &io_hdr) < 0) {
		perror("ody_scsi_open_file get task ret error");
		return -1;
	}
	if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
		if (io_hdr.sb_len_wr > 0) {
			fprintf(stderr,"get taskret sense data: ");
			DUMPBUF(sense_buffer, io_hdr.sb_len_wr);
		}
		perror("set file name error");
		return -1;
	}	
	return 0;
}
scsi_handle_t ody_scsi_open_file(int fd, const char * filename, int taskid)
{

	unsigned char CmdBlk [16] ;	
	unsigned char buff [512] ;	
	unsigned char sense_buffer[32];

	printf ("ody_scsi_open_file open %s, taskid=%d\n",filename, taskid);
	MK_CMD_SETFILENAME16(CmdBlk, taskid);

	struct sg_io_hdr io_hdr;
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	memset(buff, 0, sizeof(buff));
	memset(sense_buffer, 0, sizeof(sense_buffer));
	snprintf((char*)buff, sizeof(buff)-1,"%s", filename);

	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
	io_hdr.dxfer_len = sizeof(buff);
	io_hdr.dxferp = buff;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;
	if (ioctl(fd, SG_IO, &io_hdr) < 0) {
		perror("ody_scsi_open_file setfilename error");
		return 0;
	}
	if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
		if (io_hdr.sb_len_wr > 0) {
			fprintf(stderr,"openfile sense data: ");
			DUMPBUF(sense_buffer, io_hdr.sb_len_wr);
			
		}
		perror("set file name error");
		return 0;
	}	

	MK_CMD_GETTASKRES16(CmdBlk, (taskid & 0xff));
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = sizeof(buff);
	io_hdr.dxferp = buff;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;
	if (ioctl(fd, SG_IO, &io_hdr) < 0) {
		perror("ody_scsi_open_file get task ret error");
		return 0;
	}
	if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
		if (io_hdr.sb_len_wr > 0) {
			fprintf(stderr,"get taskreturn sense data: ");
			DUMPBUF(sense_buffer, io_hdr.sb_len_wr);
		}
		perror("get filehandle error");
		return 0;
	}	
	return buff[0];	
}

int ody_scsi_read_cmd(int fd, scsi_handle_t handle, void * buf, off64_t pos, int size)
{
	unsigned char CmdBlk [16] ;	
	unsigned char sense_buffer[32];
	int res;
	MK_CMD_READ16(CmdBlk, handle, pos, size);
//	PRINTCMD16(CmdBlk);

	struct sg_io_hdr io_hdr;
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	memset(sense_buffer, 0, sizeof(sense_buffer));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = size;
	io_hdr.dxferp = buf;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;
	while (((res = ioctl(fd, SG_IO, &io_hdr)) < 0) && (EINTR == errno));
	if (res < 0) {
		perror("reading (SG_IO) on sg device, error");
		return -1;
	}
	
	res = sg_err_category3(&io_hdr);
	switch (res) {
		case SG_LIB_CAT_CLEAN:
		case SG_LIB_CAT_RECOVERED:
			return 0;	
		case SG_LIB_CAT_ABORTED_COMMAND:
		case SG_LIB_CAT_UNIT_ATTENTION:
		case SG_LIB_CAT_MEDIUM_HARD:
		case SG_LIB_CAT_NOT_READY:
		case SG_LIB_CAT_ILLEGAL_REQ:
		default:
			sg_chk_n_print3("reading", &io_hdr, 1);
			return -1;
	}

}
int ody_scsi_write_cmd(int fd, scsi_handle_t handle,off64_t pos, const void * buf, int writesize)
{
	unsigned char CmdBlk [16] ;	
	unsigned char sense_buffer[32];
	int res;
	MK_CMD_WRITE16(CmdBlk, handle, pos, writesize);

	struct sg_io_hdr io_hdr;
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	memset(sense_buffer, 0, sizeof(sense_buffer));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
	io_hdr.dxfer_len = writesize;
	io_hdr.dxferp = buf;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;
	while (((res = ioctl(fd, SG_IO, &io_hdr)) < 0) && (EINTR == errno));
	if (res < 0) {
		if (ENOMEM == errno)
			return -2;
		perror("write (SG_IO) on sg device, error");
		return -1;
	}
	
	res = sg_err_category3(&io_hdr);
	switch (res) {
		case SG_LIB_CAT_CLEAN:
		case SG_LIB_CAT_RECOVERED:
			return 0;	
		case SG_LIB_CAT_ABORTED_COMMAND:
		case SG_LIB_CAT_UNIT_ATTENTION:
		case SG_LIB_CAT_MEDIUM_HARD:
		case SG_LIB_CAT_NOT_READY:
		case SG_LIB_CAT_ILLEGAL_REQ:
		default:
			sg_chk_n_print3("writing", &io_hdr, 1);
			return -1;
	}
}
unsigned long long  ody_scsi_getsize_cmd(int fd, scsi_handle_t handle)
{
	unsigned char CmdBlk [16] ;	
	unsigned char buff [8] ;	
	unsigned char sense_buffer[32];
	MK_CMD_GETSIZE16(CmdBlk, handle);

	struct sg_io_hdr io_hdr;
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	memset(buff, 0, sizeof(buff));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = sizeof(buff);
	io_hdr.dxferp = buff;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;

	if (ioctl(fd, SG_IO, &io_hdr) < 0) {
		perror("ody_scsi_getsize error");
		return -1;
	}
	 /* now for the error processing */
    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        if (io_hdr.sb_len_wr > 0) {
		fprintf(stderr,"get filesize sense data: ");
		DUMPBUF(sense_buffer, io_hdr.sb_len_wr);
	}
	perror("get file size error");
	return -1;
    }else {  
	return (uint64_t) buff[0] << 56 | (uint64_t) buff[1] << 48 | (uint64_t) buff[2] << 40 | (uint64_t) buff[3]<<32 | (uint64_t)buff[4]<<24 |(uint64_t)buff[5]<<16 |(uint64_t)buff[6]<<8 |(uint64_t)buff[7];
    }
}
int ody_scsi_truncate_cmd(int fd, char* filename, unsigned long long length)
{

	unsigned char CmdBlk [16] ;	
	unsigned char buff [512] ;	
	unsigned char sense_buffer[32];
	MK_CMD_TRUNCATE16(CmdBlk, length);
	PRINTCMD16(CmdBlk);

	struct sg_io_hdr io_hdr;
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	memset(buff, 0, sizeof(buff));
	memset(sense_buffer, 0, sizeof(sense_buffer));
	snprintf((char*)buff, sizeof(buff)-1,"%s", filename);

	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
	io_hdr.dxfer_len = sizeof(buff);
	io_hdr.dxferp = buff;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;
	if (ioctl(fd, SG_IO, &io_hdr) < 0) {
		perror("ody_scsi_truncate_cmd  error");
		return -1;
	}
	if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
		if (io_hdr.sb_len_wr > 0) {
			fprintf(stderr,"get truncate sense data: ");
			DUMPBUF(sense_buffer, io_hdr.sb_len_wr);
		}
		perror("truncate file error");
		return -1;
	}	
	return 0;	
}

int ody_scsi_close_cmd(int fd, scsi_handle_t handle)
{
	unsigned char CmdBlk [16] ;	
	unsigned char buff [4] ;	
	unsigned char sense_buffer[32];
	MK_CMD_CLOSEFILE16(CmdBlk, handle);

	struct sg_io_hdr io_hdr;
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(CmdBlk) ;
	io_hdr.cmdp = CmdBlk ;
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = 0;
	io_hdr.dxferp = buff;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = DEF_TIMEOUT;
	io_hdr.pack_id = 0;
	if (ioctl(fd, SG_IO, &io_hdr) < 0) {
		perror("ody_scsi_close_cmd  error");
		return -1;
	}
	if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
		if (io_hdr.sb_len_wr > 0) {
			fprintf(stderr,"get close sense data: ");
			DUMPBUF(sense_buffer, io_hdr.sb_len_wr);
		}
		perror("close file handle error");
		return -1;
	}	
	return 0;
}
