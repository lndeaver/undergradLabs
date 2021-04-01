/*
  Filename   : fs.c
  Author     : Lauren Deaver
  Course     : CSCI 380-01
  Assignment : Assignment 9: File Systems
  Description: write a UNIX-like file system based on
    topics covered in class - a lot of implementation here
    was spelled out in the sudo code comments for each function
*/


#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>


#define FS_NUM_BLOCKS    128
#define FS_MAX_FILE_SIZE 8
#define FS_MAX_FILENAME  16
#define FS_MAX_FILES     16
#define FS_BLOCK_SIZE    1024
#define CMD_LENGTH       32

struct fs_t
{
  int fd;
};

struct inode {
  char name[16];        //file name
  int size;             // file size (in number of blocks)
  int blockPointers[8]; // direct block pointers
  int used;             // 0 => inode is free; 1 => in use
};

// open the file with the above name
void
fs_open (struct fs_t *fs, char diskName[16])
{
 if((fs->fd = open (diskName, O_RDWR)) < 0)
 {
   printf ("Filesystem could not be opened... Exiting\n");
   exit(1);
 }
 printf("'%s' initialized\n", diskName);
}

// close and clean up all associated things
void
fs_close (struct fs_t *fs)
{
  close (fs->fd);
  printf("File closed\n");
}

// create a file with this name and this size
void
fs_create (struct fs_t *fs, char name[16], int size)
{

  if (size > 8)
  {
    printf("file size too big\n");
    return;
  }
  // high level pseudo code for creating a new file

  // Step 1: check to see if we have sufficient free space on disk by
  // reading in the free block list. To do this:
  // - move the file pointer to the start of the disk file.
  lseek(fs->fd, 0, SEEK_SET);
  // - Read the first 128 bytes (the free/in-use block information)
  char freeBlock[128];
  read(fs->fd, freeBlock, 128);



  // - Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size
  /* do some kind of loop with what we read in from the block to
  * check and see if theres anything on the "disk" that has an 
  * unallocated slot */
  int count = 0;
  for (int i = 0; i < 128; ++i)
  {
    if (freeBlock[i] == 0)
    {
      ++count;
    }
  }

  if (size > count)
  {
    printf("not enough free space\n");
    return;

  }

  // Step 2: we look  for a free inode on disk
  // - Read in a inode
  struct inode node;
  /* loop through and find the first inode that not allocated */
  lseek(fs->fd, 128, SEEK_SET);
  // - check the "used" field to see if it is free
  int index = -1;
  for (int i = 0; i < 16; ++i)
  {
    if (!strcmp(node.name, name))
    {
      printf("file name cant be same as another");
      return;
    }
    read (fs->fd, &node, sizeof(node));
    if (node.used == 0)
    {
      index = i;
      break;
    }
  }

  if (index < 0)
  {
    printf("no free nodes found");
    return;
  }

  // - If not, repeat the above two steps until you find a free inode

  // - Set the "used" field to 1
  node.used = 1;
  /*found a free inode so change its used part so we can put in */
  // - Copy the filename to the "name" field
  strncpy (node.name, name, 16);
  // - Copy the file size (in units of blocks) to the "size" field
  node.size = size;


  // Step 3: Allocate data blocks to the file
  // - Scan the block list that you read in Step 1 for a free block

  // - Once you find a free block, mark it as in-use (Set it to 1)
  // - Set the blockPointer[i] field in the inode to this block number.
  // - repeat until you allocated "size" blocks
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < 128; ++j)
    {
      if (freeBlock[j] == 0)
      {
        freeBlock[j] = 1;
        node.blockPointers[i] = j;
        break;
      }

    }
  }

  // Step 4: Write out the inode and free block list to disk
  // - Move the file pointer to the start of the file
  lseek (fs->fd, 0, SEEK_SET);
  // - Write out the 128 byte free block list
  write (fs->fd, freeBlock, 128);
  // - Move the file pointer to the position on disk where this inode was stored
  lseek(fs->fd, 128 + (56 * index), SEEK_SET);
  // - Write out the inode
  write(fs->fd, &node, sizeof(node));
}

// Delete the file with this name
void
fs_delete (struct fs_t *fs, char name[16])
{
  // Step 1: Locate the inode for this file
  //   - Move the file pointer to the 1st inode (129th byte)
  lseek (fs->fd, 128, SEEK_SET);
  //   - Read in a inode
  struct inode node;
  int foundIndex = -1;
  
  for (int i = 0; i <= 16; ++i)
  {
    if (i == 16)
    {
      printf("file not found");
    }
    read(fs->fd, &node, sizeof(node));
    if (node.used == 1)
    {
      if (!strcmp(node.name, name))
      {
        foundIndex = i;
        break;
      }
    }
  }
  //   - If the iinode is free, repeat above step.
  //   - If the iinode is in use, check if the "name" field in the
  //     inode matches the file we want to delete. IF not, read the next
  //     inode and repeat
  if (foundIndex < 0)
  {
    printf("file not found for deletion\n");
    return;
  }

  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  //   of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields
  char freeBlocks[128];
  lseek(fs->fd, 0, SEEK_SET);
  read(fs->fd, freeBlocks, 128);
  for (int i = 0; i < node.size; ++i)
  {
    freeBlocks[node.blockPointers[i]] = 0;
  }

  // Step 3: mark inode as free
  // Set the "used" field to 0.
  node.used = 0;

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  lseek(fs->fd, 0, SEEK_SET);
  // Write out the 128 byte free block list
  write(fs->fd, freeBlocks, 128);
  // Move the file pointer to the position on disk where this inode was stored
  lseek(fs->fd, 128 + (56 * foundIndex), SEEK_SET);
  // Write out the inode
  write(fs->fd, &node, sizeof(node));
}

// List names of all files on disk
void
fs_ls (struct fs_t *fs)
{
  // Step 1: read in each inode and print!
  // Move file pointer to the position of the 1st inode (129th byte)
  // for each inode:
  //   read it in
  //   if the inode is in-use
  //     print the "name" and "size" fields from the inode
  // end for
  lseek(fs->fd, 128, SEEK_SET);
  struct inode node;
  for (int i = 0; i < 16; ++i)
  {
    read(fs->fd, &node, sizeof(node));
    if (node.used == 1)
    {
      printf("%16s %6dB\n", node.name, node.size*1024);
    }
  }
}

// read this block from this file
void
fs_read (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{
  // Step 1: locate the inode for this file
  //   - Move file pointer to the position of the 1st inode (129th byte)
  //put after block list
  lseek(fs->fd, 128, SEEK_SET);
  //allocating block on stack that is empty but is perfect
  //size to fill in node there
  struct inode node;
  //   - Read in a inode
  for (int i = 0; i <= 16; ++i)
  {
    if (i == 16)
    {
      printf("couldn't find\n");
      return;
    }
    read (fs->fd, &node, sizeof(node));
    //   - If the inode is in use, compare the "name" field with the above file
    if (node.used == 1)
    {
      if (!strcmp(node.name, name))
      {
        if (blockNum >= node.size)
        {
          printf("whoops thats a bad blockNum size\n");
          return;
        }
        int actualBlockNum = node.blockPointers[blockNum];
        lseek(fs->fd, actualBlockNum * FS_BLOCK_SIZE, SEEK_SET);
        read(fs->fd, buf, FS_BLOCK_SIZE);
        break;
      }
    }
    lseek(fs->fd, sizeof(node), SEEK_CUR);
    //   - If the file names don't match, repeat
  }
  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // move the file pointer to the block location
  // Read in the block! => Read in 1024 bytes from this location into the buffer
  // "buf"
}

// write this block to this file
void
fs_write (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{
  lseek(fs->fd, 128, SEEK_SET);
  //size to fill in node there
  struct inode node;
  //   - Read in a inode
  for (int i = 0; i <= 16; ++i)
  {
    if (i == 16)
    {
      printf("couldn't find\n");
      return;
    }
    read (fs->fd, &node, sizeof(node));
    //   - If the inode is in use, compare the "name" field with the above file
    if (node.used == 1)
    {
      if (!strcmp(node.name, name))
      {
        if (blockNum >= node.size)
        {
          printf("whoops thats a bad blockNum size\n");
          return;
        }
        int actualBlockNum = node.blockPointers[blockNum];
        lseek(fs->fd, actualBlockNum * FS_BLOCK_SIZE, SEEK_SET);
        write(fs->fd, buf, FS_BLOCK_SIZE);
        break;
      }
    }
    lseek(fs->fd, sizeof(node), SEEK_CUR);
    //   - If the file names don't match, repeat
  }
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat

  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // move the file pointer to the block location
  // Write the block! => Write 1024 bytes from the buffer "buf" to this location
}

// REPL entry point
void
fs_repl ()
{
  //get and open disk name
  struct fs_t fs = {.fd = 0};
  char diskName[16];
  fgets(diskName, sizeof (diskName), stdin);
  strtok(diskName, "\n");
  fs_open (&fs, diskName);

  //create dummy buffer of 1's
  char dummybuff[FS_BLOCK_SIZE];
  for (int i = 0; i < FS_BLOCK_SIZE; ++i)
  {
    dummybuff[i] = 1;
  }

  char command[CMD_LENGTH];
  while (fgets (command, sizeof (command), stdin))
  {
    strtok (command, "\n");
    int size;
    char str[16];

    if (sscanf (command, "C %16s %d", str, &size) == 2)
    {
      printf ("Creating file %s\n", str);
      fs_create(&fs, str, size);

    }
    else if (sscanf (command, "D %16s", str) == 1)
    {
      printf ("Deleting file %s\n", str);
      //fs_delete
    }
    else if (strcmp (command, "L") == 0)
    {
      printf ("Listing all files:\n");
      fs_ls(&fs);
    }
    else if (sscanf (command, "R %16s %d", str, &size) == 2)
    {
      printf ("Reading file %s\n", str);
      fs_read(&fs, str, size, dummybuff);
    }
    else if (sscanf (command, "W %16s %d", str, &size) == 2)
    {
      printf ("Writing file %s\n", str);
      fs_write(&fs, str, size, dummybuff);
    }
    else
    {
      printf ("Invalid command\n");
    }
  }
}
