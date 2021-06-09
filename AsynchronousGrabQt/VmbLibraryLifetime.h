#ifndef ASYNCHRONOUSGRAB_C_VMB_LIBRARY_LIFETIME_H
#define ASYNCHRONOUSGRAB_C_VMB_LIBRARY_LIFETIME_H

namespace VmbC
{
    namespace Examples
    {

        /**
         * \brief class for managing the initialization and deinitialization of the Vmb C library 
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