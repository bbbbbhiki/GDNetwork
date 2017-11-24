//
//  GDBaseRequest.h
//  GDNetwork
//
//  Created by Journey on 2017/11/22.
//  Copyright © 2017年 GoDap. All rights reserved.
//  本类用于配置基础信息

#import <Foundation/Foundation.h>

@class GDBaseRequest;
@class GDNetworkManager;

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


#pragma mark - BaseRequestInfo

@interface GDBaseRequest (BaseRequestInfo)

- (GDNetworkManager *)requestManager;

- (NSString *)requestInfo;

- (nullable NSString *)baseUrl;

///请求参数
- (nullable id)requestArgument;

/**
 请求方法
 
 @note 默认为`GDRequestMethodGET`
 TODO 默认的应该改成 post
 */
- (GDRequestMethod)requestMethod;

/**
 请求序列化方式
 
 @note 默认为`GDHTTPRequestSerialization`
 */
- (GDRequestSerialization)requestSerializer;

/**
 请求的表单数据
 
 @warning 只有`ZBJRequestMethodPOST`请求才允许带表单数据
 */
//- (nullable NSArray<GDNetworkFormData *> *)requestFormData;

/**
 自定义请求
 
 @warning requestUrl／baseUrl／reuquestArgument／reuquestArgument／requestMethod／
 requestSerializer/requestFormData属性会在Override customeRequest之后失效
 
 @note 默认为nil
 */
- (nullable NSURLRequest *)customeRequest;

/**
 请求的优先级
 
 @note 默认为`GDRequestPriorityDefault`
 */
- (GDRequestPriority)requestPriority;

/**
 请求验证信息
 @note 默认为nil
 */
/// TODO 暂时没有发现 Godap有这种需求
//- (nullable NSString *)requestAuthorizationUsername;
//- (nullable NSString *)requestAuthorizationPassword;

/**
 请求自定义HTTP头
 
 @note 默认为nil
 */
- (nullable NSDictionary *)requestHeaderFieldValueDictionary;

/**
 请求超时时间
 
 @note 默认为60s
 */
- (NSTimeInterval)requestTimeOutInterval;

/**
 返回值序列化方式
 
 @note 默认为`ZBJStringResponseSerialization`
 */
- (GDResponseSerialization)responseSerializer;

/**
 请求成功回调
 请求成功之后会首先执行这个方法，可以在方法内部进行数据Model化
 */
- (void)requestSuccessFilter;

/**
 请求失败回调
 请求失败之后会首先执行这个方法，可以在方法内部进行异常处理
 */
- (void)requestFailureFilter;

/**
 JSON校验工具
 
 @code
 - (nullable id)jsonValidator {
 return @[@{@"name":[NSNumber class]}]]
 }
 @endcode
 
 @note 判断返回JSON数据进行值以及类型或者校验，如果校验失败，则请求被认定为请求失败
 */
- (nullable id)jsonValidator;

/**
 返回校验
 
 @description 可以在此方法中进行返回数据的校验，例如状态码校验,responseValidator比jsonValidator有更高的优先级，
 若responseValidator校验失败则认为请求失败并且执行请求失败的回调
 
 @note 默认是对状态码进行校验，状态码在200~299范围内才认为请求成功
 */
- (BOOL)responseValidator;

@end

#pragma mark - Upload Request
/// TODO 暂时无上传需求，日后有需求再补充

#pragma mark - Download request
/// TODO 暂时无下载需求，日后有需求再补充

#pragma mark - Cache

@interface GDBaseRequest (Cache)
/// 缓存命中回调 block
@property (nonatomic, copy, nullable) GDRequestCompletionBlock cacheHitedBlock;
///是否命中了缓存
@property (nonatomic, assign, readonly, getter=isCacheHited) BOOL cacheHited;
///缓存错误信息
@property (nonatomic, strong, readonly, nullable) NSError *cacheError;

/**
 添加Cache回调的Target-Action
 
 @description 可以设置多个target-action回调
 
 @param target target
 @param action action
 */
- (void)addTarget:(nullable id)target cacheHittedAction:(SEL)action;

/**
 请求缓存命中回调
 请求缓存命中之后会首先执行这个方法，可以在方法内部进行数据Model化
 */
- (void)requestCacheHittedFilter;

/**
 设置请求的成功、请求缓存命中以及请求失败的回调Block并且发起请求
 
 @description 本方法会考虑请求的缓存
 
 @param successBlock     请求成功的回调Block
 @param cacheHittedBlock 请求缓存命中的回调Block
 @param failureBlock     请求失败的回调Block
 */
- (void)startWithSuccessBlock:(nullable GDRequestCompletionBlock)successBlock
             cacheHittedBlock:(nullable GDRequestCompletionBlock)cacheHittedBlock
                 failureBlock:(nullable GDRequestCompletionBlock)failureBlock;

/**
 是否忽略缓存
 
 @note 默认为`YES`
 */
- (BOOL)ignoreCache;

/**
 决定缓存命中的时候是否仍进行请求
 
 @note 默认为`NO`，会进行请求
 
 @warning 如果返回`YES`,则回调是走请求成功；如果返回`NO`,则回调是先走缓存命中，请求完成之后会走请求成功
 */
- (BOOL)invalidateRequestWhileCacheHited;

/**
 @description 请求的method，url，cacheSensitiveArgument将决定请求缓存文件名，再去通过缓存文件名去查找缓存文件。
 可以断定三个元素之中有任何一个不一样就认定为改请求没有缓存
 @note 默认为`nil`
 */
- (nullable id)cacheSensitiveArgument;

/**
 缓存保存时间
 
 @note 默认为`0`
 */
- (NSTimeInterval)cacheTimeInSeconds;

/**
 缓存的版本号
 
 @note 默认为`0`
 */
- (long long)cacheVersion;

/**
 清空当前版本缓存
 */
- (BOOL)cleanCache;


@end







NS_ASSUME_NONNULL_END
