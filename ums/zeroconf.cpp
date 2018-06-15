/* ==================================================== ======== ======= *
*
*  zeroconf.cpp : POSIX mDNS (except MAC OS X)
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
* **********************************************************************/

#include "umserver.hpp"

#ifndef MACOSX
#include <cstdlib>       // getenv, atexit
#include <cstdio>
#include <string>
#include <unistd.h>       // darwin
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ubit/umsproto.hpp>
#include "zeroconf.hpp"
using namespace std;

/* ==================================================== ======== ======= */
// UMS SERVICE REGISTRATION

// mDNS core calls this routine to tell us about the status 
// of our registration.

static void registrationCB(mDNS* const m,
                           ServiceRecordSet* const thisRegistration,
                           mStatus status) {
  switch (status) {
  case mStatus_NoError:
    // Do nothing; our name was successfully registered.
    // We may get more call backs in the future.
    break;

  case mStatus_NameConflict:
    // In the event of a conflict, just call mDNS_RenameAndReregisterService
    //  to automatically pick a new unique name for the service.
    status = mDNS_RenameAndReregisterService(m, thisRegistration, mDNSNULL);

    // Also, what do we do if mDNS_RenameAndReregisterService returns an error.
    // Right now I have no place to send that error to.
    
    if (status != mStatus_NoError)
      cerr << "registrationCB: mDNS_RenameAndReregisterService: Error" << endl;
    break;
    
  case mStatus_MemFree:
    //cerr << "registrationCB: Memory Free" << endl;
    break;
    
  default:
    //cerr << "registrationCB: Unknown Status: " << status << endl;
    break;
  }
}

/* ==================================================== ======== ======= */

bool Mdns::registerUMSservice(const char* hostname, int portNo) {
  mDNSOpaque16 port;
  port.NotAnInteger = portNo;
  //cerr<<"registerUMSservice: host "<<hostname<<" port "<< portNo<< endl;

  domainlabel name;
  MakeDomainLabelFromLiteralString(&name, hostname);
  
  domainname type;
  MakeDomainNameFromDNSNameString(&type, UMS_ZEROCONF_NAME);

  domainname domain;
  MakeDomainNameFromDNSNameString(&domain, "local.");

  mDNSu8 text;
  
  mStatus status =
    mDNS_RegisterService(&mdns, &reg.record_set,
                         &name, &type, &domain,  // Name, type, domain
                         NULL, port,             // Host and port
                         &text, 0,                // TXT data, length
                         NULL, 0,                // Subtypes
                         mDNSInterface_Any,      // Interace ID
                         registrationCB, mDNSNULL // Callback and context
                         );
  
  if (status != mStatus_NoError) return false; else return true;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// SERVICE RESOLVER

struct ResolveAddressData {
  ResolveAddressData(UMServer* _ums, const char* _name, const char* _domain)
    : ums(_ums), name(_name), domain(_domain) {}

  void callback(const char* addr, int port)  {
    ums->addRemoteUMS(name.c_str(), addr, port, 0);
  }

  UMServer* ums;
  string name, domain;
  ServiceInfoQuery query;
  ServiceInfo info;
};

/* ==================================================== ======== ======= */
/* Should not convert IP to char string.
 * Must use query->info->InterfaceIDto connect from the correct interface.
 */
static void resolveAddressCB(mDNS* m, ServiceInfoQuery* query)
{
  struct sockaddr_storage address;
  char char_address_buf[INET6_ADDRSTRLEN + INET_ADDRSTRLEN];
  char const * char_address;
  int port;

  if (query->info->ip.type == mDNSAddrType_IPv4) {
    struct sockaddr_in *sin = (struct sockaddr_in*)&address;
#if !defined(NOT_HAVE_SA_LEN)
    //		sin->sin_len  = sizeof(*sin);
#endif
    sin->sin_family = AF_INET;
    sin->sin_port = query->info->port.NotAnInteger;
    sin->sin_addr.s_addr = query->info->ip.ip.v4.NotAnInteger;

    char_address = inet_ntop(sin->sin_family, &sin->sin_addr.s_addr, 
			     char_address_buf, sizeof char_address_buf);
    port = ntohs(sin->sin_port);
    //printf("IPv4 %s:%d\n", char_address, port);
  }

  else {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&address;
#if !defined(NOT_HAVE_SA_LEN)
    // sin6->sin6_len = sizeof(*sin6);
#endif
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = query->info->port.NotAnInteger;
    sin6->sin6_flowinfo = 0;
    sin6->sin6_addr = *(struct in6_addr*)&query->info->ip.ip.v6;
    // sin6->sin6_scope_id = ifx ? ifx->scope_id : 0;

    char_address = inet_ntop(sin6->sin6_family, &sin6->sin6_addr, 
			     char_address_buf, sizeof char_address_buf);
    port = ntohs(sin6->sin6_port);
    //printf("IPv6 address %s:%d\n", char_address, port);
  }

  ResolveAddressData* resdata = 
    static_cast<ResolveAddressData*>(query->ServiceInfoQueryContext);

  if (!char_address) {
    cerr << "resolveAddressCB: conversion of IP address failed" << endl;
  }
  else {
    //cerr << "should connect via interface: " << query->info->InterfaceID << endl;
    resdata->callback(char_address, port);
  }

  mDNS_StopResolveService(m, query);
  query->ServiceInfoQueryContext = null;
  delete resdata;
}

/* ==================================================== ======== ======= */

void Mdns::resolveAndAddRemoteUMS(const char* name,
				   const char* type, const char* domain)
{
  string fullname = string(name) + "." + type + domain;
  //cerr <<"resolveAndAddRemoteUMS: fullname " << fullname << endl;

  ResolveAddressData* resdata = new ResolveAddressData(ums, name, domain);
  MakeDomainNameFromDNSNameString(&resdata->info.name, fullname.c_str());

  resdata->info.InterfaceID = mDNSInterface_Any;
  resdata->query.ServiceInfoQueryContext = null;

  mStatus status =
    mDNS_StartResolveService(&mdns, &resdata->query, &resdata->info,
                             resolveAddressCB, resdata);
  if (status != mStatus_NoError) {
    delete resdata;
    cerr << "UMServer:resolveAddress failed: name=" << name << endl;
  }
}  

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
// UMS SERVICE BROWSER

static void serviceBrowserCB(mDNS* const m,
                             DNSQuestion* const question,
                             ResourceRecord const* const answer,
                             mDNSBool const add_record)
{
  Mdns* mdns = static_cast<Mdns*>(question->QuestionContext);
  //assert(m == &mdns.mdns);
  //assert(question == &mdns.question);

  if (answer->rrtype != kDNSType_PTR) {
    cerr << "serviceBrowserCB: unexpected reply: " << answer->rrtype << endl;
    return;
  }

  domainlabel d_name;
  domainname  d_type;
  domainname  d_domain;
  DeconstructServiceName(&answer->rdata->u.name, &d_name, &d_type, &d_domain);

  char c_name[MAX_DOMAIN_LABEL+1];
  ConvertDomainLabelToCString_unescaped(&d_name, c_name);
  
  char c_type[MAX_ESCAPED_DOMAIN_NAME];
  ConvertDomainNameToCString(&d_type, c_type);

  char c_domain[MAX_ESCAPED_DOMAIN_NAME];
  ConvertDomainNameToCString(&d_domain, c_domain);

  //cerr << "serviceBrowserCB: add_record " <<add_record
  //     << " name " << c_name
  //     << " type " << c_type << " domain " << c_domain << endl;

  if (add_record)
    mdns->resolveAndAddRemoteUMS(c_name, c_type, c_domain);
  else
    mdns->ums->removeRemoteUMS(c_name);
}

/* ==================================================== ======== ======= */

bool Mdns::browseUMSservices() {
  domainname type;
  MakeDomainNameFromDNSNameString(&type, UMS_ZEROCONF_NAME);

  domainname domain;      // "local." is copied from the examples
  MakeDomainNameFromDNSNameString(&domain, "local."); 
  
  if (mDNS_StartBrowse(&mdns, &brw.question, 
		       &type, &domain, mDNSInterface_Any, 
		       serviceBrowserCB, this)
      != mStatus_NoError)
    return false; else return true;
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

UMServer* Mdns::ums;

Mdns::Mdns(UMServer* _ums, const char* hostname) {
  ums = _ums;
  ok = false;
  reg.ok = false;
  brw.ok = false;
  
  mStatus status =
    mDNS_Init(&mdns, &sto.platform, sto.cache, 
	      sizeof sto.cache / sizeof sto.cache[0],
	      //mDNS_Init_DontAdvertiseLocalAddresses, pour client seulement
	      mDNS_Init_AdvertiseLocalAddresses,
	      mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext);
  
  if (status != mStatus_NoError) {
    cerr << "UMServer: mDNSInit failed: " << status << endl;
  }
  else {
    ok = true;

    // register as a UMS RendezVous service
    reg.ok = registerUMSservice(hostname,
                                htons(ums->getServerSocket()->getLocalPort()));
    if (reg.ok)
      cout << " - ZeroConf: registerUMSservice launched" << endl;
    else
      cerr << " - ZeroConf: registerUMSservice failed" << endl;

    // register as a UMS RendezVous service browser
    brw.ok = browseUMSservices();
    if (brw.ok)
      cout << " - ZeroConf: browseUMSservices launched" << endl;
    else
      cerr << " - ZeroConf: browseUMSservices failed" << endl;
  }
}

/* ==================================================== ======== ======= */

Mdns::~Mdns() {
  if (reg.ok) {
    if (mDNS_DeregisterService(&mdns, &reg.record_set) != 0)
      cerr << "UMServer: failed to deregister UMS service: " << endl;
  }
  
  if (brw.ok) {
    mDNS_StopBrowse(&mdns, &brw.question);
  }

  if (ok) {
    // necessary otherwise the service is not deregistered
    mDNS_Close(&mdns);
  }
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */
#endif


