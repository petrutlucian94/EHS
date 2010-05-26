/* $Id$
 *
 * EHS is a library for embedding HTTP(S) support into a C++ application
 *
 * Copyright (C) 2004 Zachary J. Hansen
 *
 * Code cleanup, new features and bugfixes: Copyright (C) 2010 Fritz Elfert
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1 as published by the Free Software Foundation;
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    This can be found in the 'COPYING' file.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef COMPILE_WITH_SSL

#include "sslerror.h"

using namespace std;

bool SslError::bMessagesLoaded = false;

int SslError::GetError ( string & irsReport, bool inPeek )
{
	// get the error code
	unsigned long nError = inPeek ? ERR_peek_error ( ) : ERR_get_error ( );

	// if there is no error, or we don't want full text, return the error
	//   code now
	if ( nError == 0 || irsReport == "noreport" ) {
		irsReport.clear();
		return nError;
	}

	// do we need to load the strings?
	if ( !bMessagesLoaded ) {
		SSL_load_error_strings ( );
		bMessagesLoaded = true;
	}

	char psBuffer [ 256 ];
	ERR_error_string_n ( nError, psBuffer, 256 );
	irsReport = psBuffer;
	return nError;
}

int SslError::PeekError ( string & irsReport )
{
	return GetError ( irsReport, true );
}


#endif // COMPILE_WITH_SSL