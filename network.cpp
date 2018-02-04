//
//  network.cpp
//  tul


#include "network.h"
#include "filehandler.h"
#include "dnsmaker.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFLEN 512
using namespace std;

network::network() {
    
}
network::~network() {

}

int network::validPort(int port){
    if(port > 0 && port <= 65535) {
        return 0;
    }
    else {
        return -1;
    }
}

int network::validIP(const char* ip) {
        struct sockaddr_in sa;
        int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
        return result != 0;
}

void network::startListen(const char* srv_ip, int port, int type) {
    
    if(validIP(srv_ip) != 1) {
        printf("Not a valid IP.\n\n");
        exit(-1);
    }
    if(validPort(port) != 0) {
        printf("Not a valid port.\n\n");
        exit(-1);
    }
    
    struct sockaddr_in my_addr, cli_addr;
    int sockfd;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFLEN];
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
        printf("Unable to create socket");
    }
    
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if ( (::bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))) < 0)
    {
        printf("Unable to bind socket");
        exit(-1);
    }
    else {
        //printf("Listener started\n");
    }

    while(1)
    {
        if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1) {
            printf("recvfrom()");
        }
        else {
            //FILE
            if(type == 1) {
                dnsmaker reciever;
                reciever.printpacket((unsigned char*)&buf);
            }
            //CHAT
            else if(type == 2) {
                dnsmaker reciever;
                cout << "Message from " << inet_ntoa(cli_addr.sin_addr) << ":"<< ntohs(cli_addr.sin_port) <<endl;
                reciever.printMessage((unsigned char*)&buf);
            }
            else {  }
        }
    }
    close(sockfd);
}

void network::sendData(const char* buf, const char* srv_ip, int port) {

    if(validIP(srv_ip) != 1) {
        printf("Not a valid IP");
        exit(-1);
    }
    if(validPort(port) != 0) {
        printf("Not a valid port");
        exit(-1);
    }
        dnsmaker sender;
        sender.create(buf, 1, srv_ip, port);
}

void network::startSend(const char* srv_ip, int port) {
    
    if(validIP(srv_ip) != 1) {
        printf("Not a valid IP");
        exit(-1);
    }
    if(validPort(port) != 0) {
        printf("Not a valid port");
        exit(-1);
    }
    string name = "";
    cout << "Enter your nickname: ";
    getline(cin, name);
    
    while(1)
    {
        printf("Message: ");
        string input = "";
        getline(cin, input);
        input = name + ":"+ input;
        if(input.size() < 25) {
            filehandler *fh = new filehandler();
            string full_packet = fh->base64_encode(reinterpret_cast<const unsigned char*>(input.c_str()), (unsigned int)input.size());
            dnsmaker *sender = new dnsmaker();
            sender->create(full_packet.c_str(), 1, srv_ip, port);
           // delete sender;
        }
        else {
            cout << "Message is too long. Try again." <<endl;
        }
    }
}