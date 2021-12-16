#pragma once
#include "BBQ_StateMachine.h" 

//class providing separate state processing
class BBQ_Serve  : public StateMachine 
{
private: 
    const std::string stateIdleEnterAction     = "";
    const std::string stateCheckingEnterAction = "OK, WAIT\n";
    const std::string stateChickenEnterAction  = "CHICKEN READY";
    const std::string stateBeefEnterAction     = "BEEF READY";
    const std::string stateMammothEnterAction  = "LAST MONTH MAMMOTH READY";
    const std::string stateServedenterAction   = "SERVED BYE\n";
    const std::string stateClosedEnterAction   = "CLOSED BYE"; 
    const std::string stateWrongTransitionAction   = "I bag your pardon." ;
    
    //allowed client requests
    const std::string REQ_HUNGRY   = "I AM HUNGRY, GIVE ME BBQ";
    const std::string REQ_ACCEPT   = "I TAKE THAT!!!";
    const std::string REQ_REECT    = "NO THANKS";        
    int newsockfd_;
private:
    std::map<std::string, StateMachine::Action> requestToevent;
    bool isEnough_;
public:
    BBQ_Serve( ) ;
    ~BBQ_Serve();
    bool clientRequest(const char * userInput);
    void runServer(int port);
 
protected:
    virtual void onEnterState(const StateMachine::State state);
    virtual void onExitState (const StateMachine::State state) ;
private:
    void commonInStateCallback(const char * message) ;
    
}; 