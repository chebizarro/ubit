/*
 *  socket.cpp: simple sockets
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */
//#include "config.h"  // HAVE_STDINT_H + socklen_t (needed on Linux but doesn't exist on BSD)
#include <ubit/ubit_features.h>
#ifdef HAVE_STDINT_H
#  include <stdint.h> // uint16_t etc.
#endif
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ubit/core/call.h>
#include <ubit/core/application.h>
#include <ubit/net/source.h>
#include <ubit/net/socket.h>
using namespace std;
namespace ubit {

// NOTE: attention aux possibles abort() sur un SIGPIPE: ce n'est pas gere
// ici, par contre ca l'est dans UMS via un signal(SIGPIPE)

const int DEFAULT_BACKLOG = 20;


ServerSocket::ServerSocket() :
listen_port(-1),
listen_sock(-1),
sin(new sockaddr_in()),
input(null) {
}

ServerSocket::ServerSocket(int _port) :
  listen_port(_port),
  listen_sock(-1),
  sin(new sockaddr_in()),
  input(null)
{
  bind(listen_port, 0, true);
}

bool ServerSocket::bind(int port, int backlog, bool reuse_address) {
  listen_port = port;
  listen_sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0) {
    //cerr << "ServerSocket: can't create listen socket" << endl;
    return false;
  }

  if (listen_port < 0) listen_port = 0;
  sin->sin_family = AF_INET;          // host byte order
  sin->sin_addr.s_addr = INADDR_ANY;  // short, network byte order
  sin->sin_port = htons(listen_port);
  //memset(&(sockaddr.sin_zero),'\0',8); //plante! zero the rest of the struct

  // pour reutiliser plusieurs fois la meme adresse
  int tmp = int(reuse_address);
  ::setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,(char*)&tmp,sizeof(tmp));
  
  if (::bind(listen_sock, (struct sockaddr*)sin, sizeof(*sin)) < 0) {
    //cerr << "ServerSocket: port " << listen_port << " busy" << endl;
    if (!reuse_address) return false;
  }

  // socklen_t required on Linux but doesn't exist on BSD
#ifdef HAVE_NO_SOCKLEN_T
  int taille = sizeof sin;
#else
  socklen_t taille = sizeof sin;
#endif

  // le serveur se met en attente sur le socket d'ecoute
  if (backlog <= 0) backlog = DEFAULT_BACKLOG;
  ::listen(listen_sock, backlog);

  // verifications sur le serveur
  if (::getsockname(listen_sock, (struct sockaddr*)sin, &taille) < 0) {
    //cerr << "ServerSocket: fatal error on getsocketname" << endl;
    listen_sock = -1;
    return false;
  }

  int nport = ntohs(sin->sin_port);
  /*if (nport != listen_port)*/ listen_port = nport;
  return true;
}

void ServerSocket::onInput(UCall& c) {
  if (!input) input = new Source();
  input->onInput(c);
  if (listen_sock >= 0) input->open(listen_sock);
}

//void ServerSocket::acceptOnInput() {
  //  onInput(ucall(this, &ServerSocket::accept));
// ne sert a rien car de toute facon il faut faire qq chose sur le socket
// (typiquement un  onInput(ucall(socket, qqchose))
//}

ServerSocket::~ServerSocket() {
  close();
  delete sin;
}

void ServerSocket::close() {
  if (listen_sock >= 0) {
    //::shutdown(sock, 2); ??
    ::close(listen_sock);
  }
  listen_sock = -1;  // indiquer com_sock inutilisable par write/read*()

  delete input;
  input = null;
}


Socket* ServerSocket::accept() {
  int sock_com = -1;

  // cf. man -s 3n accept, attention EINTR ou EWOULBLOCK ne sont pas geres!
  if ((sock_com = ::accept(listen_sock, NULL, NULL)) == -1) {
    //cerr << "ServerSocket: error on accept" << endl;
    return null;
  }

  // turn off TCP coalescence for INET sockets
  // NB: necessaire (au moins) pour MacOS, sinon delai de transmission
  int tmp = 1;
  ::setsockopt(sock_com, IPPROTO_TCP, TCP_NODELAY, (char*)&tmp,sizeof(int));

  Socket* s = createSocket();
  s->sock = sock_com;
  return s;
}

// ============================================================== ====== =======

Socket::Socket()
: remport(0), sock(-1), sin(null), input(null) {}

Socket::Socket(const char* _host, int _port) :
remport(0), sock(-1), sin(null), input(null) {
  connect(_host, _port);
}

Socket::Socket(const String& _host, int _port) :
remport(0), sock(-1), sin(null), input(null) {
  connect(_host.c_str(), _port);
}

Socket::~Socket() {
  close();  // en fait close fait delete input !
  delete input;
}

void Socket::onInput(UCall& c) {
  if (!input) input = new Source();
  input->onInput(c);
  if (sock >= 0) input->open(sock);  
}
  
void Socket::close() {
  if (sock >= 0) {
    ::shutdown(sock, 2);
    ::close(sock);
  }
  sock = -1;  // indiquer com_sock inutilisable par write/read*()
  delete sin; sin = null;
  delete input; input = null;  //9aug05: = null etait oublie !
}
  
int Socket::connect(const char* host, int port) {
  close();
  if (!host) host = "localhost";
  remport = port;
  
  // creer l'adresse du remote host a partir de son nom (getipnodebyname)
  struct hostent* hostinfo = gethostbyname(host);
  if (!hostinfo) return (sock = -2);

  sin = new sockaddr_in();
  sin->sin_family = AF_INET;
  sin->sin_port = htons(remport);
  memcpy(&sin->sin_addr, hostinfo->h_addr_list[0], hostinfo->h_length);
  //memcpy(&rhost->sin_zero, 0, 8);

  // create the socket
  sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return (sock = -1);

  // turn off TCP coalescence for INET sockets
  // NB: necessaire (au moins) pour MacOS, sinon delai de transmission
  int tmp = 1;
  ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,(char*)&tmp, sizeof(int));  // A REVOIR (option)

  // connection with the remote host
  if (::connect(sock, (struct sockaddr*)sin, sizeof(*sin)) < 0){
    //Application::error("UMSclient::openComSocket",
    //	   "Can't connect port %d of remote host %s", remote_port, remote_server);
    return (sock = -1);
  }

  if (input) input->open(sock);  
  return sock;
}

bool Socket::sendBytes(const char* data, unsigned int size) {
  if (sock < 0) return false;
  
  // !!! ici il faut une boucle car il se peut qu'il faille plusieurs write !!! A REVOIR
  // de plus si rec.pos est trop grand il faudrait decomposer

  if (::write(sock, data, size) < 0) {
    sock = -1;   // error  dangereux: 0 serait preferable ?
    return false;
  }
  return true;
}

bool Socket::sendBlock(const char* data, unsigned short size) {
  if (sock < 0) return false;

  uint16_t net_size = htons(size); // sends the size of the packet first
  if (::write(sock, &net_size, 2) != 2) {
    sock = -1;  // error
    return false;
  }
  return sendBytes(data, size);
}

bool Socket::sendBlock(OutBuffer& ob) {
  if (sock < 0) return false;

  uint16_t net_size = htons(ob.outpos);
  memcpy(ob.buffer, &net_size, 2);  // stores the size of the packet first
  
  return sendBytes(ob.buffer, ob.outpos+2);  // +2 for the size
}

bool Socket::receiveBytes(char* data, unsigned int size) {
  if (sock < 0) return false;
  unsigned int received = 0;

  // attention, plusieurs read() peuvent etre necessaires
  while (1) {
    int n = ::read(sock, data + received, size-received);
    if (n < 0) {
      close();   //error
      return false;
    }
    else if (n == 0) {  // the remote side has closed the connection
      close();
      return false;
    }
    else {
      received += n;
      if (received >= size) break;
    }
  }
  return true;
}

/*
bool Socket::receiveBlock(char*& data, unsigned short& size) {
  if (sock < 0) return false;

  uint16_t net_size = 0;   // get block size
  if (!receiveBytes((char*)&net_size, 2)) return false;

  size = ntohs(net_size);
  data = (char*) realloc(data, size);
  if (!data) return false;
  
  return receiveBytes(data, size);
}
*/
// NB: InBuffer contient egalement 2 bytes de size au debut, ce qui n'est
// pas indispensable mais permet un traitement symetrique

bool Socket::receiveBlock(InBuffer& buf) {
  if (sock < 0) return false;

  uint16_t net_size = 0;   // get block size
  if (!receiveBytes((char*)&net_size, 2)) return false;

  unsigned short size = ntohs(net_size);
  // !!NB: un peu absurde: ne pas sauver les 2 bytes dans le buffer !
  buf.resize(size);
  buf.outpos = size +2;  //13nov04: +2 rajoute
  memcpy(buf.buffer, &net_size, 2);  // coherence
  return receiveBytes(buf.buffer+2, size);
}


IOBuffer::IOBuffer() {
  buffer  = default_buffer;
  bufsize = DEFAULT_BUFSIZE;
  inpos = outpos = 2;  // skip the size
}

IOBuffer::~IOBuffer() {
  if (buffer != default_buffer && buffer) free(buffer);
  buffer = null;
}

/*
IOBuffer::IOBuffer(unsigned short _size) {
  if (_size+2 <= DEFAULT_SIZE) {
    buffer  = default_buffer;
    memsize = DEFAULT_SIZE;
  }
  else {
    buffer = (char*)malloc(_size+2);
    memsize = _size+2;
  }
  pos = 2; // skip the size
}
*/
 
char* IOBuffer::data() {
  return ((buffer && outpos > 2) ? buffer+2 : null);
}

const char* IOBuffer::data() const {
  return ((buffer && outpos > 2) ? buffer+2 : null);
}

unsigned int IOBuffer::size() const {
  return outpos > 2 ? outpos-2 : 0;
}

unsigned int IOBuffer::consumed() const {
  return inpos > 2 ? inpos-2 : 0;
}

bool IOBuffer::resize(unsigned short _size) {
  bufsize = _size+2;
  
  if (buffer == null) {  // should not happen
    buffer = (char*)malloc(bufsize);
  }
  else if (buffer == default_buffer) {
    if (bufsize > DEFAULT_BUFSIZE) {
      buffer = (char*)malloc(bufsize);
      memcpy(buffer, default_buffer, DEFAULT_BUFSIZE);
    }
  }
  else buffer = (char*)realloc(buffer, bufsize);

  if (!buffer) bufsize = 0;
  return (buffer!=null);
}

bool IOBuffer::augment(unsigned short sz) {
  int nblocks = sz / AUGMENT_QUANTUM + 1;
  bufsize += nblocks * AUGMENT_QUANTUM;
  resize(bufsize-2);
  return (buffer && bufsize-2 <= sizeof(short));
}


void OutBuffer::writeChar(char x) {
  if (outpos >= bufsize) augment(1);
  buffer[outpos++] = x;
}

void OutBuffer::writeChar(unsigned char x) {
  if (outpos >= bufsize) augment(1);
  buffer[outpos++] = char(x);
}

void OutBuffer::writeShort(short x) {
  if (outpos+1 >= bufsize) augment(2);
  uint16_t net_x = htons((uint16_t) x);
  memcpy(&buffer[outpos], &net_x, 2);
  outpos += 2;
}

void OutBuffer::writeLong(long x) {
  if (outpos+3 >= bufsize) augment(4);
  uint32_t net_x = htonl((uint32_t) x);
  memcpy(&buffer[outpos], &net_x, 4);
  outpos += 4;
}

void OutBuffer::writeString(const char* s, unsigned int ll) {
  if (!s || !*s) return; 
  if (outpos+ll >= bufsize) augment(ll+1);
  strncpy(&buffer[outpos], s, ll);
  buffer[outpos+ll] = 0;
  outpos += ll+1;
}

void OutBuffer::writeString(const char* s) {
  if (s) writeString(s, strlen(s));
}

void OutBuffer::writeString(const String& s) {
  writeString(s.c_str(), s.length());
}


void InBuffer::readChar(char& x) {
  x = (char)buffer[inpos++];
}

void InBuffer::readChar(unsigned char& x) {
  x = (unsigned char)buffer[inpos++];
}

void InBuffer::readShort(short& x) {
  uint16_t netl;
  memcpy(&netl, &buffer[inpos], 2);
  inpos += 2;
  x = short(htons(netl));
}

void InBuffer::readLong(long& x) {
  uint32_t netl;
  memcpy(&netl, &buffer[inpos], 4);
  inpos += 4;
  x = long(htonl(netl));
}

void InBuffer::readString(String& s) {
  s = "";
  // 12jan05: c'est seulement le 0 qui sert de separateur, plus space!
  int ll = 0;
  // FAUDRAIT TESTER LA TAILLE !!!!!
  while (buffer[inpos+ll] != 0 /* && buffer[inpos+ll] != ' ' */) ll++;
  s.append(buffer+inpos, ll);
  inpos += ll+1;
}


void OutBuffer::writeEvent(unsigned char event_type, unsigned char event_flow,
                         long x, long y, unsigned long detail) {
  if (outpos+13 >= bufsize) augment(14);

  buffer[outpos] = event_type;
  outpos += 1;

  buffer[outpos] = event_flow;
  outpos += 1;

  uint32_t net_x = htonl((uint32_t) x);
  memcpy(&buffer[outpos], &net_x, 4);
  outpos += 4;

  uint32_t net_y = htonl((uint32_t) y);
  memcpy(&buffer[outpos], &net_y, 4);
  outpos += 4;

  uint32_t net_d = htonl((uint32_t)detail);
  memcpy(&buffer[outpos], &net_d, 4);
  outpos += 4;
};

void InBuffer::readEvent(unsigned char& event_type, unsigned char& event_flow,
                       long& x, long& y, unsigned long& detail) {
  event_type = buffer[inpos];
  inpos += 1;

  event_flow = buffer[inpos];
  inpos += 1;

  uint32_t netl;

  memcpy(&netl, &buffer[inpos], 4);
  x = long(htonl(netl));
  inpos += 4;

  memcpy(&netl, &buffer[inpos], 4);
  y = long(htonl(netl));
  inpos += 4;

  memcpy(&netl, &buffer[inpos], 4);
  detail = (unsigned long) htonl(netl);
  inpos += 4;
};

}

