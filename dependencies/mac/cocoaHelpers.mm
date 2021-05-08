#ifdef __APPLE__

#import <Foundation/Foundation.h>
#include "cocoaHelpers.hpp"

const char* getResourcesPath(void)
{
    @autoreleasepool
    {
        //NSString* resource_path = [[NSBundle mainBundle] resourcePath];
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString *applicationSupportDirectory = [paths firstObject];
        
        return (char*)[applicationSupportDirectory UTF8String];
    }
}
#endif
