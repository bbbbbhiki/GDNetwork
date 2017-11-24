//
//  GDNetworkConfiguration.h
//  GDNetwork
//
//  Created by Journey on 2017/11/24.
//  Copyright © 2017年 GoDap. All rights reserved.
//

#import <Foundation/Foundation.h>

@class AFSecurityPolicy;
@class GDBaseRequest;

@protocol GDRequestAccessory;

NS_ASSUME_NONNULL_BEGIN


@interface GDNetworkConfiguration : NSObject

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;

+ (instancetype)defaultConfiguration;

@property (nonatomic, readonly) NSURLSessionConfiguration *sessionConfiguration;

@property (nonatomic, copy, nullable) NSString *baseUrl;

/// 下载请求的默认存储地址
/// @note 默认为cache文件夹下的`GDetwork`->`DownloadDirectory`文件夹,resumeData默认保存在此文件夹下的`resume`文件夹中。放在cache文件夹中的好处是当系统内存警告的时候，系统会自动清理cache文件夹，而不需要人为进行管理
/// @warning 文件下载会保存在tmp文件夹中，但是会立即被清理掉，所以此属性必定不能为nil，如果在GDBaseRequest中配置了downloadDirectory，此属性将会失效
@property (nonatomic, strong) NSURL *downloadDirectory;
@property (nonatomic, readonly) NSURL *downloadResumeDirectory;

@property (nonatomic, strong) NSURL *cacheDirectory;

@property (nonatomic, copy, nullable) NSString *requestAuthorizationUsername;

/// 请求安全策略
/// @note 默认为`AFSSLPinningModeNone`
@property (nonatomic, strong, nullable) AFSecurityPolicy *securityPolicy;

/// 请求所处的线程
/// @note 默认为"me.dingtonge.requestConcurrentQueue"并发线程
@property (nonatomic, strong, nullable) dispatch_queue_t requestQueue;

/// 请求结束的回调线程
/// @note 默认为"me.dingtonge.completedConcurrentQueue"并发线程
/// @warning 尽量不要将completionQueue修改为主线程因为syn(同步请求)一般实现原理通过信号量来实现双线程同步，如果将completionQueue修改为主线程则会造成DeadLock
@property (nonatomic, strong, nullable) dispatch_queue_t completionQueue;

/// 请求结束的回调Group
/// @note 默认为`nil`
@property (nonatomic, strong, nullable) dispatch_group_t completionGroup;

/// 请求状态的代理，主要回调请求开始于结束的状态，并且可以设置多个代理
/// 可以利用此方法hook请求
/// @note 默认为`nil`
@property (nonatomic, strong, nullable) NSMutableSet<id<GDRequestAccessory> > *requestAccessories;

/// 添加请求状态的代理
- (void)addReuestAccessory:(id<GDRequestAccessory>)requestAccessory;

#pragma mark - class method & property

/// 全局请求状态代理
/// 可以利用此方法hook所有经过此框架的请求
/// @note 默认为`nil`
@property (nonatomic, strong, class, nullable) NSMutableSet<id<GDRequestAccessory>> *globalRequestAccessories;

/// 添加全局的请求状态代理
+ (void)addGlobalReuestAccessory:(id<GDRequestAccessory>)globalRequestAccessories;


@end

NS_ASSUME_NONNULL_END
