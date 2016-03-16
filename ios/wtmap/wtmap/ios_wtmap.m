//
//  ios_wtmap.m
//  wtmap
//
//  Created by Nicholas Wasilewski on 3/15/16.
//  Copyright © 2016 Nicholas Wasilewski. All rights reserved.
//

#import "ios_wtmap.h"

#import <mach/mach.h>
#import <mach/mach_init.h>
#import <mach/mach_time.h>
#import <mach/vm_map.h>

#import "wtmap_platform.h"

ios_state State;
ios_offscreen_buffer OffScreenBuffer;
game_memory GameMemory;
CADisplayLink *DisplayLink;

@interface wtmapViewController : UIViewController
@end

@implementation wtmapViewController

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
    return UIInterfaceOrientationLandscapeRight;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscapeRight;
}

- (BOOL)shouldAutorotate {
    return YES;
}

@end

@implementation AppDelegate

-(void)doFrame:(CADisplayLink*)sender {
    static uint64_t lastTime = 0;
    static float machToNano = 0.0;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(OffScreenBuffer.memory,
                                                 OffScreenBuffer.width,
                                                 OffScreenBuffer.height,
                                                 OffScreenBuffer.bytesPerPixel*8/4,
                                                 OffScreenBuffer.pitch,
                                                 colorSpace,
                                                 (CGBitmapInfo)(kCGImageAlphaPremultipliedFirst
                                                                | kCGBitmapByteOrder32Little));
    CGColorSpaceRelease(colorSpace);
    
    if (context)
    {
        //paint it black
        CGContextSetRGBFillColor(context, 0, 0, 0, 1);
        CGContextFillRect(context, CGRectMake(0, 0, OffScreenBuffer.width, OffScreenBuffer.height));
        
        game_screen_buffer GameBuffer;
        GameBuffer.Memory = OffScreenBuffer.memory;
        GameBuffer.Width = OffScreenBuffer.width;
        GameBuffer.Height = OffScreenBuffer.height;
        GameBuffer.Pitch = OffScreenBuffer.pitch;
        GameBuffer.BytesPerPixel = OffScreenBuffer.bytesPerPixel;

        game_input GameInput = {0};
        uint64_t now = mach_absolute_time();
        if (machToNano == 0.0) {
            mach_timebase_info_data_t sTimebaseInfo;
            mach_timebase_info(&sTimebaseInfo);
            machToNano = (float)sTimebaseInfo.numer / (float)sTimebaseInfo.denom;
        }
        GameInput.dt = (float)(now - lastTime) * machToNano / (float)1000000000;
        lastTime = now;
        
        UpdateAndRender(&GameInput, &GameInput, &GameMemory, &GameBuffer);
        CGImageRef image = CGBitmapContextCreateImage(context);
        CGContextRelease(context);
        self.window.layer.contents = (__bridge id)image;
//        self.window.layer.contents = CFBridgingRelease(image);
        CGImageRelease(image);
    }
    else
    {
        NSLog(@"Can't create graphics context, we can just stop and cry here\n");
    }
}

- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)
launchOptions {
    self.window = [[UIWindow alloc] init];
    self.window.rootViewController = [[wtmapViewController alloc] init];
    [self.window makeKeyAndVisible];
    self.window.frame = [[UIScreen mainScreen] bounds];
    
    State.pointToPixelScale = [UIScreen mainScreen].scale;
    CGSize screen = [[UIScreen mainScreen] nativeBounds].size;
    int pixelWidth = (int)(screen.width + 0.5);
    int pixelHeight = (int)(screen.height + 0.5);
    if (pixelWidth < pixelHeight) {
        int tmp = pixelWidth;
        pixelWidth = pixelHeight;
        pixelHeight = tmp;
    }
    int bytesPerPixel = 4;
    
    OffScreenBuffer.height = pixelHeight;
    OffScreenBuffer.width = pixelWidth;
    OffScreenBuffer.bytesPerPixel = bytesPerPixel;
    OffScreenBuffer.pitch = pixelWidth * bytesPerPixel;
    
    GameMemory.PermanentStorageSize = Megabytes(256);
    GameMemory.TransientStorageSize = Megabytes(64);
    GameMemory.IsInitialized = 0;
    
    size_t backBufferSize = pixelWidth*pixelHeight*bytesPerPixel;
    size_t gameStorageSize = (size_t)GameMemory.PermanentStorageSize + (size_t)GameMemory.TransientStorageSize;
    
    State.totalMemorySize = gameStorageSize + backBufferSize;
    State.storageMemorySize = gameStorageSize;
    
    vm_address_t baseAddress;
    kern_return_t result = vm_allocate(
                                       (vm_map_t)mach_task_self(),
                                       &baseAddress,
                                       (vm_size_t)(State.totalMemorySize),
                                       (boolean_t)1);
    if (result != KERN_SUCCESS)
    {
        NSLog(@"memory not allocated...here's where you panic a little and then run away");
    }
    
    State.memory = State.storageMemory = (void *)baseAddress;
    
    GameMemory.PermanentStorage = (void*)baseAddress;
    GameMemory.TransientStorage = (void*)((uint8_t *)baseAddress+GameMemory.PermanentStorageSize);
    OffScreenBuffer.memory = (void*)((uint8_t*)baseAddress +
                                     GameMemory.PermanentStorageSize +
                                     GameMemory.TransientStorageSize);
    
    
    DisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(doFrame:)];
    DisplayLink.frameInterval = 2;
    [DisplayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    
    return YES;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
}

- (void)applicationWillTerminate:(UIApplication *)application {
    
}

@end