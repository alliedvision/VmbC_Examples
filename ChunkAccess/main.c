/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>

#include "ChunkAccessProg.h"

int main( int argc, char* argv[] )
{
    printf( "////////////////////////////////////\n" );
    printf( "/// Vmb API Chunk Access Example ///\n" );
    printf( "////////////////////////////////////\n\n" );

    if ( 1 < argc )
    {
        printf( "No parameters expected. Execution will not be affected by the provided parameter(s).\n\n" );
    }
    
    return ChunkAccessProg();
}
