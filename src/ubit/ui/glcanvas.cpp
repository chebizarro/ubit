/*
 *  glcanvas.cpp: a widget for rendering OpenGL graphics (requires X11 and OpenGL)
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#if UBIT_WITH_GL

#include <iostream>
#include <algorithm>
#include <ubit/ugl.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uview.hpp>
#include <ubit/core/event.h>
#include <ubit/ustyle.hpp>
#include <ubit/utimer.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uon.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uglcontext.hpp>
#include <ubit/uglcanvas.hpp>
#include <ubit/udispX11.hpp>
using namespace std;
namespace ubit {



GLCanvas::GLCanvas(Args a) :
USubwin(a), is_init(false), share_glresources(false)
{
  addAttr(UOn::paint / ucall(this, &GLCanvas::paintImpl));
  // resizeImpl est ajouté dans USubwin
  //addAttr(UOn::resize / ucall(this, &GLCanvas::resizeImpl));
} 

GLCanvas::~GLCanvas() {}

GLContext* GLCanvas::getGlcontext() const {
  return hardImpl()->getGlcontext();
}

//void GLCanvas::shareContextResources(GLContext* gc) {shared_res_ctx = gc;}
//void GLCanvas::setAutoBufferSwap(bool state) {is_autoswap = state;}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void enableClips() {
  glEnable(GL_CLIP_PLANE0);
  glEnable(GL_CLIP_PLANE1);
  glEnable(GL_CLIP_PLANE2);
  glEnable(GL_CLIP_PLANE3);  
}

void disableClips() {
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool GLCanvas::realize() {
  if (! wmodes.IS_HARDWIN) {
    Application::error("GLCanvas::realize","can't be a SOFTWIN: can't realize object %p",this);
    return false;
  }
  if (! USubwin::realize()) return false;
  UHardwinImpl* hw = hardImpl();
  
  GLContext* sharelists = null;
  if (share_glresources) sharelists = hw->getDisp()->default_context->toGlcontext();

#ifndef UBIT_WITH_GLUT
  // when GLUT is used, each hardwin has its own glcontext, it must be created otherwise
  hw->glcontext = new GLContext(hw->getDisp(), sharelists);
#endif
  
  if (! hw->glcontext) {
    error("GLCanvas::realize","the Graphic Context could not be created for *p",this);
    return false;
  }
  else {
    hw->glcontext->setDest(hw,0,0);  // NB: trop tot pour calcul correct des tailles
    return true;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GLCanvas::initImpl() {
  is_init = true;
  UHardwinImpl* hw = hardImpl();
  hw->glcontext->makeCurrent();
  disableClips();
  initGL();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GLCanvas::resizeImpl(UResizeEvent& e) 
{
  USubwin::resizeImpl(e); // ne pas oublier !!
  
  if (!is_init && (getView()->getWidth() > 1 && getView()->getHeight() > 1)) {
    initImpl();  // creates the GLContext
  }

  if (is_init) {
    UHardwinImpl* hw = hardImpl();
    //cerr << "@GLCanvas: begin resizeGL gc " <<hw->glcontext<<endl;
    hw->glcontext->setDest(hw,0,0);
    hw->glcontext->makeCurrent();
    disableClips();
    resizeGL(e, int(getView()->getWidth()), int(getView()->getHeight()));
    //cerr << "@GLCanvas: end resizeGL gc "<< hw->glcontext<<endl;
   }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GLCanvas::paintImpl(PaintEvent& e) 
{
  if (is_init) {
    UHardwinImpl* hw = hardImpl();
    //cerr << ">>> GLCanvas paintGL: HW: " << hw << " / winGC "<< hw->glcontext<<endl;
    hw->glcontext->setDest(hw,0,0);  // set hardwin and adapts drawing to its size
    hw->glcontext->makeCurrent();
    disableClips();

    paintGL(e);
    //if (is_autoswap) swapBuffers(); // fait dans ~Graph
    
    // glFlush() sinon ce qui a ete tracé avec le precedent glcontext n'apparait pas
    glFlush();
    
    // pour afficher correctement les enfants
    GLContext* default_gc = hw->getDisp()->getDefaultContext()->toGlcontext();
    if (default_gc) default_gc->makeCurrent();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GLCanvas::makeCurrent() {
  UHardwinImpl* hw = hardImpl();
  if (hw->glcontext) hw->glcontext->makeCurrent();
  disableClips();
}

void GLCanvas::swapBuffers() {
  UHardwinImpl* hw = hardImpl();
  if (hw->glcontext) hw->glcontext->swapBuffers();
}

}
#endif
