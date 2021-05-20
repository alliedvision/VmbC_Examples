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
 * \brief The main window of the AsynchronousGrab example of
 *              VmbC.
 * \author Fabian Klein
 */

#ifndef ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H
#define ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H

#include <QMainWindow>

#include "VimbaC/Include/VimbaC.h"

#include "res/ui_MainWindow.h"

#include <ApiController.h>

using VmbC::Examples::ApiController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, Qt::WindowFlags flags = 0 );
    ~MainWindow();

private:
    // The Qt GUI
    Ui::AsynchronousGrabClass ui;
    // Our controller that wraps API access
    ApiController m_ApiController;
    // A list of known camera IDs
    std::vector<std::string> m_cameras;
    // Are we streaming?
    bool m_bIsStreaming;
    // Our Qt image to display
    QImage m_Image;

    //
    // Queries and lists all known camera
    //
    void UpdateCameraListBox();
    
    //
    // Prints out a given logging string, error code and the descriptive representation of that error code
    //
    // Parameters:
    //  [in]    strMsg          A given message to be printed out
    //  [in]    eErr            The API status code
    //
    void Log( std::string strMsg, VmbErrorType eErr );

    //
    // Prints out a given logging string
    //
    // Parameters:
    //  [in]    strMsg          A given message to be printed out
    //
    void Log( std::string strMsg);
    
    //
    // Copies the content of a byte buffer to a Qt image with respect to the image's alignment
    //
    // Parameters:
    //  [in]    pInbuffer       The byte buffer as received from the cam
    //  [in]    ePixelFormat    The pixel format of the frame
    //  [out]   OutImage        The filled Qt image
    //
    VmbErrorType CopyToImage( VmbUchar_t *pInBuffer, VmbPixelFormat_t ePixelFormat, QImage &pOutImage, const float *Matrix = NULL );

private slots:
    // The event handler for starting / stopping acquisition
    void OnBnClickedButtonStartstop();
    
    //
    // This event handler (Qt slot) is triggered through a Qt signal posted by the frame observer
    //
    // Parameters:
    //  [in]    status          The frame receive status (complete, incomplete, ...)
    //
    void OnFrameReady( int status );
    
    //
    // This event handler (Qt slot) is triggered through a Qt signal posted by the camera observer
    //
    // Parameters:
    //  [in]    reason          The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
    //
    void OnCameraListChanged( int reason );
};

#endif // ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H
