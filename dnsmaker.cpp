#include "dnsmaker.h"
#include "filehandler.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <stdlib.h>    /
#include <string.h>
using namespace std;

//List of DNS Servers registered on the system
//char dns_servers[10][100];
//int dns_server_count = 0;
//Types of DNS resource records :)

/*
 
 data = The data to be sent in the query
 Query type  = The type of query, MX, A etc.
 dest_ip = the ip to send the dns query to
 port = the port to send the query to, normally 53
 
*/


dnsmaker::dnsmaker() {
    
}

dnsmaker::~dnsmaker() {
    
}

 
void dnsmaker::create(const char* data,int query_type, const char* dest_ip, int port) {

unsigned char buf[65536],*qname;
int s;
query_type = T_A; //Set it to A Record as we only use that right now.

//struct sockaddr_in a;

//struct RES_RECORD answers[20],auth[20],addit[20]; //the replies from the DNS server
struct sockaddr_in dest;

struct DNS_HEADER *dns = NULL;
struct QUESTION *qinfo = NULL;

//printf("Resolving %s" , host);

s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); //UDP packet for DNS queries

dest.sin_family = AF_INET;
dest.sin_port = htons(port);
dest.sin_addr.s_addr = inet_addr(dest_ip); //dns servers

//Set the DNS structure to standard queries
dns = (struct DNS_HEADER *)&buf;

dns->id = (unsigned short) htons(getpid());
dns->qr = 0; //This is a query
dns->opcode = 0; //This is a standard query
dns->aa = 0; //Not Authoritative
dns->tc = 0; //This message is not truncated
dns->rd = 1; //Recursion Desired
dns->ra = 0; //Recursion not available! hey we dont have it (lol)
dns->z = 0;
dns->ad = 0;
dns->cd = 0;
dns->rcode = 0;
dns->q_count = htons(1); //we have only 1 question
dns->ans_count = 0;
dns->auth_count = 0;
dns->add_count = 0;

//point to the query portion
qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];

ChangetoDnsNameFormat(qname , (unsigned char*)data);
qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; //fill it

qinfo->qtype = htons( query_type ); //type of the query , A , MX , CNAME , NS etc
qinfo->qclass = htons(1); //Set value to 1 which is internet.
    
if( sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest)) < 0) {
    perror("sendto failed");
    
} else {
    //printf("Done\n\n");
    //printf("Packet sent\n");
    close(s);
}

}

void dnsmaker::printMessage(unsigned char* packet) {
    unsigned char *qname;
    qname = (unsigned char*)&packet[sizeof(struct DNS_HEADER)];
    stringstream ss;
    ss << qname;
    string data = ss.str();
    if(data.size() > 1) {
        string spacket = data.substr(1,data.size());
        struct DNS_HEADER *dns = NULL;
        dns = (struct DNS_HEADER*) &packet;
        filehandler fh;
        unsigned char *reader;
        reader = &packet[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
        string decoded = fh.base64_decode(spacket);
        cout << decoded <<endl;
    }else {
        cout << "Invalid message" << endl;
    }
}
void dnsmaker::printpacket(unsigned char* packet) {
    unsigned char *qname;
    qname = (unsigned char*)&packet[sizeof(struct DNS_HEADER)];
    stringstream ss;
    ss << qname;
    string data = ss.str();
    if(data.size() > 1) {
    string spacket = data.substr(1,data.size());
    struct DNS_HEADER *dns = NULL;
    dns = (struct DNS_HEADER*) &packet;
    unsigned char *reader;
    reader = &packet[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
    
    filehandler fh;
    string decoded = fh.base64_decode(spacket);
    //cout << "Decoded: " << decoded <<endl;
    
    vector<string> tmp = fh.explode(decoded,'|');
    if(tmp.size() == 2) {
        string packet_contents = fh.base64_decode(tmp.at(1));
        fh.writeFile(tmp.at(0).c_str(),packet_contents.c_str());
    } else { cout << "Packet error: "<< decoded <<endl; }
   // printf("Response %s", reader);
    /*
        printf("\nThe response contains : ");
        printf("\n %d Questions.",ntohs(dns->q_count));
        printf("\n %d Answers.",ntohs(dns->ans_count));
        printf("\n %d Authoritative Servers.",ntohs(dns->auth_count));
        printf("\n %d Additional records.\n\n",ntohs(dns->add_count));
     */
    }else {
        cout << "oob data:" << data << endl;
    }

}

void dnsmaker::ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");
    
    for(i = 0 ; i < strlen((char*)host) ; i++)
    {
        if(host[i]=='.')
        {
            *dns++ = i-lock;
            for(;lock<i;lock++)
            {
                *dns++=host[lock];
            }
            lock++; 
        }
    }
    *dns++='\0';
}
