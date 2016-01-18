/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ApiController.c

  Description: VimbaC ApiController for LoadSaveSettings example.

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
#include <DiscoverGigECameras.h>
#include <ApiController.h>

VmbBool_t g_apiFlag = VmbBoolFalse;
VmbHandle_t g_cameraHandle = NULL;

//  =========================================================================
//  name        :  StartVimba()
//  parameter   :  -
//  return val  :  -
//  description :  starts VimbaC API
//	=========================================================================
VmbError_t StartVimba()
{
    VmbError_t err = VmbErrorSuccess;

    if( VmbBoolFalse == g_apiFlag )
    {
        err = VmbStartup();
        if( VmbErrorSuccess != err )
        {
            printf( "Could not start Vimba [error code: %i]", err );
        }

        g_apiFlag = VmbBoolTrue;
    }

    return err;
}

//  =========================================================================
//  name        :  StopVimba()
//  parameter   :  -
//  return val  :  -
//  description :  stops VimbaC API
//	=========================================================================
void StopVimba()
{
    if( VmbBoolTrue == g_apiFlag )
    {
        VmbShutdown();
    }
}

//  =========================================================================
//  name        :  OpenCamera()
//  parameter   :  -
//  return val  :  -
//  description :  open first camera in list
//	=========================================================================
VmbError_t OpenCamera()
{
    VmbError_t err = VmbErrorSuccess;
	VmbUint32_t nCount = 0;
	VmbCameraInfo_t *pCameras = NULL;

    err = DiscoverGigECameras();
    if( VmbErrorSuccess != err )
    {
        printf( "Could discover GigE cameras [error code: %i]", err );
    }
	
	err = VmbCamerasList( NULL, 0, &nCount, sizeof(*pCameras) );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not retrieve number of cameras [error code: %i]", err );
		return err;
	}

	if( 0 == nCount )
	{
		printf( "No Camera found [error code: %i]", err );
		return VmbErrorOther;
	}

	pCameras = (VmbCameraInfo_t *) malloc( nCount * sizeof(*pCameras) );
	if( NULL == pCameras )
	{
		printf( "No Camera found [error code: %i]", err );
		return VmbErrorOther;
	}

	err = VmbCamerasList( pCameras, nCount, &nCount, sizeof(*pCameras) );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not retrieve info pointers for connected cameras [error code: %i]", err );
		return err;
	}

	err = VmbCameraOpen( pCameras[0].cameraIdString, VmbAccessModeFull, &g_cameraHandle );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not open camera in Full Access mode [error code: %i]", err );		
	}

	free( pCameras );
	pCameras = NULL;

    return err;
}

//  =========================================================================
//  name        :  OpenCameraWithId()
//  parameter   :  camera id
//  return val  :  -
//  description :  open specific camera with id
//	=========================================================================
VmbError_t OpenCameraWithId( const char *id )
{
    VmbError_t err = VmbErrorSuccess;

	err = VmbCameraOpen( id, VmbAccessModeFull, &g_cameraHandle );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not open camera in Full Access mode [error code: %i]", err );
	}

    return err;
}

//  =========================================================================
//  name        :  CloseCamera()
//  parameter   :  -
//  return val  :  -
//  description :  close camera
//	=========================================================================
VmbError_t CloseCamera()
{
    VmbError_t err = VmbErrorSuccess;

	err = VmbCameraClose( g_cameraHandle );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not close camera [error code: %i]", err );		
	}

    return err;
}

//  =========================================================================
//  name        :  LoadCameraDefault()
//  parameter   :  -
//  return val  :  -
//  description :  loads via user set camera default settings
//	=========================================================================
VmbError_t LoadCameraDefault()
{
    VmbError_t err = VmbErrorSuccess;

	err = VmbFeatureEnumSet( g_cameraHandle, "UserSetSelector", "Default" );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not set feature value 'UserSetSelector' to 'Default' [error code: %i]", err );
		return err;
	}

	err = VmbFeatureCommandRun( g_cameraHandle, "UserSetLoad" );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not run 'UserSetLoad' command [error code: %i]", err );		
	}

    return err;
}

//  =========================================================================
//  name        :  SaveCameraSettings()
//  parameter   :  XML file name
//  return val  :  -
//  description :  saves all feature value to XML file
//	=========================================================================
VmbError_t SaveCameraSettings( const char *fileName )
{
    VmbError_t err = VmbErrorSuccess;
	
	err = VmbCameraSettingsSave( g_cameraHandle, fileName, NULL, 0 );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not save feature values to given XML file '%s' [error code: %i]", fileName, err );
	}

    return err;
}

//  =========================================================================
//  name        :  LoadCameraSettings()
//  parameter   :  XML file name
//  return val  :  -
//  description :  loads all feature value from XML file to camera
//	=========================================================================
VmbError_t LoadCameraSettings( const char *fileName )
{
    VmbError_t err = VmbErrorSuccess;

	VmbFeaturePersistSettings_t settings;
	settings.loggingLevel = 4;
	settings.maxIterations = 5;
	settings.persistType = VmbFeaturePersistNoLUT;

	err = VmbCameraSettingsLoad( g_cameraHandle, fileName, &settings, sizeof(settings) );
	if( VmbErrorSuccess != err )
	{
		printf( "Could not load feature values from given XML file '%s' [error code: %i]", fileName, err );
	}

    return err;
}