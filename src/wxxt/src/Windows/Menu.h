/*								-*- C++ -*-
 * $Id: Menu.h,v 1.1 1996/01/10 14:57:16 markus Exp $
 *
 * Purpose: simple menu class
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

#ifndef Menu_h
#define Menu_h

#ifdef __GNUG__
#pragma interface
#endif

#if defined(Have_Xt_Types)
class wxMenu_Widgets {
public:
    Widget shell;
    Widget menu;
};
#else
class wxMenu_Widgets;
#endif

class wxCommandEvent;
class wxColour;
class wxFont;

typedef void (*wxFunction)(wxObject&, wxCommandEvent&);

typedef void *wxMenuItem;

class wxMenu : public wxObject {
DECLARE_DYNAMIC_CLASS(wxMenu)
public:
    wxMenu(char *title=NULL, wxFunction func=NULL);
    ~wxMenu(void);

    // popup menu (used by wxWindow);
#   ifdef Have_Xt_Types
    Bool  PopupMenu(Widget in_w, int root_x, int root_y);
#   endif

    // add items to menu
    void  Append(int id, char *label, char *help=NULL, Bool checkable=FALSE);
    void  Append(int id, char *label, wxMenu *submenu, char *help=NULL);
    void  AppendSeparator(void);
    /* MATTHEW: */
    void  DeleteItem(int id, int pos);
    void  Delete(int id);
    void  DeleteByPosition(int pos);
    // modify items
    void  Break(void) {}; // not supported
    void  Check(int id, Bool flag);
    Bool  Checked(int id);
    void  Enable(int id, Bool flag);
    char  *GetHelpString(int id);
    char  *GetLabel(int id);
    char  *GetTitle(void);
    void  SetHelpString(int id, char *help);
    void  SetLabel(int id, char *label);
    void  SetTitle(char *title);
    // set font & colour
    void  SetFont(wxFont *newfont)            { if (newfont) font = newfont; }
    void  SetBackgroundColour(wxColour *col)  { if (col) bg = col; }
    void  SetForegroundColour(wxColour *col)  { if (col) fg = col; }
    // miscellaneous
    char  *GetClientData(void) { return client_data; }
    void  SetClientData(char *data) { client_data = data; }
    // search for item by label
    int   FindItem(char *label);
private:
    // allow callback and menubar access to private data
    friend class wxMenuBar;

    wxMenuItem  *FindItemForId(int id, wxMenu **menu=NULL); // search for internal data
#   ifdef Have_Xt_Types
    static void EventCallback(Widget, XtPointer, XtPointer);
#   endif

    // necessary for popup menus
    wxMenu_Widgets* X;
    wxFunction callback;
    // font & colours
    wxFont*   font;
    wxColour* fg;
    wxColour* bg;
    // internal representation
    wxMenuItem* title;		// title, if specified with constructor
    wxMenuItem* top;		// first menu item
    wxMenuItem* last;		// last menu item for wxMenu::Append
    wxMenuItem* topdummy;
    wxMenuItem **owner; /* MATTHEW: Pointer to pointer to top */
    // for special associated data
    char* client_data;
};

#endif // Menu_h
