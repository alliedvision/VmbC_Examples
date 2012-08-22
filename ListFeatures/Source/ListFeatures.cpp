#include <iostream>
#include <windows.h>

#include <ListFeatures.h>

#include <VimbaC/Include/VimbaC.h>

void ListFeatures( const char *pStrID )
{
    VmbError_t err = VmbStartup();                                                       // Initialize the Vimba API
    VmbHandle_t hCamera = NULL;                                                          // A handle to our camera
    bool bIsGigE = false;                                                                // GigE transport layer present
    VmbFeatureInfo_t *pFeatures = NULL;                                                  // A list of static details of camera features

    // The changeable value of a feature
    VmbInt64_t  nValue;                                                                  // An int value
    double      fValue;                                                                  // A float value
    char        *pStrValue;                                                              // A string value
    bool        bValue;                                                                  // A bool value

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

        if ( NULL == pStrID )                                                           // If no ID was provided use the first camera
        {
            VmbCameraInfo_t *pCameras = NULL;
            VmbUint32_t nCount = 0;
            err = VmbCamerasList(   NULL,                                               // Get the amount of known cameras
                                    0,
                                    &nCount,
                                    sizeof *pCameras );
            if (    VmbErrorSuccess == err
                && 0 < nCount )
            {
                pCameras = new VmbCameraInfo_t[ nCount ];
                if ( NULL != pCameras )
                {
                    err = VmbCamerasList(   pCameras,                                   // Get all known cameras
                                            nCount,
                                            &nCount,
                                            sizeof *pCameras );
                    if ( VmbErrorSuccess == err )
                    {
                        err = VmbCameraOpen(    pCameras[0].cameraIdString,             // Finally open the first one
                                                VmbAccessModeFull,
                                                &hCamera );
                    }
                    else
                    {
                        std::cout << "Could not list cameras. Error code: " << err << std::endl;
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
        }
        else
        {
            err = VmbCameraOpen(    pStrID,                                             // Open the camera with the given ID
                                    VmbAccessModeFull,
                                    &hCamera );
        }

        // Query all static details as well as the value of all fetched features and print them out.
        if ( VmbErrorSuccess == err )
        {
            VmbUint32_t nCount = 0;
            err = VmbFeaturesList(  hCamera,                                            // Get the amount of features
                                    NULL,
                                    0,
                                    &nCount,
                                    sizeof *pFeatures );
            if (    VmbErrorSuccess == err
                 && 0 < nCount )
            {
                pFeatures = new VmbFeatureInfo_t[ nCount ];
                if ( NULL != pFeatures )
                {
                    err = VmbFeaturesList(  hCamera,                                    // Get the features
                                            pFeatures,
                                            nCount,
                                            &nCount,
                                            sizeof *pFeatures );
                    if ( VmbErrorSuccess == err )
                    {
                        for ( VmbUint32_t i=0; i<nCount; ++i )
                        {
                            std::cout << "/// Feature Name: " << ( NULL == pFeatures[i].name ? "" : pFeatures[i].name ) << std::endl;
                            std::cout << "/// Display Name: " << ( NULL == pFeatures[i].displayName ? "" : pFeatures[i].displayName ) << std::endl;
                            std::cout << "/// Tooltip: " << ( NULL == pFeatures[i].tooltip ? "" : pFeatures[i].tooltip ) << std::endl;
                            std::cout << "/// Description: " << ( NULL == pFeatures[i].description ? "" : pFeatures[i].description ) << std::endl;
                            std::cout << "/// SNFC Namespace: " << ( NULL == pFeatures[i].sfncNamespace ? "" : pFeatures[i].sfncNamespace ) << std::endl;
                            std::cout << "/// Value: ";

                            switch ( pFeatures[i].featureDataType )
                            {
                                case VmbFeatureDataBool:
                                    err = VmbFeatureBoolGet( hCamera, pFeatures[i].name, &bValue );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        std::cout << bValue << std::endl;
                                    }
                                    break;
                                case VmbFeatureDataEnum:
                                    err = VmbFeatureEnumGet( hCamera, pFeatures[i].name, (const char**)&pStrValue );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        std::cout << pStrValue << std::endl;
                                    }
                                    break;
                                case VmbFeatureDataFloat:    
                                    err = VmbFeatureFloatGet( hCamera, pFeatures[i].name, &fValue );
                                    {
                                        std::cout << fValue << std::endl;
                                    }
                                    break;
                                case VmbFeatureDataInt:      
                                    err = VmbFeatureIntGet( hCamera, pFeatures[i].name, &nValue );
                                    {
                                        std::cout << nValue << std::endl;
                                    }
                                    break;                            
                                case VmbFeatureDataString:
                                    {

                                    VmbUint32_t nSize = 0;
                                    err = VmbFeatureStringGet( hCamera, pFeatures[i].name, NULL, 0, &nSize );
                                    if (    VmbErrorSuccess == err
                                         && 0 < nSize )
                                    {
                                        pStrValue = new char[ nSize ];
                                        err = VmbFeatureStringGet( hCamera, pFeatures[i].name, pStrValue, nSize, &nSize );
                                        if ( VmbErrorSuccess == err )
                                        {
                                            std::cout << pStrValue << std::endl;
                                        }
                                        delete[] pStrValue;
                                    }
                                    }
                                    break;
                                case VmbFeatureDataCommand:
                                default:
                                    std::cout << "[None]" << std::endl;
                                    break;                            

                                if ( VmbErrorSuccess != err )
                                {
                                    std::cout << "Could not get feature value. Error code: " << err << std::endl;
                                }
                            }

                            std::cout << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Could not get features. Error code: " << err << std::endl;
                    }
                }
                else
                {
                    std::cout << "Could not allocate feature list." << std::endl;
                }
            }
            else
            {
                std::cout << "Could not get features or the camera does not provide any. Error code: " << err << std::endl;
            }

            VmbCameraClose( hCamera );                                                  // Close the camera
        }
        else
        {
            std::cout << "Could not open camera. Error code: " << err << std::endl;
        }
        
        VmbShutdown();                                                                  // Close Vimba
    }
    else
    {
        std::cout << "Could not start system. Error code: " << err << std::endl;
    }
}
