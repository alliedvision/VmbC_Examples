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
 * \brief Definition of a class responsible for starting/stopping VmbC
 */

#ifndef ASYNCHRONOUSGRAB_C_VMB_LIBRARY_LIFETIME_H
#define ASYNCHRONOUSGRAB_C_VMB_LIBRARY_LIFETIME_H

namespace VmbC
{
    namespace Examples
    {

        /**
         * \brief class for managing the initialization and deinitialization of
         *        the Vmb C library 
         */
        class VmbLibraryLifetime
        {
        public:
            VmbLibraryLifetime();

            ~VmbLibraryLifetime();

            VmbLibraryLifetime(VmbLibraryLifetime const&) = delete;
            VmbLibraryLifetime(VmbLibraryLifetime &&) = delete;
            VmbLibraryLifetime& operator=(VmbLibraryLifetime const&) = delete;
            VmbLibraryLifetime& operator=(VmbLibraryLifetime&&) = delete;
        private:
        };
    }
}

#endif