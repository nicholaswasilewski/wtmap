//
//  ios_wtmap.h
//  wtmap
//
//  Created by Nicholas Wasilewski on 3/15/16.
//  Copyright © 2016 Nicholas Wasilewski. All rights reserved.
//

#ifndef ios_wtmap_h
#define ios_wtmap_h

#import <UIKit/UIKit.h>


typedef struct {
    void *memory;
    int width;
    int height;
    int pitch;
    int bytesPerPixel;
} ios_offscreen_buffer;

typedef struct {
    size_t totalMemorySize;
    size_t storageMemorySize;
    void *memory;
    void *storageMemory;
    float pointToPixelScale;
} ios_state;

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;

@end

#endif /* ios_wtmap_h */
