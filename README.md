# HTTP server in CPP
This project is a demonstration of an http server in cpp.

This project is composed of a namespace called `http`
and that name space is composed of three classes:

### httpServer:
```cpp
class httpServer{
  std::string start(std::string ip, int port, void (*logBack)(void));
  std::string Listen(int (*callBack)(http::Request, http::Response), int requestByteSize);
  void closeServer();
};
```

### Response:
```cpp
class Response{
  public:
  int clientFD;
  int sendData(std::string header, std::string content);
};
```

### Request:
```cpp
class Request{
  public:
  std::map<std::string,std::vector<std::string>> data;
  std::string body, header, requestString;
};
```

### usage
```cpp

int handleReq(http::Request req, http::Response res){...}
void log(){std::cout<<"listening on "<<"http://localhost:5500"<<'\n';}
void error(std::string err){if(err[0]!='\0') std::cout<<err<<'\n', exit(1);}

void startServer(){
  http::httpServer server;
  std::string err = server.start("localhost",5500,&log);
  error(err);
  
  while(1){
    err = server.Listen(&handleReq, 9000);
    error(err);
  }
}

int main(){
  startServer();
  return 0;
}
```

make sure to see the example