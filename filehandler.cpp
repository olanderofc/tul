//
//  filehandler.cpp
//  tul


#include "filehandler.h"
#include "network.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <stdlib.h>    //malloc
#include <string.h>
#include <vector>

#define FILE "|file|"
#define START "|start|"
#define LAST "|last|"
#define MORE "|more|"
#define ALL "|all|"

// https://github.com/ReneNyffenegger/cpp-base64
filehandler::filehandler() {
    base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}

filehandler::~filehandler() {
    
}

void filehandler::writeFile(const char* fname, const char* data) {
    
    fstream fs;
    fs.open(fname, ios::app | ios::binary | ios_base::out);
    if(fs.is_open()) {
        fs << data;
    }
    else {
        cout << "Error opening fle: '" << fname << "'. Unable to write to file";
    }
    fs.close();
}

void filehandler::sendFile(const char* fName, const char* dest_ip, int dest_port) {

    ifstream::pos_type size;
    ifstream file (fName, ios::in|ios::binary|ios::ate);
    char * memblock;
    network np;

    if (file.is_open())
    {
        
        size = file.tellg();
        cout << "File is " << fName <<  " and the size is: " <<  size << " bytes " <<endl;
        cout << "Will now initiate transfer to server: " << dest_ip << endl;
        //650 MB max.
        if (size < 681574400) {
            memblock = new char [size];
            file.seekg (0, ios::beg);
            file.read (memblock, size);
            
            // |file|base64|start|
            // |file|base64|more|
            // |file|base64|last|
            
            //Send file data in chunks of 30.
            unsigned int chunk_size = 16;
            unsigned int left_overs = size % chunk_size;
            long rounds = ((long)size - left_overs) / chunk_size;
            
            cout << "This will take: "<< rounds <<" rounds "<<endl;
            cout << "There are a total of " << left_overs << " bytes of leftovers" <<endl;
            //exit(0);
            /*
             
             One round = |file|base64|all|
             Two rounds |file|base64|start|
                        |file|base64|last|
             
             Three or more |file|base64|start|
                            |file|base64|more|
                            ....
                            |file|base64|last|
             
             
             */
            stringstream ss;
            int tmp = 0;
            for(int i=0; i<rounds; i++) {
                char * mem;
                mem = new char[chunk_size];
                memcpy(mem,memblock+(chunk_size*i), chunk_size);
                
                //Encode the first data from the file.
                string encoded = filehandler::base64_encode(reinterpret_cast<const unsigned char*>(mem), chunk_size);
                
                //Concatinate the data with the file name
                ss << fName << "|" << encoded;
                
                //Create the final packet
                string packet = ss.str();
                
                //Clear the stringstream
                ss.clear();
                
                string full_packet = "";
                full_packet = filehandler::base64_encode(reinterpret_cast<const unsigned char*>(packet.c_str()), (unsigned int)packet.size());
                
                np.sendData(full_packet.c_str(), dest_ip, dest_port);
                
                //Clear the stringstream and memory.
                ss.str(string());
                delete [] mem;
                //Show status
                cout << i << "/" << rounds+1 <<endl;
                //Sleep. Not nercessary but could improve performance..
                usleep(100 * 1000);
                tmp = i;
            }
            if(left_overs != 0) {
                
                cout << tmp << "/" << rounds+1 <<endl;
                char * left = new char[left_overs];
                long start = (long)size-left_overs;
                
                memcpy(left, memblock + (start), left_overs);
                string encoded = filehandler::base64_encode(reinterpret_cast<const unsigned char*>(left), left_overs);
                ss << fName << "|" << encoded;
                string packet = ss.str();
                //cout << packet <<endl;
                encoded = filehandler::base64_encode(reinterpret_cast<const unsigned char*>(packet.c_str()), (unsigned int)packet.size());
                np.sendData(encoded.c_str(), dest_ip, dest_port);
               // cout << encoded <<endl;
                ss.str(string());
                delete [] left;
                
            }
           
            file.close();
            delete[] memblock;
        }
    }
    else {
        printf("Unable to open file: %s\n",fName);
        exit(0);
    }
    cout << "File successfully sent" <<endl;
}

//Thank you stack overflow
vector<string> filehandler::explode(const string& str, const char& ch) {
    string next;
    vector<string> result;
    
    // For each character in the string
    for (string::const_iterator it = str.begin(); it != str.end(); it++) {
        // If we've hit the terminal character
        if (*it == ch) {
            // If we have some characters accumulated
            if (!next.empty()) {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            }
        } else {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
        result.push_back(next);
    return result;
}


/*
 
 https://github.com/ReneNyffenegger/cpp-base64
 
 */

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string filehandler::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    
    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for(i = 0; (i <4) ; i++)
                ret += filehandler::base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (j = 0; (j < i + 1); j++)
            ret += filehandler::base64_chars[char_array_4[j]];
        
        while((i++ < 3))
            ret += '=';
        
    }
    
    return ret;
    
}

std::string filehandler::base64_decode(std::string const& encoded_string) {
    unsigned long in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = filehandler::base64_chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;
        
        for (j = 0; j <4; j++)
            char_array_4[j] = filehandler::base64_chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    
    return ret;
}