/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
#include <string>
USING_NS_CC;
using namespace std;

Scene* GetScene1();
Scene* GetSceneGameOver(int);

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}
// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//background
	Sprite* background = Sprite::create("background.png");
	float scale = MAX(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
	background->setScale(scale);
	background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(background, 0);

	//Title
	cocos2d::Label *titleLabel;
	titleLabel = Label::createWithTTF("ASTEROID DODGE", "fonts/Marker Felt.ttf", 60);
	titleLabel->setColor(Color3B::WHITE);
	titleLabel->setPosition(Point(visibleSize.width/2, visibleSize.height - 300));
	this->addChild(titleLabel , 1);

	//music audio
	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("backgroundmusic.wav");
	audio->playBackgroundMusic("backgroundmusic.wav", true);

	Vector<MenuItem*>menuItems;

	auto menuItemLabel1 = Label::createWithTTF("Play Game", "fonts/Marker Felt.ttf", 30);
	auto menuItem1 = MenuItemLabel::create(menuItemLabel1,
		[&](Ref* sender) {
		
		Director::getInstance()->pushScene(GetScene1());

	}
	);
	menuItem1->setPosition(Vec2(menuItem1->getPosition().x, menuItem1->getPosition().y + 30));
	menuItems.pushBack(menuItem1);


	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
	closeItem->setAnchorPoint(Vec2(1, 1));
	closeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	menuItems.pushBack(closeItem);
	auto menu = Menu::createWithArray(menuItems);

	this->addChild(menu, 1);

	return true;
}

Scene* GetScene1()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto scene = Scene::createWithPhysics();

	//effect audio
	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	audio->preloadEffect("explosion.wav");

	//create player sprite
	Sprite* playerbot = Sprite::create("botfront1.png");
	playerbot->setScale(2);
	playerbot->setTag(10);
	playerbot->setPosition(Vec2(visibleSize.width / 2 + origin.x, 350));
	Vector<SpriteFrame*> playerbot_animFrames;
	playerbot_animFrames.reserve(3);
	playerbot_animFrames.pushBack(SpriteFrame::create("botfront1.png", Rect(0, 0, 32, 59)));
	playerbot_animFrames.pushBack(SpriteFrame::create("botfront2.png", Rect(0, 0, 32, 60)));
	playerbot_animFrames.pushBack(SpriteFrame::create("botfront3.png", Rect(0, 0, 32, 63)));

	auto playerbot_animation = Animation::createWithSpriteFrames(playerbot_animFrames, 0.1f);
	auto playerbot_animate = Animate::create(playerbot_animation);
	playerbot->runAction(RepeatForever::create(playerbot_animate));

	scene->addChild(playerbot, 1);

	//create background
	Sprite* background = Sprite::create("background.png");
	float scale = MAX(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
	background->setScale(scale);
	background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	scene->addChild(background, 0);

	//close menu
	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
		[&](Ref* sender) {
		Director::getInstance()->replaceScene(HelloWorld::create());
		
	}
	);
	closeItem->setAnchorPoint(Vec2(1, 1));
	closeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	auto menu = Menu::create(closeItem, NULL);
	scene->addChild(menu);

	//touch drag controls
	auto _eventDispatcher = Director::getInstance()->getEventDispatcher();
	auto listener1 = EventListenerTouchOneByOne::create();
	listener1->setSwallowTouches(true);

	listener1->onTouchBegan = [&](Touch* touch, Event* event) {

		return true;
	};

	listener1->onTouchMoved = [=](Touch* touch, Event* event)
	{
		if (touch->getLocation().x < visibleSize.width && touch->getLocation().x > 0)
		{
			playerbot->setPositionX(touch->getLocation().x);
		}
		if (touch->getLocation().y < visibleSize.height && touch->getLocation().y > 0)
		{
			playerbot->setPositionY(touch->getLocation().y);
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, scene);

	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); //Draw hitboxes for testing 

	//create boundary line
	Sprite* boundary = Sprite::create("boundary line.png");
	boundary->setTag(20);
	boundary->setScale(2);
	boundary->setPosition(visibleSize.width / 2, -10); 
	auto boundaryphysicsBody = PhysicsBody::createBox(boundary->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	boundaryphysicsBody->setDynamic(false);
	boundaryphysicsBody->setContactTestBitmask(0x01);
	boundary->setPhysicsBody(boundaryphysicsBody);
	scene->addChild(boundary, 1);
	
	//physics for player
	auto spritephysicsBody = PhysicsBody::createBox(playerbot->getContentSize()/2, PhysicsMaterial(0.1f, 1.0, 0.0f)); //make hitbox half the size of the player for less frustration
	spritephysicsBody->setContactTestBitmask(0x01);
	spritephysicsBody->setDynamic(false);
	playerbot->setPhysicsBody(spritephysicsBody);

	//spawn first slow platforms
	auto scheduler = Director::getInstance()->getScheduler();
	scheduler->schedule([=](float dt)
	{	
		Sprite* asteroid = Sprite::create("Asteroid Brown.png");
		asteroid->setScale(0.7);
		asteroid->setSkewY(30.0f);
		asteroid->setTag(5);
		asteroid->setPosition(cocos2d::RandomHelper::random_int(5, 530), 1000);
		auto asteroidPhysicsBody = PhysicsBody::createBox(asteroid->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
		asteroidPhysicsBody->setVelocity(Vec2(0, -500));
		asteroidPhysicsBody->setContactTestBitmask(0x01);
		asteroid->setPhysicsBody(asteroidPhysicsBody);
		scene->addChild(asteroid,1);
		 
	}, scene, 0.5f, 10, 0.0f, false, "myCallbackKey");

	//spawn faster platforms
	auto scheduler2 = Director::getInstance()->getScheduler();
	scheduler2->schedule([=](float dt)
	{	
		Sprite* asteroid = Sprite::create("Asteroid Brown.png");
		asteroid->setScale(0.7);
		asteroid->setSkewY(30.0f);
		asteroid->setTag(5);
		asteroid->setPosition(cocos2d::RandomHelper::random_int(5, 530), 1000);
		auto asteroidPhysicsBody = PhysicsBody::createBox(asteroid->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
		asteroidPhysicsBody->setVelocity(Vec2(0, -1000));
		asteroidPhysicsBody->setContactTestBitmask(0x01);
		asteroid->setPhysicsBody(asteroidPhysicsBody);
		scene->addChild(asteroid, 1);

	}, scene, 0.5f, 10, 7.0f, false, "myCallbackKey2");

	//spawn fastest platforms
	auto scheduler3 = Director::getInstance()->getScheduler();
	scheduler2->schedule([=](float dt)
	{
		Sprite* asteroid = Sprite::create("Asteroid Brown.png");
		asteroid->setScale(0.7);
		asteroid->setSkewY(30.0f);
		asteroid->setTag(5);
		asteroid->setPosition(cocos2d::RandomHelper::random_int(5, 530), 1000);
		auto asteroidPhysicsBody = PhysicsBody::createBox(asteroid->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
		asteroidPhysicsBody->setVelocity(Vec2(0, -1200));
		asteroidPhysicsBody->setContactTestBitmask(0x01);
		asteroid->setPhysicsBody(asteroidPhysicsBody);
		scene->addChild(asteroid, 1);
		
	}, scene, 0.4f, kRepeatForever, 12.0f, false, "myCallbackKey3");

	//sometimes spawn 2 platforms when in fastest mode
	auto scheduler4 = Director::getInstance()->getScheduler();
	scheduler4->schedule([=](float dt)
	{
		Sprite* asteroid = Sprite::create("Asteroid Brown.png");
		asteroid->setScale(0.7);
		asteroid->setSkewY(30.0f);
		asteroid->setTag(5);
		asteroid->setPosition(cocos2d::RandomHelper::random_int(5, 530), 1000);
		auto asteroidPhysicsBody = PhysicsBody::createBox(asteroid->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
		asteroidPhysicsBody->setVelocity(Vec2(0, -1200));
		asteroidPhysicsBody->setContactTestBitmask(0x01);
		asteroid->setPhysicsBody(asteroidPhysicsBody);
		scene->addChild(asteroid, 1);

	}, scene, 1.0f, kRepeatForever, 20.0f, false, "myCallbackKey4");

	//score
	Sprite* sprScore = Sprite::create("botfront1.png");
	sprScore->setTag(10000);
	scene->addChild(sprScore, -2);
	cocos2d::Label *scoreLabel;
	scoreLabel = Label::createWithTTF(to_string(0), "fonts/Marker Felt.ttf", 35);
	scoreLabel->setColor(Color3B::WHITE);
	scoreLabel->setPosition(Point(120, visibleSize.height-30));
	scene->addChild(scoreLabel, 1);
	cocos2d::Label *scoreLabelName;
	scoreLabelName = Label::createWithTTF("Score: ", "fonts/Marker Felt.ttf", 35);
	scoreLabelName->setColor(Color3B::WHITE);
	scoreLabelName->setPosition(Point(50, visibleSize.height - 30));
	scene->addChild(scoreLabelName, 1);

	//contact
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [=](PhysicsContact& contact)
	{	
		auto nodeA = contact.getShapeA()->getBody()->getNode();
		auto nodeB = contact.getShapeB()->getBody()->getNode();
		scoreLabel->setString(to_string((sprScore->getTag() - 10000)));
		
		if (nodeA&&nodeB)
		{
			if (nodeA->getTag() == 10 && nodeB->getTag() == 5|| nodeA->getTag() == 5 && nodeB->getTag() == 10)
			{
				audio->playEffect("explosion.wav");
				Director::getInstance()->pushScene(TransitionFade::create(0.5,GetSceneGameOver(sprScore->getTag())));
				//return false; //invicibility for testing
			}

			else if (nodeA->getTag() == 5 && nodeB->getTag() == 20)
			{
				sprScore->setTag(sprScore->getTag() + 1);//score up
				nodeA->removeFromParentAndCleanup(true);
			}
			else if (nodeB->getTag() == 5 && nodeA->getTag() == 20)
			{
				sprScore->setTag(sprScore->getTag() + 1);//score up
				nodeB->removeFromParentAndCleanup(true);
			}
			else if (nodeB->getTag() == 5 && nodeA->getTag() == 5)
			{
				return false;
			}
		}

		return true;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, scene);

	return scene;

}

Scene* GetSceneGameOver(int score)
 {
	 auto visibleSize = Director::getInstance()->getVisibleSize();
	 Vec2 origin = Director::getInstance()->getVisibleOrigin();
	 auto scene = Scene::create();

	 //background
	 Sprite* background = Sprite::create("gameoverbackground.jpg");
	 float scale = MAX(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
	 background->setScale(scale);
	 background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	 scene->addChild(background, 0);

	 //gameover label
	 auto label = Label::createWithTTF("Game Over", "fonts/Marker Felt.ttf", 70);
	 auto vecLabel = Vec2(visibleSize.width / 2, visibleSize.height/1.25);
	 label->setPosition(vecLabel);
	 scene->addChild(label, 1);

	 //score
	 cocos2d::Label *scoreLabel;
	 scoreLabel = Label::createWithTTF(to_string(score-10000), "fonts/Marker Felt.ttf", 35);
	 scoreLabel->setColor(Color3B::WHITE);
	 scoreLabel->setPosition(Point(visibleSize.width/2+40, visibleSize.height/1.5));
	 scene->addChild(scoreLabel, 1);
	 cocos2d::Label *scoreLabelName;
	 scoreLabelName = Label::createWithTTF("Score: ", "fonts/Marker Felt.ttf", 35);
	 scoreLabelName->setColor(Color3B::WHITE);
	 scoreLabelName->setPosition(Point(visibleSize.width/2-40, visibleSize.height/1.5));
	 scene->addChild(scoreLabelName, 1);

	 Vector<MenuItem*>menuItems;
	 //Try again menu
	 auto menuItemLabel1 = Label::createWithTTF("Try Again? ", "fonts/Marker Felt.ttf", 35);
	 auto menuItem1 = MenuItemLabel::create(menuItemLabel1,
		 [&](Ref* sender) {

		 Director::getInstance()->replaceScene(GetScene1());


	 }
	 );

	 menuItem1->setPosition(Vec2(menuItem1->getPosition().x, menuItem1->getPosition().y + 30));
	 menuItems.pushBack(menuItem1);

	 //close menu
	 auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
		 [&](Ref* sender) {
		 Director::getInstance()->replaceScene(HelloWorld::create());

	 }
	 );
	 closeItem->setAnchorPoint(Vec2(1, 1));
	 closeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	 menuItems.pushBack(closeItem);
	 auto menu = Menu::createWithArray(menuItems);
	 scene->addChild(menu, 1);
	 
	
	 return scene;
 }

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
