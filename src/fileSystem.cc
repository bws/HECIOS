#include <map>
#include "types.h"

typedef struct attrEntry
{
	fsMetaData meta;
	simtime_t time_stamp;
	list<fsHandle>::iterator refLRU;
} attrEntry;

typedef struct dirEntry
{
	fsHandle handle;
	simtime_t time_stamp;
	list<string>::iterator refLRU;
} attrEntry;

typedef struct handleRange
{
	int first;
	int last;
} handleRange;

class fileSystem
{
	private:

		/* attribute (metadata) cache */
		hash_map<fsHandle, attrEntry> attrCache;
		maxAttr = 100; /* max number of attr cache entries */
		maxAttrTime = 100.0; /* max seconds before attr entry times out */
		list<fsHandle> attrLRU;

		/* directory (name) cache */
		hash_map<string, fshandle> dirCache;
		maxDir = 100; /* max number of dir cache entries */
		maxDirTime = 100.0; /* max seconds before dir entry times out */
		list<fsHandle> dirLRU;

		/* servers */
		vector<handleRange> servers;

	public:

		fileSystem fileSystem()
		{
			return *this;
		}

		void ~fileSystem()
		{
			attrCache->clear();
			attrLRU->clear();
			dirCache->clear();
			dirLRU->clear();
		}

		bool fsServerNotUsed(int snum, int dist, int count, mpiDatatype)
		{
			/* for now let all servers participate */
			return false;
		}

		void insertAttr(fsHandle handle, fsMetaData meta)
		{
			pair<fsHandle, attrEntry> entry;
			pair<iterator, bool> retval;
			entry.second = new attrEntry;
			attrLRU.push_front(handle);
			entry.second->refLRU = attrLRU.begin();
			entry.second->meta = meta;
			entry.second->time_stamp = simulation->simTime();
			entry.first = handle;
			retval = attrCache.insert(entry);
			if (!retval.second)
			{
				/* already existed in map */
				attrLRU.erase(entry.second->refLRU);
			}
		}

		fsMetaData lookupAttr(fsHandle handle)
		{
			pair<fsHandle, attrEntry> entry;
			entry = attrCache.find(handle);
			if (entry == attrCache.end())
			{
				/* not found */
				return NULL;
			}
			attrLRU.erase(entry.second->refLRU);
			if (entry.second->time_stamp < simulation->simTIme - maxAttrTime)
			{
				/* entry timed out */
				attrCache.erase(handle);
				return NULL;
			}
			/* return to front of LRU list */
			attrLRU.push_front(handle);
			entry.second.refLRU = attrLRU.begin();
			return entry.second->meta;
		}

		void removeAttr(fsHandle handle)
		{
			pair<fsHandle, attrEntry> entry;
			entry = attrCache.find(handle);
			if (entry == attrCache.end())
			{
				/* not found */
				return;
			}
			attrLRU.erase(entry.second->refLRU);
			delete entry.second;
			attrCache.erase(handle);
		}
	
		void insertDir(string path, fsHandle handle)
		{
			pair<string, dirEntry> entry;
			pair<iterator, bool> retval;
			entry.second = new dirEntry;
			dirLRU.push_front(path);
			entry.second->refLRU = dirLRU.begin();
			entry.second->time_stamp = simulation->simTime();
			entry.second->handle = handle;
			entry.first = string;
			dirCache.insert(entry);
			if (!retval.second)
			{
				/* already existed in map */
				dirLRU.erase(entry.second->refLRU);
			}
		}

		fsHandle lookupDir(string path)
		{
			pair<string, dirEntry> entry;
			entry = dirCache.find(path);
			if (entry == dirCache.end())
			{
				/* not found */
				return NULL;
			}
			dirLRU.erase(entry.second->refLRU);
			if (entry.second->time_stamp < simulation->simTIme - maxDirTime)
			{
				/* entry timed out */
				dirCache.erase(path);
				return NULL;
			}
			/* return to front of LRU list */
			dirLRU.push_front(path);
			entry.second.refLRU = dirLRU.begin();
			return entry.second->handle;
		}

		void removeDir(string path)
		{
			pair<string, dirEntry> entry;
			entry = attrCache.find(path);
			if (entry == attrCache.end())
			{
				/* not found */
				return;
			}
			attrLRU.erase(entry.second->refLRU);
			delete entry.second;
			dirCache.erase(path);
		}
};
