#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>



int read_write_new(int fd1, int size, int fd2){

		
	char buf[size];
	buf[size] = '\0';


	read(fd1, buf, sizeof(buf));

	printf("buf: %s\n", buf);
	

	if(write(fd2, buf, sizeof(buf)) == -1){
		printf("Failed to write\n");
	}

	return 0;
}




int copy_map(int fd1, int fd2){


	int file_size;

	if((file_size = lseek(fd1, 0, SEEK_END)) == -1){

		printf("Cannot get file size\n");
		return 1;
	}

	char* file_one = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd1, 0);

	if(file_one == MAP_FAILED){

		printf("map falied file one\n");
		return 1;
	}

	if(ftruncate(fd2, file_size) == -1){

		printf("truncate falied file two\n");
		return 1;
	}


	char* file_two = mmap(NULL, file_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd2, 0);

	if(file_two == MAP_FAILED){

		printf("map falied file two\n");
		return 1;

	}


	memcpy(file_two, file_one, file_size);

	munmap(file_one, file_size);
	munmap(file_two, file_size);

	return 0;



}



void help_func(){

	printf("The below execution copies contents of source file to destination file using mmap() and memcpy():\n\n");
	printf("\tcopy [-m] <source_file_name> <destination_file_name> \n\n");
	printf("The below execution copies contents of source file to destination file using read() and write():\n\n");
	printf("\tcopy <source_file_name> <destination_file_name> \n\n");
	printf("The below execution will show this help statements:\n\n");
	printf("\tcopy [-h]\n\n");
}



int main(int argc, char* argv[]){


	if(argc == 1){

		help_func();


	}else{

		int selection;

		bool mmap_call = false;


		while((selection = getopt(argc, argv, "mh")) != -1){
			switch(selection){

				case 'm':

					mmap_call = true;
					break;
				case 'h':

					printf("help called\n");
					help_func();
					return 0;

				default:

					printf("help called in default\n");
					help_func();
					return 0;
			}


		}

		int file1, file2;

		if(mmap_call){
			file1 = 2;
			file2 = 3;
		}else{
			file1 = 1;
			file2 = 2;
		}

		int fd1 = open(argv[file1], O_RDONLY);

		if(fd1 == -1){

			printf("Failed to open the file for reading.\n");
			exit(1);
		}

		struct stat st;
		stat(argv[file1], &st);
		int size = st.st_size;

		//printf("The size of file to read is:%d\n", size );


		int fd2 = open(argv[file2], O_RDWR , O_APPEND, 0666);

		bool open_fail = false;

		if(fd2 == -1){

			open_fail = true;

			printf("Failed to open the existing file for writing.\n");
			printf("Creating a new file for writing...\n");
			int fd3 = open(argv[file2], O_CREAT, 0666);

			if(fd3 == -1){
				printf("Failed to open a new file for writing\n");
				exit(1);
			}else{

				fd2 = open(argv[file2], O_RDWR, O_TRUNC, 0666);

				if(fd2 == -1){
					printf("Failed to open the new created file\n");
					exit(1);
				}
			}
			
		}

		if(!open_fail){
			if(lseek(fd2, 0, SEEK_END) == -1){
				printf("lseek didnt work\n");
			}
		}


		if(mmap_call){

			printf("Using mmap() to copy one file to another\n");
			copy_map(fd1, fd2);
			

		}else{

			printf("Using read() write() to copy one file to another\n");
			read_write_new(fd1, size, fd2);

		}


		if(close(fd1)){
			printf("Cannot close the read file\n");
			return 1;
		}

		if(close(fd2)){
			printf("Cannot close the write file\n");
			return 1;
		}

	}

	return 0;
}