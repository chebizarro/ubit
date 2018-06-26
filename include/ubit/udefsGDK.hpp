/*************************************************************************
 * 
 *  udefsGDK.hpp: GDK implementation
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

#include <ubit/ubit_features.h>
#if UBIT_WITH_GDK

#ifndef _udefsGDK_hpp_
#define	_udefsGDK_hpp_ 1

typedef GdkDisplay*    USysDisp;
typedef GdkWindow*     USysWin;
typedef GdkColormap*   USysColormap;
typedef GdkGC*         USysGC;
typedef GdkVisual*     USysVisual;

#  define DrawPoint(D,Win,G,X,Y)    gdk_draw_point(Win,G,X,Y)
#  define DrawLine(D,Win,G,X1,Y1,X2,Y2) gdk_draw_line(Win,G,X1,Y1,X2,Y2)
#  define DrawRect(D,Win,G,X,Y,W,H) gdk_draw_rectangle(Win,G,FALSE,X,Y,W,H)
#  define FillRect(D,Win,G,X,Y,W,H) gdk_draw_rectangle(Win,G,TRUE,X,Y,W,H)
#  define Flush(D)                  gdk_flush()

//NB:  false/*writeable*/, true/*best_match*/
#  define AllocColor(D,Map,Col)     gdk_colormap_alloc_color(Map,Col,false,true)
#  define GetAtom(D,Name)           gdk_atom_intern(Name,FALSE)

#  define CreatePixmap(D,Win,w,h,bpp) gdk_pixmap_new(Win,w,h,bpp)
#  define DestroyPixmap(D,Pix)      g_object_unref(Pix)
#  define GetImage(D,Win,X,Y,W,H)   gdk_drawable_get_image(Win,X,Y,W,H)
#  define DestroyImage(I)           gdk_image_destroy(I)
#  define DrawImage(D,Wn,G,I,Xs,Ys,Xd,Yd,W,H) gdk_draw_image(Wn,G,I,Xs,Ys,Xd,Yd,W,H)
#  define PutPixel(I,X,Y,P)         gdk_image_put_pixel(I,X,Y,P)
#  define GetPixel(I,X,Y)           gdk_image_get_pixel(I,X,Y)
#  define CopyArea(D,G,Wns,Xs,Ys,W,H,Wnd,Xd,Yd) gdk_draw_drawable(Wnd,G,Wns,Xs,Ys,Xd,Yd,W,H)
#  define SetCursor(D,W,C)          gdk_window_set_cursor(W,C)

// Graphics Context
#  define GCValues           GdkGCValues
#  define GCFUNCTION         GDK_GC_FUNCTION
#  define GCFOREGROUND       GDK_GC_FOREGROUND
#  define GCBACKGROUND       GDK_GC_BACKGROUND
#  define GCFONT             GDK_GC_FONT
#  define GCLINEWIDTH        GDK_GC_LINE_WIDTH
#  define GCEXPOSURES        GDK_GC_EXPOSURES
#  define SetCopyFunction(G) (G).function = GDK_COPY
#  define SetXorFunction(G)  (G).function = GDK_XOR
#  define SetForeground(G,P) (G).foreground.pixel = (P)
#  define SetBackground(G,P) (G).background.pixel = (P)
#  define SetFont(G,F)       (G).font = (F)
#  define SetGC(D,G,M,V)     gdk_gc_set_values(G,V,(GdkGCValuesMask)(M))
#  define CreateGC(D,W,M,V)  gdk_gc_new_with_values(W,V,(GdkGCValuesMask)(M))
#  define DestroyGC(D,G)     g_object_unref(G)
#  define SetClipOrigin(D,G,X,Y)  gdk_gc_set_clip_origin(G,X,Y)
#  define SetClipMask(D,G,M)      gdk_gc_set_clip_mask(G,M)
#  define SetClipRectangle(D,G,R) {gdk_gc_set_clip_origin(G,0,0); gdk_gc_set_clip_rectangle(G,R);}

#endif
#endif
