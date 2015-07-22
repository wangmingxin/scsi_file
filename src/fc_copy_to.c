#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "ody_scsi_file.h"

static void usage()
{
	fprintf(stderr,"Usage: fc_copy_to sg_dev_path localfile remotefile\n");
	return ;
}

static void print_fc_file(fc_file_t *file)
{
	printf("open_flag = %d\n",file->open_flag);
	printf("scsi_handle = %d\n",file->scsi_handle);
	printf("length = %llu\n",file->file_length);
	printf("curpos = %llu\n",file->pos);
}

int main(int argc, char * argv[])
{
	fc_file_t * file = NULL;

	if(argc < 4){
		usage();
		return -1;
	}
	FILE* from = NULL;
	
	if(initlib(argv[1])<0){
		perror("initlib error\n");
		return -1;
	}

	file = fc_open(argv[3], SCSI_FILE_OPEN_WRITE,0);
	if(file == NULL){
		perror("cannot open file");
		return -1;
	}

	from = open(argv[2],"r+");

	if(from <0){
		perror("open localfile to read error");
	}
	
	char buff[64*1024];
	int writesize=0;
	int read_size=0;
	
	while((read_size=read(from, buff, sizeof(buff)))>0){
		writesize = fc_write(file, buff, read_size);
		if(writesize <=0){
			fprintf(stderr,"fc_write writesize=%d \n",writesize);
			break;
		}
	}
	
	print_fc_file(file);
	close(from);

	return fc_close(file);
}
