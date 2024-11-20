#include "../http_server.hpp"
#include <iostream>
#include <sstream> 
#include <fstream>

int handleReq(http::Request req, http::Response res){

  std::map<std::string, std::vector<std::string>> files = {
    {"/", {"text/html","Example/index.html"}},
    {"/example.css", {"text/css","Example/example.css"}},
    {"/example.js", {"text/js","Example/example.js"}},
    {"/favicon.ico", {"image/x-con", "Example/favicon.ico"}},
    {"/image.jpg", {"image/jpg", "Example/image.jpg"}}
  };

  std::cout<<"ENTIRE REQUEST:\n"<<req.requestString<<"\n\n";
  std::cout<<"REQUEST HEADER:\n"<<req.header<<"\n\n";
  std::cout<<"REQUEST BODY:\n"<<req.body<<"\n\n";

  if(req.data["type"][0].compare("GET")==0 && files.count(req.data["url"][0])){
    std::ifstream file;
    std::string txt, line, header;
    if(files[req.data["url"][0]][0][0]=='t'){ 
      file.open(files[req.data["url"][0]][1]);
      while(std::getline(file,line)) txt+=line, txt+='\n';
      file.close();
      header = "HTTP/1.1 200 OK\r\nContent-Type: " + files[req.data["url"][0]][0]+"\r\n\r\n";
      res.sendData(header,txt);
    } else{
      file.open(files[req.data["url"][0]][1],std::ios::binary);
      std::ostringstream oss;
      oss<<file.rdbuf();
      txt = oss.str();
      file.close();
      header = "HTTP/1.1 200 OK\r\nContent-Type: " + files[req.data["url"][0]][0]+"\r\nContent-Length: " + std::to_string(txt.length()) + "\r\n\r\n";
      res.sendData(header, txt);
    }
  }
  return 0;
}

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
