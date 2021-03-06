/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the Contiki desktop environment
 *
 * $Id: netconf.c,v 1.13 2004/07/04 11:35:07 adamdunkels Exp $
 *
 */

#include "ek.h"
#include "uip.h"
#include "uiplib.h"
#include "uip_arp.h"
#include "resolv.h"
#include "ctk.h"
#include "ctk-draw.h"

#include "loader.h"

/* TCP/IP configuration window. */
static struct ctk_window tcpipwindow;

#ifdef WITH_ETHERNET
static struct ctk_label ipaddrlabel =
  {CTK_LABEL(0, 1, 10, 1, "IP address")};
static char ipaddr[17];
static struct ctk_textentry ipaddrtextentry =
  {CTK_TEXTENTRY(11, 1, 16, 1, ipaddr, 16)};
static struct ctk_label netmasklabel =
  {CTK_LABEL(0, 3, 10, 1, "Netmask")};
static char netmask[17];
static struct ctk_textentry netmasktextentry =
  {CTK_TEXTENTRY(11, 3, 16, 1, netmask, 16)};
static struct ctk_label gatewaylabel =
  {CTK_LABEL(0, 5, 10, 1, "Gateway")};
static char gateway[17];
static struct ctk_textentry gatewaytextentry =
  {CTK_TEXTENTRY(11, 5, 16, 1, gateway, 16)};
static struct ctk_label dnsserverlabel =
  {CTK_LABEL(0, 7, 10, 1, "DNS server")};
static char dnsserver[17];
static struct ctk_textentry dnsservertextentry =
  {CTK_TEXTENTRY(11, 7, 16, 1, dnsserver, 16)};
#else /* WITH_ETHERNET */
static struct ctk_label ipaddrlabel =
  {CTK_LABEL(0, 2, 10, 1, "IP address")};
static char ipaddr[17];
static struct ctk_textentry ipaddrtextentry =
  {CTK_TEXTENTRY(11, 2, 16, 1, ipaddr, 16)};
static struct ctk_label dnsserverlabel =
  {CTK_LABEL(0, 4, 10, 1, "DNS server")};
static char dnsserver[17];
static struct ctk_textentry dnsservertextentry =
  {CTK_TEXTENTRY(11, 4, 16, 1, dnsserver, 16)};
#endif /* WITH_ETHERNET */

static struct ctk_button tcpipclosebutton =
  {CTK_BUTTON(0, 9, 2, "Ok")};

EK_EVENTHANDLER(netconf_eventhandler, ev, data);
EK_PROCESS(p, "Network configuration", EK_PRIO_NORMAL,
	   netconf_eventhandler, NULL, NULL);
static ek_id_t id = EK_ID_NONE;

/*static DISPATCHER_SIGHANDLER(netconf_sighandler, s, data);
static struct dispatcher_proc p =
  {DISPATCHER_PROC("Network config", NULL, netconf_sighandler, NULL)};
  static ek_id_t id;*/


static void makestrings(void);

/*-----------------------------------------------------------------------------------*/
LOADER_INIT_FUNC(netconf_init, arg)
{
  arg_free(arg);
    
  if(id == EK_ID_NONE) {
    id = ek_start(&p);
  }
}
/*-----------------------------------------------------------------------------------*/
static char *
makebyte(u8_t byte, char *str)
{
  if(byte >= 100) {
    *str++ = (byte / 100 ) % 10 + '0';
  }
  if(byte >= 10) {
    *str++ = (byte / 10) % 10 + '0';
  }
  *str++ = (byte % 10) + '0';

  return str;
}
/*-----------------------------------------------------------------------------------*/
static void
makeaddr(u16_t *addr, char *str)
{
  str = makebyte(HTONS(addr[0]) >> 8, str);
  *str++ = '.';
  str = makebyte(HTONS(addr[0]) & 0xff, str);
  *str++ = '.';
  str = makebyte(HTONS(addr[1]) >> 8, str);
  *str++ = '.';
  str = makebyte(HTONS(addr[1]) & 0xff, str);
  *str++ = 0;
}
/*-----------------------------------------------------------------------------------*/
static void
makestrings(void)
{
  u16_t addr[2], *addrptr;

#ifdef WITH_UIP
  uip_gethostaddr(addr);
  makeaddr(addr, ipaddr);
  
#ifdef WITH_ETHERNET  
  uip_getnetmask(addr);
  makeaddr(addr, netmask);
  
  uip_getdraddr(addr);
  makeaddr(addr, gateway);
#endif /* WITH_ETHERNET */

  addrptr = resolv_getserver();
  if(addrptr != NULL) {
    makeaddr(addrptr, dnsserver);
  }
  
#endif /* WITH_UIP */

}
/*-----------------------------------------------------------------------------------*/
static void
nullterminate(char *cptr)
{
  /* Find the first space character in the ipaddr and put a zero there
     to end the string. */
  for(; *cptr != ' ' && *cptr != 0; ++cptr);
  *cptr = 0;
}
/*-----------------------------------------------------------------------------------*/
static void
apply_tcpipconfig(void)
{
  u16_t addr[2];

#ifdef WITH_UIP
  nullterminate(ipaddr);
  if(uiplib_ipaddrconv(ipaddr, (unsigned char *)addr)) {
    uip_sethostaddr(addr);
  }
  
#ifdef WITH_ETHERNET
  nullterminate(netmask);
  if(uiplib_ipaddrconv(netmask, (unsigned char *)addr)) {
    uip_setnetmask(addr);
  }

  nullterminate(gateway);
  if(uiplib_ipaddrconv(gateway, (unsigned char *)addr)) {
    uip_setdraddr(addr);
  }
#endif /* WITH_ETHERNET */
  
  nullterminate(dnsserver);
  if(uiplib_ipaddrconv(dnsserver, (unsigned char *)addr)) {
    resolv_conf(addr);
  }
#endif /* WITH_UIP */
}
/*-----------------------------------------------------------------------------------*/
static void
netconf_quit(void)
{
  ek_exit();
  id = EK_ID_NONE;
  LOADER_UNLOAD();
}
/*-----------------------------------------------------------------------------------*/
EK_EVENTHANDLER(netconf_eventhandler, ev, data)
{
  EK_EVENTHANDLER_ARGS(ev, data);

  if(ev == EK_EVENT_INIT) {
    /* Create TCP/IP configuration window. */
    ctk_window_new(&tcpipwindow, 30, 10, "TCP/IP config");
    /*    if(ctk_desktop_width(tcpipwindow.desktop) < 30) {
      ctk_window_move(&tcpipwindow, 0,
		      (ctk_desktop_height(tcpipwindow.desktop) - 10) / 2 - 2);
    } else {
      ctk_window_move(&tcpipwindow,
		      (ctk_desktop_width(tcpipwindow.desktop) - 30) / 2,
		      (ctk_desktop_height(tcpipwindow.desktop) - 10) / 2 - 2);
		      }*/
    
#ifdef WITH_ETHERNET
    CTK_WIDGET_ADD(&tcpipwindow, &ipaddrlabel);  
    CTK_WIDGET_ADD(&tcpipwindow, &ipaddrtextentry);
    CTK_WIDGET_ADD(&tcpipwindow, &netmasklabel);
    CTK_WIDGET_ADD(&tcpipwindow, &netmasktextentry);
    CTK_WIDGET_ADD(&tcpipwindow, &gatewaylabel);
    CTK_WIDGET_ADD(&tcpipwindow, &gatewaytextentry);
    CTK_WIDGET_ADD(&tcpipwindow, &dnsserverlabel);
    CTK_WIDGET_ADD(&tcpipwindow, &dnsservertextentry);
#else
    CTK_WIDGET_ADD(&tcpipwindow, &ipaddrlabel);  
    CTK_WIDGET_ADD(&tcpipwindow, &ipaddrtextentry);
    CTK_WIDGET_ADD(&tcpipwindow, &dnsserverlabel);
    CTK_WIDGET_ADD(&tcpipwindow, &dnsservertextentry);  
#endif /* WITH_ETHERNET */
    
    CTK_WIDGET_ADD(&tcpipwindow, &tcpipclosebutton);
    
    CTK_WIDGET_FOCUS(&tcpipwindow, &ipaddrtextentry);  

    /* Fill the configuration strings with values from the current
       configuration */
    makestrings();
    
    /*    dispatcher_listen(ctk_signal_button_activate);
	  dispatcher_listen(ctk_signal_window_close);*/
    ctk_window_open(&tcpipwindow);
  } else if(ev == ctk_signal_button_activate) {   
    if(data == (ek_data_t)&tcpipclosebutton) {
      apply_tcpipconfig();
      ctk_window_close(&tcpipwindow);
      netconf_quit();
      /*      ctk_desktop_redraw(tcpipwindow.desktop);*/
    }
  } else if(ev == ctk_signal_window_close ||
	    ev == EK_EVENT_REQUEST_EXIT) {
    ctk_window_close(&tcpipwindow);
    netconf_quit();
  }
}
/*-----------------------------------------------------------------------------------*/
