/*************************************************************************
*
*  zeroconf.hpp - UMS Server
*  Ubit GUI Toolkit - Version 6.0
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

#ifndef _zeroconf_hpp_
#define	_zeroconf_hpp_

#include "umserver.hpp"
#ifdef MACOSX
#  include <CoreFoundation/CoreFoundation.h>
#  include <DNSServiceDiscovery/DNSServiceDiscovery.h>
#else
#  include "mdns/mDNSCore/mDNSClientAPI.h" // API to the mDNS core code
#  include "mdns/mDNSPosix/mDNSPosix.h"    // specific types to run mDNS on this platform
#endif

/* ==================================================== ======== ======= */

class Mdns {
public:
  Mdns(UMServer*, const char* hostname);
  ~Mdns();

  bool registerUMSservice(const char* hostname, int portNo);
  bool browseUMSservices();
  void resolveAndAddRemoteUMS(const char* name, const char* type, const char* domain);
  
  static UMServer* ums;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef MACOSX
  dns_service_discovery_ref browse_client;
#else
  mDNS mdns;
  bool ok;
  struct {
    mDNS_PlatformSupport platform;  // Stores this platform's globals
    CacheRecord cache[500];
  } sto;
  struct {
    DNSQuestion question;
    bool ok;
  } brw;
  struct reg {
    ServiceRecordSet record_set;
    bool ok;
  } reg;
#endif
};

#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
