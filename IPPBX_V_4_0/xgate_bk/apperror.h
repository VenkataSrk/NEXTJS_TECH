/* $Header: /xGate/apperror.h 2     9/17/02 6:06p Bennylp $ */
#ifndef __APPERROR_H_
#define __APPERROR_H_

// Application error constants.
// While these values are positives, they will be converted to negative
// values when calling task->SetLastError

namespace Err {
    enum AppError {

	// Default/unknown/unspecified
	UNKNOWN = -1,

	// File
#ifdef FILE_UNKNOWN
#undef FILE_UNKNOWN
#endif

	FILE_UNKNOWN	    = -0x0100,		// unknown file error
	FILE_INVALIDHANDLE  = FILE_UNKNOWN-1,
	FILE_TOOMANY	    = FILE_UNKNOWN-2,	// too many opened files
	FILE_NOENT	    = FILE_UNKNOWN-3,	// file is not found
	FILE_ACCESS	    = FILE_UNKNOWN-4,	// file access error (probably read-only)
	FILE_INVAL	    = FILE_UNKNOWN-5,	// name contains invalid character
	FILE_BADNUMBER	    = FILE_UNKNOWN-6,	// bad file number,
	FILE_EXIST	    = FILE_UNKNOWN-7,	// file exists
	FILE_SYSTOOMANY	    = FILE_UNKNOWN-8,	// too many opened files (on system)
	FILE_NOSPACE	    = FILE_UNKNOWN-9,	// no space left

    };

};

#endif