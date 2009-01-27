/* 
 * This file is part of Hecios
 *
 * Copyright (C) 2007 Brad Settlemyer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "lanl_trace_scan_actions.h"

/* Definition of bool for use in this translation unit only */
#define bool int
#define false 0
#define true !0

/* Global variables */
bool collectInput = false;
char* beginLine = 0;

/* Definitions controlling lex code generation */
#define YY_MAIN 0

/** Determine if the current state is correct */
void checkState(int currentState, const char* currentText)
{
	if (0 != currentState)
	{
		fprintf(stderr, "ERROR: MPI construct ending was not detected\n");
		fprintf(stderr, "\tProblem Text: %s\n", currentText);
		exit(237);
	}
}

%}
%option yylineno

/**
 * Note that we're using the paradigm of a begin match and end match (with
 * an accompanying start state) because Lex/Flex has exponential runtime
 * when I try to match the whole expression with a single regular expression.
 * It may be a fault with the particular regular expressions I am trying to
 * use, but this works well and is easy enough to extend.  Note that if
 * it doesn't find the correct end condition it will silently eat the call
 * it found.  A relatively serious limitation that needs to be worked on.
 */
%s allreduce
%s barrier
%s bcast
%s comm_create
%s comm_dup
%s comm_free
%s comm_rank
%s comm_size
%s file_close
%s file_delete
%s file_get_byte_offset
%s file_get_amode
%s file_get_info
%s file_get_group
%s file_get_position
%s file_get_size
%s file_iread
%s file_iread_at
%s file_iread_shared
%s file_iwrite
%s file_iwrite_at
%s file_iwrite_shared
%s file_open
%s file_read
%s file_read_all
%s file_read_at
%s file_read_at_all
%s file_read_shared
%s file_seek
%s file_seek_shared
%s file_set_size
%s file_write
%s file_write_all
%s file_write_at
%s file_write_at_all
%s file_write_shared
%s finalize
%s get_processor_name
%s info_create
%s info_dup
%s info_free
%s init
%s io_wait
%s wtime

WILDCARD .+

TIMESTAMP [0-9]+:[0-9]+:[0-9]+[.][0-9]+

MPI_ALLREDUCE_BEGIN ^.*\ MPI_Allreduce\(
MPI_ALLREDUCE_END ^.*MPI_Allreduce\ resumed.*\n

MPI_BARRIER_BEGIN ^.*\ MPI_Barrier\(
MPI_BARRIER_END ^.*MPI_Barrier\ resumed.*\n

MPI_BCAST_BEGIN ^.*\ MPI_Bcast\(
MPI_BCAST_END ^.*MPI_Bcast\ resumed.*\n

MPI_BCAST_IMMEDIATE ^.*\ MPI_Bcast\(.*\)\ =\ 0.*\n

MPI_COMM_CREATE_BEGIN ^.*\ MPI_Comm_create\(
MPI_COMM_CREATE_END ^.*MPI_Comm_create\ resumed.*\n

MPI_COMM_DUP_BEGIN ^.*\ MPI_Comm_dup\(
MPI_COMM_DUP_END MPI_Comm_dup\ resumed.*\n

MPI_COMM_FREE_BEGIN ^.*\ MPI_Comm_free\(
MPI_COMM_FREE_END \n

MPI_COMM_RANK_BEGIN ^.*\ MPI_Comm_rank\(
MPI_COMM_RANK_END ^.*MPI_Comm_rank\ resumed.*\n

MPI_COMM_SIZE_BEGIN ^.*\ MPI_Comm_size\(
MPI_COMM_SIZE_END \n

MPI_FILE_CLOSE_BEGIN ^.*\ MPI_File_close\(
MPI_FILE_CLOSE_END \n

MPI_FILE_DELETE_BEGIN ^.*\ MPI_File_delete\(
MPI_FILE_DELETE_END MPI_File_delete\ resumed.*\n

MPI_FILE_GET_AMODE_BEGIN ^.*\ MPI_File_get_amode\(
MPI_FILE_GET_AMODE_END MPI_File_get_amode\ resumed.*\n

MPI_FILE_GET_GROUP_BEGIN ^.*\ MPI_File_get_group\(
MPI_FILE_GET_GROUP_END MPI_File_get_group\ resumed.*\n

MPI_FILE_GET_INFO_BEGIN ^.*\ MPI_File_get_info\(
MPI_FILE_GET_INFO_END MPI_File_get_info\ resumed.*\n

MPI_FILE_GET_POSITION_BEGIN ^.*\ MPI_File_get_position\(
MPI_FILE_GET_POSITION_END MPI_File_get_position\ resumed.*\n

MPI_FILE_GET_SIZE_BEGIN ^.*\ MPI_File_get_size\(
MPI_FILE_GET_SIZE_END MPI_File_get_size\ resumed.*\n

MPI_FILE_OPEN_BEGIN ^.*\ MPI_File_open\(
MPI_FILE_OPEN_END MPI_File_open\ resumed.*\n

MPI_FILE_READ_BEGIN ^.*\ MPI_File_read\(
MPI_FILE_READ_END MPI_File_read\ resumed.*\n

MPI_FILE_READ_ALL_BEGIN ^.*\ MPI_File_read_all\(
MPI_FILE_READ_ALL_END MPI_File_read_all\ resumed.*\n

MPI_FILE_READ_AT_BEGIN ^.*\ MPI_File_read_at\(
MPI_FILE_READ_AT_END MPI_File_read_at\ resumed.*\n

MPI_FILE_READ_AT_ALL_BEGIN ^.*\ MPI_File_read_at_all\(
MPI_FILE_READ_AT_ALL_END MPI_File_read_at_all\ resumed.*\n

MPI_FILE_SET_SIZE_BEGIN ^.*\ MPI_File_set_size\(
MPI_FILE_SET_SIZE_END MPI_File_set_size\ resumed.*\n

MPI_FILE_SEEK_BEGIN ^.*\ MPI_File_seek\(
MPI_FILE_SEEK_END \n

MPI_FILE_WRITE_BEGIN ^.*\ MPI_File_write\(
MPI_FILE_WRITE_END MPI_File_write\ resumed.*\n

MPI_FILE_WRITE_ALL_BEGIN ^.*\ MPI_File_write_all\(
MPI_FILE_WRITE_ALL_END MPI_File_write_all\ resumed.*\n

MPI_FILE_WRITE_AT_BEGIN ^.*\ MPI_File_write_at\(
MPI_FILE_WRITE_AT_END MPI_File_write_at\ resumed.*\n

MPI_FILE_WRITE_AT_ALL_BEGIN ^.*\ MPI_File_write_at_all\(
MPI_FILE_WRITE_AT_ALL_END MPI_File_write_at_all\ resumed.*\n

MPI_FINALIZE_BEGIN ^.*\ MPI_Finalize\(
MPI_FINALIZE_END MPI_Finalize\ resumed.*\n

MPI_INFO_CREATE_BEGIN ^.*\ MPI_Info_create\(
MPI_INFO_CREATE_END \n

MPI_INFO_DUP_BEGIN ^.*\ MPI_Info_dup\(
MPI_INFO_DUP_END \n

MPI_INFO_FREE_BEGIN ^.*\ MPI_Info_free\(
MPI_INFO_FREE_END \n

MPI_INIT_BEGIN ^.*\ MPI_Init\(
MPI_INIT_END MPI_Init\ resumed.*\n

MPI_GET_ELEMENTS ^.*\ MPI_Get_elements\(

MPI_GET_PROCESSOR_NAME_BEGIN ^.*\ MPI_Get_processor_name\(
MPI_GET_PROCESSOR_NAME_END \n

MPI_TYPE_SIZE ^.*\ MPI_Type_size\(

MPI_WTIME_BEGIN ^.*\ MPI_Wtime\(
MPI_WTIME_END \n

UNMATCHED_MPI_CALL ^[0-9.:]+\ MPI_.*\(

%%
{MPI_ALLREDUCE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(allreduce);
        collectInput = true;
        yymore();
        }
<allreduce>{MPI_ALLREDUCE_END} {
        collectInput = false;
        handleMPIAllReduce(yytext);
        BEGIN(0);
        }

{MPI_BARRIER_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(barrier);
        collectInput = true;
        yymore();
        }
<barrier>{MPI_BARRIER_END} {
        collectInput = false;
        handleMPIBarrier(yytext);
        BEGIN(0);
        }

{MPI_BCAST_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(bcast);
        collectInput = true;
        yymore();
        }
<bcast>{MPI_BCAST_END} {
        collectInput = false;
        handleMPIBcast(yytext);
		BEGIN(0);
        }

{MPI_BCAST_IMMEDIATE} {
		checkState(YY_START, yytext);
        collectInput = true;
		handleMPIBcast(yytext);
        }
        
{MPI_COMM_CREATE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(comm_create);
        collectInput = true;
        yymore();
        }
<comm_create>{MPI_COMM_CREATE_END} {
        collectInput = false;
        handleMPICommCreate(yytext);
        BEGIN(0);
        }

{MPI_COMM_DUP_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(comm_dup);
        collectInput = true;
        yymore();
        }
<comm_dup>{MPI_COMM_DUP_END} {
        collectInput = false;
        handleMPICommDup(yytext);
        BEGIN(0);
        }

{MPI_COMM_FREE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(comm_free);
        collectInput = true;
        yymore();
        }
<comm_free>{MPI_COMM_FREE_END} {
        collectInput = false;
        handleMPICommFree(yytext);
        BEGIN(0);
        }

{MPI_COMM_RANK_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(comm_rank);
        collectInput = true;
        yymore();
        }
<comm_rank>{MPI_COMM_RANK_END} {
        collectInput = false;
        handleMPICommRank(yytext);
        BEGIN(0);
        }

{MPI_COMM_SIZE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(comm_size);
        collectInput = true;
        yymore();
        }
<comm_size>{MPI_COMM_SIZE_END} {
        collectInput = false;
        handleMPICommSize(yytext);
        BEGIN(0);
        }

{MPI_FILE_CLOSE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_close);
        collectInput = true;
        yymore();
        }
<file_close>{MPI_FILE_CLOSE_END} {
        collectInput = false;
        handleMPIFileClose(yytext);
        BEGIN(0);
        }

{MPI_FILE_DELETE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_delete);
        collectInput = true;
        yymore();
        }
<file_delete>{MPI_FILE_DELETE_END} {
        collectInput = false;
        handleMPIFileDelete(yytext);
        BEGIN(0);
        }

{MPI_FILE_GET_AMODE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_get_amode);
        collectInput = true;
        yymore();
        }
<file_get_amode>{MPI_FILE_GET_AMODE_END} {
        collectInput = false;
        handleMPIFileGetAMode(yytext);
        BEGIN(0);
        }

{MPI_FILE_GET_GROUP_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_get_group);
        collectInput = true;
        yymore();
        }
<file_get_group>{MPI_FILE_GET_GROUP_END} {
        collectInput = false;
        handleMPIFileGetGroup(yytext);
        BEGIN(0);
        }

{MPI_FILE_GET_SIZE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_get_size);
        collectInput = true;
        yymore();
        }
<file_get_size>{MPI_FILE_GET_SIZE_END} {
        collectInput = false;
        handleMPIFileGetSize(yytext);
        BEGIN(0);
        }

{MPI_FILE_OPEN_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_open);
        collectInput = true;
        yymore();
        }
<file_open>{MPI_FILE_OPEN_END} {
        collectInput = false;
        handleMPIFileOpen(yytext);
        BEGIN(0);
        }

{MPI_FILE_READ_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_read);
        collectInput = true;
        yymore();
        }
<file_read>{MPI_FILE_READ_END} {
        collectInput = false;
        handleMPIFileRead(yytext);
        BEGIN(0);
        }

{MPI_FILE_READ_ALL_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_read_all);
        collectInput = true;
        yymore();
        }
<file_read_all>{MPI_FILE_READ_ALL_END} {
        collectInput = false;
        handleMPIFileReadAll(yytext);
        BEGIN(0);
        }

{MPI_FILE_READ_AT_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_read_at);
        collectInput = true;
        yymore();
        }
<file_read_at>{MPI_FILE_READ_AT_END} {
        collectInput = false;
        handleMPIFileReadAt(yytext);
        BEGIN(0);
        }

{MPI_FILE_READ_AT_ALL_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_read_at_all);
        collectInput = true;
        yymore();
        }
<file_read_at_all>{MPI_FILE_READ_AT_ALL_END} {
        collectInput = false;
        handleMPIFileReadAtAll(yytext);
        BEGIN(0);
        }

{MPI_FILE_SEEK_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_seek);
        collectInput = true;
        yymore();
        }
<file_seek>{MPI_FILE_SEEK_END} {
        collectInput = false;
        handleMPIFileSeek(yytext);
        BEGIN(0);
        }

{MPI_FILE_SET_SIZE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_set_size);
        collectInput = true;
        yymore();
        }
<file_set_size>{MPI_FILE_SET_SIZE_END} {
        collectInput = false;
        handleMPIFileSetSize(yytext);
        BEGIN(0);
        }

{MPI_FILE_WRITE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_write);
        collectInput = true;
        yymore();
        }
<file_write>{MPI_FILE_WRITE_END} {
        collectInput = false;
        handleMPIFileWrite(yytext);
        BEGIN(0);
        }

{MPI_FILE_WRITE_ALL_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_write_all);
        collectInput = true;
        yymore();
        }
<file_write_all>{MPI_FILE_WRITE_ALL_END} {
        collectInput = false;
        handleMPIFileWriteAll(yytext);
        BEGIN(0);
        }

{MPI_FILE_WRITE_AT_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_write_at);
        collectInput = true;
        yymore();
        }
<file_write_at>{MPI_FILE_WRITE_AT_END} {
        collectInput = false;
        handleMPIFileWriteAt(yytext);
        BEGIN(0);
        }

{MPI_FILE_WRITE_AT_ALL_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(file_write_at_all);
        collectInput = true;
        yymore();
        }
<file_write_at_all>{MPI_FILE_WRITE_AT_ALL_END} {
        collectInput = false;
        handleMPIFileWriteAtAll(yytext);
        BEGIN(0);
        }

{MPI_FINALIZE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(finalize);
        collectInput = true;
        yymore();
        }
<finalize>{MPI_FINALIZE_END} {
        collectInput = false;
        handleMPIFinalize(yytext);
        BEGIN(0);
        }

{MPI_GET_ELEMENTS} {
        handleMPIGetElements(yytext);
        }

{MPI_GET_PROCESSOR_NAME_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(get_processor_name);
        collectInput = true;
        yymore();
        }
<get_processor_name>{MPI_GET_PROCESSOR_NAME_END} {
        collectInput = false;
        handleMPIGetProcessorName(yytext);
        BEGIN(0);
        }

{MPI_INFO_CREATE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(info_create);
        collectInput = true;
        yymore();
        }
<info_create>{MPI_INFO_CREATE_END} {
        collectInput = false;
        handleMPIInfoCreate(yytext);
        BEGIN(0);
        }

{MPI_INFO_DUP_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(info_dup);
        collectInput = true;
        yymore();
        }
<info_dup>{MPI_INFO_DUP_END} {
        collectInput = false;
        handleMPIInfoDup(yytext);
        BEGIN(0);
        }

{MPI_INFO_FREE_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(info_free);
        collectInput = true;
        yymore();
        }
<info_free>{MPI_INFO_FREE_END} {
        collectInput = false;
        handleMPIInfoFree(yytext);
        BEGIN(0);
        }

{MPI_INIT_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(init);
        collectInput = true;
        yymore();
        }
<init>{MPI_INIT_END} {
        collectInput = false;
        handleMPIInit(yytext);
        BEGIN(0);
        }

{MPI_TYPE_SIZE} {
		checkState(YY_START, yytext);
        handleMPITypeSize(yytext);
        }

{MPI_WTIME_BEGIN} {
		checkState(YY_START, yytext);
        BEGIN(wtime);
        collectInput = true;
        yymore();
        }
<wtime>{MPI_WTIME_END} {
        collectInput = false;
        handleMPIWTime(yytext);
        BEGIN(0);
        }

{UNMATCHED_MPI_CALL} {
        fprintf(stderr, "%d: Scanner Error: Unmatched MPI Call: %s\n", 
                yylineno, 
                yytext);
        }
        
.       |
\n      {
        if (collectInput)
            yymore();
        }
%%

/* On EOF, no more files. */
int yywrap(void)
{
    return 1;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */

