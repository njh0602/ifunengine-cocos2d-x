#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "funapi/funapi_session.h"

class FunapiTest_TCP : public cocos2d::Node
{
    
public:
    
    static cocos2d::Scene* createScene();
    static FunapiTest_TCP* create();
    
private:
    
    bool init() override;
    void update(float dt) override;
    void sendEchoMessage();
    
private:
    
    std::shared_ptr<fun::FunapiSession> _session = nullptr;
    
    cocos2d::ui::Text* _connectButton;
    cocos2d::ui::Text* _disconnectButton;
    cocos2d::ui::Text* _sendEchoMsgButton;
    
};
