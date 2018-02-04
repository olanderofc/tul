//
//  filehandler.h
//  tul


#ifndef __tul__filehandler__
#define __tul__filehandler__

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class filehandler
{
    
public:
    void sendFile(const char*,const char*, int);
    void writeFile(const char*, const char*);
    std::string base64_encode(unsigned char const* , unsigned int len);
    std::string base64_decode(std::string const& s);
    vector<string> explode(const string&, const char& );
    
    ~filehandler();
    filehandler();
private:
    std::string base64_chars;
    
};
#endif /* defined(__tul__filehandler__) */
