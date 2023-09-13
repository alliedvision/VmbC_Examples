/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
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
