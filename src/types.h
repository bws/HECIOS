
typedef int mpiDatatype; /* for now indicates number of bytes */

typedef long long fsHandle;

typedef struct fsMetaData
{
   int mode;
   int owner;
   int group;
   int nlinks;
   int size;
   int meta_handle;
   vector<int> handles; /* size of handles is server count */
   int dist; /* for now just strip size in bytes */
} fsMetaData;
                                                                                
typedef struct fsOpenFile
{
   fileSystem fs;
   int handle;
   int state;
   fsMetaData meta;
   int fileptr;
} fsOpenFile;

