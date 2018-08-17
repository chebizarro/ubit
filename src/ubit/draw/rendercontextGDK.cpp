/*
*
*  URenderContextGDK.cpp: native GDK graphics
*  Ubit GUI Toolkit - Version 6.0
*  (C) 2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h> 
#if UBIT_WITH_GDK

#include <iostream>
#include <cmath>
#include <ubit/udefs.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/uima.hpp>
#include <ubit/upix.hpp>
#include <ubit/nat/udispGDK.hpp>
#include <ubit/nat/uhardima.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/nat/ugraphGDK.hpp>
#include <ubit/nat/uhardfont.hpp>

namespace ubit {


URenderContextGDK::URenderContextGDK(Display* d, RenderContext* notused) : RenderContext(d)
{
  sys_disp = ((UDispGDK*)d)->getSysDisp();
  sys_gc = ....;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::flush() const {
  Flush(SYS_DISP);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::resetGraph() {
  GdkGCValues gcval;
  gcval.function = GDK_COPY;
  gcval.line_width = 0;
  gdk_gc_set_values(gc, &gcval,
                    (GdkGCValuesMask)(GDK_GC_FUNCTION|GDK_GC_LINE_WIDTH));}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::resetClip() {
  GdkRectangle c = {(int)clip.x, (int)clip.y, (unsigned int)clip.width, (unsigned int)clip.height};
  gdk_gc_set_clip_origin(sys_gc, 0, 0);
  gdk_gc_set_clip_rectangle(sys_gc, &c);
}

void URenderContextGDK::setClip(double x, double y, double width, double height) {
  clip.setRect(x, y, width, height);
  GdkRectangle c = {(int)x, (int)y, (unsigned int)width, (unsigned int)height};
  gdk_gc_set_clip_origin(sys_gc, 0, 0);
  gdk_gc_set_clip_rectangle(sys_gc, &c);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
void URenderContextGDK::setFont(const FontDescription& fd) {
  GdkGCValues gcval;
  gcval.font = graph->font->sysf;
  gdk_gc_set_values(sys_gc, &gcval, GDK_GC_FONT);  //(GdkGCValuesMask)GDK_GC_FONT
} 
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  void URenderContextGDK::setWidth(double w) {  
    GCValues gcval;
    gcval.line_width = int(w);
    SetGC(SYS_DISP, SYS_GC, GCLINEWIDTH, &gcval);
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  void URenderContextGDK::setColor(const Color& c) {
    graph->color = c.getPixel(disp);
    
    GCValues gcval;        //NB: alpha pas pris en compte sous X11
    if (in_xor_mode) SetForeground(gcval, color ^ bgcolor);
    else SetForeground(gcval, color);
    SetGC(SYS_DISP, SYS_GC, GCFOREGROUND, &gcval);
  }
  
void URenderContextGDK::setBackground(const Color& c) {
  graph->bgcolor = c.getPixel(disp);
  GCValues gcval;
  SetBackground(gcval, bgcolor);
  SetGC(SYS_DISP, SYS_GC, GCBACKGROUND, &gcval);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
  
void URenderContextGDK::drawLine(double x1, double y1, double x2, double y2) const {
  DrawLine(sys_disp, dest->sys_win, sys_gc, xwin + x1, ywin + y1, xwin + x2, ywin + y2);  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::drawRect(double x, double y, double w, double h, bool filled) const {
  if (filled)
    FillRect(sys_disp, dest->sys_win, sys_gc, xwin + x, ywin + y, w, h);
  else
    DrawRect(sys_disp, dest->sys_win, sys_gc, xwin + x, ywin + y, w, h);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::drawArc(double x, double y, double w, double h, 
                            double start, double ext, bool filled) const 
{
  gdk_draw_arc(dest->sys_win, sys_gc, (filled ? TRUE : FALSE),
               xwin + x, ywin + y, w, h,
               int(start * 64.), int(ext * 64.));   //att: au *64 !
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::drawString(const UHardFont*, const char* str, int str_len, 
                               double x, double y) const 
{
  gdk_draw_text(dest->sys_win, nf->sysf, sys_gc,
                int(xwin + x), int(ywin + y), str, str_len);  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).

void URenderContextGDK::drawPolygon(const float* coords2d, int card, int polytype) const {
  if (card <= 0 || coords2d == null) return;
  int card2 = card * 2;
  
  USysPoint pstack[POLYCARD+1];
  USysPoint* pmem = null;
  USysPoint* syspts = card < POLYCARD ? pstack : (pmem = new USysPoint[card+1]);
  for (int k = 0, kk = 0; k < card2; k++, kk += 2) {
    syspts[k].x = int(coords2d[kk] + xwin);
    syspts[k].y = int(coords2d[kk+1] + ywin);
  }
  
  if (polytype == PolyLine)
    gdk_draw_lines(dest->sys_win, sys_gc, syspts, card);
  else if (polytype == PolyGon) 
    gdk_draw_polygon(dest->sys_win, sys_gc, FALSE, syspts, card);
  else
    gdk_draw_polygon(dest->sys_win, sys_gc, TRUE, syspts, card);
  
  delete[] pmem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::drawPolygon(const std::vector<Point>& points, int polytype) const {
  int card = points.size();
  if (card <= 0) return;
  
  USysPoint pstack[POLYCARD+1];
  USysPoint* pmem = null;
  USysPoint* syspts = card < POLYCARD ? pstack : (pmem = new USysPoint[card+1]);
  for (int k = 0; k < card; ++k) {
    syspts[k].x = int(points[k].x + xwin);
    syspts[k].y = int(points[k].y + ywin);
  }
  
  if (polytype == PolyLine)
    gdk_draw_lines(dest->sys_win, sys_gc, syspts, card);
  else if (polytype == PolyGon) 
    gdk_draw_polygon(dest->sys_win, sys_gc, FALSE, syspts, card);
  else
    gdk_draw_polygon(dest->sys_win, sys_gc, TRUE, syspts, card);
  
  delete[] pmem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URenderContextGDK::copyArea(double x, double y, double w, double h, double delta_x, double delta_y,
                            bool generate_refresh_events_when_obscured) const
{
  GCValues gcval;
  if (generate_refresh_events_when_obscured) {
    gcval.graphics_exposures = true;
    SetGC(SYS_DISP, SYS_GC, GCEXPOSURES, &gcval);
  }
  
  CopyArea(SYS_DISP, SYS_GC,
           // from:
           ((HARDWIN*)gelem->dest)->sys_win,
           int(gelem->xwin + x), int(gelem->ywin + y), int(w), int(h),
           // to:
           ((HARDWIN*)gelem->dest)->sys_win,
           int(gelem->xwin + x + delta_x), int(gelem->ywin + y + delta_y));
  
  if (generate_refresh_events_when_obscured) {
    gcval.graphics_exposures = false;
    SetGC(SYS_DISP, SYS_GC, GCEXPOSURES, &gcval);
  }
  
  Flush(SYS_DISP);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
#endif

