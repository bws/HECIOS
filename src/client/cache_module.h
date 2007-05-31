#ifndef CACHE_MODULE_H
#define CACHE_MODULE_H

// Name: 
// File: cache_module.h

//int dataCacheReplacePolicy;


enum dataReplaceTypes
{

	DATA_CACHE_LRU_POLICY = 0,
	DATA_CACHE_FIFO_POLICY

} dataCacheReplacePolicy;


#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
