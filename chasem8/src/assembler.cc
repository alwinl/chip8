/*
 * Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "assembler.h"

/*
 * we expect between 2 and 4 arguments
 * argv[1] = out_file name
 * argv[2] = in_file name
 * argv[3] = list_file name
 */
int Assembler::parse_arguments( int argc, char ** argv )
{
	if( argc < 2 )
		return NoTargetError;

	OutFileName = argv[1];


#if 0

	strcpy( OutFileName, argv[ 1 ] );

	if( argc >= 3 )
		strcpy( InFileName, argv[ 2 ] );
	else {
		strcpy( InFileName, OutFileName );
		strcat( InFileName, InExtDefault );
	}

	if( !strcmp( InFileName, DefFileName ) ) {
		strcpy( InFileName, OutFileName );
		strcat( InFileName, InExtDefault );
	}

	if( !strcmp( InFileName, StdFileName ) )
		strcpy( InFileName, StdInFileName );

	if( argc >= 4 )
		strcpy( ListFileName, argv[ 3 ] );
	else {
		strcpy( ListFileName, OutFileName );
		strcat( ListFileName, ListExtDefault );
	}

	if( !strcmp( ListFileName, DefFileName ) ) {
		strcpy( ListFileName, OutFileName );
		strcat( ListFileName, ListExtDefault );
	}

	if( !strcmp( ListFileName, StdFileName ) )
		strcpy( ListFileName, StdOutFileName );

    if( fprintf( stderr, "TargetFile: %s\n", OutFileName ) <= 0 )
        RunError( True, FileWriteError );

    OutFile = fopen( OutFileName, "w+b" );
    if( !OutFile )
        RunError( True, FileAccessError );

    if( fprintf( stderr, "SourceFile: %s\n", InFileName ) <= 0 )
        RunError( True, FileWriteError );

    if( strcmp( InFileName, StdInFileName ) )
        InFile = fopen( InFileName, "r" );
    else
        InFile = stdin;

    if( !InFile )
        RunError( True, NoSourceError );

    if( fprintf( stderr, "ListFile: %s\n", ListFileName ) <= 0 )
        RunError( True, FileWriteError );

    if( strcmp( ListFileName, StdOutFileName ) )
        ListFile = fopen( ListFileName, "w" );
    else
        ListFile = stdout;

    if( !ListFile )
        RunError( True, FileAccessError );

    fprintf( stderr, "\n" );
    if( fprintf( ListFile, "%s\n\nTargetFile: %s\nSourceFile: %s\nListFile: %s\n\n", CopyRight, OutFileName, InFileName, ListFileName ) <= 0 )
        RunError( True, FileWriteError );

    return( True );

#endif // 0

	return NoError;
}

int Assembler::run( int argc, char ** argv )
{
	if( parse_arguments( argc, argv ) != NoError )
		return 1;

	return 0;
}
