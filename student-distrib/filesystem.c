#include "filesystem.h"
#define BLOCK_SIZE 0x1000
#define EIGHT_MB 0x0800000
#define EIGHT_KB 0x2000
static uint32_t data_block_loc, inode_num_loc;
static uint32_t inode_start_num, file_size;
static uint32_t data_block_begin;   // file system start address
bootblock_t* bootblk_addr;


/*
*   void fs_init(void)
*   Inputs: bootblock address
*   Outputs: none
*   Function: initialize filesystem
*/
void fs_init(uint32_t addr)
{
	// initialize filesystem descriptors
	bootblk_addr = (bootblock_t*) addr;
	data_block_loc = bootblk_addr->data_block_num;   
	inode_num_loc = bootblk_addr->inode_num;   
	file_size = bootblk_addr->dir_entry_num;
	// initialize variables   
	inode_start_num = addr + BLOCK_SIZE;  
	data_block_begin = inode_start_num + inode_num_loc * BLOCK_SIZE;   
}

/*
*   void dir_read(int32_t fd, uint8_t* buf, int32_t nbytes)
*   Inputs: fd, file descriptor, buf, buffer used to store filenames, nbytes, length of bytes to read from directory
*   Outputs: none
*   Function: read in file name given directory name
*/
int32_t dir_read(int32_t fd, uint8_t* buf, int32_t nbytes) {
	// get currenct pcb and corresponding file position
	pcb_t * curr_process = (pcb_t *) (EIGHT_MB - EIGHT_KB * (pid + 1));
	uint32_t offset = (curr_process->file_array[fd].file_position);
	curr_process->file_array[fd].file_position = offset + 1;
	// sanity check
	if (1 + offset >= file_size) 
		return 0;
	if ((fd <= 1) || (fd >= MAX_FD_NUMBER))  
		return -1;
	uint8_t* dentry_name = bootblk_addr->dir_entry[offset].file_name;
	strncpy((int8_t*)buf, (int8_t*)dentry_name, nbytes);
	return strlen((int8_t*)dentry_name);
}


/*
*   void read_file_in_dir(uint32_t offset, uint8_t* buf, uint32_t length)
*   Inputs: offset, offset of directory entry, buf, buffer used to store filenames, nbytes, length of bytes to read from directory
*   Outputs: none
*   Function: read a file given a directory name
*/
int32_t read_file_in_dir(uint32_t offset, uint8_t* buf, uint32_t length)
{
	// sanity check
	if (buf == NULL)
		return -1;
	if (offset + 1 >= file_size)
		return 0;

	// read in dir entry name in bootblk
	uint8_t* dentry_name =  bootblk_addr->dir_entry[offset].file_name;
	strncpy((int8_t*)buf, (int8_t*)dentry_name, length);
	return strlen((int8_t*)dentry_name);
}


/*
 * int32_t dir_write(int32_t fd, const uint8_t* buf, int32_t nbytes)
 * input: 	fd -- file descriptor
 * 			buf -- buffer to be written
 * 			nbytes -- number of bytes to be written
 * output: 	0 on success
 * 			-1 on failure
 */
int32_t dir_write(int32_t fd, const uint8_t* buf, int32_t nbytes) {
	return -1;
}


/*
 * int32_t dir_open (const uint8_t* filename)
 * input: 	filename -- name of the file
 * output: 	0 on success
 * 			-1 on failure
 */
int32_t dir_open(const uint8_t* filename) {
	return 0;
}

/*
 * int32_t dir_close(int32_t fd)
 * input: 	fd -- file descriptor
 * output: 	0 on success
 * 			-1 on failure
 */
int32_t dir_close(int32_t fd) {
	return 0;
}

/*
 * int32_t read_file_and_directory(int32_t fd, void* buf, int32_t nbytes)
 * INPUT: fd -- file descriptor
 * 		  buf -- buffer to be written
 * 		  nbytes -- number of bytes to be written
 * OUTPUT: bytes have been read
 * Description: read data from a file or read filenames from a directory
*/
int32_t file_read (int32_t fd, uint8_t* buf, int32_t nbytes)
{
	// get current pcb
	pcb_t * curr_process = (pcb_t *) (EIGHT_MB - EIGHT_KB * (pid + 1));
	// sanity check
	if (fd <= 1 || fd >= MAX_FD_NUMBER)
		return -1;
	// read data
	int32_t file_len_read = read_data(curr_process->file_array[fd].inode, curr_process->file_array[fd].file_position, buf, nbytes); 
	//update file position
	if (file_len_read != -1) {
		curr_process->file_array[fd].file_position += file_len_read;
		return file_len_read;
	}
	return -1;
}

/*
 * int32_t file_write(int32_t fd, const uint8_t* buf, int32_t nbytes)
 * input: 	fd -- file descriptor
 * 			buf -- buffer to be written
 * 			nbytes -- number of bytes to be written
 * output: 	0 on success
 * 			-1 on failure
 */
int32_t file_write(int32_t fd, const uint8_t * buf, int32_t nbytes) {
	return -1;
}

/*
 * int32_t file_open(const uint8_t* filename)
 * input: 	filename -- name of the file
 * output: 	0 on success
 * 			-1 on failure
 */
int32_t file_open(const uint8_t* filename) {
	return 0;
}


/*
 * int32_t file_close(int32_t fd)
 * input: 	fd -- file descriptor
 * output: 	0 on success
 * 			-1 on failure
 */
int32_t file_close(int32_t fd) {
	return 0;

}

/*
 * int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry)
 * input:	fname -- the name of the file we are looking for
 * 			dentry -- the pointer used to pass the dentry struct back to caller
 * output: 	0 on success
 * 			-1 on failure
 * description: given a file name and return the dentry structure that corresponding to that file
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
	int i;
	for (i = 0; i < file_size; i++) { 
		//check whether there is filename the same as fname.
		uint8_t check_flag = strncmp((int8_t*)fname, (int8_t*)(bootblk_addr->dir_entry[i].file_name), MAX_FILENAME_LEN - 1); 
		if (!check_flag) {
			// copy filename and update descriptor
			strncpy((int8_t*)(dentry->file_name), (int8_t*)(fname), MAX_FILENAME_LEN);   
			dentry->file_type = bootblk_addr->dir_entry[i].file_type;
			dentry->inode_num = bootblk_addr->dir_entry[i].inode_num;
			return 0;
		}
	}
	return -1;   
}

/*
 * int32_t read_dentry_by_index(uint32_t index,dentry_t* dentry)
 * input: 	index -- index of the file we are looking for
 * 			dentry -- the pointer used to pass the dentry struct back to caller
 * output: 	0 on success
 * 			-1 on failure
 * description: given a file index and return the dentry structure that corresponding to that file
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
	// sanity check
	if (index >= file_size || bootblk_addr->dir_entry[index].file_type != REGULAR_FILE)  
		return -1;
	// copy filename and update descriptor
	strncpy((int8_t*)(dentry->file_name), (int8_t*)(bootblk_addr->dir_entry[index].file_name), MAX_FILENAME_LEN);
	dentry->file_type = bootblk_addr->dir_entry[index].file_type;
	dentry->inode_num = bootblk_addr->dir_entry[index].inode_num;
	return 0;
}


/* int32_t read_data(uint32_t inode,uint32_t offset, uint32_t* buf,uint32_t length)
 * Discription: read bytes from the given file
 * output: 	0 on success
 * 			-1 on failure
 * Input: 	inode -- inode index, offset : start of the file
 *			buf -- store the read , length: the length of bytes from the file.
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	inode_t* curr_inode = (inode_t*)(inode_start_num + inode * BLOCK_SIZE);
	uint32_t i, read_len_each_time, block_index, byte_already_read = 0;
	// sanity check!!!!
	if (offset >= curr_inode->data_length)   
		return 0;
	if (inode >= inode_num_loc)  
		return -1;
	if ((offset + length) >= curr_inode->data_length) 
		length = curr_inode->data_length - offset;

	// read data block by block
	for (i = offset / BLOCK_SIZE ; i <= (offset + length) / BLOCK_SIZE ; i++)
	{	
		// find the right index
		read_len_each_time = BLOCK_SIZE - (offset % BLOCK_SIZE);
		//read_len_each_time = BLOCK_SIZE - offset;
		block_index = curr_inode->data_index[i];
		if (block_index >= data_block_loc) 
			// here we need to check!!
			return -1;
		if (((offset % BLOCK_SIZE) + length) < BLOCK_SIZE) 
			read_len_each_time = length;
		// copy data
		memmove(buf, (uint8_t*)(data_block_begin + block_index * BLOCK_SIZE + (offset % BLOCK_SIZE)), read_len_each_time); 
		// update descriptors
		offset += read_len_each_time;   
		buf += read_len_each_time;   
		length -= read_len_each_time;
		byte_already_read += read_len_each_time;
	}
	return byte_already_read;
}


/*
 * void test_read_file(void)
 * input:	void
 * output:	void
 * description: test the filesystem functions and print out the
 * 				information about the file system
 */
 /*
void test_read_file(void) {
	clear();
	printf("reading file test_case ...\n");
	dentry_t test_dentry; //declare a dentry to fetch result from read file function
	uint8_t buffer[6000];
	uint8_t file_name_buffer[32];
	if (read_dentry_by_name((uint8_t* )"ls", &test_dentry) == -1) //check if this file exist
	{
		printf("invalid read\n");
		return;
	}
	printf("file type is %d\n", test_dentry.file_type); //print file type
	printf("file inode number is %d\n", test_dentry.inode_index); //print file inode number

	//read data in the data block
	if (read_data(test_dentry.inode_index, 0, buffer, 6000) <= 0) {
		printf("invalid read_data or end of file!\n");
		return;
	}
	printf("\nthe file length is %d\n", ((inode_t *)(inode_start + test_dentry.inode_index * (4 * KB)))->length);
	int i;
	//print data in the file
	for (i = 0; i < ((inode_t *)(inode_start + test_dentry.inode_index * (4 * KB)))->length; i++) {
		printf("%c", buffer[i]);
	}

	//traverse all the file in file system and print out their names
	for (i = 0; i < 64; i++) {
		test_dentry = boot_pointer -> dentry [i];
		strncpy((int8_t *)file_name_buffer, (int8_t *)test_dentry.file_name, 32);
		printf("file name %s    ", file_name_buffer);
	}

	//print out the data length
	printf("read_data success!");
	return ;
}
*/

