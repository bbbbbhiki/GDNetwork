//
//  GDBaseRequest.m
//  GDNetwork
//
//  Created by Journey on 2017/11/22.
//  Copyright © 2017年 GoDap. All rights reserved.
//

#import "GDBaseRequest.h"
#import "GDNetworkManager.h"
#import "GDNetworkConfiguration.h"

@implementation GDBaseRequest

- (instancetype)init {
    self = [super init];
    if(!self) {
        return nil;
    }
    
    //_requesta = [NSMutableSet<id<GDRequestAccessory>> setWithCapacity:1];
    return self;
}

//-(NSMutableSet<id<GDRequestAccessory>> *)requestAccessories {
//    @synchronized (_req) {
//        return _requestAccessories;
//    }
//}

/// 开始请求
- (void)start {
    
}



@end
