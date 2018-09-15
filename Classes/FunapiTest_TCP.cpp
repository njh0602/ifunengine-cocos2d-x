#include "FunapiTest_TCP.h"
USING_NS_CC;

// for clean interface
#include "json/json.hpp"

Scene* FunapiTest_TCP::createScene()
{
    auto scene = Scene::create();
    auto layer = FunapiTest_TCP::create();
    scene->addChild(layer);
    return scene;
}

FunapiTest_TCP* FunapiTest_TCP::create()
{
    auto ret = new (std::nothrow) FunapiTest_TCP();
    if ( ret && ret->init() )
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FunapiTest_TCP::init()
{
    if ( !Node::init() )
        return false;
    
    scheduleUpdate(); // for loop
    
    std::string serverIP = "your server ip";
    
    _session = fun::FunapiSession::Create(serverIP.c_str());
    _session->AddSessionEventCallback([this](const std::shared_ptr<fun::FunapiSession> &session,
                                             const fun::TransportProtocol protocol,
                                             const fun::SessionEventType type,
                                             const std::string &_sessionid,
                                             const std::shared_ptr<fun::FunapiError> &error) {
        
        if (type == fun::SessionEventType::kOpened)
        {
            log("session opened: %s", _sessionid.c_str());
        }
        else if (type == fun::SessionEventType::kChanged)
        {
            // session id changed
        }
        else if (type == fun::SessionEventType::kClosed)
        {
            log("session closed");
            _session = nullptr;
        }
        
    });
    
    _session->AddTransportEventCallback([this](const std::shared_ptr<fun::FunapiSession> &session,
                                               const fun::TransportProtocol protocol,
                                               const fun::TransportEventType type,
                                               const std::shared_ptr<fun::FunapiError> &error) {
        
        if (type == fun::TransportEventType::kStarted)
        {
            log("Transport Started called.");
        }
        else if (type == fun::TransportEventType::kStopped)
        {
            log("Transport Stopped called.");
        }
        else if (type == fun::TransportEventType::kConnectionFailed)
        {
            log("Transport Connection Failed (%s)", fun::TransportProtocolToString(protocol).c_str());
            _session = nullptr;
        }
        else if (type == fun::TransportEventType::kConnectionTimedOut)
        {
            log("Transport Connection Timedout called");
            _session = nullptr;
        }
        else if (type == fun::TransportEventType::kDisconnected)
        {
            log("Transport Disconnected called (%s)", fun::TransportProtocolToString(protocol).c_str());
        }
        
    });
    
    _session->AddJsonRecvCallback([](const std::shared_ptr<fun::FunapiSession> &session,
                                     const fun::TransportProtocol protocol,
                                     const std::string &msg_type,
                                     const std::string &json_string) {
        
        if (msg_type.compare("echo") == 0)
        {
            log("msg '%s' arrived.", msg_type.c_str());
            log("json: %s", json_string.c_str());
        }
        
        if (msg_type.compare("_maintenance") == 0)
        {
            log("Maintenance message : %s", json_string.c_str());
        }
        
    });
    
    // ----
    _connectButton = ui::Text::create("Connect", "", 50);
    _connectButton->setVisible(false);
    _connectButton->setTouchEnabled(true);
    _connectButton->setPosition(_director->getVisibleSize()/2);
    _connectButton->addClickEventListener([this](Ref* ref){
        
        if ( _session )
        {
            auto protocol = fun::TransportProtocol::kTcp;
            _session->Connect(protocol, 8012, fun::FunEncoding::kJson);
            _session->SetDefaultProtocol(protocol);
        }
        
    });
    addChild(_connectButton);
    
    // ----
    _disconnectButton = ui::Text::create("Disconnect", "", 50);
    _disconnectButton->setVisible(false);
    _disconnectButton->setTouchEnabled(true);
    _disconnectButton->setPosition(Vec2(_director->getVisibleSize().width/2, _director->getVisibleSize().height-100));
    _disconnectButton->addClickEventListener([this](Ref* ref){
        
        if ( _session )
        {
            _session->Close();
        }
    });
    addChild(_disconnectButton);
    
    // ----
    _sendEchoMsgButton = ui::Text::create("Send Echo Msg", "", 50);
    _sendEchoMsgButton->setVisible(false);
    _sendEchoMsgButton->setTouchEnabled(true);
    _sendEchoMsgButton->setPosition(Vec2(_disconnectButton->getPositionX(), _disconnectButton->getPositionY()-100));
    _sendEchoMsgButton->addClickEventListener([this](Ref* ref){
        
        if ( _session )
        {
            for (int i = 0; i < 10; ++i)
            {
                nlohmann::json j;
                j["message"] = StringUtils::format("hello world - %d", i);
                _session->SendMessage("echo", j.dump());
            }
        }
        
    });
    addChild(_sendEchoMsgButton);
    
    return true;
}

void FunapiTest_TCP::update(float dt)
{
    fun::FunapiSession::UpdateAll();
    
    if ( _session )
    {
        // update ui
        _connectButton->setVisible(!_session->IsConnected());
        _disconnectButton->setVisible(_session->IsConnected());
        _sendEchoMsgButton->setVisible(_session->IsConnected());
    }
}
