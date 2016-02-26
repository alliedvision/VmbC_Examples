/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.c

  Description: Main entry point of LoadSaveSettings example of VimbaC.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <VimbaC/Include/VimbaC.h>
#include <DiscoverGigECameras.h>

int main( int argc, char *argv[] )
{
    VmbError_t err = VmbErrorSuccess;
    VmbUint32_t nCount = 0;
    VmbCameraInfo_t *pCameras = NULL;
    VmbHandle_t handle = NULL;
    const char *fileName = "current_settings.xml";
    const char *cameraId = NULL;

//  prepare settings struct to determine behaviour
//  during loading or saving operation
//  (This is optional. Passing NULL triggers default settings)
    VmbFeaturePersistSettings_t settings;
    settings.loggingLevel = 4;      //  full loggin details (trace, debug, warning, error and info messages)
    settings.maxIterations = 5;     //  how many iterations during loading shall be performed at max 
                                    //  (comparing desired with device value. in case of difference new iteration will be executed)
    settings.persistType = VmbFeaturePersistNoLUT;      //  determines which features shall be considered
                                                        //  NoLUT means: all features except for lookUp table features

    printf( "\n" );
    printf( "////////////////////////////////////////////\n" );
    printf( "/// Vimba API Load/Save Settings Example ///\n" );
    printf( "////////////////////////////////////////////\n" );
    printf( "\n" );

//  start VimbaC API
    err = VmbStartup();
    if( VmbErrorSuccess != err )
    {
        printf( "Could not start Vimba [error code: %i]\n", err );
        return err;
    }

    printf( "--> VimbaC has been started\n" );
    
//  check for GigE cameras
    err = DiscoverGigECameras();
    if( VmbErrorSuccess != err )
    {
        VmbShutdown();
        printf( "Could discover GigE cameras [error code: %i]\n", err );
        return err;
    }
    
//  get amount of connected cameras
    err = VmbCamerasList( NULL, 0, &nCount, sizeof(*pCameras) );
    if( VmbErrorSuccess != err )
    {
        VmbShutdown();
        printf( "Could not retrieve number of cameras [error code: %i]\n", err );
        return err;
    }

//  in case no camera is connected
    if( 0 == nCount )
    {
        VmbShutdown();
        printf( "No Camera found [error code: %i]\n", err );
        return err;
    }

//  allocate space for camera list
    pCameras = (VmbCameraInfo_t *) malloc( nCount * sizeof(*pCameras) );
    if( NULL == pCameras )
    {
        VmbShutdown();
        printf( "No Camera found [error code: %i]\n", err );
        return VmbErrorOther;
    }

//  retrieve camera list from Vimba
    err = VmbCamerasList( pCameras, nCount, &nCount, sizeof(*pCameras) );
    if( VmbErrorSuccess != err )
    {
        VmbShutdown();
        printf( "Could not retrieve info pointers for connected cameras [error code: %i]\n", err );
        return err;
    }

//  get camera id string
    cameraId = pCameras[0].cameraIdString;
    if( NULL == cameraId )
    {
        VmbShutdown();
        printf( "Could not retrieve camera id [error code: %i]\n", VmbErrorOther );
        return err;
    }

//  open camera (first in list)
    err = VmbCameraOpen( cameraId, VmbAccessModeFull, &handle );
    if( VmbErrorSuccess != err )
    {
        VmbShutdown();
        printf( "Could not open camera in Full Access mode [error code: %i]\n", err );
        return err;
    }

    printf( "--> Camera with id '%s' has been opened\n", cameraId );


//  save current camera feature values
    err = VmbCameraSettingsSave( handle, fileName, NULL, 0 );
    if( VmbErrorSuccess != err )
    {
        printf( "Could not save feature values to given XML file '%s' [error code: %i]\n", fileName, err );
        err = VmbCameraClose( handle );
        if( VmbErrorSuccess != err )
        {
            printf( "Could not close camera [error code: %i]\n", err );
        }
        VmbShutdown();
        return err;
    }

    printf( "--> Camera settings have been saved\n" );

//  set all feature values to factory default
    err = VmbFeatureEnumSet( handle, "UserSetSelector", "Default" );
    if( VmbErrorSuccess != err )
    {
        printf( "Could not set feature value 'UserSetSelector' to 'Default' [error code: %i]\n", err );
        err = VmbCameraClose( handle );
        if( VmbErrorSuccess != err )
        {
            printf( "Could not close camera [error code: %i]\n", err );
        }
        VmbShutdown();
        return err;
    }
    err = VmbFeatureCommandRun( handle, "UserSetLoad" );
    if( VmbErrorSuccess != err )
    {
        printf( "Could not run 'UserSetLoad' command [error code: %i]\n", err );
        err = VmbCameraClose( handle );
        if( VmbErrorSuccess != err )
        {
            printf( "Could not close camera [error code: %i]\n", err );
        }
        VmbShutdown();
        return err;
    }

    printf( "--> All feature values have been restored to default\n" );

//  load feature values to selected camera from xml file
    err = VmbCameraSettingsLoad( handle, fileName, &settings, sizeof(settings) );
    if( VmbErrorSuccess != err )
    {
        printf( "Could not load feature values from given XML file '%s' [error code: %i]\n", fileName, err );
        err = VmbCameraClose( handle );
        if( VmbErrorSuccess != err )
        {
            printf( "Could not close camera [error code: %i]\n", err );
        }
        VmbShutdown();
        return err;
    }

    printf( "--> Feature values have been loaded from given XML file\n" );
    
//  close camera
    err = VmbCameraClose( handle );
    if( VmbErrorSuccess != err )
    {
        printf( "Could not close camera [error code: %i]\n", err );
        VmbShutdown();
        return err;
    }

    printf( "--> Camera has been closed\n" );

//  shutdown VimbaC API
    VmbShutdown();
    printf( "--> VimbaC has been stopped\n" );

//  free allocated space for camera list
    free( pCameras );
    pCameras = NULL;

    return 0;
}
