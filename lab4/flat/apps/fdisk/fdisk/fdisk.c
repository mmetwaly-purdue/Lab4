#include "usertraps.h"
#include "misc.h"
#include "fdisk.h"
#include "dfs_shared.h"

dfs_superblock sb;
dfs_inode inodes[FDISK_NUM_INODES];
uint32 fbv[DFS_FBV_MAX_NUM_WORDS];

int diskblocksize = 0; // These are global in order to speed things up
int disksize = 0;      // (i.e. fewer traps to OS to get the same number)

int FdiskWriteBlock(uint32 blocknum, dfs_block *b); //You can use your own function. This function 
//calls disk_write_block() to write physical blocks to disk

void main (int argc, char *argv[])
{
	// STUDENT: put your code here. Follow the guidelines below. They are just the main steps. 
	// You need to think of the finer details. You can use bzero() to zero out bytes in memory
  int i;
  int j;
  int num_filesystem_blocks;
  dfs_inode cur;
  dfs_block* ptr_dfs_block; 
  int fs_data_start;
  uint32 os[64];

  // Need to invalidate filesystem before writing to it to make sure that the OS
  // doesn't wipe out what we do here with the old version in memory
  // You can use dfs_invalidate(); but it will be implemented in Problem 2. You can just do 
  // sb.valid = 0
  //superblock setup

  diskblocksize = 1024; 
  disksize = 256 * 1024 * 1024;
  num_filesystem_blocks = disksize / diskblocksize;

  sb.valid = 0;
  sb.dfs_blocksize = diskblocksize;
  sb.dfs_blocknum = num_filesystem_blocks;
  sb.dfs_inode_start = 1;
  sb.dfs_num_inodes = 512;
  sb.dfs_fbv_start = 44;

  // Make sure the disk exists before doing anything else
  if (disk_create() == DISK_FAIL) {
    Printf("Fatal Error: Disk Creation fail!\n");
    return;
  }
  Printf("Disk Create Succeed!\n");

  // Write all inodes as not in use and empty (all zeros)
  // Next, setup free block vector (fbv) and write free block vector to the disk
  // Finally, setup superblock as valid filesystem and write superblock and boot record to disk: 
  // boot record is all zeros in the first physical block, and superblock structure goes into the second physical block
  
  //Write all inodes as not in use and empty (all zeros)

  //step1 write and store all inode
  Printf("Inode size is: %d\n", sizeof(dfs_inode));
  Printf("Printing FDISK_NUM_INODES  %d\n",FDISK_NUM_INODES);
  for (i = 0; i < FDISK_NUM_INODES; i++) {
    cur = inodes[i];
    cur.inuse = 0;
    cur.max_size = 0;
    cur.filename[0] = '\0';
    for (j = 0; j < 10; j++) {
      cur.vblock_table[j] = UNINIT;
    }
    cur.vblock_index = UNINIT;
    cur.vblock_index_double = UNINIT;
  }

  ptr_dfs_block = (dfs_block *) (&inodes);
  for (i = 1; i < FDISK_INODE_BLOCK_START; i++) {
    if (FdiskWriteBlock(i, ptr_dfs_block + (i - FDISK_INODE_BLOCK_START)) == DISK_FAIL) {
      Printf("Fatal Error: FdiskWriteBlock fail\n");
      return;
    }
  }

  //step2 write and store fbv
  //all block before data block are used for file system meta-data
  //0 - used, 1 - free
  fs_data_start = dataStartBlock();
  Printf("data block start is: %d\n", fs_data_start);

  //1 fbv integer represent 32 blocks, 131072 fs blocks need DFS_FBV_MAX_NUM_WORDS of fbv integers 4096 fbv
  //1 int is 4 byte, total byte is 8192 * 4 = 32768 byte
  //32768 byte / 1024 byte per fs block = 32 fs blocks
  Printf("sizeof fbv is = %d\n",sizeof(fbv));
  Printf("DFS_FBV_MAX_NUM_WORDS is %d\n" , DFS_FBV_MAX_NUM_WORDS);

  for (i = 3; i < DFS_FBV_MAX_NUM_WORDS; i++) {
    //Printf("Illegal access happens here at: %d\n", i); //at iteration 2414
    fbv[i] = NOT_USE_MASK;
  }

  Printf("I come after the illegal access print\n");
  for (i = 0; i < fs_data_start / 32; i++) {
    fbv[i] = IN_USE_MASK;
  }
  fbv[fs_data_start / 32] = (NOT_USE_MASK << (fs_data_start % 32)) & fbv[fs_data_start / 32];
  //fbv[fs_data_start / 32] = NOT_USE_MASK;
  //Printf("Hello World\n");
  //Printf("flag is %d\n", (NOT_USE_MASK << (fs_data_start % 32)));
  //Printf("fbv end is: %d\n", fbv[fs_data_start / 32]);
  
  ptr_dfs_block = (dfs_block *) fbv;
  for (i = FDISK_FBV_BLOCK_START; i < fs_data_start; i++) {
    if (FdiskWriteBlock(i, ptr_dfs_block) == DISK_FAIL) {
      Printf("Fatal Error: FdiskWriteBlock fail\n");
      return;   
    }
    ptr_dfs_block = ptr_dfs_block + 1;
  }

  //step3 
  // setup superblock as valid filesystem and write superblock and boot record to disk: 
  // boot record is all zeros in the first physical block, and superblock structure goes into the second physical block
  //Write OS 256 bytes -> 64 int array

  for (i = 0; i < 64; i++) {
    os[i] = 0;
  }
  if (disk_write_block(0, (char *)os) == DISK_FAIL) {
    Printf("Fatal Error: OS init fail\n");
    return;
  }

  //Write superblock
  sb.valid = 1;
  if (disk_write_block(1, (char *)&sb) == DISK_FAIL) {
    Printf("Fatal Error: Superblock init fail\n");
    return;
  }

  Printf("fdisk (%d): Formatted DFS disk for %d bytes.\n", getpid(), disksize);
}

int FdiskWriteBlock(uint32 blocknum, dfs_block *b) {
  // STUDENT: put your code here
  int dbsize = disk_blocksize();
  // buffer for storing the data converted from dfs_block to char
  //phsical/virtual block size ratio
  uint32 pvr = DFS_BLOCKSIZE/dbsize;
  uint32 diskblock_idx;
  int i;
  char* addr = (char*) b;
 
  for(i=0; i<pvr; i++){
    // compute the corresponding physical disk number
    diskblock_idx = pvr * blocknum + i;
    if(disk_write_block(diskblock_idx, addr) == DISK_FAIL)
    {   
        Printf("  ERROR: fail to write in physdisk block %d!\n");
        return DISK_FAIL;
    }
    addr += dbsize;
  }
  return DISK_SUCCESS;

}

int fdiskFactor() {
  if (sb.dfs_blocksize % DISK_BLOCKSIZE != 0) {
    Printf("FATAL ERROR: file system disk zize not a multiple of physical disk size\n");
  }
  return sb.dfs_blocksize / DISK_BLOCKSIZE;
}

int fsBlockStartToPhysicalBlockStart(int fs_block_start) {
  return fdiskFactor() * fs_block_start;
}

int dataStartBlock() {
  //256 * 1024 * 1024 / 1024 = 262144 fs blocks
  //fbv: 8192 words
  //1024 per block -> 32 word per block
  //512 blocks total needed for fbv??
  Printf("Printing return value from fbv %d  \n" , fbvRequiredBlocks());
  Printf("FDISK_FBV_BLOCK_START %d \n" , FDISK_FBV_BLOCK_START);
  return FDISK_FBV_BLOCK_START + fbvRequiredBlocks() ;
}

int fbvRequiredBlocks() { // 32 fs blocks
  if (DFS_FBV_MAX_NUM_WORDS * 4 % 1024 != 0) {
    Printf("Fatal Error: FBV is not dividable\n");
  }
  if (DFS_FBV_MAX_NUM_WORDS * 4 / 1024 != 32) { //each word is 4 byte
    Printf("Fatal Error: FBV not take 256 blocks as we expected\n");
  }
  return DFS_FBV_MAX_NUM_WORDS * 4 / 1024;
}
