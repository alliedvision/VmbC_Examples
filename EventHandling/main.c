/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>
#include "EventHandling.h"

int main(int argc, char* argv[])
{
    printf("////////////////////////////////////\n");
    printf("/// VmbC Event Handling Example ///\n");
    printf("////////////////////////////////////\n\n");

    if (2 < argc)
    {
        printf("Usage: EventHandling [CameraID]\n\n");
        printf("Parameters:   CameraID    ID of the camera to use (using first camera if not specified)\n");
    }
    else if (2 == argc)
    {        
        return CameraEventDemo(argv[1]);
    }
    else
    {
        return CameraEventDemo("");
    }
}
