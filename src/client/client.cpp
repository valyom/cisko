#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

#include <sstream>
#include <vector>
#include <sys/ioctl.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

std::vector<std::string> split (std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if (token.length() > 0) res.push_back (token);
    }

    token = s.substr (pos_start);
    if (token.length() > 0) res.push_back (token);
    return res;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
 
    while (1) 
    {
        bzero(buffer,256) ;
        std::cout << "Please enter BBQ request or Quit for exit  : "  ;
        fgets(buffer,255,stdin);
        if ( strstr(buffer,"Quit")) {
            break;
        }
        std::string input = buffer;
        size_t pos = 0;
        std::string  delimiter = "\n";
        std::string token = input;
        std::vector<std::string> tokens = split(buffer, "\n");
        
        
        
        for( std::string token : tokens) {
            std::cout << "Client  : " << token << std::endl;
            n = write(sockfd, token.c_str(), token.length());
            if (n < 0) {
                error("ERROR writing to socket");
                break;
            }
            //waith for th answer[s] 

            int bytesAvailable;
            do { 
                bzero(buffer,256);
                n = read(sockfd,buffer,255);
                if (n > 0) {
                    char *ptr = strtok( buffer, "\n");
                    while (ptr) {     
                        std::cout << "BBQ shop : " << ptr << std::endl; 
                        ptr = strtok( 0, "\n");
                    }
                }
                ioctl(sockfd, FIONREAD, &bytesAvailable);
            }
            while (  bytesAvailable > 0); 
        }
    
    }
    write(sockfd, "Quit", 4);
    close(sockfd);
    return 0;
}
