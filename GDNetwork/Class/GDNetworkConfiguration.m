//
//  GDNetworkConfiguration.m
//  GDNetwork
//
//  Created by Journey on 2017/11/24.
//  Copyright © 2017年 GoDap. All rights reserved.
//

#import "GDNetworkConfiguration.h"
#import "AFSecurityPolicy.h"

@implementation GDNetworkConfiguration

+ (instancetype)defaultConfiguration {
    GDNetworkConfiguration *defualtConfiguration = [[GDNetworkConfiguration alloc]initWithSessionConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]];
    return defualtConfiguration;
}

+ (instancetype)networkConfigurationWithSessionConfiguration:(NSURLSessionConfiguration *)sessionConfiguration {
    GDNetworkConfiguration *configurtion = [[GDNetworkConfiguration alloc] initWithSessionConfiguration:sessionConfiguration];
    return configurtion;
}


- (instancetype)initWithSessionConfiguration:(NSURLSessionConfiguration *)sessionConfiguration {
    self = [super init];
    if(self){
        _baseUrl = nil;
        _downloadDirectory = nil;
        _completionGroup = nil;
        _downloadDirectory = [NSURL fileURLWithPath:[self getDefaultDownloadDirectory] isDirectory:YES];
        _downloadResumeDirectory = [NSURL fileURLWithPath:[self getDownloadResumeDirectory] isDirectory:YES];
        _cacheDirectory = [NSURL fileURLWithPath:[self getDefaultCachedDirectory] isDirectory:YES];
        _securityPolicy = [AFSecurityPolicy policyWithPinningMode:AFSSLPinningModeNone];
        _sessionConfiguration = sessionConfiguration;
        
        NSString *addressIdentifier = [NSString stringWithFormat:@"%p",self];
        addressIdentifier = [addressIdentifier stringByReplacingOccurrencesOfString:@"0x" withString:@""];
        NSString *requestQueueIdentifier = [NSString stringWithFormat:@"me.dingtonge.requestConcurrentQueue.%@",addressIdentifier];
        NSString *responseQueueIdentifier = [NSString stringWithFormat:@"me.dingtonge.responseConcurrentQueue.%@",addressIdentifier];
        
        _requestQueue = dispatch_queue_create([requestQueueIdentifier UTF8String], DISPATCH_QUEUE_CONCURRENT);
        _completionQueue = dispatch_queue_create([responseQueueIdentifier UTF8String], DISPATCH_QUEUE_CONCURRENT);
        _requestAccessories = [NSMutableSet<id<GDRequestAccessory>> setWithCapacity:1];
    }
    return self;
}

- (NSString *)getDefaultDownloadDirectory{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *documentDirectory = [paths objectAtIndex:0];
    NSString *downloadDirectory = [NSString stringWithFormat:@"%@/GDNetwork/DownloadDirectory", documentDirectory];
    
    NSFileManager *manager = [NSFileManager defaultManager];
    if(![manager fileExistsAtPath:downloadDirectory])
    {
        NSError *error = nil;
        [manager createDirectoryAtPath:downloadDirectory
           withIntermediateDirectories:YES
                            attributes:nil
                                 error:&error];
    }
    return downloadDirectory;
}

- (NSString *)getDownloadResumeDirectory {
    NSString *downloadDirectory = _downloadDirectory.path;
    NSString *downloadResumeDirectory = [NSString stringWithFormat:@"%@/resume", downloadDirectory];
    
    NSFileManager *manager = [NSFileManager defaultManager];
    if(![manager fileExistsAtPath:downloadResumeDirectory])
    {
        NSError *error = nil;
        [manager createDirectoryAtPath:downloadResumeDirectory
           withIntermediateDirectories:YES
                            attributes:nil
                                 error:&error];
    }
    return downloadResumeDirectory;
}

- (NSString *)getDefaultCachedDirectory {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *documentDirectory = [paths objectAtIndex:0];
    NSString *cachedDirectory = [NSString stringWithFormat:@"%@/GDNetwork/CachedDirectory", documentDirectory];
    
    NSFileManager *manager = [NSFileManager defaultManager];
    if(![manager fileExistsAtPath:cachedDirectory])
    {
        NSError *error = nil;
        [manager createDirectoryAtPath:cachedDirectory
           withIntermediateDirectories:YES
                            attributes:nil
                                 error:&error];
    }
    return cachedDirectory;
}

-(NSMutableSet<id<GDRequestAccessory>> *)requestAccessories {
    @synchronized (_requestAccessories) {
        return _requestAccessories;
    }
}

-(void)addReuestAccessory:(id<GDRequestAccessory>)requestAccessory {
    @synchronized (_requestAccessories) {
        [_requestAccessories addObject:requestAccessory];
    }
}

#pragma mark - 添加全局状态代理

static NSMutableSet<id<GDRequestAccessory>> *gd_network_globalRequestAccessories = nil;
static NSString *kGDNetworkGlobalAccessoryIdentifier = @"kGDNetworkGlobalAccessoryIdentifier";

+ (NSMutableSet<id<GDRequestAccessory>> *)globalRequestAccessories {
    @synchronized (kGDNetworkGlobalAccessoryIdentifier) {
        return gd_network_globalRequestAccessories;
    }
}

+ (void)setGlobalRequestAccessories:(NSMutableSet<id<GDRequestAccessory>> *)globalRequestAccessories {
    @synchronized (kGDNetworkGlobalAccessoryIdentifier) {
        gd_network_globalRequestAccessories = globalRequestAccessories;
    }
}

+ (void)addGlobalReuestAccessory:(id<GDRequestAccessory>)globalRequestAccessories {
    @synchronized (kGDNetworkGlobalAccessoryIdentifier) {
        if (!gd_network_globalRequestAccessories) {
            gd_network_globalRequestAccessories = [[NSMutableSet<id<GDRequestAccessory>> alloc] initWithCapacity:1];
        }
        
        [gd_network_globalRequestAccessories addObject:globalRequestAccessories];
    }
}

@end
