/*************************************************************************
 * 
 *  udefsX11.hpp: X11 implementation
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
#if UBIT_WITH_X11

#ifndef _udefsX11_hpp_
#define	_udefsX11_hpp_ 1

typedef Display*       USysDisp;
typedef Window         USysWin;
typedef Colormap       USysColormap;
typedef GC             USysGC;
typedef XVisualInfo*   USysVisual;

#  define DrawPoint(D,Win,G,X,Y)    XDrawPoint(D,Win,G,int(X),int(Y))
#  define DrawLine(D,Win,G,X1,Y1,X2,Y2) XDrawLine(D,Win,G,int(X1),int(Y1),int(X2),int(Y2))
#  define DrawRect(D,Win,G,X,Y,W,H) XDrawRectangle(D,Win,G,int(X),int(Y),int(W),int(H))
#  define FillRect(D,Win,G,X,Y,W,H) XFillRectangle(D,Win,G,int(X),int(Y),int(W),int(H))
#  define Flush(D)                  XFlush(D)

// dans le cas Xlib il faudrait chercher une couleur approchee dans la cmap
#  define AllocColor(D,Map,Col)     XAllocColor(D,Map,Col)
#  define GetAtom(D,Name)           XInternAtom(D,Name,False)

#  define CreatePixmap(D,Win,w,h,bpp) XCreatePixmap(D,Win,w,h,bpp)
#  define DestroyPixmap(D,Pix)      XFreePixmap(D,Pix)
#  define GetImage(D,Win,X,Y,W,H)   XGetImage(D,Win,X,Y,W,H,AllPlanes,ZPixmap)
#  define DestroyImage(I)           XDestroyImage(I)
#  define DrawImage(D,Wn,G,I,Xs,Ys,Xd,Yd,W,H) XPutImage(D,Wn,G,I,int(Xs),int(Ys),int(Xd),int(Yd),int(W),int(H))
#  define PutPixel(I,X,Y,P)         XPutPixel(I,X,Y,P)
#  define GetPixel(I,X,Y)           XGetPixel(I,X,Y)
#  define CopyArea(D,G,Ws,Xs,Ys,W,H,Wd,Xd,Yd) XCopyArea(D,Ws,Wd,G,Xs,Ys,W,H,Xd,Yd)
#  define SetCursor(D,W,C)          XDefineCursor(D,W,C)

// Graphics Context
#  define GCValues           XGCValues
#  define GCFUNCTION         GCFunction
#  define GCFOREGROUND       GCForeground
#  define GCBACKGROUND       GCBackground
#  define GCFONT             GCFont
#  define GCLINEWIDTH        GCLineWidth
#  define GCEXPOSURES        GCGraphicsExposures
#  define SetCopyFunction(G) (G).function = GXcopy
#  define SetXorFunction(G)  (G).function = GXxor
#  define SetForeground(G,P) (G).foreground = (P)
#  define SetBackground(G,P) (G).background = (P)
#  define SetFont(G,F)       (G).font = (F)->fid
#  define SetGC(D,G,M,V)     XChangeGC(D,G,M,V)
#  define CreateGC(D,W,M,V)  XCreateGC(D,W,M,V)
#  define DestroyGC(D,G)     XFreeGC(D,G)
#  define SetClipOrigin(D,G,X,Y)  XSetClipOrigin(D,G,X,Y)
#  define SetClipMask(D,G,M)      XSetClipMask(D,G,M)
#  define SetClipRectangle(D,G,R) XSetClipRectangles(D,G,0,0,R,1,Unsorted)

#endif
#endif
