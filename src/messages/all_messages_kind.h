//
//
//

// Enumeration of all possible simulator message kinds (i.e. the possible
// message types
enum AllMessageKind = {
    // Null Message Type
    NULL_MESSAGE_KIND = 0,

    // PVFS Request Protocol Messages
    FS_CREATE_REQUEST_KIND,
    FS_REMOVE_REQUEST_KIND,
    FS_READ_REQUEST_KIND,
    FS_WRITE_REQUEST_KIND,
    FS_GET_ATTR_REQEUST_KIND,
    FS_SET_ATTR_REQUEST_KIND,
    FS_LOOKUP_PATH_REQUEST_KIND,
    FS_CREATE_DIR_ENT_REQUEST_KIND,
    FS_REMOVE_DIR_ENT_REQUEST_KIND,
    FS_CHANGE_DIR_ENT_REQUEST_KIND,
    FS_TRUNCATE_REQUEST_KIND,
    FS_MAKE_DIR_REQUEST_KIND,
    FS_READ_DIR_REQUEST_KIND,
    FS_FLUSH_REQUEST_KIND,
    FS_STAT_REQUEST_KIND,
    FS_LIST_ATTR_REQUEST_KIND,
    
    // PVFS Response Protocol Messages
    FS_CREATE_RESPONSE_KIND,
    FS_REMOVE_RESPONSE_KIND,
    FS_READ_RESPONSE_KIND,
    FS_WRITE_RESPONSE_KIND,
    FS_GET_ATTR_REQEUST_KIND,
    FS_SET_ATTR_RESPONSE_KIND,
    FS_LOOKUP_PATH_RESPONSE_KIND,
    FS_CREATE_DIR_ENT_RESPONSE_KIND,
    FS_REMOVE_DIR_ENT_RESPONSE_KIND,
    FS_CHANGE_DIR_ENT_RESPONSE_KIND,
    FS_TRUNCATE_RESPONSE_KIND,
    FS_MAKE_DIR_RESPONSE_KIND,
    FS_READ_DIR_RESPONSE_KIND,
    FS_WRITE_COMPLETION_RESPONSE_KIND,
    FS_FLUSH_RESPONSE_KIND,
    FS_STAT_RESPONSE_KIND,
    FS_LIST_ATTR_RESPONSE_KIND,

    // MPI-IO Messages
    MPI_FILE_OPEN_REQUEST_KIND,
    MPI_FILE_OPEN_RESPONSE_KIND,
    MPI_FILE_CLOSE_REQUEST_KIND,
    MPI_FILE_CLOSE_RESPONSE_KIND,
    MPI_FILE_DELETE_REQUEST_KIND,
    MPI_FILE_DELETE_RESPONSE_KIND,
    MPI_FILE_SET_SIZE_REQUEST_KIND,
    MPI_FILE_SET_SIZE_RESPONSE_KIND,
    MPI_FILE_PREALLOCATE_REQUEST_KIND,
    MPI_FILE_PREALLOCATE_RESPONSE_KIND,
    MPI_FILE_GET_SIZE_REQUEST_KIND,
    MPI_FILE_GET_SIZE_RESPONSE_KIND,
    MPI_FILE_GET_INFO_REQUEST_KIND,
    MPI_FILE_GET_INFO_RESPONSE_KIND,
    MPI_FILE_SET_INFO_REQUEST_KIND,
    MPI_FILE_SET_INFO_RESPONSE_KIND,
    MPI_FILE_READ_AT_REQUEST_KIND,
    MPI_FILE_READ_AT_RESPONSE_KIND,
    MPI_FILE_READ_REQUEST_KIND,
    MPI_FILE_READ_RESPONSE_KIND,
    MPI_FILE_WRITE_AT_REQUEST_KIND,
    MPI_FILE_WRITE_AT_RESPONSE_KIND,
    MPI_FILE_WRITE_REQUEST_KIND,
    MPI_FILE_WRITE_RESPONSE_KIND,
    
};

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */