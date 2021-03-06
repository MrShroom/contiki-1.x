/**
 * \file
 * uIP DNS resolver code header file.
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Copyright (c) 2002-2003, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
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
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: resolv.h,v 1.7 2005/02/07 07:08:03 adamdunkels Exp $
 *
 */
#ifndef __RESOLV_H__
#define __RESOLV_H__

#include "uip.h"

/**
 * Event that is broadcasted when a DNS name has been resolved.
 */
extern ek_event_t resolv_event_found;

/* Callbacks. */
void resolv_found(char *name, u16_t *ipaddr);

/* Functions. */
void resolv_conf(u16_t *dnsserver);
u16_t *resolv_getserver(void);
/*LOADER_INIT_FUNC(resolv_init, arg);*/
void resolv_init(char *arg);
u16_t *resolv_lookup(char *name);
void resolv_query(char *name);

#endif /* __RESOLV_H__ */
