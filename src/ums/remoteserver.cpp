/* ==================================================== ======== ======= *
*
*  remoteserver.cpp : UMS requests from a UMS client
*  Part of the Ubit GUI Toolkit
*  (C) 2003-2008 Eric Lecolinet / ENST Paris
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

#include <cstdio>
#include <string.h>
#include "umserver.hpp"
#include "flow.hpp"
#include "events.hpp"
#include "remoteserver.hpp"
#include <ubit/umsproto.hpp>
#include <ubit/umservice.hpp>
using namespace std;

/* ==================================================== ===== ======= */

RemoteUMS::RemoteUMS(const char* _name, const char* _addr, int _port, int _pos) :
  name(_name), 
  address( (!_addr || !*_addr) ? _name : _addr ),
  port( (_port == 0) ? UMS_PORT_NUMBER : _port ),
  pos(_pos), 
  cnx_time(0), 
  client(null) {
}

RemoteUMS::~RemoteUMS() {
  delete client;
}

/* ==================================================== ======== ======= */

void UMServer::sendRemoteUMSState(RemoteUMS* r, int state) {
  for (CnxList::iterator c = cnxs.begin(); c != cnxs.end(); c++) {
    if ((*c)->browse_servers) { // n'envoyer qu'a ceux qui le demandent
      char msg[300];
      sprintf(msg, "_umsBrowse %s\t%s\t%d\t%d", 
              r->name.c_str(), UMS_ZEROCONF_NAME, state, r->pos);
      events.sendMessage((*c)->sock, msg);
    }
  }
}

void UMServer::sendRemoteUMSList(Cnx* to) {
  for (RemoteUMSList::iterator r = remotes.begin(); r!=remotes.end(); r++) {
    char msg[300];
    sprintf(msg, "_umsBrowse %s\t%s\t%d\t%d", 
            (*r)->name.c_str(), UMS_ZEROCONF_NAME, RemoteUMS::ADD, (*r)->pos);
    events.sendMessage(to->sock, msg);
  }
}

/* ==================================================== ======== ======= */

static void _sendNeighborState(UMServer* ums, USocket* sock,
			       RemoteUMS* r, int pos) {
  char msg[300];
  if (r) {
    sprintf(msg, "_umsNeighbor %s\t%s\t%d\t%d",
	    r->name.c_str(), 
	    r->address.c_str(), 
	    r->port, 
	    pos);
  }
  else sprintf(msg, "_umsNeighbor none\tnone\t0\t%d", pos);
  //cerr << "sendNeighborState "<< msg << endl;
  ums->events.sendMessage(sock, msg);
}

void UMServer::sendNeighborState(RemoteUMS* r, int pos) {
  for (CnxList::iterator c = cnxs.begin(); c != cnxs.end(); c++) {
    if ((*c)->browse_servers)    // n'envoyer qu'a ceux qui le demandent
      _sendNeighborState(this, (*c)->sock, r, pos);
  }
}

void UMServer::sendNeighborList(Cnx* to) {
  _sendNeighborState(this, to->sock, l_neighbor, 'l');
  _sendNeighborState(this, to->sock, r_neighbor, 'r');
  _sendNeighborState(this, to->sock, t_neighbor, 't');
  _sendNeighborState(this, to->sock, b_neighbor, 'b');
}

/* ==================================================== ======== ======= */

RemoteUMSList::iterator UMServer::findRemoteUMS(const char* host) {
  for (RemoteUMSList::iterator k = remotes.begin(); k != remotes.end(); k++) {
    if ((*k)->name == host || (*k)->address == host) return k;
  }
  return remotes.end();
}

/* ==================================================== ======== ======= */

RemoteUMS* UMServer::addRemoteUMS(const char* name, const char* address,
                                  int port, int pos) {
  if (!name || !*name) return null;

  RemoteUMSList::iterator k = findRemoteUMS(name);
  if (k != remotes.end()) {
    // mettre a jour si changement
    if (address) (*k)->address = address;
    if (port) (*k)->port = port;
    if (pos) (*k)->pos = pos;
    //sendRemoteUMSState(*k, RemoteUMS::CHANGE);
    return *k;  
  }
  else {
    RemoteUMS* r = new RemoteUMS(name, address, port, pos);
    remotes.push_back(r);
    sendRemoteUMSState(r, RemoteUMS::ADD);
    if (r->pos != 0) sendNeighborState(r, r->pos);
    return r;
  }
}

/* ==================================================== ======== ======= */

void UMServer::removeRemoteUMS(const char* name) {
  RemoteUMSList::iterator k = findRemoteUMS(name);
  
  if (k == remotes.end())    // pas dans la liste
    return;
  else {
    if ((*k)->pos) setNeighborUMS((*k)->pos, "none");
    sendRemoteUMSState(*k, RemoteUMS::REMOVE);
    delete *k;
    remotes.erase(k);
  }
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */

RemoteUMS* UMServer::getNeighborUMS(int pos) const {
  switch (pos) {
    case RemoteUMS::LEFT:   return l_neighbor;
    case RemoteUMS::RIGHT:  return r_neighbor;
    case RemoteUMS::TOP:    return t_neighbor;
    case RemoteUMS::BOTTOM: return b_neighbor;
    default: return null;
  }
}

RemoteUMS* UMServer::setNeighborUMS(int pos, const char* host) {
  RemoteUMS** pr = null;

  switch (pos) {
    case RemoteUMS::LEFT:   pr = &l_neighbor; break;
    case RemoteUMS::RIGHT:  pr = &r_neighbor; break;
    case RemoteUMS::TOP:    pr = &t_neighbor; break;
    case RemoteUMS::BOTTOM: pr = &b_neighbor; break;
    default: return null;
  }

  // interdire les connections sur lui-meme (blocages!)
  // + empty name <=> unset
  if (!host || !*host || strcmp(host,"none") == 0
      // vaut mieux interdire "localhost" pour eviter les blocages
      // ou bouclages
      || strcmp(host,"localhost") == 0
      || host == hostname
      )
    goto END;
  
  {
    RemoteUMSList::iterator k = findRemoteUMS(host);
    if (k != remotes.end()) {
      if ((*k)->name == hostname) goto END;
      if ((*k)->pos != pos) {
        (*k)->pos = pos;
        sendNeighborState(*k, pos);
      }
      *pr = *k;
    }
    else {
      *pr = addRemoteUMS(host, null, 0, pos);
      sendNeighborState(*pr, pos);
    }
  }
  return *pr;
  
 END:
  if (*pr) {
    (*pr)->pos = 0;
    *pr = null;
  }
  sendNeighborState(null, pos);
  return null;
}

/* ==================================================== ======== ======= */
/*
RemoteUMS* UMServer::setNeighborUMS(const char* pos, const char* name) {
  //if (!name || !name) return false;
  
  switch (pos[0]) {
    case 't':  // top
      //if (pos[1] == 'l' || pos[1] == 'w') return addNeighbor(-1,-1, host);
      //else if (pos[1] == 'r' || pos[1] == 'e')return addNeighbor(-1,+1, host);
      return setNeighborUMS(RemoteUMS::TOP, name);
      break;
      
    case 'l':  // left
      return setNeighborUMS(RemoteUMS::LEFT, name);
      break;

    case 'r':  // rigth
      return setNeighborUMS(RemoteUMS::RIGHT, name);
      break;

    case 'b':  // bottom
      //if (pos[1] == 'l' || pos[1] == 'w')return addNeighbor(+1,-1, host);
      //else if (pos[1] == 'r' || pos[1] == 'e')return addNeighbor(+1,+1, host);

      return setNeighborUMS(RemoteUMS::BOTTOM, name);
      break;
  }
  return false;
}
*/
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

