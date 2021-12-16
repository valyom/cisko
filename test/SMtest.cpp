 #include "BBQ_StateMachine.hpp"
#include <iostream>

void reportTestResult (bool result) {
    if (!result) {
        std::cout << "Test Failed" << std::endl;
    }
    std::cout << std::endl; 
}
//State machine tests

bool testTakeChicken (BBQ_Serve &bbq) {
    bool result = true;

    std::cout << "Test Buy chicken " << std::endl; 
    
    result = result && bbq.clientRequest("I AM HUNGRY, GIVE ME BBQ");
    result = result && StateMachine::Chicken == bbq.getCurrentState(); 
    
    result = result && bbq.clientRequest( "I TAKE THAT!!!");
    result = result && StateMachine::Idle == bbq.getCurrentState(); 
    
    reportTestResult(result);
    return result;
}

bool testTakeBeef (BBQ_Serve &bbq) { 
 
    bool result = true;
 
    std::cout << "Test Buy beef " << std::endl; 

    result = result && bbq.clientRequest("I AM HUNGRY, GIVE ME BBQ");
    result = result && StateMachine::Chicken == bbq.getCurrentState();
    
    result = result && bbq.clientRequest( "NO THANKS");
    result = result && StateMachine::Beef == bbq.getCurrentState();     
            
    result = result && bbq.clientRequest( "I TAKE THAT!!!");   
    result = result && StateMachine::Idle == bbq.getCurrentState();    

    reportTestResult(result);
    return result;
}

bool testTakeMammoth (BBQ_Serve &bbq) {

    bool result = true;
    std::cout << "Test Buy mammoth " << std::endl;  
    
    result = result && bbq.clientRequest("I AM HUNGRY, GIVE ME BBQ");
    result = result && StateMachine::Chicken == bbq.getCurrentState();

    result = result && bbq.clientRequest( "NO THANKS"); 
    result = result && StateMachine::Beef == bbq.getCurrentState();

    result = result && bbq.clientRequest( "NO THANKS"); 
    result = result && StateMachine::Mammoth == bbq.getCurrentState();

    result = result && bbq.clientRequest( "I TAKE THAT!!!");
    result = result && StateMachine::Idle == bbq.getCurrentState();
 
    reportTestResult(result);
    return result;
}     


bool testWrongInput (BBQ_Serve &bbq) { 

    bool result = true;
    
    std::cout << "Test wrong input " << std::endl;  
    result = StateMachine::Idle == bbq.getCurrentState();

    result = result && bbq.clientRequest( "Hellow");  
    result = result && StateMachine::Idle == bbq.getCurrentState();

    result = result && bbq.clientRequest( "NO THANKS"); 
    result = result && StateMachine::Idle == bbq.getCurrentState();

    result = result && bbq.clientRequest( "I TAKE THAT!!!");   
    result = result && StateMachine::Idle == bbq.getCurrentState();
    
    reportTestResult(result);
    return result;
}
 
 int mainAllStateMachineTests()
 {
    bool result;
    BBQ_Serve bbq;

    //stop on first error
    result = testWrongInput(bbq);
    result = result && testTakeChicken(bbq);
    result = result && testTakeBeef(bbq);
    result = result && testTakeMammoth(bbq);
    
    return result ? 1 : 0;
}
