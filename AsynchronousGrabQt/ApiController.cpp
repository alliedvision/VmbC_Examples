/**
 * \date 2021
 * \copyright Allied Vision Technologies.  All Rights Reserved.
 *
 * \copyright Redistribution of this file, in original or modified form, without
 *            prior written consent of Allied Vision Technologies is prohibited.
 *
 * \warning THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
 * NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \brief Implementation file for the ApiController helper class that
 *        demonstrates how to implement an asynchronous, continuous image
 *        acquisition with VmbC.
 * \author Fabian Klein
 */

#include <ApiController.h>
#include <sstream>
#include <iostream>
#include "Common/StreamSystemInfo.h"
#include "Common/ErrorCodeToMessage.h"

namespace VmbC {
namespace Examples {

enum    { NUM_FRAMES=3, };

ApiController::ApiController()
    // Get a reference to the Vimba singleton
    : m_system( VimbaSystem::GetInstance() )
{
}

ApiController::~ApiController()
{
}

std::string ApiController::ErrorCodeToMessage( VmbErrorType eErr ) const
{
    return AVT::VmbAPI::Examples::ErrorCodeToMessage( eErr );
}

VmbErrorType ApiController::StartUp()
{
    VmbErrorType res;

    // Start Vimba
    res = m_system.Startup();
    if( VmbErrorSuccess == res )
    {
        // This will be wrapped in a shared_ptr so we don't delete it
        SP_SET( m_pCameraObserver , new CameraObserver() );
        // Register an observer whose callback routine gets triggered whenever a camera is plugged in or out
        res = m_system.RegisterCameraListObserver( m_pCameraObserver );
    }

    return res;
}

void ApiController::ShutDown()
{
    // Release Vimba
    m_system.Shutdown();
}
/*** helper function to set image size to a value that is dividable by modulo 2 and a multiple of the increment.
\note this is needed because VimbaImageTransform does not support odd values for some input formats
*/
inline VmbErrorType SetValueIntMod2( const CameraPtr &camera, const std::string &featureName, VmbInt64_t &storage )
{
    VmbErrorType    res;
    FeaturePtr      pFeature;
    VmbInt64_t      minValue = 0;
    VmbInt64_t      maxValue = 0;
    VmbInt64_t      incrementValue = 0;

    res = SP_ACCESS( camera )->GetFeatureByName( featureName.c_str(), pFeature );
    if( VmbErrorSuccess != res )
    {
        return res;
    }

    res = SP_ACCESS( pFeature )->GetRange( minValue, maxValue );
    if( VmbErrorSuccess != res )
    {
        return res;
    }

    res = SP_ACCESS( pFeature )->GetIncrement( incrementValue);
    if( VmbErrorSuccess != res)
    {
        return res;
    }

    maxValue = maxValue - ( maxValue % incrementValue );
    if( maxValue % 2 != 0)
    {
        maxValue -= incrementValue;
    }

    res = SP_ACCESS( pFeature )->SetValue( maxValue );
    if( VmbErrorSuccess != res )
    {
        return res;
    }

    storage = maxValue;
    return res;
}

VmbErrorType ApiController::StartContinuousImageAcquisition( const std::string &rStrCameraID )
{
    // Open the desired camera by its ID
    VmbErrorType res = m_system.OpenCameraByID( rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera );
    if( VmbErrorSuccess == res )
    {
        // Set the GeV packet size to the highest possible value
        // (In this example we do not test whether this cam actually is a GigE cam)
        FeaturePtr pCommandFeature;
        if( VmbErrorSuccess == SP_ACCESS( m_pCamera )->GetFeatureByName( "GVSPAdjustPacketSize", pCommandFeature ) )
        {
            if( VmbErrorSuccess == SP_ACCESS( pCommandFeature )->RunCommand() )
            {
                bool bIsCommandDone = false;
                do
                {
                    if( VmbErrorSuccess != SP_ACCESS( pCommandFeature )->IsCommandDone( bIsCommandDone ) )
                    {
                        break;
                    }
                } while( false == bIsCommandDone );
            }
        }
        res = SetValueIntMod2( m_pCamera,"Width", m_nWidth );
        if( VmbErrorSuccess == res )
        {
            res = SetValueIntMod2( m_pCamera, "Height", m_nHeight );
            if( VmbErrorSuccess == res )
            {
                // Store currently selected image format
                FeaturePtr pFormatFeature;
                res = SP_ACCESS( m_pCamera )->GetFeatureByName( "PixelFormat", pFormatFeature );
                if( VmbErrorSuccess == res )
                {
                    res = SP_ACCESS( pFormatFeature )->GetValue( m_nPixelFormat );
                    if ( VmbErrorSuccess == res )
                    {
                        // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
                        SP_SET( m_pFrameObserver , new FrameObserver( m_pCamera ) );
                        // Start streaming
                        res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( NUM_FRAMES,  m_pFrameObserver );
                    }
                }
            }
        }
        if ( VmbErrorSuccess != res )
        {
            // If anything fails after opening the camera we close it
            SP_ACCESS( m_pCamera )->Close();
        }
    }

    return res;
}

VmbErrorType ApiController::StopContinuousImageAcquisition()
{
    // Stop streaming
    SP_ACCESS( m_pCamera )->StopContinuousImageAcquisition();

    // Close camera
    return  m_pCamera->Close();
}

CameraPtrVector ApiController::GetCameraList()
{
    CameraPtrVector cameras;
    // Get all known cameras
    if( VmbErrorSuccess == m_system.GetCameras( cameras ) )
    {
        // And return them
        return cameras;
    }
    return CameraPtrVector();
}

int ApiController::GetWidth() const
{
    return static_cast<int>(m_nWidth);
}

int ApiController::GetHeight() const
{
    return static_cast<int>(m_nHeight);
}

VmbPixelFormatType ApiController::GetPixelFormat() const
{
    return static_cast<VmbPixelFormatType>(m_nPixelFormat);
}

FramePtr ApiController::GetFrame()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver )->GetFrame();
}

void ApiController::ClearFrameQueue()
{
    SP_DYN_CAST( m_pFrameObserver,FrameObserver )->ClearFrameQueue();
}

VmbErrorType ApiController::QueueFrame( FramePtr pFrame )
{
    return SP_ACCESS( m_pCamera )->QueueFrame( pFrame );
}

QObject* ApiController::GetCameraObserver()
{
    return SP_DYN_CAST( m_pCameraObserver, CameraObserver ).get();
}

QObject* ApiController::GetFrameObserver()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver ).get();
}

std::string ApiController::GetVersion() const
{
    std::ostringstream os;
    os << m_system;
    return os.str();
}

}} // namespace VmbC::Examples
