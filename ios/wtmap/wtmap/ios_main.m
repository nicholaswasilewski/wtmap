//
//  ios_main.m
//  wtmap
//
//  Created by Nicholas Wasilewski on 3/15/16.
//  Copyright © 2016 Nicholas Wasilewski. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ios_wtmap.h"

@interface wtmapApp : UIApplication
@end

@implementation wtmapApp
//could read touch events in this class later
@end


int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, NSStringFromClass([wtmapApp class]), NSStringFromClass([AppDelegate class]));
    }
}
