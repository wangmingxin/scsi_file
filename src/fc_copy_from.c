#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "ody_scsi_file.h"

static void usage()
{
	fprintf(stderr,"Usage: fc_copy_from sg_dev_path filename localfile\n");
	return ;
}

static void print_fc_file(fc_file_t *file)
{
	printf("open_flag = %d\n",file->open_flag);
	printf("scsi_handle = %d\n",file->scsi_handle);
	printf("length = %lld\n",file->file_length);
	printf("curpos = %lld\n",file->pos);
}

int main(int argc, char * argv[])
{
	fc_file_t * file = NULL;

	if(argc < 4){
		usage();
		return -1;
	}
	int to = -1;
	
	if(initlib(argv[1])<0){
		perror("initlib error\n");
		return -1;
	}

	file = fc_open(argv[2], SCSI_FILE_OPEN_READONLY,0);
	if(file == NULL){
		perror("cannot open file");
		return -1;
	}

	to = open(argv[3],O_RDWR|O_CREAT);

	if(to <0){
		perror("open localfile to write error");
	}
	
	char buff[64*1024];
	int read_size=0;
	
	while((read_size=fc_read(file, buff, sizeof(buff)))>0){
		write(to, buff, read_size);
	}
	
	print_fc_file(file);
	close(to);

	return fc_close(file);
}
