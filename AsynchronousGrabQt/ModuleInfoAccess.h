
#ifndef ASYNCHRONOUSGRAB_C_MODULE_INFO_ACCESSOR_H
#define ASYNCHRONOUSGRAB_C_MODULE_INFO_ACCESSOR_H

#include <vector>

#include "VimbaC/Include/VimbaC.h"

#include "VmbException.h"

namespace VmbC
{
    namespace Examples
    {
        template<typename RetrievalFunction>
        struct RetrievalFunctionInfo;

        template<typename RetrievalFunction>
        std::vector<typename RetrievalFunctionInfo<RetrievalFunction>::InfoType> ListModules()
        {
            VmbUint32_t count;

            VmbError_t error = retrievalFunction(nullptr, 0, &count, sizeof(InfoType));
            if (error != VmbErrorSuccess)
            {
                throw VmbException(error, RetrievalFunctionInfo<RetrievalFunction>::FunctionName);
            }

            std::vector<typename RetrievalFunctionInfo<RetrievalFunction>::InfoType> result(count);

            VmbUint32_t filledCount;

            error = retrievalFunction(result.data(), count, &filledCount, sizeof(InfoType));

            // for simplicity we ignore the case where the list grows between calls
            if (error != VmbErrorSuccess && error != VmbErrorMoreData)
            {
                throw VmbException(error, RetrievalFunctionInfo<RetrievalFunction>::FunctionName);
            }

            if (filledCount < count)
            {
                result.resize(filledCount);
            }
            return result;
        }

    }
}

#endif