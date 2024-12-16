#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
                   
namespace http {

  class Request{
    public:
    std::map<std::string,std::vector<std::string>> data;
    std::string body, header, requestString;
  };

  class Response{
    public:
    int clientFD;
    int sendData(std::string header, std::string content){
      std::string file;  
      file+= header + content;
      send(this->clientFD,file.data(),file.length(),0);
      close(this->clientFD);
      return 0;
    }
  };
 
  class httpServer{
    private:
    int started, port, socketFD, maxHeaderSize;
    std::string ipAddress;
    struct sockaddr_in socketAddress;
    
    http::Request parseRequest(char *src, int *bodyStart){
      http::Request req;
      std::string s, prev, h;
      for(int i = 0, vut = 0, nl = 1, p = 0, q = 0; src[i]!='\0' && i<this->maxHeaderSize; i++, (*bodyStart)+=1){
        if(src[i]=='\r' && src[i+1]=='\n' && src[i+2]=='\r' && src[i+3]=='\n'){(*bodyStart) = i+4; break;}
        h+=src[i];
        if(src[i]=='\n') nl = 1;
        if(src[i]=='(') p+=1;
        if(src[i]==')'  && p>=1) p-=1;
        if(src[i]=='"' && !q) {q = 1, s+='"'; continue;}
        if(src[i]=='"' && q) q = 0;
        if(p==1 || q==1) s+=src[i];
        if(src[i]>32 && src[i]!=',' && !p && !q) s+=src[i];
        if(s[0]!=0 && !p && !q && (src[i]<=32 || (src[i]==':' && nl) ||  src[i]==',' || src[i+1]=='\0')) { 
          if(vut==2) req.data["version"].push_back(s); 
          if(vut==1) req.data["url"].push_back(s); 
          if(vut==0) req.data["type"].push_back(s);
          if(src[i]==':' && nl){prev = s.substr(0,s.length()-1), s="", nl=0; continue;}
          if(prev[0]!='\0'){
            if(s[0]=='(') req.data[prev][req.data[prev].size()-1]+=s; 
            else req.data[prev].push_back(s);
          }
          s="",vut+=1;
        }
      }
      req.header = h;
      return req;
    }

    public:
    std::string start(std::string ip, int port, void (*logBack)(void)){
      if(!this->started){ 
        this->socketFD = socket(AF_INET, SOCK_STREAM, 0);
        if(this->socketFD<0) return "Error: socket initialization failed!";
        this->port = port;
        this->ipAddress = ip;
        this->socketAddress.sin_family = AF_INET;
        this->socketAddress.sin_port = htons(this->port);
        this->socketAddress.sin_addr.s_addr = INADDR_ANY;
        if(bind(this->socketFD, (struct sockaddr*)&this->socketAddress, sizeof(this->socketAddress))>0) return "Error: socket binding failed!"; 
        if(listen(this->socketFD,10)<0) return "Error: socket connection failed!";
        if(*logBack!=NULL) logBack();
        this->started = 1;
        this->maxHeaderSize = 12000;
      }
      return "";
    }

    std::string Listen(int (*callBack)(http::Request, http::Response), int requestByteSize){
      if(!this->started) return "Error: server has not been started!"; 
      struct sockaddr_in clientAddress;
      socklen_t clientAddressLength = sizeof(clientAddress);
      int clientFD = 0, bodyStart = 0;

      if((clientFD = accept(this->socketFD, (struct sockaddr*)&clientAddress, &clientAddressLength))<0) return "";
      char buffer[requestByteSize];
      ssize_t bytes = recv(clientFD, buffer, requestByteSize, 0);

      if(bytes==-1) return "Error: data recieving failed!";
      if(bytes>requestByteSize) return "Error: incoming http request size is larger than the size provided!";
      if(bytes>0) {
        http::Request req = this->parseRequest(buffer, &bodyStart); 
        http::Response res;
        res.clientFD = clientFD;
        req.requestString = buffer;
        if(req.data.count("Content-Length")) req.body = req.requestString.substr(bodyStart, (std::stoi(req.data["Content-Length"][0])));
        callBack(req, res);
      }
      return "";
    } 
  };
}

#endif
