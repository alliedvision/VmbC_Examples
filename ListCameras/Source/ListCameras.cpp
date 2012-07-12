#include <iostream>
#include <windows.h>

#include <ListCameras.h>

#include <VimbaC/Include/VimbaC.h>

void ListCameras()
{
    VmbError_t err = VmbStartup();                                                      // Initialize the Vimba API
    Sleep( 200 );                                                                       // We need to wait a little before Vimba has come to life
    VmbCameraInfo_t *pCameras = NULL;                                                   // A list of camera details
    VmbUint32_t nCount = 0;                                                             // Number of found cameras
    bool bIsGigE = false;                                                               // GigE transport layer present

    if ( VmbErrorSuccess == err )
    {
        err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );            // Is Vimba connected to a GigE transport layer?
        if (    VmbErrorSuccess == err
             && true == bIsGigE )
        {
            err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce");           // Send discovery packets to GigE cameras
            if ( VmbErrorSuccess == err )
            {
                Sleep( 200 );                                                           // And wait for them to return
            }
            else
            {
                std::cout << "Could not ping GigE cameras over the network. Reason: " << err << std::endl << std::endl;
            }
        }
        else
        {
            std::cout << "Could not query Vimba for the presence of a GigE transport layer. Reason: " << err << std::endl << std::endl;
        }        

        err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );                     // Get the amount of known cameras
        if (    VmbErrorSuccess == err
             && 0 < nCount )
        {
            std::cout << "Cameras found: " << nCount << std::endl << std::endl;
        
            pCameras = new VmbCameraInfo_t[ nCount ];
            if ( NULL != pCameras )
            {
                err = VmbCamerasList( pCameras, nCount, &nCount, sizeof *pCameras );    // Query all static details of all known cameras
                                                                                        // Without having to open the cameras
                for ( VmbUint32_t i=0; i<nCount; ++i )                                  // And print them out
                {
                    std::cout << "/// Camera Name: " << pCameras[i].cameraName << \
                    std::endl << "/// Model Name: " << pCameras[i].modelName << \
                    std::endl << "/// Camera ID: " << pCameras[i].cameraIdString << \
                    std::endl << "/// Serial Number: " << pCameras[i].serialString << \
                    std::endl << "/// @ Interface ID: " << pCameras[i].interfaceIdString << \
                    std::endl << std::endl;
                }
            }
            else
            {
                std::cout << "Could not allocate camera list." << std::endl;
            }
        }
        else
        {
            std::cout << "Could not list cameras or no cameras present. Error code: " << err << std::endl;
        }
        
        VmbShutdown();                                                                  // Close Vimba
    }
    else
    {
        std::cout << "Could not start system. Error code: " << err << std::endl;
    }
}
