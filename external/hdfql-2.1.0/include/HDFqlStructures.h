// Copyright (C) 2016-2019
// This file is part of the Hierarchical Data Format query language (HDFql)
// For more information about HDFql, please visit the website http://www.hdfql.com

// $Rev: 192 $



#ifndef HDFQL_STRUCTURES_HEADER
	#define HDFQL_STRUCTURES_HEADER



/*! \mainpage HDFql C Structures Documentation
 *
 *  Library of HDFql C structures to manage HDF5 files
 *
 *  \n
 *  \n
 *  \li \ref SectionCursor
 *  \n
 *  \n
 *  \li \ref SectionVariable
*/



//===========================================================
// ENABLE USAGE FROM C++
//===========================================================
#ifdef __cplusplus
	extern "C"
	{
#endif



//===========================================================
// STRUCTURES
//===========================================================
/** @defgroup SectionCursor Structures
 *  List of cursor structures.
 *  @{
*/

typedef struct hdfql_cursor
{
	int                      data_type;
	int                      count;
	int                      position;
	struct hdfql_cursor_data *head;
	struct hdfql_cursor_data *tail;
	struct hdfql_cursor_data *current;
}HDFQL_CURSOR;

struct hdfql_cursor_data
{
	void                     *data;
	int                      size;
	struct hdfql_cursor_data *next;
	struct hdfql_cursor_data *previous;
	struct hdfql_cursor      *subcursor;
};

/** @} */



/** @defgroup SectionVariable Structures
 *  List of variable structures.
 *  @{
*/

typedef struct hdfql_variable_length
{
	unsigned long long count;
	void               *address;
}HDFQL_VARIABLE_LENGTH;

/** @} */



#ifdef __cplusplus
	}
#endif



#endif   // HDFQL_STRUCTURES_HEADER

