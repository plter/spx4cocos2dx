#include "HelloWorldScene.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCTextureCache *cache = CCTextureCache::sharedTextureCache();
    //support spx
    spx = CCSPXSprite::create("spx_test.sprite", cache->addImage("spx_test.png"), 0);
    spx->setPosition(ccp(200,200));
    addChild(spx);
    
    //support spx3
    spx3 = CCSPX3Sprite::create("spx3_test.sprite", 0, cache->addImage("spx3_test.png"),NULL);
    spx3->setPosition(ccp(200, 100));
    spx3->setLoopCount(-1);
    addChild(spx3);
    
    setTouchEnabled(true);
    scheduleUpdate();
    
    return true;
}

void HelloWorld::update(float dt){
    spx->tick(dt);
    spx3->tick(dt);
}


void HelloWorld::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
    spx->playAnimation(0);
    spx3->playAnimation(0);
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}
