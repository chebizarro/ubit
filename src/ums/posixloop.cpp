/* ==================================================== ======== ======= *
*
*  posixloop.cpp : POSIX main loop
*  Part of the Ubit GUI Toolkit
*  Authors: Stuart Pook, Eric Lecolinet
*  (C) 2003-2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
*
* ***********************************************************************
* COPYRIGHT NOTICE :
* THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
* IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
* YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
* GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
* EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
* SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
* ***********************************************************************/

#include "umserver.hpp"
#ifndef MACOSX

#include <cstdlib>       // getenv, atexit
#include <cstdio>
#include <string>
#include <unistd.h>       // darwin
#include <sys/stat.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include "source.hpp"
#include "zeroconf.hpp"
using namespace std;

/* ==================================================== ======== ======= */

#define MAX_FD(maxfd,fd) {if ((fd) > (maxfd)) (maxfd) = (fd);}

bool UMServer::addSourceToMainLoop(EventSource*) {
  return true;
}

bool UMServer::addSourceToMainLoop(USocket*) {
  return true;
}

void UMServer::initMainLoop() {
}

void UMServer::runMainLoop() {
  UServerSocket* serv_sock = getServerSocket();
  
  while (true) {
    getAndProcessXEvents();
  
    fd_set read_set;
    int maxfd = 0;
    FD_ZERO(&read_set);

    // test X events
    FD_SET(xconnection, &read_set);
    MAX_FD(maxfd, xconnection);

    // test input sources from drivers
    for (unsigned int k = 0; k < sources.size(); k++) {
      FD_SET(sources[k]->filedesc(), &read_set);
      MAX_FD(maxfd, sources[k]->filedesc());
    }

    // test opened connections on client sockets
    for (CnxList::iterator c = cnxs.begin(); c != cnxs.end(); c++) {
      // verif validite sinon plantage si = -1
      if ((*c)->sock->getDescriptor() >= 0) {
        FD_SET((*c)->sock->getDescriptor(), &read_set);
        MAX_FD(maxfd, (*c)->sock->getDescriptor());
      }
    }

    // test requests on the server socket
    FD_SET(serv_sock->getDescriptor(), &read_set);
    MAX_FD(maxfd, serv_sock->getDescriptor());

    maxfd++;

    struct timeval timeout;
    // timeout.tv_sec = 0x3FFFFFFF; trop grand sur MacOSX!
    timeout.tv_sec = 0x3FFFFFF;
    timeout.tv_usec = 0;

    //ifdef HAVE_mDNS
    mDNSPosixGetFDSet(&mdns->mdns, &maxfd, &read_set, &timeout);
    //endif

    int has_input = select(maxfd,
                           &read_set,  // read
                           NULL,       // write
                           NULL,       // except
                           &timeout);  // timeout

    if (has_input < 0) {
      cerr << "UMServer::mainLoop: error on select" << endl;
      struct stat statbuf;

      // test open connections which have become invalid
      for (CnxList::iterator c = cnxs.begin(); c != cnxs.end(); c++) {
        if (fstat((*c)->sock->getDescriptor(), &statbuf) < 0) {
          removeCnx((*c)->sock);
        }
      }
      continue;
    }

    //ifdef HAVE_mDNS
    mDNSPosixProcessFDSet(&mdns->mdns, &read_set);
    //endif

    //process X events
    //if (FD_ISSET(xconnection, &read_set)) {
    //}

    // process input sources
    for (unsigned int k = 0; k < sources.size(); k++) {
      if (FD_ISSET(sources[k]->filedesc(), &read_set)) sources[k]->read();
    }

    // process opened connections on client sockets
    CnxList::iterator c2;
    for (CnxList::iterator c = cnxs.begin(); c != cnxs.end(); c = c2) {
      // attention: (*c) peut etre detruit par readComSocket()
      c2 = c; c2++;
      USocket* sock = (*c)->sock;
      
      if (sock->getDescriptor() < 0)
        removeCnx(sock);
      else if (FD_ISSET(sock->getDescriptor(), &read_set)) {
        UInbuf inb;
        if (sock->receiveBlock(inb)) processRequest(sock, inb);
        else removeCnx(sock);
      }
    }

    // process requests on server socket
    if (FD_ISSET(serv_sock->getDescriptor(), &read_set)) {
      USocket* s = serv_sock->accept();
      if (s) addCnx(s);
    }
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
#endif
