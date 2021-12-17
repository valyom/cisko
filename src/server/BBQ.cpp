 
#include <iostream>
#include <utility>
#include "BBQ.h" 
#include <algorithm> 
#include <string>   

#include <stdio.h>
#include <string.h>//bzero
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h> 


#include <signal.h>
#include <sstream>
#include <sys/ioctl.h>
 
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//class providing separate state processing
BBQ_Serve::BBQ_Serve( ) : m_clientSock(-1) 
    , m_bindSock (-1)
    , m_isEnough(false)
{
    std::cout << "BBQ Opened" << std::endl; 
     
    m_requestToevent[REQ_HUNGRY] = NewClient;
    m_requestToevent[REQ_ACCEPT] = Accept;
    m_requestToevent[REQ_REECT]  = Reject;
}

BBQ_Serve::~BBQ_Serve(){
    processSMEvent(Close);
    closeSockets(); // close sockets
}


void BBQ_Serve::onEnterState(const StateMachine::State state) {
    //separate processing per state 
    switch (state) {
        case StateMachine::Idle     : commonInStateCallback (stateIdleEnterAction.c_str());    break;
        case StateMachine::Checking : commonInStateCallback (stateCheckingEnterAction.c_str()); break;
        case StateMachine::Chicken  : commonInStateCallback (stateChickenEnterAction.c_str()); break;
        case StateMachine::Beef     : commonInStateCallback (stateBeefEnterAction.c_str());    break;
        case StateMachine::Mammoth  : commonInStateCallback (stateMammothEnterAction.c_str()); break;
        case StateMachine::Served   : commonInStateCallback (stateServedenterAction.c_str());  break;
        case StateMachine::Closed   : commonInStateCallback (stateClosedEnterAction.c_str());  break;
        default : {
            commonInStateCallback (stateWrongTransitionAction.c_str()); 
        }   
    }
}

void BBQ_Serve::onExitState(const StateMachine::State state) {
    if (Served == state) {
        std::string answer ;
        std::cout << "Enough ? [N/y] : ";
        std::cin >> answer;
        m_isEnough = answer == "y" || answer == "Y";
    }
}

void BBQ_Serve::commonInStateCallback(const char * message) {
    
    if (m_clientSock != -1) {
        send(m_clientSock, message, strlen(message), 0); 
    }
    else
        std::cout << message << std::endl;
}

bool BBQ_Serve::clientRequest(const char * userInput) {

    if (nullptr != userInput && '\0' != *userInput ) {
        std::string request = userInput;
        transform(request.begin(), request.end(), request.begin(), ::toupper);
        const bool isKnown = m_requestToevent.end() != m_requestToevent.find(request) ;
        const StateMachine::Event action = isKnown ? m_requestToevent.at(request) : WrongInput;
        processSMEvent(action);
    }

    return true;
}


void BBQ_Serve::runServer(int portNum){
    int portno=portNum;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
 
     
    m_bindSock =  socket(AF_INET, SOCK_STREAM, 0);
    if (m_bindSock < 0)  
        error("ERROR opening socket");
       
    // clear address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));

    
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = INADDR_ANY;  
    //  converted into network byte order
    serv_addr.sin_port = htons(portno);
 

    if (bind(m_bindSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
 
    bool reconnect = true;
    while (reconnect) {
        reconnect = false;
        // Here, we set the maximum size for the backlog queue to 1.
        std::cout << "server: wait a new client"   << std::endl;
        listen(m_bindSock, 1);

        clilen = sizeof(cli_addr); 
        m_clientSock = accept(m_bindSock,  (struct sockaddr *) &cli_addr, &clilen);
        if (m_clientSock < 0) 
            error("ERROR on accept"); 

        std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr)<< " port " <<  ntohs(cli_addr.sin_port)  << std::endl; 
        

        while (!m_isEnough) {
            bzero(buffer,256); 
            n = read(m_clientSock,buffer,255); 
            if (n < 0)
                error("ERROR reading from socket");
            if (strstr(buffer, "Quit")) {
                 reconnect = true;
                 break;   
            }
            clientRequest(buffer); 
        }
        close(m_clientSock);
        m_clientSock = -1;
    }
    std::cout << std::endl << "Exiting..." << std::endl; 

    close(m_bindSock);
    m_bindSock = -1; 
    m_clientSock = -1;
}

void BBQ_Serve::closeSockets(){

    if (this->m_clientSock) close(this->m_clientSock);     
    if (this->m_bindSock) close(this->m_bindSock );
    exit(1); 
}


static BBQ_Serve bbq; 

//callback to be called on signals 
void signalHandler( int value )
{
    bbq.closeSockets(); 
}

int main (int argc, char *argv[])
{
    if (argc > 1) {
       
        struct sigaction sigIntHandler; 
        sigIntHandler.sa_handler = signalHandler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
//run the server
        bbq.runServer(atoi(argv[1]));
    }
    else {
        std::cout << "ERROR, no port provided" <<std::endl;
    }  
    return 0; 
} 

 


