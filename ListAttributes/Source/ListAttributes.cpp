
#include <stdio.h>

#ifndef _WINDOWS
#include <unistd.h>
#include <time.h>
#include <signal.h>

void Sleep(unsigned int time)
{
    struct timespec t,r;
    
    t.tv_sec    = time / 1000;
    t.tv_nsec   = (time % 1000) * 1000000;    
    
    while(nanosleep(&t,&r)==-1)
        t = r;
}
#else
#include <windows.h>
#endif

#include <VimbaC/Include/VimbaC.h>

const char* DataType2String(VmbFeatureData_t aType)
{
    switch(aType)
    {
        case VmbFeatureDataInt:
            return "Int";
        case VmbFeatureDataFloat:
            return "Float";
        case VmbFeatureDataEnum:
            return "Enum";        
        case VmbFeatureDataString:
            return "String";        
        case VmbFeatureDataBool:
            return "Bool";        
        case VmbFeatureDataCommand:
            return "Command";        
        case VmbFeatureDataRaw:
            return "Raw";
        case VmbFeatureDataNone:
            return "None";
        default:
            return "Unknown";  
    }
}

const char* Visibility2String(VmbFeatureVisibility_t aType)
{
    switch(aType)
    {
        case VmbFeatureVisibilityBeginner:
            return "Beginner";
        case VmbFeatureVisibilityExpert:
            return "Expert";
        case VmbFeatureVisibilityGuru:
            return "Guru";
        case VmbFeatureVisibilityInvisible:
            return "Invisible";
        default:
            return "n/a";     
    }
}

VmbError_t PrintFeatureRange(VmbHandle_t aHandle,const char* aLabel,VmbFeatureData_t aType)
{
    VmbError_t lError = VmbErrorSuccess;

    switch(aType)
    {
        case VmbFeatureDataInt:
        {
            VmbInt64_t lMin,lMax;
            
            if(!(lError = VmbFeatureIntRangeQuery(aHandle,aLabel,&lMin,&lMax)))
                printf("\trange\t\t\t= %lld to %lld\n",lMin,lMax);      
            else
                printf("\trange\t\t\t= FAILED (%d)\n",lError); 
                
            break;     
        }
        case VmbFeatureDataFloat:
        { 
            double lMin,lMax;
            
            if(!(lError = VmbFeatureFloatRangeQuery(aHandle,aLabel,&lMin,&lMax)))
                printf("\trange\t\t\t= %lf to %lf\n",lMin,lMax);     
            else
                printf("\trange\t\t\t= FAILED (%d)\n",lError);            
        
            break;
        }  
         
        case VmbFeatureDataEnum: 
        {
            const char* lValues[256];
            VmbUint32_t  lFilled;
            bool        lAvail;
        
            if(!(lError = VmbFeatureEnumRangeQuery(aHandle,aLabel,lValues,256,&lFilled)))
            {
                printf("\trange\t\t\t= ");
                
                for(VmbUint32_t i=0;i<lFilled;i++)
                    if(!VmbFeatureEnumIsAvailable(aHandle,aLabel,lValues[i],&lAvail) && lAvail)
                        printf("%s ",lValues[i]);
                    
                printf("\n");      
            }
            else
                printf("\trange\t\t\t= FAILED (%d)\n",lError);                 
        
            break;
        }                       
        default:        
            break; 
    }
    
    return lError;    
}

VmbError_t PrintFeatureIncrement(VmbHandle_t aHandle,const char* aLabel,VmbFeatureData_t aType)
{
    VmbError_t lError = VmbErrorSuccess;
    
    switch(aType)
    {
        case VmbFeatureDataInt:
        {
            VmbInt64_t lValue;
            
            if(!(lError = VmbFeatureIntIncrementQuery(aHandle,aLabel,&lValue)))
                printf("\tincrement\t\t= %lld\n",lValue);      
            else
                printf("\tincrement\t\t= FAILED (%d)\n",lError); 
                
            break;     
        }
        default:        
            printf("\tincrement\t\t= n/a\n");  
    }
    
    return lError;    
}

VmbError_t PrintFeatureValue(VmbHandle_t aHandle,const char* aLabel,VmbFeatureData_t aType)
{
    VmbError_t lError = VmbErrorSuccess;

    switch(aType)
    {
        case VmbFeatureDataInt:
        {
            VmbInt64_t lValue;
            
            if(!(lError = VmbFeatureIntGet(aHandle,aLabel,&lValue)))
                printf("\tvalue\t\t\t= %lld\n",lValue);      
            else
                printf("\tvalue\t\t\t= FAILED (%d)\n",lError); 
                
            break;     
        }
        case VmbFeatureDataFloat:
        { 
            double lValue;
            
            if(!(lError = VmbFeatureFloatGet(aHandle,aLabel,&lValue)))
                printf("\tvalue\t\t\t= %lf\n",lValue);      
            else
                printf("\tvalue\t\t\t= FAILED (%d)\n",lError);            
        
            break;
        }  
        case VmbFeatureDataString: 
        {
            char lString[256];
        
            if(!(lError = VmbFeatureStringGet(aHandle,aLabel,lString,256,NULL)))
                printf("\tvalue\t\t\t= %s\n",lString);      
            else
                printf("\tvalue\t\t\t= FAILED (%d)\n",lError);                 
        
            break;
        }
        case VmbFeatureDataEnum: 
        {
            const char* lValue;
        
            if(!(lError = VmbFeatureEnumGet(aHandle,aLabel,&lValue)))
                printf("\tvalue\t\t\t= %s\n",lValue);      
            else
                printf("\tvalue\t\t\t= FAILED (%d)\n",lError);                 
        
            break;
        }               
        case VmbFeatureDataBool:
        { 
            bool lValue;
            
            if(!(lError = VmbFeatureBoolGet(aHandle,aLabel,&lValue)))
                printf("\tvalue\t\t\t= %s\n",lValue ? "true" : "false");      
            else
                printf("\tvalue\t\t\t= FAILED (%d)\n",lError);            
        
            break;
        }        
        default:        
            printf("\tvalue\t\t\t= n/a\n");  
    }
    
    return lError;
}

void PrintFeatureFlags(VmbFeatureFlags_t aFlags)
{
    printf("\tfeatureFlags\t\t= ");
    
    if(aFlags & VmbFeatureFlagsRead)
        printf("read ");
    if(aFlags & VmbFeatureFlagsWrite)
        printf("write ");
    if(aFlags & VmbFeatureFlagsConst)
        printf("const ");
    if(aFlags & VmbFeatureFlagsVolatile)
        printf("volatile ");
    if(aFlags & VmbFeatureFlagsModifyWrite)
        printf("modifyWrite");
        
    printf("\n");
}

void PrintFeatureImpact(VmbHandle_t aHandle,const VmbFeatureInfo_t* aInfo)
{
    if(aInfo->hasAffectedFeatures)
    {
        VmbFeatureInfo_t lFeatures[256];
        VmbUint32_t      lFound;

        printf("\taffectedFeatures\t= ");

        if(!VmbFeatureListAffected(aHandle,aInfo->name,lFeatures,256,&lFound,sizeof(VmbFeatureInfo_t)))
            for(VmbUint32_t i=0;i<lFound;i++)
                printf("%s ",lFeatures[i].name);  

        printf("\n");
    }       
}

void ListAllFeaturesInfo(VmbHandle_t aHandle)
{
    VmbUint32_t lFound;

    if(!VmbFeaturesList(aHandle,NULL,0,&lFound,sizeof(VmbFeatureInfo_t)))
    {    
        VmbFeatureInfo_t* lFeatures = new VmbFeatureInfo_t[lFound];

        printf("%u features found\n",lFound);        

        if(!VmbFeaturesList(aHandle,lFeatures,lFound,NULL,sizeof(VmbFeatureInfo_t)))
        {
            for(VmbUint32_t i=0;i<lFound;i++)
            {
                printf("%s/%s\n",lFeatures[i].category,lFeatures[i].name);
                
                PrintFeatureValue(aHandle,lFeatures[i].name,lFeatures[i].featureDataType);
                PrintFeatureRange(aHandle,lFeatures[i].name,lFeatures[i].featureDataType);
                PrintFeatureIncrement(aHandle,lFeatures[i].name,lFeatures[i].featureDataType);
                PrintFeatureFlags(lFeatures[i].featureFlags);
                PrintFeatureImpact(aHandle,&lFeatures[i]);
                
                printf("\tdisplayName\t\t= %s\n",lFeatures[i].displayName ? lFeatures[i].displayName : "n/a");
                printf("\tfeatureDataType\t\t= %s\n",DataType2String(lFeatures[i].featureDataType));
                printf("\tpollingTime\t\t= %u\n",lFeatures[i].pollingTime);
                printf("\tunit\t\t\t= %s\n",lFeatures[i].unit ? lFeatures[i].unit : "n/a");
                printf("\trepresentation\t\t= %s\n",lFeatures[i].representation ? lFeatures[i].representation : "n/a");
                printf("\tvisibility\t\t= %s\n",Visibility2String(lFeatures[i].visibility));
                printf("\tdeprecated\t\t= %s\n",lFeatures[i].isDeprecated ? "yes" : "no");
                printf("\ttooltip\t\t\t= %s\n",lFeatures[i].tooltip ? lFeatures[i].tooltip : "n/a");
                printf("\tdescription\t\t= %s\n",lFeatures[i].description ? lFeatures[i].description : "n/a");
                printf("\tsfncNamespace\t\t= %s\n",lFeatures[i].sfncNamespace ? lFeatures[i].sfncNamespace : "n/a");
            }
        } 
        else
            printf("failed to retreive list of features\n");
            
        delete [] lFeatures;     
    } 
    else
        printf("failed to retreive list of features\n"); 
}

bool doStuffWithCamera(int argc,char **argv)
{
        VmbHandle_t lHandle = NULL;
        int        lTries  = 0;   
    bool        lDone   = false;   
        
        printf("waiting for camera ...\n"); 
          
        while(lTries++ < 4 && VmbCameraOpen(argv[1],VmbAccessModeFull,&lHandle))
            Sleep(250);
        
        if(lHandle)
        {
            printf("camera opened!!\n");
            
            ListAllFeaturesInfo(lHandle);
            
            //PrintFeatureValue(lHandle,"DeviceID",VmbFeatureDataString);
            
            printf("closing the camera!\n");
            
            VmbCameraClose(lHandle);
            
            printf("camera closed\n");
        
        lDone = true;
        }
        else
            printf("failed to open the camera ... \n");  
        
    return lDone;         
}

bool doStuffWithInterface(int argc,char **argv)
{
    VmbHandle_t lHandle = NULL;
    int         lTries  = 0;  
    bool        lDone   = false;    
    
    printf("waiting for interface ...\n"); 
      
    while(lTries++ < 4 && VmbInterfaceOpen(argv[1],&lHandle))
        Sleep(250);
    
    if(lHandle)
    {
        printf("interface opened!!\n");
        
        ListAllFeaturesInfo(lHandle);
                    
        printf("closing the interface!\n");
        
        VmbInterfaceClose(lHandle);
        
        printf("interface closed\n");
        
        lDone = true;
    } 
    else
        printf("failed to open the interface ... \n");
       
    return lDone;               
}

int main(int argc,char **argv)
{
    if(VmbStartup() == VmbErrorSuccess)
    {   
        VmbFeatureCommandRun(gVimbaHandle,"GeVDiscoveryAllAuto");

        Sleep(500);
        
        if(argc < 2)
            ListAllFeaturesInfo(gVimbaHandle);
        else
            if(!doStuffWithInterface(argc,argv))
                doStuffWithCamera(argc,argv);

        VmbShutdown();
    }
    else
        printf("failed to startup API\n");
}
