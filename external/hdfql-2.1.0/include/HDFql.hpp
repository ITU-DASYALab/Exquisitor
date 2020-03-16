// Copyright (C) 2016-2019
// This file is part of the Hierarchical Data Format query language (HDFql)
// For more information about HDFql, please visit the website http://www.hdfql.com

// $Rev: 208 $



#ifndef HDFQL_CPP_HEADER
	#define HDFQL_CPP_HEADER



/*! \mainpage HDFql C++ Documentation
 *
 *  Library of HDFql C++ functions to manage HDF5 files
 *
 *  \n
 *  \n
 *  \li \ref SectionGeneralConstants
 *  \n
 *  \n
 *  \li \ref SectionObjectConstants
 *  \n
 *  \n
 *  \li \ref SectionDataTypeConstants
 *  \n
 *  \n
 *  \li \ref SectionEndiannessConstants
 *  \n
 *  \n
 *  \li \ref SectionCharsetConstants
 *  \n
 *  \n
 *  \li \ref SectionFillConstants
 *  \n
 *  \n
 *  \li \ref SectionLibraryBoundsConstants
 *  \n
 *  \n
 *  \li \ref SectionStatusConstants
 *  \n
 *  \n
 *  \li \ref SectionProgrammingLanguagesConstants
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
#include <string>
#include <cstring>
#include "HDFql.h"



//===========================================================
// NAMESPACE DECLARATION
//===========================================================
namespace HDFql
{



	//===========================================================
	// CLASSES DECLARATION
	//===========================================================
	class Cursor
	{

		public:
			HDFQL_CURSOR cursor;

			HDFQL_EXPORT_SYMBOL Cursor();

			HDFQL_EXPORT_SYMBOL ~Cursor();

	};



	/** @defgroup SectionGeneralConstants General Constants
	 *  List of general constants.
	 *  @{
	*/

	const char Version[] = HDFQL_VERSION;

	const int Yes = HDFQL_YES;

	const int No = HDFQL_NO;

	const int Enabled = HDFQL_ENABLED;

	const int Disabled = HDFQL_DISABLED;

	const int Unlimited = HDFQL_UNLIMITED;

	const int Undefined = HDFQL_UNDEFINED;


	const int Global = HDFQL_GLOBAL;

	const int Local = HDFQL_LOCAL;


	const int Tracked = HDFQL_TRACKED;

	const int Indexed = HDFQL_INDEXED;


	const int Contiguous = HDFQL_CONTIGUOUS;

	const int Compact = HDFQL_COMPACT;

	const int Chunked = HDFQL_CHUNKED;


	const int Early = HDFQL_EARLY;

	const int Incremental = HDFQL_INCREMENTAL;

	const int Late = HDFQL_LATE;

	/** @} */



	/** @defgroup SectionObjectConstants Object Constants
	 *  List of object constants.
	 *  @{
	*/

	const int Directory = HDFQL_DIRECTORY;

	const int File = HDFQL_FILE;

	const int Group = HDFQL_GROUP;

	const int Dataset = HDFQL_DATASET;

	const int Attribute = HDFQL_ATTRIBUTE;

	const int SoftLink = HDFQL_SOFT_LINK;

	const int ExternalLink = HDFQL_EXTERNAL_LINK;

	/** @} */



	/** @defgroup SectionDataTypeConstants Data Type Constants
	 *  List of data type constants.
	 *  @{
	*/

	const int TinyInt = HDFQL_TINYINT;

	const int UnsignedTinyInt = HDFQL_UNSIGNED_TINYINT;

	const int SmallInt = HDFQL_SMALLINT;

	const int UnsignedSmallInt = HDFQL_UNSIGNED_SMALLINT;

	const int Int = HDFQL_INT;

	const int UnsignedInt = HDFQL_UNSIGNED_INT;

	const int BigInt = HDFQL_BIGINT;

	const int UnsignedBigInt = HDFQL_UNSIGNED_BIGINT;

	const int Float = HDFQL_FLOAT;

	const int Double = HDFQL_DOUBLE;

	const int Char = HDFQL_CHAR;

	const int VarTinyInt = HDFQL_VARTINYINT;

	const int UnsignedVarTinyInt = HDFQL_UNSIGNED_VARTINYINT;

	const int VarSmallInt = HDFQL_VARSMALLINT;

	const int UnsignedVarSmallInt = HDFQL_UNSIGNED_VARSMALLINT;

	const int VarInt = HDFQL_VARINT;

	const int UnsignedVarInt = HDFQL_UNSIGNED_VARINT;

	const int VarBigInt = HDFQL_VARBIGINT;

	const int UnsignedVarBigInt = HDFQL_UNSIGNED_VARBIGINT;

	const int VarFloat = HDFQL_VARFLOAT;

	const int VarDouble = HDFQL_VARDOUBLE;

	const int VarChar = HDFQL_VARCHAR;

	const int Opaque = HDFQL_OPAQUE;

	const int BitField = HDFQL_BITFIELD;

	const int Enumeration = HDFQL_ENUMERATION;

	const int Compound = HDFQL_COMPOUND;

	const int Array = HDFQL_ARRAY;

	const int Reference = HDFQL_REFERENCE;

	/** @} */



	/** @defgroup SectionEndiannessConstants Endianness Constants
	 *  List of endianness constants.
	 *  @{
	*/

	const int LittleEndian = HDFQL_LITTLE_ENDIAN;

	const int BigEndian = HDFQL_BIG_ENDIAN;

	/** @} */



	/** @defgroup SectionCharsetConstants Charset Constants
	 *  List of charset constants.
	 *  @{
	*/

	const int Ascii = HDFQL_ASCII;

	const int Utf8 = HDFQL_UTF8;

	/** @} */



	/** @defgroup SectionFillConstants Fill Constants
	 *  List of fill constants.
	 *  @{
	*/

	const int FillDefault = HDFQL_FILL_DEFAULT;

	const int FillUserDefined = HDFQL_FILL_USER_DEFINED;

	const int FillUndefined = HDFQL_FILL_UNDEFINED;

	/** @} */



	/** @defgroup SectionLibraryBoundsConstants Library Bounds Constants
	 *  List of library bounds constants.
	 *  @{
	*/

	const int Earliest = HDFQL_EARLIEST;

	const int Latest = HDFQL_LATEST;

	const int Version18 = HDFQL_VERSION_18;

	/** @} */



	/** @defgroup SectionStatusConstants Status Constants
	 *  List of status constants.
	 *  @{
	*/

	const int Success = HDFQL_SUCCESS;

	const int ErrorParse = HDFQL_ERROR_PARSE;

	const int ErrorNotFound = HDFQL_ERROR_NOT_FOUND;

	const int ErrorNoAccess = HDFQL_ERROR_NO_ACCESS;

	const int ErrorNotOpen = HDFQL_ERROR_NOT_OPEN;

	const int ErrorInvalidFile = HDFQL_ERROR_INVALID_FILE;

	const int ErrorNotSupported = HDFQL_ERROR_NOT_SUPPORTED;

	const int ErrorNotEnoughSpace = HDFQL_ERROR_NOT_ENOUGH_SPACE;

	const int ErrorNotEnoughMemory = HDFQL_ERROR_NOT_ENOUGH_MEMORY;

	const int ErrorAlreadyExists = HDFQL_ERROR_ALREADY_EXISTS;

	const int ErrorEmpty = HDFQL_ERROR_EMPTY;

	const int ErrorFull = HDFQL_ERROR_FULL;

	const int ErrorBeforeFirst = HDFQL_ERROR_BEFORE_FIRST;

	const int ErrorAfterLast = HDFQL_ERROR_AFTER_LAST;

	const int ErrorOutsideLimit = HDFQL_ERROR_OUTSIDE_LIMIT;

	const int ErrorNoAddress = HDFQL_ERROR_NO_ADDRESS;

	const int ErrorUnexpectedType = HDFQL_ERROR_UNEXPECTED_TYPE;

	const int ErrorUnexpectedDataType = HDFQL_ERROR_UNEXPECTED_DATA_TYPE;

	const int ErrorUnexpectedStorageType = HDFQL_ERROR_UNEXPECTED_STORAGE_TYPE;

	const int ErrorDanglingLink = HDFQL_ERROR_DANGLING_LINK;

	const int ErrorNotRegistered = HDFQL_ERROR_NOT_REGISTERED;

	const int ErrorInvalidRegularExpression = HDFQL_ERROR_INVALID_REGULAR_EXPRESSION;

	const int ErrorUnknown = HDFQL_ERROR_UNKNOWN;

	/** @} */



	/** @defgroup SectionProgrammingLanguagesConstants Programming Languages Constants
	 *  List of programming languages constants.
	 *  @{
	*/

	const int _C = _HDFQL_C;

	const int _Cpp = _HDFQL_CPP;

	const int _Java = _HDFQL_JAVA;

	const int _Python = _HDFQL_PYTHON;

	const int _CSharp = _HDFQL_CSHARP;

	const int _Fortran = _HDFQL_FORTRAN;

	const int _R = _HDFQL_R;

	/** @} */



	/** @defgroup SectionExecute Execute Functions
	 *  List of execute functions.
	 *  @{
	*/

	HDFQL_EXPORT_SYMBOL int execute(const char *script);

	HDFQL_EXPORT_SYMBOL int execute(const std::string &script);

	HDFQL_EXPORT_SYMBOL int executeGetStatus(void);

	/** @} */



	/** @defgroup SectionError Error Functions
	 *  List of error functions.
	 *  @{
	*/

	HDFQL_EXPORT_SYMBOL int errorGetLine(void);

	HDFQL_EXPORT_SYMBOL int errorGetPosition(void);

	HDFQL_EXPORT_SYMBOL char *errorGetMessage(void);

	/** @} */



	/** @defgroup SectionCursor Cursor Functions
	 *  List of cursor functions.
	 *  @{
	*/

	HDFQL_EXPORT_SYMBOL int cursorInitialize(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorUse(const Cursor *cursor);

	HDFQL_EXPORT_SYMBOL int cursorUseDefault(void);

	HDFQL_EXPORT_SYMBOL int cursorClear(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorClone(Cursor *cursorClone);

	HDFQL_EXPORT_SYMBOL int cursorClone(const Cursor *cursorOriginal, Cursor *cursorClone);

	HDFQL_EXPORT_SYMBOL int cursorGetDataType(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorGetCount(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int subcursorGetCount(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorGetPosition(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int subcursorGetPosition(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorFirst(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int subcursorFirst(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorLast(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int subcursorLast(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorNext(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int subcursorNext(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorPrevious(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int subcursorPrevious(Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int cursorAbsolute(int position);

	HDFQL_EXPORT_SYMBOL int cursorAbsolute(Cursor *cursor, int position);

	HDFQL_EXPORT_SYMBOL int subcursorAbsolute(int position);

	HDFQL_EXPORT_SYMBOL int subcursorAbsolute(Cursor *cursor, int position);

	HDFQL_EXPORT_SYMBOL int cursorRelative(int position);

	HDFQL_EXPORT_SYMBOL int cursorRelative(Cursor *cursor, int position);

	HDFQL_EXPORT_SYMBOL int subcursorRelative(int position);

	HDFQL_EXPORT_SYMBOL int subcursorRelative(Cursor *cursor, int position);

	HDFQL_EXPORT_SYMBOL int cursorGetSize(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int subcursorGetSize(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL void *cursorGet(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL void *subcursorGet(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL char *cursorGetTinyInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL char *subcursorGetTinyInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned char *cursorGetUnsignedTinyInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned char *subcursorGetUnsignedTinyInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL short *cursorGetSmallInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL short *subcursorGetSmallInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned short *cursorGetUnsignedSmallInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned short *subcursorGetUnsignedSmallInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int *cursorGetInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL int *subcursorGetInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned int *cursorGetUnsignedInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned int *subcursorGetUnsignedInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL long long *cursorGetBigInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL long long *subcursorGetBigInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned long long *cursorGetUnsignedBigInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL unsigned long long *subcursorGetUnsignedBigInt(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL float *cursorGetFloat(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL float *subcursorGetFloat(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL double *cursorGetDouble(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL double *subcursorGetDouble(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL char *cursorGetChar(const Cursor *cursor = NULL);

	HDFQL_EXPORT_SYMBOL char *subcursorGetChar(const Cursor *cursor = NULL);

	/** @} */



	/** @defgroup SectionVariable Variable Functions
	 *  List of variable functions.
	 *  @{
	*/

	HDFQL_EXPORT_SYMBOL int variableRegister(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableTransientRegister(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableUnregister(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableUnregisterAll(void);

	HDFQL_EXPORT_SYMBOL int variableGetNumber(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableGetDataType(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableGetCount(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableGetSize(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableGetDimensionCount(const void *variable);

	HDFQL_EXPORT_SYMBOL int variableGetDimension(const void *variable, int index);

	HDFQL_EXPORT_SYMBOL int variableGetElementCount(const void *variable, int index);

	/** @} */



	/** @defgroup SectionMPI MPI Functions
	 *  List of MPI functions.
	 *  @{
	*/

	HDFQL_EXPORT_SYMBOL int mpiGetSize(void);

	HDFQL_EXPORT_SYMBOL int mpiGetRank(void);

	/** @} */



	/** @defgroup SectionInitializerFinalizer Initializer/Finalizer Functions
	 *  List of initializer/finalizer functions.
	 *  @{
	*/

	HDFQL_EXPORT_SYMBOL void initialize(void);

	HDFQL_EXPORT_SYMBOL void finalize(void);

	/** @} */



	/** @defgroup SectionAuxiliary Auxiliary Functions
	 *  List of auxiliary functions.
	 *  @{
	*/

	HDFQL_EXPORT_SYMBOL char *getCanonicalPath(const char *objectName);

	HDFQL_EXPORT_SYMBOL char *getCanonicalPath(const std::string &objectName);

	/** @} */

};



/** @} */



#endif   // HDFQL_CPP_HEADER

