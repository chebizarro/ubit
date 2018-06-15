/*************************************************************************
 *
 *  remoteserver.hpp - UMS Server
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#ifndef _umsrserv_hpp_
#define	_umsrserv_hpp_
#include <string>
#include <ubit/umservice.hpp>

/** Remote UMServer
*/
struct RemoteUMS {
  enum {LEFT='l', RIGHT='r', TOP='t', BOTTOM='b'};
  enum {REMOVE=0, ADD=1, CHANGE=2};
  
  RemoteUMS(const char* name, const char* addr, int port, int pos);
  ~RemoteUMS();

  std::string name, address;
  int port;
  int pos;
  TimeID cnx_time;
  class ubit::UMService* client;
};

#endif
/* ==================================================== [TheEnd] ======= */
