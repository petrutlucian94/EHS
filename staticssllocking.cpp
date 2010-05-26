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

#include "staticssllocking.h"


// deal with static variables
pthread_mutex_t * StaticSslLocking::poMutexes;


void StaticSslLocking::SslStaticLockCallback ( int inMode,
        int inMutex,
        const char *,
        int )
{
    if ( inMode & CRYPTO_LOCK ) {
        pthread_mutex_lock ( &StaticSslLocking::poMutexes [ inMutex ] );
    } else {
        pthread_mutex_unlock ( &StaticSslLocking::poMutexes [ inMutex ] );
    }
}

unsigned long StaticSslLocking::SslThreadIdCallback ( )
{
    return ( (unsigned long) pthread_self ( ) );
}


StaticSslLocking::StaticSslLocking ( )
{
    // allocate the needed number of locks
    StaticSslLocking::poMutexes = new pthread_mutex_t [ CRYPTO_num_locks ( ) ];

    assert ( StaticSslLocking::poMutexes != NULL );

    // initialize the mutexes
    for ( int i = 0; i < CRYPTO_num_locks ( ); i++ ) {
        pthread_mutex_init ( &StaticSslLocking::poMutexes [ i ], NULL );
    }
    // set callbacks
    CRYPTO_set_id_callback ( StaticSslLocking::SslThreadIdCallback );
    CRYPTO_set_locking_callback ( StaticSslLocking::SslStaticLockCallback );
}

StaticSslLocking::~StaticSslLocking ( )
{
    assert ( StaticSslLocking::poMutexes != NULL );

    CRYPTO_set_id_callback ( NULL );
    CRYPTO_set_locking_callback ( NULL );
    for ( int i = 0; i < CRYPTO_num_locks ( ); i++ ) {
        pthread_mutex_destroy ( &StaticSslLocking::poMutexes [ i ] );
    }
    delete [] StaticSslLocking::poMutexes;
    StaticSslLocking::poMutexes = NULL;
}

#endif // COMPILE_WITH_SSL