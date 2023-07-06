/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <assert.h>
#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <VmbC/VmbCTypeDefinitions.h>

#include <VmbCExamplesCommon/PrintVmbVersion.h>

#include "ListFeatures.h"

#define VMB_PARAM_TL                    "/t"
#define VMB_PARAM_INTERFACE             "/i"
#define VMB_PARAM_REMOTE_DEVICE         "/c"
#define VMB_PARAM_LOCAL_DEVICE          "/l"
#define VMB_PARAM_FEATURE_VISIBILITY    "/v"
#define VMB_PARAM_STREAM                "/s"
#define VMB_PARAM_USAGE                 "/?"

typedef struct VisibilityOption
{
    char const* m_fullName;
    char m_shortName;
    VmbFeatureVisibility_t m_enumValue;
} VisibilityOption;

static VisibilityOption const VisibilityOptions[4] =
{
    { "beginner",   'b', VmbFeatureVisibilityBeginner   },
    { "expert",     'e', VmbFeatureVisibilityExpert     },
    { "guru",       'g', VmbFeatureVisibilityGuru       },
    { "invisible",  'i', VmbFeatureVisibilityInvisible  },
};

/**
 * Try finding the visibility option as string
 *
 * \return the visibility or VmbFeatureVisibilityUnknown, if it isn't found
 */
VmbFeatureVisibility_t FindVisibilityByFullName(char const* value)
{
    VisibilityOption const* pos = VisibilityOptions;
    VisibilityOption const* const end = VisibilityOptions + sizeof(VisibilityOptions) / sizeof(*VisibilityOptions);
    for (; pos != end; ++pos)
    {
        if (strcmp(value, pos->m_fullName) == 0)
        {
            return pos->m_enumValue;
        }
    }
    return VmbFeatureVisibilityUnknown;
}

/**
 * Try finding the visibility option as string
 *
 * \return the visibility or VmbFeatureVisibilityUnknown, if it isn't found
 */
VmbFeatureVisibility_t FindVisibilityByShortName(char const* value)
{
    VisibilityOption const* pos = VisibilityOptions;
    VisibilityOption const* const end = VisibilityOptions + sizeof(VisibilityOptions) / sizeof(*VisibilityOptions);

    if ((*value == '\0') || (value[1] != '\0'))
    {
        return VmbFeatureVisibilityUnknown; // the input is not a single char
    }

    char c = *value;
    for (; pos != end; ++pos)
    {
        if (c == pos->m_shortName)
        {
            return pos->m_enumValue;
        }
    }
    return VmbFeatureVisibilityUnknown;
}

/**
 * Try finding the visibility option as string
 *
 * \return the visibility, if parsed successfully or VmbFeatureVisibilityUnknown
 */
VmbFeatureVisibility_t FindVisibilityByEnumConstant(char const* value)
{
    char* parseEnd;
    unsigned long intValue = strtoul(value, &parseEnd, 10);

    if ((*parseEnd != '\0') || (intValue < VmbFeatureVisibilityBeginner) || (intValue > VmbFeatureVisibilityInvisible))
    {
        // input is not a valid integral version of the enum
        return VmbFeatureVisibilityUnknown;
    }
    return (VmbFeatureVisibility_t)intValue;
}

void PrintUsage(void)
{
    printf("Usage:\n\n"
           "  ListFeatures %s                                                    Print this usage information\n"
           "  ListFeatures <Options>                                             Print the remote device features of the first camera\n"
           "  ListFeatures <Options> %s TransportLayerIndex                      Show Transport Layer features\n"
           "  ListFeatures <Options> %s InterfaceIndex                           Show interface features\n"
           "  ListFeatures <Options> %s (CameraIndex | CameraId)                 Show the remote device features of the specified camera\n"
           "  ListFeatures <Options> %s (CameraIndex | CameraId)                 Show the local device features of the specified camera\n"
           "  ListFeatures <Options> %s (CameraIndex | CameraId) [StreamIndex]   Show the features of a stream for the specified camera\n"
           "Options:\n",
           VMB_PARAM_USAGE, VMB_PARAM_TL, VMB_PARAM_INTERFACE, VMB_PARAM_REMOTE_DEVICE, VMB_PARAM_LOCAL_DEVICE, VMB_PARAM_STREAM);

    // print options for visibility
    printf("  %s (", VMB_PARAM_FEATURE_VISIBILITY);

    for (int i = 0; i != (sizeof(VisibilityOptions) / sizeof(*VisibilityOptions)); ++i)
    {
        VisibilityOption const* const option = VisibilityOptions + i;
        printf("%c|%s|", option->m_shortName, option->m_fullName);
    }
    printf("[1-4])\n");

    // print visibility option descriptions
    for (int i = 0; i != (sizeof(VisibilityOptions) / sizeof(*VisibilityOptions)); ++i)
    {
        VisibilityOption const* const option = VisibilityOptions + i;
        assert(option->m_enumValue == (i + 1)); // array value should match the array index
        printf("     %c, %s, %u %*s: list features up to visibility %c%s\n",
               option->m_shortName,
               option->m_fullName,
               (unsigned) option->m_enumValue,
               (int)(sizeof("invisible") - 1 - strlen(option->m_fullName)), "",
               toupper(*(option->m_fullName)), option->m_fullName + 1
        );
    }
}

/**
 * Try to apply a locale that allows for UTF-8 string output
 */
void TrySetUtf8CompatibleLocale(void)
{
    if (
        (setlocale(LC_CTYPE, ".UTF8") == NULL)      // should work for Windows
        && (setlocale(LC_CTYPE, "C.UTF8") == NULL)  // may work for Linux
        )
    {
        // Maybe the user preferred locale supports UTF-8
        setlocale(LC_CTYPE, "");

        fprintf(stderr, "WARNING: could not set UTF-8 compatible locale; Strings containing non-ASCII characters may be displayed incorrectly\n");
    }
}

int main(int argc, char* argv[])
{
    printf( "\n" );
    printf( "///////////////////////////////////////\n" );
    printf( "/// Vmb API List Features Example   ///\n" );
    printf( "///////////////////////////////////////\n" );
    printf( "\n" );

    PrintVmbVersion();
    printf("\n");

    TrySetUtf8CompatibleLocale();

    VmbFeatureVisibility_t printedFeatureMaximumVisibility = VmbFeatureVisibilityGuru;

    // parse visibility (optional parameter)
    if (argc >= 2)
    {
        if (strcmp(argv[1], VMB_PARAM_FEATURE_VISIBILITY) == 0)
        {
            if (argc == 2)
            {
                printf("the value of the %s command line option is missing\n", VMB_PARAM_FEATURE_VISIBILITY);
                PrintUsage();
                return 1;
            }
            // parse visibility
            char const* visibilityParam = argv[2];
            printedFeatureMaximumVisibility = FindVisibilityByFullName(visibilityParam);
            if (printedFeatureMaximumVisibility == VmbFeatureVisibilityUnknown)
            {
                printedFeatureMaximumVisibility = FindVisibilityByShortName(visibilityParam);
                if (printedFeatureMaximumVisibility == VmbFeatureVisibilityUnknown)
                {
                    printedFeatureMaximumVisibility = FindVisibilityByEnumConstant(visibilityParam);
                    if (printedFeatureMaximumVisibility == VmbFeatureVisibilityUnknown)
                    {
                        // none of the attempts to parse the visibility was successful -> error
                        printf("invalid visibility specified as command line parameter: %s\n", visibilityParam);
                        PrintUsage();
                        return 1;
                    }
                }
            }

            argc -= 2;
            argv += 2;
        }
    }

    if(argc < 2)
    {
        return ListCameraFeaturesAtIndex(0, true, printedFeatureMaximumVisibility);
    }
    else
    {
        char const* const moduleCommand = argv[1];
        if (strcmp(moduleCommand, VMB_PARAM_TL) == 0)
        {
            if (argc < 3)
            {
                printf("transport layer index missing but required for option %s\n", VMB_PARAM_TL);
            }
            else
            {
                char* end = argv[2];
                unsigned long index = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    printf("transport layer index required but found %s\n", argv[2]);
                }
                else
                {
                    return ListTransportLayerFeatures(index, printedFeatureMaximumVisibility);
                }
            }
        }
        else if (strcmp(moduleCommand, VMB_PARAM_INTERFACE) == 0)
        {
            if (argc < 3)
            {
                printf("interface index missing but required for option %s\n", VMB_PARAM_INTERFACE);
            }
            else
            {
                char* end = argv[2];
                unsigned long index = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    printf("interface index required but found %s\n", argv[2]);
                }
                else
                {
                    return ListInterfaceFeatures(index, printedFeatureMaximumVisibility);
                }
            }
        }
        else if (strcmp(moduleCommand, VMB_PARAM_REMOTE_DEVICE) == 0
                 || strcmp(moduleCommand, VMB_PARAM_LOCAL_DEVICE) == 0)
        {
            bool remoteDevice = (strcmp(moduleCommand, VMB_PARAM_REMOTE_DEVICE) == 0);
            if (argc < 3)
            {
                printf("listing device features requires the index or the id of the camera to be provided\n");
            }
            else
            {
                char* end = argv[2];
                unsigned long index = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    // no an index -> try using the parameter as id
                    return ListCameraFeaturesAtId(argv[2], remoteDevice, printedFeatureMaximumVisibility);
                }
                else
                {
                    return ListCameraFeaturesAtIndex(index, remoteDevice, printedFeatureMaximumVisibility);
                }
            }
        }
        else if (strcmp(moduleCommand, VMB_PARAM_STREAM) == 0)
        {
            if (argc < 3)
            {
                printf("listing stream feature requires the index or the id of the camera to be provided\n");
            }
            else
            {
                unsigned long streamIndex = 0;
                if (argc >= 4)
                {
                    char* end = argv[2];
                    streamIndex = strtoul(argv[3], &end, 10);
                    if (*end != '\0')
                    {
                        printf("the index of the stream needs to be valid, but found %s\n", argv[3]);
                        return 1;
                    }
                }
                char* end = argv[2];
                unsigned long cameraIndex = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    // no an index -> try using the parameter as camers id
                    return ListStreamFeaturesAtId(argv[2], streamIndex, printedFeatureMaximumVisibility);
                }
                else
                {
                    return ListStreamFeaturesAtIndex(cameraIndex, streamIndex, printedFeatureMaximumVisibility);
                }
            }
        }
        else if(strcmp(moduleCommand, VMB_PARAM_USAGE) == 0)
        {
            PrintUsage();
            return 0;
        }
        else
        {
            printf("invalid parameter: %s\n\n", argv[1]);
            PrintUsage();
        }
        return 1;
    }

}
