# tul
tiny udp leaker - sending files and chat over dns


Technical details
---------------------
I made a thing back in 2013 and never finished it. It works quite ok, you can send files and chat over dns. It works great on OSX and Linux, it is actually built in Xcode.

Compiling
---------------------
````
Linux compile with g++ -lpthread main.cpp filehandler.cpp network.cpp dnsmaker.cpp -o tmp
````
On OSX just use Xcode

Acknowledgements
---------------------

https://github.com/ReneNyffenegger/cpp-base64
http://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/
