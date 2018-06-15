/************************************************************************
*
*  macloop.cpp : MacOSX CF main loop
*  Part of the Ubit GUI Toolkit
*  Authors: Stuart Pook, Eric Lecolinet
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

// !!! NOTE: l'API utilisee est OBSOLETE: voir DNSServiceDiscovery !!!

#include "umserver.hpp"
#ifdef MACOSX

#include <cstdlib>       // getenv, atexit
#include <cstdio>
#include <string>
#include <unistd.h>       // darwin
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "source.hpp"
#include <ubit/usocket.hpp>
#include <ubit/umsproto.hpp>
#include "zeroconf.hpp"
#define __CARBONSOUND__ 1  // hack pour eviter un bug de mise a jour de MacOSX
#include <Carbon/Carbon.h>
using namespace std;

static CFRunLoopRef runloop;

/* ==================================================== ======== ======= */

static void sourceReadCB(CFSocketRef s, CFSocketCallBackType type,
                         CFDataRef ref, const void *data, void *info) {
  EventSource* so = (EventSource*)info;
  if (CFSocketGetNative(s) == so->filedesc()) so->read();
}

/* ==================================================== ======== ======= */

static void cnxReadCB(CFSocketRef s, CFSocketCallBackType type,
                      CFDataRef ref, const void *data, void *info) {
  if (type != kCFSocketReadCallBack) return;
  USocket* sock = (USocket*)info;
  UInbuf inb;
  if (sock->receiveBlock(inb)) Mdns::ums->processRequest(sock, inb);
  else Mdns::ums->removeCnx(sock);
}

/* ==================================================== ======== ======= */

static void xReadCB(CFSocketRef s, CFSocketCallBackType type,
                    CFDataRef ref, const void *data, void *info) {
  if (type != kCFSocketReadCallBack) return;
  UMServer* ums = (UMServer*)info;
  
  if (CFSocketGetNative(s) == ums->getXConnection())
    ums->getAndProcessXEvents();
}

/* ==================================================== ======== ======= */

static void servsockReadCB(CFSocketRef s, CFSocketCallBackType type,
                           CFDataRef ref, const void* data, void* info) {
  if (type != kCFSocketReadCallBack) return;
  UMServer* ums = (UMServer*)info;
  
  if (CFSocketGetNative(s) == ums->getServerSocket()->getDescriptor()) {
    USocket* s = ums->getServerSocket()->accept();
    if (s) ums->addCnx(s);
  }
}

/* ==================================================== ======== ======= */

static bool loopAddSource(int fd, CFSocketCallBack callback, void* info)
{
  CFSocketContext context = {0, info, 0, 0, 0};
  CFSocketCallBackType type = kCFSocketReadCallBack;  // correct pour MouseSource ?
  CFSocketRef sf = CFSocketCreateWithNative(kCFAllocatorDefault, fd, type, 
                                            callback, &context);
  if (!sf) return false;
  CFRunLoopSourceRef rlsf = CFSocketCreateRunLoopSource(kCFAllocatorDefault,
                                                        sf, 0);
  if (!rlsf) return false;
  CFRunLoopAddSource(runloop, rlsf, kCFRunLoopDefaultMode);
  return true;
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// CF_LOOP SERVICES

static void addServiceCB(CFMachPortRef port, void *msg, CFIndex size, 
                         void *info) {
  (void)port; // Unused
  (void)size; // Unused
  (void)info; // Unused
  DNSServiceDiscovery_handleReply(msg);
}

static bool loopAddService(dns_service_discovery_ref client) {
  mach_port_t port = DNSServiceDiscoveryMachPort(client);
  if (!port) {
    cerr << "DNSServiceDiscoveryMachPort failed" << endl;
    return false;
  }
  CFMachPortContext context = { 0, 0, NULL, NULL, NULL };
  Boolean shouldFreeInfo;
  CFMachPortRef cfMachPort = 
    CFMachPortCreateWithPort(kCFAllocatorDefault,
                             port,
                             addServiceCB,
                             &context,
                             &shouldFreeInfo);
  CFRunLoopSourceRef rls = CFMachPortCreateRunLoopSource(NULL, cfMachPort, 0);
  CFRunLoopAddSource(runloop, rls, kCFRunLoopDefaultMode);
  CFRelease(rls);
  return true;
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// UMS SERVICE REGISTRATION

struct RegistrationData {
  RegistrationData(char const* s, int p) : port(p), name(s), n(0) {}
  const int port;
  const string name;
  dns_service_discovery_ref client;
  int n;
};

static void registrationCB(DNSServiceRegistrationReplyErrorType errorCode,
                           void *context) {
  switch (errorCode) {
    case kDNSServiceDiscoveryNoError:
      break;
      
    case kDNSServiceDiscoveryNameConflict: {
      RegistrationData* i = (RegistrationData*)context;
      if (i->n > 100) {
        cerr << "too many Mdns name collisions, giving up" << endl;
        return;
      }
      static char format[] = "%s (%d %d)";
      char buf[10 + sizeof format + sizeof((struct utsname*)0)->nodename];
      
      if (i->n++) sprintf(buf, format, i->name.c_str(), ntohs(i->port), i->n);
      else sprintf(buf, "%s (%d)", i->name.c_str(), ntohs(i->port));
      
      i->client = 
        DNSServiceRegistrationCreate(buf, UMS_ZEROCONF_NAME,
                                     "", i->port, "", registrationCB, i);
    }
      break;
      
    default:
      cerr << "registrationCB: unexpected error " <<  errorCode << endl;
      return;
  }
}

bool Mdns::registerUMSservice(const char* hostname, int port) {
  RegistrationData* regdata = new RegistrationData(hostname, port);
  
  regdata->client =
    // DNSServiceRegister
    DNSServiceRegistrationCreate(hostname,                // name
                                 UMS_ZEROCONF_NAME,       // type (_ums._tcp.)
                                 "",                      // domain (= local.)
                                 port,                    // port
                                 "",                      // host (default)
                                 registrationCB, regdata);// calback
    
    if (!regdata->client || !loopAddService(regdata->client)) {
      delete regdata;
      return false;
    }
    else return true;
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// SERVICE RESOLVER

struct ResolveAddressData {
  ResolveAddressData(UMServer* _ums, const char* _name, const char* _domain)
  : ums(_ums), name(_name), domain(_domain), resolve_client(0) {}
  
  void callback(const char* addr, int port)  {
    ums->addRemoteUMS(name.c_str(), addr, port, 0);
  }
  
  UMServer* ums;
  string name, domain;
  dns_service_discovery_ref resolve_client;
};

/* ==================================================== ======== ======= */

static void resolveAddressCB(struct sockaddr *interface,
                             struct sockaddr *address,
                             const char *txtRecord,
                             DNSServiceDiscoveryReplyFlags flags,
                             void *context) {
  char char_address_buf[INET6_ADDRSTRLEN + INET_ADDRSTRLEN];
  char const* char_address = 0;
  int port;
  
  ResolveAddressData* resdata = static_cast<ResolveAddressData*>(context);
  
  switch (address->sa_family) {
    case AF_INET: {
      struct sockaddr_in* sin = reinterpret_cast<struct sockaddr_in*>(address);
      char_address = inet_ntop(sin->sin_family, &sin->sin_addr.s_addr,
                               char_address_buf, sizeof char_address_buf);
      port = ntohs(sin->sin_port);
      //cerr << "resolveAddressCB: adr: " << char_address << " port " << port << endl;
      resdata->callback(char_address, port);
    }
      break;
      
    case AF_INET6: {
      struct sockaddr_in6* sin6 = reinterpret_cast<struct sockaddr_in6*>(address);
      char_address = inet_ntop(sin6->sin6_family, &sin6->sin6_addr,
                               char_address_buf, sizeof char_address_buf);
      port = ntohs(sin6->sin6_port);
      //cerr << "resolveAddressCB: adr: " << char_address << " port " << port << endl;
      resdata->callback(char_address, port);
    }
      break;
      
    default:
      cerr << "resolveAddressCB: unknown address family: " 
      << address->sa_family << endl;
      return;
  }
  
  if (resdata->resolve_client) 
    DNSServiceDiscoveryDeallocate(resdata->resolve_client);
  delete resdata;
}

/* ==================================================== ======== ======= */

void Mdns::resolveAndAddRemoteUMS(const char* name,
                                  const char* type, const char* domain)
{
  ResolveAddressData* resdata = new ResolveAddressData(ums, name, domain);
  
  dns_service_discovery_ref resolve_client =
    // DNSServiceResolve
    DNSServiceResolverResolve(name, type, domain, resolveAddressCB, resdata);
  
  if (resolve_client) {
    resdata->resolve_client = resolve_client;
    loopAddService(resolve_client);
  }
  else {
    delete resdata;
    cerr << "UMServer: resolveAddress failed: name=" << name << endl;
  }
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// UMS SERVICE BROWSER

static void serviceBrowserCB(DNSServiceBrowserReplyResultType resultType,
                             const char* name, const char* type, const char* domain,
                             DNSServiceDiscoveryReplyFlags flags, void *context)
{
  Mdns* mdns = static_cast<Mdns*>(context);
  
  switch (resultType) {
    case DNSServiceBrowserReplyAddInstance:
      //cerr << "UMS added: name " << name << " domain " << domain << endl;
      mdns->resolveAndAddRemoteUMS(name, type, domain);
      break;
      
    case DNSServiceBrowserReplyRemoveInstance:
      mdns->ums->removeRemoteUMS(name);
      break;
      
    default:
      //cerr << "serviceBrowserCB: unknown action: name " << name
      //<< " type " << type << " domain " << domain << endl;
      break;
  }
}

/* ==================================================== ======== ======= */

bool Mdns::browseUMSservices() {
  // DNSServiceBrowse
  browse_client = 
  DNSServiceBrowserCreate(UMS_ZEROCONF_NAME, // type (_ums._tcp.)
                          "", // domain (default= local.)
                          serviceBrowserCB, this);
  if (!browse_client || !loopAddService(browse_client)) return false;
  else return true;
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */

UMServer* Mdns::ums;

Mdns::Mdns(UMServer* _ums, const char* hostname) {
  ums = _ums;
  browse_client = null;
  
  // register as a UMS RendezVous service
  if (registerUMSservice(hostname, htons(ums->getServerSocket()->getLocalPort())))
    cout << " - ZeroConf: registerUMSservice launched" << endl;
  else
    cerr << " - ZeroConf: registerUMSservice failed" << endl;
  
  // register as a UMS RendezVous service browser
  if (browseUMSservices())
    cout << " - ZeroConf: browseUMSservices launched" << endl;
  else
    cerr << " - ZeroConf: browseUMSservices failed" << endl;
}

/* ==================================================== ======== ======= */

Mdns::~Mdns() {
  //cerr << "Mdns::~Mdns()" << endl;
  if (browse_client) DNSServiceDiscoveryDeallocate(browse_client);
  browse_client = null;
}

/* ==================================================== ===== ======= */

void UMServer::initMainLoop() {
  EventLoopRef evloop = GetMainEventLoop();
  runloop = (CFRunLoopRef)GetCFRunLoopFromEventLoop(evloop);
  
  if (!loopAddSource(getXConnection(), xReadCB, this)
      ||
      !loopAddSource(getServerSocket()->getDescriptor(), servsockReadCB, this)
      ) {
    cerr << "fatal error: can't add X connection or Server socket to CF loop" << endl;
    exit(1);
  }
}

void UMServer::runMainLoop() {
  // CFRunLoopRun();
  RunApplicationEventLoop();
  
  //cerr << "UMServer: CFRunLoopRun: finished" << endl;
  // registration seems to be removed automatically on exit
}

bool UMServer::addSourceToMainLoop(EventSource* s) {
  return loopAddSource(s->filedesc(), sourceReadCB, s);
}

bool UMServer::addSourceToMainLoop(USocket* s) {
  return loopAddSource(s->getDescriptor(), cnxReadCB, s);
}

#endif


