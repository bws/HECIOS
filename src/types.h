
typedef int mpiDatatype; /* for now indicates number of bytes */

typedef long long fsHandle;

typedef struct fsMetaData
{
   int mode;            /* standard Posix file metadata */
   int owner;
   int group;
   int nlinks;
   int size;            /* number of bytes in file */
   int meta_handle;     /* handle of the metadata object */
   vector<int> handles; /* size of handles is server count */
   int dist;            /* for now just strip size in bytes */
} fsMetaData;
                                                                                
typedef struct fsOpenFile
{
   fileSystem fs;
   int handle;          /* hand of the file - the metadata object */
   fsMetaData meta;
   int fileptr;         /* offset of current position in file */
} fsOpenFile;

typedef struct handleRange
{
   fsHandle first;
   fsHandle last;
} handleRange;

