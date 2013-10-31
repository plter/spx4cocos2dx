#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "spx4cc.h"

USING_NS_CC;

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    
    virtual void update(float dt);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    CCSPXSprite *spx;
    CCSPX3Sprite *spx3;
};

#endif // __HELLOWORLD_SCENE_H__
