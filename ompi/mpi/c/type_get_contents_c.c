/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2020 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2008 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ompi_config.h"

#include "ompi/mpi/c/bindings.h"
#include "ompi/runtime/params.h"
#include "ompi/communicator/communicator.h"
#include "ompi/errhandler/errhandler.h"
#include "ompi/datatype/ompi_datatype.h"
#include "ompi/memchecker.h"

#if OMPI_BUILD_MPI_PROFILING
#if OPAL_HAVE_WEAK_SYMBOLS
#pragma weak MPI_Type_get_contents_c = PMPI_Type_get_contents_c
#endif
#define MPI_Type_get_contents_c PMPI_Type_get_contents_c
#endif

static const char FUNC_NAME[] = "MPI_Type_get_contents_c";


int MPI_Type_get_contents_c(MPI_Datatype mtype,
                            MPI_Count max_integers,
                            MPI_Count max_addresses,
                            MPI_Count max_large_counts,
                            MPI_Count  max_datatypes,
                            int array_of_integers[],
                            MPI_Aint array_of_addresses[],
                            MPI_Count  array_of_large_counts[],
                            MPI_Datatype array_of_datatypes[])
{
    int rc, i;
    MPI_Datatype newtype;

    MEMCHECKER(
        memchecker_datatype(mtype);
    );

    if( MPI_PARAM_CHECK ) {
        OMPI_ERR_INIT_FINALIZE(FUNC_NAME);
        if (NULL == mtype || MPI_DATATYPE_NULL == mtype) {
            return OMPI_ERRHANDLER_NOHANDLE_INVOKE(MPI_ERR_TYPE,
                                          FUNC_NAME );
        } else if( ((NULL == array_of_integers) && (max_integers != 0)) ||
                   ((NULL == array_of_addresses) && (max_addresses != 0)) ||
                   ((NULL == array_of_datatypes) && (max_datatypes != 0)) ) {
            return OMPI_ERRHANDLER_NOHANDLE_INVOKE(MPI_ERR_ARG,
                                          FUNC_NAME );
        }
    }

/* TODO:BIGCOUNT: Need to embiggen ompi_datatype_get_args */
    rc = ompi_datatype_get_args( mtype, 1, (int *)&max_integers, array_of_integers,
                            (int *)&max_addresses, array_of_addresses,
                            (int *)&max_datatypes, array_of_datatypes, NULL );
    if( rc != MPI_SUCCESS ) {
        OMPI_ERRHANDLER_NOHANDLE_RETURN( MPI_ERR_INTERN, 
                                MPI_ERR_INTERN, FUNC_NAME );
    }

    for( i = 0; i < max_datatypes; i++ ) {
        /* if we have a predefined datatype then we return directly a pointer to
         * the datatype, otherwise we should create a copy and give back the copy.
         */
        if( !(ompi_datatype_is_predefined(array_of_datatypes[i])) ) {
            if( (rc = ompi_datatype_duplicate( array_of_datatypes[i], &newtype )) != MPI_SUCCESS ) {
                ompi_datatype_destroy( &newtype );
                OMPI_ERRHANDLER_NOHANDLE_RETURN( MPI_ERR_INTERN, 
                                        MPI_ERR_INTERN, FUNC_NAME );
            }
            ompi_datatype_copy_args( array_of_datatypes[i], newtype );
            array_of_datatypes[i] = newtype;
        }
    }

    return MPI_SUCCESS;
}
