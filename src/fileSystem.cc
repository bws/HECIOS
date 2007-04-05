#include <map>
#include <pfs_types.h>

typedef struct attrEntry
{
	FSMetaData meta;
	simtime_t time_stamp;
	list<FSHandle>::iterator refLRU;
} attrEntry;

typedef struct dirEntry
{
	FSHandle handle;
	simtime_t time_stamp;
	list<string>::iterator refLRU;
} attrEntry;

class fileSystem
{
	private:

		/* attribute (metadata) cache */
		hash_map<FSHandle, attrEntry> attrCache;
		maxAttr = 100; /* max number of attr cache entries */
		maxAttrTime = 100.0; /* max seconds before attr entry times out */
		list<fsHandle> attrLRU;

		/* directory (name) cache */
		hash_map<string, FSHandle> dirCache;
		maxDir = 100; /* max number of dir cache entries */
		maxDirTime = 100.0; /* max seconds before dir entry times out */
		list<FSHandle> dirLRU;

		/* servers */
		int totalNumServers = 64;
		int defaultNumServers = 16;

	public:

		vector<struct HandleRange> servers;

		int root = 0; /* index of server holding the root directory */

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

		int getTotalNumServers()
		{
			return totalNumServers;
		}

		int getDefaultNumServers()
		{
			return defaultNumServers;
		}

		bool fsServerNotUsed(int snum, int dist, int count, mpiDatatype)
		{
			/* for now let all servers participate */
			return false;
		}

		int selectServer()
		{
			/* called during create to select servers for new file */
			/* randomly selects a server from 0 to S-1 where S is */
         /* totalNumServers */
			return 0;
		}

		int fsHashPath(string path)
		{
			/* hashes path toa number from 0 to S-1 where S is */
			/* totalNumServers */
			return 0;
		}

		void insertAttr(FSHandle handle, FSMetaData meta)
		{ /* Insert Attributes into Cache {{{1 */
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
		} /* }}}1 */

		FSMetaData lookupAttr(FSHandle handle)
		{ /* Look up directory in cache {{{1 */
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
		} /* }}}1 */

		void removeAttr(FSHandle handle)
		{ /* Remove attributes from cache {{{1 */
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
		} /* }}}1 */
	
		void insertDir(string path, FSHandle handle)
		{ /* Insert directory into cache {{{1 */
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
		} /* }}}1 */

		fsHandle lookupDir(string path)
		{ /* Look up directory in cache {{{1 */
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
		} /* }}}1 */

		void removeDir(string path)
		{ /* Remove directory from cache {{{1 */
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
		} /* }}}1 */
};

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */

