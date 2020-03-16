// Copyright (C) 2016-2019
// This file is part of the Hierarchical Data Format query language (HDFql)
// For more information about HDFql, please visit the website http://www.hdfql.com

// $Rev: 208 $



#ifndef HDFQL_CONSTANTS_HEADER
	#define HDFQL_CONSTANTS_HEADER



/*! \mainpage HDFql C Constants Documentation
 *
 *  Library of HDFql C constants to manage HDF5 files
 *
 *  \n
 *  \n
 *  \li \ref SectionGeneral
 *  \n
 *  \n
 *  \li \ref SectionObject
 *  \n
 *  \n
 *  \li \ref SectionDataType
 *  \n
 *  \n
 *  \li \ref SectionEndianness
 *  \n
 *  \n
 *  \li \ref SectionCharset
 *  \n
 *  \n
 *  \li \ref SectionFill
 *  \n
 *  \n
 *  \li \ref SectionLibraryBounds
 *  \n
 *  \n
 *  \li \ref SectionStatus
 *  \n
 *  \n
 *  \li \ref SectionProgrammingLanguages
*/



//===========================================================
// ENABLE USAGE FROM C++
//===========================================================
#ifdef __cplusplus
	extern "C"
	{
#endif



//===========================================================
// DEFINITIONS
//===========================================================
/** @defgroup SectionGeneral General Constants
 *  List of general constants.
 *  @{
*/

#ifdef _WIN32
	#define HDFQL_EXPORT_SYMBOL                __declspec(dllexport)
#else
	#define HDFQL_EXPORT_SYMBOL
#endif

#define HDFQL_VERSION                          "2.1.0"

#define HDFQL_YES                              0

#define HDFQL_NO                               -1

#define HDFQL_ENABLED                          0

#define HDFQL_DISABLED                         -1

#define HDFQL_UNLIMITED                        -1

#define HDFQL_UNDEFINED                        -1


#define HDFQL_GLOBAL                           1

#define HDFQL_LOCAL                            2


#define HDFQL_TRACKED                          1

#define HDFQL_INDEXED                          2


#define HDFQL_CONTIGUOUS                       1

#define HDFQL_COMPACT                          2

#define HDFQL_CHUNKED                          4


#define HDFQL_EARLY                            1

#define HDFQL_INCREMENTAL                      2

#define HDFQL_LATE                             4

/** @} */



/** @defgroup SectionObject Object Constants
 *  List of object constants.
 *  @{
*/

#define HDFQL_DIRECTORY                        1

#define HDFQL_FILE                             2

#define HDFQL_GROUP                            4

#define HDFQL_DATASET                          8

#define HDFQL_ATTRIBUTE                        16

#define HDFQL_SOFT_LINK                        32

#define HDFQL_EXTERNAL_LINK                    64

/** @} */



/** @defgroup SectionDataType Data Type Constants
 *  List of data type constants.
 *  @{
*/

#define HDFQL_TINYINT                          1

#define HDFQL_UNSIGNED_TINYINT                 2

#define HDFQL_SMALLINT                         4

#define HDFQL_UNSIGNED_SMALLINT                8

#define HDFQL_INT                              16

#define HDFQL_UNSIGNED_INT                     32

#define HDFQL_BIGINT                           64

#define HDFQL_UNSIGNED_BIGINT                  128

#define HDFQL_FLOAT                            256

#define HDFQL_DOUBLE                           512

#define HDFQL_CHAR                             1024

#define HDFQL_VARTINYINT                       2048

#define HDFQL_UNSIGNED_VARTINYINT              4096

#define HDFQL_VARSMALLINT                      8192

#define HDFQL_UNSIGNED_VARSMALLINT             16384

#define HDFQL_VARINT                           32768

#define HDFQL_UNSIGNED_VARINT                  65536

#define HDFQL_VARBIGINT                        131072

#define HDFQL_UNSIGNED_VARBIGINT               262144

#define HDFQL_VARFLOAT                         524288

#define HDFQL_VARDOUBLE                        1048576

#define HDFQL_VARCHAR                          2097152

#define HDFQL_OPAQUE                           4194304

#define HDFQL_BITFIELD                         8388608

#define HDFQL_ENUMERATION                      16777216

#define HDFQL_COMPOUND                         33554432

#define HDFQL_ARRAY                            67108864

#define HDFQL_REFERENCE                        134217728

/** @} */



/** @defgroup SectionEndianness Endianness Constants
 *  List of endianness constants.
 *  @{
*/

#define HDFQL_LITTLE_ENDIAN                    1

#define HDFQL_BIG_ENDIAN                       2

/** @} */



/** @defgroup SectionCharset Charset Constants
 *  List of charset constants.
 *  @{
*/

#define HDFQL_ASCII                            1

#define HDFQL_UTF8                             2

/** @} */



/** @defgroup SectionFill Fill Constants
 *  List of fill constants.
 *  @{
*/

#define HDFQL_FILL_DEFAULT                     1

#define HDFQL_FILL_USER_DEFINED                2

#define HDFQL_FILL_UNDEFINED                   4

/** @} */



/** @defgroup SectionLibraryBounds Library Bounds Constants
 *  List of library bounds constants.
 *  @{
*/

#define HDFQL_EARLIEST                         1

#define HDFQL_LATEST                           2

#define HDFQL_VERSION_18                       4

/** @} */



/** @defgroup SectionStatus Status Constants
 *  List of status constants.
 *  @{
*/

#define HDFQL_SUCCESS                          0

#define HDFQL_ERROR_PARSE                      -1

#define HDFQL_ERROR_NOT_FOUND                  -2

#define HDFQL_ERROR_NO_ACCESS                  -3

#define HDFQL_ERROR_NOT_OPEN                   -4

#define HDFQL_ERROR_INVALID_FILE               -5

#define HDFQL_ERROR_NOT_SUPPORTED              -6

#define HDFQL_ERROR_NOT_ENOUGH_SPACE           -7

#define HDFQL_ERROR_NOT_ENOUGH_MEMORY          -8

#define HDFQL_ERROR_ALREADY_EXISTS             -9

#define HDFQL_ERROR_EMPTY                      -10

#define HDFQL_ERROR_FULL                       -11

#define HDFQL_ERROR_BEFORE_FIRST               -12

#define HDFQL_ERROR_AFTER_LAST                 -13

#define HDFQL_ERROR_OUTSIDE_LIMIT              -14

#define HDFQL_ERROR_NO_ADDRESS                 -15

#define HDFQL_ERROR_UNEXPECTED_TYPE            -16

#define HDFQL_ERROR_UNEXPECTED_DATA_TYPE       -17

#define HDFQL_ERROR_UNEXPECTED_STORAGE_TYPE    -18

#define HDFQL_ERROR_DANGLING_LINK              -19

#define HDFQL_ERROR_NOT_REGISTERED             -20

#define HDFQL_ERROR_INVALID_REGULAR_EXPRESSION -21

#define HDFQL_ERROR_UNKNOWN                    -99

/** @} */



/** @defgroup SectionProgrammingLanguages Programming Languages Constants
 *  List of programming languages constants.
 *  @{
*/

#define _HDFQL_C                               1

#define _HDFQL_CPP                             2

#define _HDFQL_JAVA                            4

#define _HDFQL_PYTHON                          8

#define _HDFQL_CSHARP                          16

#define _HDFQL_FORTRAN                         32

#define _HDFQL_R                               64

/** @} */



#ifdef __cplusplus
	}
#endif



#endif   // HDFQL_CONSTANTS_HEADER

