#ifndef __DFS_SHARED__
#define __DFS_SHARED__

#include "files_shared.h"

typedef struct dfs_superblock {
  // STUDENT: put superblock internals here
  int valid; // If the super block is valid
  //int dfs_disksize; // Total disk size
  int dfs_blocksize; // Virtual block size
  int dfs_blocknum;  // Total number of file system blocks
  int dfs_inode_start; // Virtual block inodes start at
  int dfs_num_inodes; // Number of available inodes in pool
  int dfs_fbv_start; // Starting virtual block number of free block vector
} dfs_superblock;

#define DFS_BLOCKSIZE 1024  // Must be an integer multiple of the disk blocksize

typedef struct dfs_block {
  char data[DFS_BLOCKSIZE];
} dfs_block;

typedef struct dfs_inode {
  // STUDENT: put inode structure internals here
  // IMPORTANT: sizeof(dfs_inode) MUST return 84 in order to fit in enough
  // inodes in the filesystem (and to make your life easier).  To do this, 
  // adjust the maximumm length of the filename until the size of the overall inode 
  // is 84 bytes.
  uint32 max_size; // Size of file inode represents //4 bytes
  char filename[FILE_MAX_FILENAME_LENGTH]; // Filename of inode //84 - (16 + 40) = 28
  uint32 vblock_table[10]; // Data block table //40 bytes
  uint32 vblock_index; // Indirect address block //4 bytes
  uint32 inuse; // Marks if inode is currently being used //4 bytes
  uint32 vblock_index_double; //4 bytes
} dfs_inode;

#define DFS_MAX_FILESYSTEM_SIZE 0x10000000  // 256MB

#define DFS_FAIL -1
#define DFS_SUCCESS 1

#define FDISK_NUM_INODES      512 
#define DISK_BLOCKSIZE  256
#define DFS_FBV_MAX_NUM_WORDS ((DFS_MAX_FILESYSTEM_SIZE/ DFS_BLOCKSIZE)/32)

#endif
