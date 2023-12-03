#ifndef __DFS_H__
#define __DFS_H__

#include "dfs_shared.h"
#include "files_shared.h"

void DfsModuleInit(); //Task 1 & 2
int DfsOpenFileSystem(); //Task 2
void DfsInvalidate(); //Task 2
int DfsCloseFileSystem(); //Task 2

//DFS Layer
uint32 DfsAllocateBlock(); //Task2
int DfsFreeBlock(uint32 blocknum); //Task 2
int DfsReadBlock(uint32 blocknum, dfs_block *b); //Task 2
int DfsWriteBlock(uint32 blocknum, dfs_block *b); //Task 2

//Inode Layer
uint32 DfsInodeFilenameExists(char *filename); //Task 3
uint32 DfsInodeOpen(char *filename); //Task 3
int DfsInodeDelete(uint32 handle); //Task 3
int DfsInodeReadBytes(uint32 handle, void *mem, int start_byte, int num_bytes); //Task 3
int DfsInodeWriteBytes(uint32 handle, void *mem, int start_byte, int num_bytes); //Task 3
uint32 DfsInodeFilesize(uint32 handle); //Task 3
uint32 DfsInodeAllocateVirtualBlock(uint32 handle, uint32 virtual_blocknum); //Task 3
uint32 DfsInodeTranslateVirtualToFilesys(uint32 handle, uint32 virtual_blocknum); //Task 3

//DFS Related Function
void DfsValidate();

int readSuperBlock();
int readInode();
int readFBV();
int writeBackSuperBlock();
int fbvRequiredBlocks();

int writeBackInode();
int writeBackFBV();

//Multiple Helper Functions
//DFS Layer
int fdiskFactor(); //at our lab, this is 2, 512MB(fs) / 256MB(physical)
int fsBlockStartToPhysicalBlockStart(int fs_block_start); //as name suggested
int dataStartBlock();
int physicalToFsdisk(uint32 blocknum); //input: a physical block, output: start of fs block index
//Inode Layer
int numInuseInode();
int deleteAllInode();
int inodeMaxBlocks();
int atDirect(int vblock); //return true(1) if the virtual block is direct block
int atSingleIndirect(int vblock); //return true(1) if the virtual block is single direct block
int total_direct_blocks();
int total_indirect_blocks();
int total_double_indirect_blocks();
int get_level1_index_for_double_indirect(int vblock);
int get_level2_index_for_double_indirect(int vblock);

#define SUPER_BLOCK_PHYSICAL_DISK 1
#define UNINIT 0
#endif
