/*								-*- C++ -*-
 * $Id: Client.h,v 1.1 1996/01/10 14:56:24 markus Exp $
 *
 * Purpose: client part of an IPC connection
 *
 * Authors: Markus Holzem and Julian Smart
 *
 * Copyright: (C) 1995, AIAI, University of Edinburgh (Julian)
 * Copyright: (C) 1995, GNU (Markus)
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef Client_h
#define Client_h

#ifdef __GNUG__
#pragma interface
#endif

class wxClient : public wxIPCObject {
DECLARE_DYNAMIC_CLASS(wxClient)
public:
    wxClient(void);
    ~wxClient(void);

    virtual Bool ValidHost(char *host);
    virtual wxConnection *MakeConnection(char *host, char *server,
					 char *topic);
    virtual wxConnection *OnMakeConnection(void)
	{ return new wxConnection; }
private:
    int client_socket;
};


#endif // Client_h
