/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Entry point of the Asynchronous Grab Qt example using the VmbC API
 */

#include <QApplication>
#include <QMessageBox>

#include "UI/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}
