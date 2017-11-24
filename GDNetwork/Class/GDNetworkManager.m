//
//  GDNetworkManager.m
//  GDNetwork
//
//  Created by Journey on 2017/11/24.
//  Copyright © 2017年 GoDap. All rights reserved.
//

#import "GDNetworkManager.h"
#import "GDBaseRequest.h"
#import <AFNetworking/AFURLSessionManager.h>
#import <AFNetworking/AFURLRequestSerialization.h>

@interface GDNetworkManager (){
    pthread_mutex_t _lock;
}

@property (nonatomic, strong) AFURLSessionManager *sessionManager;

///保存已经请求 task,防止再次发送
@property (nonatomic, strong) NSMutableSet<GDBaseRequest *> *request;

@end

@implementation GDNetworkManager



@end
