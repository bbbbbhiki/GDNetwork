//
//  GDNetworkManager.m
//  GDNetwork
//
//  Created by Journey on 2017/11/24.
//  Copyright © 2017年 GoDap. All rights reserved.
//

#import "GDNetworkManager.h"
#import "GDBaseRequest.h"
#import "GDNetworkConfiguration.h"
#import <AFNetworking/AFURLSessionManager.h>
#import <AFNetworking/AFURLRequestSerialization.h>
#import <pthread.h>

@interface GDNetworkManager (){
    pthread_mutex_t _lock;
}

@property (nonatomic, strong) AFURLSessionManager *sessionManager;

@property (nonatomic, strong) GDNetworkConfiguration *configuration;

///保存已经请求 task,防止再次发送
@property (nonatomic, strong) NSMutableSet<GDBaseRequest *> *requests;

@end

@implementation GDNetworkManager

+ (instancetype)defaultManager {
    static GDNetworkManager *defaultManager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        GDNetworkConfiguration *defaultConfiguration = [GDNetworkConfiguration defaultConfiguration];
        defaultManager = [[GDNetworkManager alloc]initWithConfiguration:defaultConfiguration];
    });
    return defaultManager;
}

+ (instancetype)engineWithConfiguration:(GDNetworkConfiguration *)configuration {
    return [[self alloc] initWithConfiguration:configuration];
}

- (instancetype)initWithConfiguration:(GDNetworkConfiguration *)configurtion {
    self = [super init];
    if (self) {
        pthread_mutex_init(&_lock, NULL);
        
        _configuration = configurtion;
        _requests = [NSMutableSet<GDBaseRequest *> set];
        _sessionManager = [[AFURLSessionManager alloc] initWithSessionConfiguration:configurtion.sessionConfiguration];
        _sessionManager.securityPolicy = configurtion.securityPolicy;
        _sessionManager.completionGroup = configurtion.completionGroup;
        _sessionManager.completionQueue = configurtion.completionQueue;
        _sessionManager.responseSerializer = [AFHTTPResponseSerializer serializer];
        
        [self setUpRedirectionBlock];
    }
    return self;
}

#pragma mark - main method

- (void)startRequest:(GDBaseRequest *)request {
    BOOL success = self rec
}



#pragma mark - --------- Record Request ---------

/**
 判断请求是否在队列中
 
 @param request 要判断的请求
 @return 是否在队列中
 */
- (BOOL)isRequestInQueue:(GDBaseRequest *)request {
    pthread_mutex_lock(&_lock);
    BOOL isRequestInQueue = [self.requests containsObject:request];
    pthread_mutex_unlock(&_lock);
    return isRequestInQueue;
}

/**
 记录已经开始的请求
 
 @param request 要记录的请求
 @return 记录是否成功，如果请求已经开始就会记录失败
 */
- (BOOL)recordRequest:(GDBaseRequest *)request {
    if (!request) {
        return NO;
    }
    
    pthread_mutex_lock(&_lock);
    BOOL shouldRecordRequest = ![self.requests containsObject:request];
    if (shouldRecordRequest) {
        [self.requests addObject:request];
    }
    pthread_mutex_unlock(&_lock);
    
    return shouldRecordRequest;
}


/**
 移除记录的请求，一般是在请求结束时移除
 
 @param request 请求
 */
- (void)removeRequest:(GDBaseRequest *)request {
    if (!request) {
        return;
    }
    
    pthread_mutex_lock(&_lock);
    [self.requests removeObject:request];
    pthread_mutex_unlock(&_lock);
}

/// 清空变量 初始化internal
- (void)prepareForRequest:(GDBaseRequest *)request {
//    request.response = nil;
//    request.cacheHited = NO;
//    request.cacheError = nil;
//    request.responseError = nil;
//    request.responseData = nil;
//    
//    request.internalRequest = [[__ZBJBaseRequestInternal alloc] init];
//    request.internalRequest.request = request;
//    request.internalRequest.engine = self;
//    [request.internalRequest prepare];
}

#pragma mark - HTTP Redirection

- (void)setUpRedirectionBlock {
    __weak typeof(self) weakSelf = self;
    
    NSURLRequest *(^redirectionBlock)(NSURLSession *, NSURLSessionTask *, NSURLResponse *, NSURLRequest *) = ^(NSURLSession * _Nonnull session, NSURLSessionTask * _Nonnull task, NSURLResponse * _Nonnull response, NSURLRequest * _Nonnull request) {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (!strongSelf) {
            return request;
        }
        
        pthread_mutex_lock(&strongSelf->_lock);
        NSSet<GDBaseRequest *> *requests = strongSelf.requests.copy;
        pthread_mutex_unlock(&strongSelf->_lock);
        
        __block GDBaseRequest *gdRequest = nil;
        [requests enumerateObjectsUsingBlock:^(GDBaseRequest * _Nonnull obj, BOOL * _Nonnull stop) {
            if (obj.task == task) {
                gdRequest = obj;
                *stop = YES;
            }
        }];
        
        if (gdRequest && gdRequest.redirectionBlock) {
            __block NSURLRequest *redirectRequest = nil;
            dispatch_async(dispatch_get_main_queue(), ^{
                redirectRequest = gdRequest.redirectionBlock(session,task,response,request);
            });
            return redirectRequest;
        }
        
        return request;
    };
    
    [self.sessionManager setTaskWillPerformHTTPRedirectionBlock:redirectionBlock];
}

#pragma mark - dealloc

- (void)dealloc {
    pthread_mutex_destroy(&_lock);
}

@end
