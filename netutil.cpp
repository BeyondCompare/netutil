/**
 * @file netutil.cpp
 * @brief 
 * @author culbertyang
 * @version 1.0
 * @date 2017-08-31
 */

#include <string>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

/*******************************************************
 * 通过net_name获取对应的ip
 * net_name : eth1
 * net_ip   : 返回的ip(数字形式)
 * ******************************************************/
int net2ip(const std::string& net_name, unsigned& net_ip)
{
  #define BUFFERSIZE 1024
  int sock_fd = -1;
  struct ifconf _if_conf;
  struct ifreq*  _if_req; 
  char buffer[BUFFERSIZE];
  int num = 0;
  int iter = 0;

  sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
  if(sock_fd < 0)
  {
    return -1;
  }
  _if_conf.ifc_len = BUFFERSIZE;
  _if_conf.ifc_buf = buffer;

  if(ioctl(sock_fd, SIOCGIFCONF, &_if_conf) < 0 )
  {
    close(sock_fd);
    return -2;
  }
  num = _if_conf.ifc_len / sizeof(struct ifreq);
  _if_req = _if_conf.ifc_req;

  for(iter = 0; iter < num; ++iter)
  {
    struct sockaddr_in* _sin = (struct sockaddr_in *)(&_if_req->ifr_addr);
    if(ioctl(sock_fd, SIOCGIFFLAGS, _if_req) < 0 )
    {
      close(sock_fd);
      return -3;
    }
    if((_if_req->ifr_flags & IFF_UP) && strcmp(net_name.c_str(),_if_req->ifr_name) == 0 ) 
    {   
      net_ip = static_cast<unsigned>(_sin->sin_addr.s_addr);
      close(sock_fd);  

      return 0;
    }   
    ++_if_req;
  }
  close(sock_fd);
  return -4;
}

/**************************************
 * 获取指定net_name的点分十进制
 * net_name "eth1"
 **************************************/
std::string net2strip(const std::string& net_name)
{
  struct in_addr _addr;
  net2ip(net_name, _addr.s_addr);
  return inet_ntoa(_addr);
}

int main(int argc, char** argv)
{
  unsigned net_ip = 0;
  std::string net_name = "eth1";
  int iRet = net2ip(net_name, net_ip);
  if(0 == iRet)
  {
    std::cout<<"net_ip = "<<net_ip<<endl;
    std::string strIp = net2strip(net_name);
    std::cout<<"strIp  = "<<strIp<<endl;
  }
  return EXIT_SUCCESS;
}
