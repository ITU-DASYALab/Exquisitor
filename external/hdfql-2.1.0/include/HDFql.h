// Copyright (C) 2016-2019
// This file is part of the Hierarchical Data Format query language (HDFql)
// For more information about HDFql, please visit the website http://www.hdfql.com

// $Rev: 201 $



#ifndef HDFQL_HEADER
	#define HDFQL_HEADER



/*! \mainpage HDFql C Functions Documentation
 *
 *  Library of HDFql C functions to manage HDF5 files
 *
 *  \n
 *  \n
 *  \li \ref SectionExecute
 *  \n
 *  \n
 *  \li \ref SectionError
 *  \n
 *  \n
 *  \li \ref SectionCursor
 *  \n
 *  \n
 *  \li \ref SectionVariable
 *  \n
 *  \n
 *  \li \ref SectionMPI
 *  \n
 *  \n
 *  \li \ref SectionInitializerFinalizer
 *  \n
 *  \n
 *  \li \ref SectionAuxiliary
*/



//===========================================================
// INCLUDE FILES
//===========================================================
#include <stdlib.h>
#include <stdio.h>
#include "HDFqlConstants.h"
#include "HDFqlStructures.h"



//===========================================================
// ENABLE USAGE FROM C++
//===========================================================
#ifdef __cplusplus
	extern "C"
	{
#endif



//===========================================================
// FUNCTIONS PROTOTYPE
//===========================================================
/** @defgroup SectionExecute Execute Functions
 *  List of execute functions.
 *  @{
*/

HDFQL_EXPORT_SYMBOL int hdfql_execute(const char *script);

HDFQL_EXPORT_SYMBOL int hdfql_execute_get_status(void);

/** @} */



/** @defgroup SectionError Error Functions
 *  List of error functions.
 *  @{
*/

HDFQL_EXPORT_SYMBOL int hdfql_error_get_line(void);

HDFQL_EXPORT_SYMBOL int hdfql_error_get_position(void);

HDFQL_EXPORT_SYMBOL char *hdfql_error_get_message(void);

/** @} */



/** @defgroup SectionCursor Cursor Functions
 *  List of cursor functions.
 *  @{
*/

HDFQL_EXPORT_SYMBOL int hdfql_cursor_initialize(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_use(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_use_default(void);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_clear(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_clone(const HDFQL_CURSOR *cursor_original, HDFQL_CURSOR *cursor_clone);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_get_data_type(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_get_count(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_get_count(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_get_position(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_get_position(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_first(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_first(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_last(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_last(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_next(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_next(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_previous(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_previous(HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_absolute(HDFQL_CURSOR *cursor, int position);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_absolute(HDFQL_CURSOR *cursor, int position);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_relative(HDFQL_CURSOR *cursor, int position);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_relative(HDFQL_CURSOR *cursor, int position);

HDFQL_EXPORT_SYMBOL int hdfql_cursor_get_size(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int hdfql_subcursor_get_size(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL void *hdfql_cursor_get(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL void *hdfql_subcursor_get(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL char *hdfql_cursor_get_tinyint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL char *hdfql_subcursor_get_tinyint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned char *hdfql_cursor_get_unsigned_tinyint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned char *hdfql_subcursor_get_unsigned_tinyint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL short *hdfql_cursor_get_smallint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL short *hdfql_subcursor_get_smallint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned short *hdfql_cursor_get_unsigned_smallint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned short *hdfql_subcursor_get_unsigned_smallint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int *hdfql_cursor_get_int(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL int *hdfql_subcursor_get_int(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned int *hdfql_cursor_get_unsigned_int(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned int *hdfql_subcursor_get_unsigned_int(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL long long *hdfql_cursor_get_bigint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL long long *hdfql_subcursor_get_bigint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned long long *hdfql_cursor_get_unsigned_bigint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL unsigned long long *hdfql_subcursor_get_unsigned_bigint(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL float *hdfql_cursor_get_float(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL float *hdfql_subcursor_get_float(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL double *hdfql_cursor_get_double(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL double *hdfql_subcursor_get_double(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL char *hdfql_cursor_get_char(const HDFQL_CURSOR *cursor);

HDFQL_EXPORT_SYMBOL char *hdfql_subcursor_get_char(const HDFQL_CURSOR *cursor);

/** @} */



/** @defgroup SectionVariable Variable Functions
 *  List of variable functions.
 *  @{
*/

HDFQL_EXPORT_SYMBOL int hdfql_variable_register(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_transient_register(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_unregister(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_unregister_all(void);

HDFQL_EXPORT_SYMBOL int hdfql_variable_get_number(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_get_data_type(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_get_count(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_get_size(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_get_dimension_count(const void *variable);

HDFQL_EXPORT_SYMBOL int hdfql_variable_get_dimension(const void *variable, int index);

HDFQL_EXPORT_SYMBOL int hdfql_variable_get_element_count(const void *variable, int index);

/** @} */



/** @defgroup SectionMPI MPI Functions
 *  List of MPI functions.
 *  @{
*/

HDFQL_EXPORT_SYMBOL int hdfql_mpi_get_size(void);

HDFQL_EXPORT_SYMBOL int hdfql_mpi_get_rank(void);

/** @} */



/** @defgroup SectionInitializerFinalizer Initializer/Finalizer Functions
 *  List of initializer/finalizer functions.
 *  @{
*/

HDFQL_EXPORT_SYMBOL void hdfql_initialize(void);

HDFQL_EXPORT_SYMBOL void hdfql_finalize(void);

/** @} */



/** @defgroup SectionAuxiliary Auxiliary Functions
 *  List of auxiliary functions.
 *  @{
*/
HDFQL_EXPORT_SYMBOL char *hdfql_get_canonical_path(const char *object_name, char *object_name_result);

HDFQL_EXPORT_SYMBOL char *_hdfql_get_canonical_path_with_size(const char *object_name, int *object_name_size);

HDFQL_EXPORT_SYMBOL int _hdfql_execute(const char *script, int script_size, int programming_language);

HDFQL_EXPORT_SYMBOL int _hdfql_execute_reset(void);

HDFQL_EXPORT_SYMBOL char *_hdfql_show_use_group(void);

HDFQL_EXPORT_SYMBOL char *_hdfql_cursor_get_char_with_size(HDFQL_CURSOR *cursor, int *size);

HDFQL_EXPORT_SYMBOL char *_hdfql_subcursor_get_char_with_size(HDFQL_CURSOR *cursor, int *size);

HDFQL_EXPORT_SYMBOL unsigned long long _hdfql_get_last_data_count(void);

HDFQL_EXPORT_SYMBOL char *_hdfql_get_compiler(void);

HDFQL_EXPORT_SYMBOL char *_hdfql_get_architecture(void);

/** @} */



#ifdef __cplusplus
	}
#endif



#endif   // HDFQL_HEADER

