//
//  network.h
//  tul


#ifndef __tul__network__
#define __tul__network__

class network {
    
public:
    network();
    ~network();

    void startListen(const char* , int, int);
    void startSend(const char* , int);
    void sendData(const char*,const char*,int);
    int validPort(int);
    int validIP(const char*);
    
private:

    
};

#endif /* defined(__tul__network__) */
