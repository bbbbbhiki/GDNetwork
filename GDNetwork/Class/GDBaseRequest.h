//
//  GDBaseRequest.h
//  GDNetwork
//
//  Created by Journey on 2017/11/22.
//  Copyright © 2017年 GoDap. All rights reserved.
//  本类用于配置基础信息

#import <Foundation/Foundation.h>

@class GDBaseRequest;

NS_ASSUME_NONNULL_BEGIN

#pragma mark - Error declaration

FOUNDATION_EXPORT NSString *const GDNetworkErrorDomain;

typedef NS_ENUM(NSInteger, GDNetWorkErrorCode) {
    /// Json 数据校验错误
    GDNetWorkErrorCodeJSONValidateError
};

# pragma mark - Enum declaration

typedef NS_ENUM(NSInteger, GDRequestMethod) {
    /// GET 请求
    GDRequestMethodGET,
    /// POST请求
    GDRequestMethodPOST,
    /// HEAD请求
    GDRequestMethodHEAD,
    /// PUT请求
    GDRequestMethodPUT,
    /// PATCH请求
    GDRequestMethodPATCH,
    /// DELETE请求
    GDRequestMethodDELETE
};

#pragma mark - Enum priority

typedef NS_ENUM(NSInteger, GDRequestPriority) {
    /// 低优先级
    GDRequestPriorityLow,
    /// 默认优先级
    GDRequestPriorityDefault,
    /// 高优先级
    GDRequestPriorityHigh
};

#pragma mark - Enum requestState

typedef NS_ENUM(NSInteger, GDRequestState) {
    /// 请求准备中
    GDRequestStatePrepared = 0,
    /// 请求中
    GDRequestStateRunning,
    /// 请求暂停
    GDRequestStateSuspended,
    /// 请求取消
    GDRequestStateCanceled,
    /// 请求完成
    GDRequestStateCompleted
};

#pragma mark - request serialization

typedef NS_ENUM(NSInteger, GDRequestSerialization) {
    /** 默认序列化
     * 请求ContentType为`application/x-www-form-urlencoded`,
     * 请求HTTPBody为name=xxx&password=xxx
     */
    GDHTTPRequestSerialization,
    /** JSON序列化
     * 请求ContentType为`application/json`
     * 请求HTTPBody为{"name":xxx,"password":xxx}
     */
    GDJSONRequestSerialization,
    /** 全局加密
     * 请求Content-Type为`application/octet-stream`
     * 请求HTTPBody为全局AES加密数据
     */
    GDEncryptionRequestSerialization
};

#pragma mark - response serialization

typedef NS_ENUM(NSInteger, GDResponseSerialization) {
    /** 默认序列化
     * 返回数据为UTF-8编码的NSString对象
     */
    GDStringResponseSerialization,
    /** JSON序列化
     * 返回数据反序列化为JSONObject
     */
    GDJSONResponseSerialization,
    /** 全局加密序列化
     * 返回数据`解密`之后反序列化为JSONObject
     */
    ZBJDecryptionResponseSerialization,
    /** 不进行序列化操作
     * 返回数据不做任何处理
     */
    GDDataResponseSerialization
};

#pragma mark - Block declaration

/// 请求进度block
typedef void (^GDRequestProgressBlock)(__kindof NSProgress *progress);
/// 请求结束回调block
typedef void (^GDRequestCompletionBlock)(__kindof GDBaseRequest *request);
/// 重定向block
typedef NSURLRequest *_Nullable (^GDRequestWillRedirectionBlock)(NSURLSession * _Nonnull session, NSURLSessionTask *_Nonnull task, NSURLResponse *_Nonnull response, NSURLRequest *_Nonnull redirectRequest);

#pragma mark - GDRequestDelegate

@protocol GDRequestDelegate <NSObject>

@optional

/**
 @brief 缓存命中的回调
 
 @param request 对应的请求
 
 @discussion 对于`缓存命中就不进行网络连接的请求`，请求最后执行的代理是本代理。zbjRequestSucceed并不会执行。
 @see -invalidateRequestWhileCacheHited
 */
- (void)gdRequestCatched:(__kindof GDBaseRequest *)request;

/**
 请求成功的回调
 
 @param request 对应的请求
 */
- (void)gdRequestSucceed:(__kindof GDBaseRequest *)request;

/**
 请求失败的回调
 
 @param request 对应的请求
 */
- (void)gdRequestFailed:(__kindof GDBaseRequest *)request;

@end

#pragma mark - GDRequesAccessory

@protocol GDRequestAccessory <NSObject>

@optional

/**
 @brief 请求即将开始执行，接下来会首先进行请求属性的配置以及缓存的判定
 
 @param request 相应的请求
 */
- (void)gdRequestWillStart:(__kindof GDBaseRequest *)request;

/**
 @brief 请求开始执行
 
 @param request 相应的请求
 */
- (void)gdRequestDidStart:(__kindof GDBaseRequest *)request;

/**
 @brief 请求即将结束，此时请求错误，请求数据等并未开始赋值
 
 @param request 相应的请求
 */
- (void)gdRequestWillStop:(__kindof GDBaseRequest *)request;

/**
 @brief 请求结束执行
 
 @param request 对应的请求
 
 @discussion 请求结束的回调是在请求结束的委托对象[delegate -gdRequestSuccess]之后执行的
 */
- (void)gdRequestDidStop:(__kindof GDBaseRequest *)request;

@end


@interface GDBaseRequest : NSObject

@property (nonatomic, strong, readonly, nullable) NSURLRequest *request;

@property (nonatomic, strong, readonly, nullable) NSURLResponse *response;

@property (nonatomic, strong, readonly, nullable) NSURLSessionTask *task;

@property (nonatomic, strong, readonly, nullable) NSData *responseData;

/**
 * er
 */
@property (nonatomic, copy, readonly, nullable) NSString *responseString;

/**
 二进制数据经过`GDJSONResponseSerialization`序列化之后的对象
 */
@property (nonatomic, strong, readonly, nullable) id responseJSONObject;

/**
 请求错误信息
 @see GDNetworkErrorCode
 */
@property (nonatomic, strong, readonly, nullable) NSError *responseError;

/// HTTP请求的状态码
@property (nonatomic, assign, readonly) NSInteger responseStatusCode;

/// 请求所处的状态
@property (nonatomic, readonly, assign) GDRequestState requestState;

/**
 * 发起请求
 * @description 对于已经发起的请求，并且该请求并未结束，执行本方法将不会重新执行请求。
   对于配置了缓存的请求，执行此方法会首先检查缓存，如果缓存命中，则会返回缓存数据。
 */
- (void)start;

/**
 * 无论有没有缓存都发起请求
 */
- (void)startWithOutCache;

/*
 * 暂停请求
 */
- (void)suspend;

/*
 * 恢复请求
 */
- (void)resume;

/*
 * 取消请求
 */
- (void)cancel;

@property (nonatomic, weak, nullable) id <GDRequestDelegate> delegate;

@property (nonatomic, copy, nullable) GDRequestCompletionBlock successBlock;

@property (nonatomic, copy, nullable) GDRequestCompletionBlock failedBlock;

@property (nonatomic, copy, nullable) GDRequestProgressBlock uploadProgressBlock;

@property (nonatomic, copy, nullable) GDRequestWillRedirectionBlock redirectionBlock;


///请求成功/失败回调
- (void)startWithSuccessBlock:(nullable GDRequestCompletionBlock)successBlock
                 failureBlock:(nullable GDRequestCompletionBlock)failureBlock;

/// 请求状态的代理，主要回调请求开始于结束的状态
@property (nonatomic, readonly, nullable) NSMutableSet<id<GDRequestAccessory> > *requestAccessories;

/**
 添加请求状态的代理
 
 @param requestAccessory 请求状态代理对象
 */
- (void)addReuestAccessory:(id<GDRequestAccessory>)requestAccessory;


@end







NS_ASSUME_NONNULL_END
