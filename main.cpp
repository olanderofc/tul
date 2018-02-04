//
//  main.cpp
//  tul


#define VERSION "1.1"
#include "network.h"
#include "filehandler.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <netdb.h>
#include <iostream>
#include <ctime>
#include <sstream>




struct arg_struct {
    int port;
    string ip;
    string filename;
};


void *startMessageReciever(void *arguments) {
    
    struct arg_struct *argu = (struct arg_struct *)arguments;
    network *np = new network();
    np->startListen( argu->ip.c_str(), argu->port, 2);
    delete np;
    pthread_exit(NULL);
}

void *startMessageSender(void *arguments) {

    network *np = new network();
    string conn_ip = "";
    string conn_port = "";
    
    cout << "Enter IP to connect to: " <<endl;
    getline(cin, conn_ip);
    
    while(np->validIP(conn_ip.c_str()) != 1) {
        cout << "Enter IP to connect to: " <<endl;
        getline(cin, conn_ip);
    }
    
    cout << "Enter port to connect to:" <<endl;
    getline(cin, conn_port);
    
    while(np->validPort(atoi(conn_port.c_str())) != 0) {
        cout << "Enter port to connect to:" <<endl;
        getline(cin, conn_port);
    }    
    
    np->startSend(conn_ip.c_str(),atoi(conn_port.c_str()));
    delete np;
    pthread_exit(NULL);
}



int main(int argc, const char * argv[])
{
    int listen = 0;
    if(argc < 4 || argc > 6)
    {
        cout << "tiny UDP leaker "<< VERSION <<endl;
        cout << "************************FILE MODE*****************************" <<endl;
        cout << "To start a file recieve server:\n -l <Server-IP> <Port>" <<endl;
        cout << "To send a file to a server:\n -f <File> <Server-Ip <Port>" <<endl;
        cout << "***********************MESSAGE MODE****************************" <<endl;
        cout << "To start a message service:\n -m <Server-IP> <Port>" <<endl;
        cout << "**************************************************************" <<endl;
        cout << endl;
        exit(0);
    }
    
    //Setup threads
    pthread_attr_t attr;
    void *status;
    struct arg_struct args;
    
    for(int i=0; i<argc; i++) {
        if(strncmp("-m",argv[i], 2) == 0) {
            cout << "Preparing to start in normal send mode" <<endl;
            listen = 0;
        }
        if(strncmp("-l", argv[i], 2) == 0) {
            cout << "Preparing to start in file listen mode" <<endl;
            listen = 1;
        }
        if(strncmp("-f",argv[i], 2) == 0) {
            cout << "Starting file send mode!" <<endl;
            listen = 2;
        }
    }
    
    switch(listen) {
        case 0:
            //Send Messages
            cout << "Messages mode started" <<endl;
            args.ip = argv[2];
            args.port = atoi(argv[3]);
            break;
        case 1:
            cout << "Listener for files started" <<endl;
            args.ip = argv[2];
            args.port = atoi(argv[3]);
            break;
        case 2:
            //Send files
            cout << "Send file started" <<endl;
            args.ip = argv[3];
            args.port = atoi(argv[4]);
            args.filename = argv[2];
            break;
        default:
            cout << "Mode not set. Quitting." <<endl;
            exit(0);
            break;
    };
    
    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    int ct,lt;
    pthread_t messageSenderThread;
    pthread_t messageRecThread;
    if(listen == 0) {
    //Start the command thread
        ct = pthread_create(&messageSenderThread, NULL, startMessageSender, (void *)&args );
        //Start the license thread
        lt = pthread_create(&messageRecThread, NULL, startMessageReciever, (void *)&args );
        
        if (lt){
            exit(-1);
        }
        
        if (ct){
            exit(-1);
        }
        
        pthread_attr_destroy(&attr);
        ct = pthread_join(messageRecThread, &status);
        
        pthread_attr_destroy(&attr);
        ct = pthread_join(messageSenderThread, &status);
        
        if (ct){
            exit(-1);
        }
        if (lt) {
            exit(-1);
        }
        
        pthread_exit(NULL);
    }
    else if(listen == 1) {
        network * np = new network();
        np->startListen( (argv[2]), atoi(argv[3]),1);
        delete np;
    }
    else if(listen == 2) {
        filehandler * fh = new filehandler();
        fh->sendFile(argv[2],argv[3],atoi(argv[4]));
        delete fh;
    }
    else {
        cout << "Invalid selection" <<endl;
        exit(-1);
    }

    return 0;
}

