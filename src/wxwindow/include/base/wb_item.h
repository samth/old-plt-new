/*
 * File:	wb_item.h
 * Purpose:	Declares panel items base class
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

#ifndef wxb_itemh
#define wxb_itemh

#include "common.h"
#include "wx_panel.h"
#include "wx_dcpan.h"
#include "wx_stdev.h"

class wxFont;
class wxPanel;

// General item class
class wxbItem: public wxWindow
{
 public:
    wxbItem(wxPanel *pnl);
   ~wxbItem(void);

   wxFont *buttonFont;
   wxFont *labelFont;
   wxColour *backColour;
   wxColour *labelColour;
   wxColour *buttonColour;

   int labelPosition;

   virtual void GetSize(int *width, int *height) = 0;
   virtual void GetPosition(int *x, int *y) = 0;
   virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) = 0;
   // Avoid compiler warning
   void SetSize(int w, int h) { wxWindow::SetSize(w, h); }
   virtual void SetClientSize(int width, int height);
   virtual void SetFocus(void) = 0;
   virtual void SetLabel(char *label) = 0;
   virtual char *GetLabel(void) = 0;

   virtual void Command(wxCommandEvent *event);        // Simulates an event
   virtual void ProcessCommand(wxCommandEvent *event); // Calls the callback and 
                                                                 // appropriate event handlers
  // Places item in centre of panel - so can't be used BEFORE panel->Fit()
  void Centre(int direction = wxHORIZONTAL);

  inline virtual wxFont  *GetLabelFont(void)        { return labelFont ; }
  inline virtual wxFont  *GetButtonFont(void)       { return buttonFont ; }
  inline virtual wxColour*GetBackgroundColour(void) { return backColour ; }
  inline virtual wxColour*GetLabelColour(void)      { return labelColour ; }
  inline virtual wxColour*GetButtonColour(void)     { return buttonColour ; }
};

#endif // wxb_itemh
