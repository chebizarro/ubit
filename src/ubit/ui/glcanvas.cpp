/*
 *  glcanvas.cpp: a widget for rendering OpenGL graphics (requires X11 and OpenGL)
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */
#include <ubit/ubit_features.h>
#if UBIT_WITH_GL

#include <iostream>
#include <algorithm>
#include <ubit/ugl.hpp>
#include <ubit/core/call.h>
#include <ubit/ui/window.h>
#include <ubit/ui/view.h>
#include <ubit/core/event.h>
#include <ubit/draw/style.h>
#include <ubit/ui/timer.h>
#include <ubit/core/application.h>
#include <ubit/core/on.h>
#include <ubit/draw/graph.h>
#include <ubit/uglcontext.hpp>
#include <ubit/ui/glcanvas.h>
#include <ubit/udispX11.hpp>
using namespace std;
namespace ubit {



GLCanvas::GLCanvas(Args a) :
SubWindow(a), is_init(false), share_glresources(false)
{
  addAttr(UOn::paint / ucall(this, &GLCanvas::paintImpl));
  // resizeImpl est ajouté dans SubWindow
  //addAttr(UOn::resize / ucall(this, &GLCanvas::resizeImpl));
} 

GLCanvas::~GLCanvas() {}

GLContext* GLCanvas::getGlcontext() const {
  return hardImpl()->getGlcontext();
}

//void GLCanvas::shareContextResources(GLContext* gc) {shared_res_ctx = gc;}
//void GLCanvas::setAutoBufferSwap(bool state) {is_autoswap = state;}


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


bool GLCanvas::realize() {
  if (! wmodes.IS_HARDWIN) {
    Application::error("GLCanvas::realize","can't be a SOFTWIN: can't realize object %p",this);
    return false;
  }
  if (! SubWindow::realize()) return false;
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


void GLCanvas::initImpl() {
  is_init = true;
  UHardwinImpl* hw = hardImpl();
  hw->glcontext->makeCurrent();
  disableClips();
  initGL();
}


void GLCanvas::resizeImpl(UResizeEvent& e) 
{
  SubWindow::resizeImpl(e); // ne pas oublier !!
  
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
