/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include "EventHandling.h"

#include <VmbC/VmbC.h>
#include <VmbCExamplesCommon/ListCameras.h>

/**
* \brief This function will be called when the specified camera feature has changed
*/
void VMB_CALL EventCB(VmbHandle_t handle, const char* name, void* context)
{
    // Here an action can be perform based on the event that has occured
     printf("\nEvent %s has occured.\n", name);
}


int CameraEventDemo(char const* cameraId)
{         
    // Initialize the Vmb API
    VmbError_t err = VmbStartup(NULL);
    if (err == VmbErrorSuccess)
    {
        VmbHandle_t cameraHandle = NULL;

        // No camera ID provided
        if ((cameraId == NULL) || (cameraId[0] == '\0'))
        {
            // Get a list of cameras currently connected to the system
            VmbCameraInfo_t* cameras = NULL;
            VmbUint32_t cameraCount;
            err = ListCameras(&cameras, &cameraCount);

            if (err == VmbErrorSuccess && cameraCount > 0)
            {
                // Use the first camera found in the list
                VmbCameraInfo_t* selectedCamera = cameras;
                
                // Open the camera
                err = VmbCameraOpen(selectedCamera->cameraIdString, VmbAccessModeFull, &cameraHandle);
            }
            else
            {
                printf("Error! Could not get a list of cameras or no cameras detected. Error code: %d\n", err);
            }

            // Deallocate the memory for the camera list
            free(cameras);
        }
        // Camera ID provided by user
        else
        {
            // Open the specified camera
            err = VmbCameraOpen(cameraId, VmbAccessModeFull, &cameraHandle); 
        }

        if (err == VmbErrorSuccess && cameraHandle != NULL)
        {
            // Activate the notification for the desired camera event
            err = ActivateNotification(cameraHandle);
            if (err == VmbErrorSuccess)
            {
                // Register the event callback function
                err = RegisterEventCallback(cameraHandle);
                if (err == VmbErrorSuccess)
                {
                    // Start acquisition on the camera to trigger the event
                    printf("Starting acquisition to trigger event...\n");
                    err = VmbFeatureCommandRun(cameraHandle, "AcquisitionStart");
                }
            }
        }
        else
        {
            printf("Could not open camera or no camera available. Error code: %d\n", err);
        }

        // Stop acquisition
        VmbFeatureCommandRun(cameraHandle, "AcquisitionStop");

        // Close Vmb
        VmbShutdown();
    }
    else
    {
        printf("Could not start Vmb system. Error code: %d\n", err);
    }

    return (err == VmbErrorSuccess ? 0 : 1);
}


VmbErrorType ActivateNotification(VmbHandle_t cameraHandle)
{
    printf("Activating notifications for 'AcquisitionStart' events.\n");

    // EventSelector is used to specify the particular Event to control
    // Configure the AcquisitionStart camera event. 
    VmbErrorType err = VmbFeatureEnumSet(cameraHandle, "EventSelector", "AcquisitionStart");

    if (err == VmbErrorSuccess)
    {
        // EventNotification is used to enable/disable the notification of the event specified by EventSelector.
        err = VmbFeatureEnumSet(cameraHandle, "EventNotification", "On");
    }

    return err;
}


VmbErrorType RegisterEventCallback(VmbHandle_t cameraHandle)
{
    printf("Registering observer for 'EventAcquisitionStart' feature.\n");

    // Each of the events listed in the EventSelector enumeration will have a corresponding event identifier feature. 
    // This feature will be used as a unique identifier of the event to register the callback function. 

    // Register a callback function to be notified that the event happened
    VmbErrorType err = VmbFeatureInvalidationRegister(cameraHandle, "EventAcquisitionStart", EventCB, NULL);

    if (err != VmbErrorSuccess)
    {
        printf("Could not register observer. Error code: %d\n", err);
    } 

    return err;
}