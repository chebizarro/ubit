/*
 *  timer.cpp
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
#include <iostream>
#include <unistd.h>       // darwin
#include <limits.h>       // fedora
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ubit/core/application.h>
#include <ubit/core/appimpl.h>
#include <ubit/core/on.h>
#include <ubit/ui/timer.h>
#include <ubit/ugl.hpp>
#include <ubit/core/call.h>
using namespace std;
namespace ubit {

// NOTE GLUT: There is no support for canceling a registered callback. 
// Instead, ignore a callback based on its value parameter when it is triggered.
  
namespace impl {

#if UBIT_WITH_GLUT          // !!! bug si unique_ptr<Timer> et ptr mis a null

static void _timerCB(int timer_no) {
  UTimerImpl::Timers& timers = Application::impl.timer_impl.timers;
  
  if (timer_no < 0 || timer_no >= (int)timers.size()) return;
  Timer* t = timers[timer_no];

  // cas ou le timer devrait avoir ete enleve mais GLUT ne permet pas de le faire
  if (timer_no != t->getTimerNo()) {
    timers[timer_no] = null;  // can now be removed from the list
    return;
  }
    
  bool valid = t->timerCB();
  Application::impl.processPendingRequests();

  if (valid  && t->isRunning())  // il faut alors rappeler timerCB
    glutTimerFunc(t->getDelay(), _timerCB, timer_no);
}

#elif UBIT_WITH_GDK

static gboolean _timerCB(gpointer timer) {
  Timer* t = (Timer*)timer;
  bool valid = t->timerCB();
  Application::impl.processPendingRequests();
  if (valid  && t->isRunning())  // il faut alors rappeler timerCB
    return true;   // indique a GDK de rappeler timerCB
  else 
    return false;  // indique a GDK de ne pas rappeler timerCB
}

#endif
} //namespace utimer

bool Timer::timerCB() {
  if (isDestructed()) return false;
  
  if (is_running) {
    TimerEvent e(UOn::action, this, Application::getTime());
    fire(e);
  }
  
  if (!is_looping) {
    ntimes--;
    if (ntimes <= 0) {
      is_running = false;
      removeTimer();
      if (auto_delete && isDeletable() && getPtrCount() == 0) {
        delete this;
        return false;
      }
    }
  }
  return true;
}

void Timer::removeTimer() {
  UTimerImpl::Timers& timers = Application::impl.timer_impl.timers;
  if (timer_no >= 0 && timer_no < (int)timers.size()) timers[timer_no] = null;
  timer_no = -1;
}


//void Timer::start(unsigned long d, int ntimes, bool _auto_delete) {
void Timer::start(unsigned long d, int _ntimes) {
  //auto_delete = _auto_delete;
  ntimes = _ntimes;
  is_running = true;
  is_looping = (ntimes <= 0);
  setDelay(d);
}
  
void Timer::start() {
  is_running = true;
  setDelay(delay);
}

void Timer::setDelay(unsigned long d) {
  delay = d;
  if (!is_running) return;
  
  int num = -1;
  UTimerImpl::Timers& timers = Application::impl.timer_impl.timers;
  if (timer_no >= 0 && timer_no < (int)timers.size()) {
#ifndef UBIT_WITH_GLUT
    // dans le cas GLUT on ne peut pas recuperer cet element tu tableau car il
    // est impossible d'enlever l'appel correspondant de glutTimerFunc. Il faut
    // donc que cette invocation s'excute, sans rien faire, en detectant un 
    // timer_no qui sera different pour le callback et pour l'objet considere
    num = timer_no;
#endif
  }
  else {
    for (unsigned k = 0; k < timers.size(); ++k) {
      if (timers[k] == null) num = k;
    }
  }

  if (num >= 0) {
    timer_no = num;
    timers[timer_no] = this;
  }
  else {
    timer_no = timers.size();
    timers.push_back(this);
  }
  
#if UBIT_WITH_X11
  UTimerImpl::getTime(timeout);
  UTimerImpl::addTime(timeout, delay);
#elif UBIT_WITH_GDK
  if (i != timers->cend()) g_source_remove(gid);
  gid = g_timeout_add(delay, impl::_timerCB, (gpointer)this); 
#elif UBIT_WITH_GLUT
  // NB remove du precedent timer impossible, cf. plus haut
  glutTimerFunc(delay, impl::_timerCB, timer_no);  
#endif
}

void Timer::stop() {
  is_running = false;
#if UBIT_WITH_X11
  removeTimer();
 #elif UBIT_WITH_GDK
  g_source_remove(gid);
  //removeTimer(); inutile, cette liste sert a rien dans ce cas
#elif UBIT_WITH_GLUT
  //removeTimer(); faux car impossible d'annuler 
#endif 
}


Timer::Timer(bool _auto_delete) :
auto_delete(_auto_delete), is_running(false), is_looping(true),
timer_no(-1), ntimes(0), delay(0)
#if UBIT_WITH_X11
, timeout(*new timeval)
#elif UBIT_WITH_GDK
, gid(0)
#endif
{}

Timer::Timer(unsigned long d, int _ntimes, bool _auto_delete) : 
auto_delete(_auto_delete), is_running(false), is_looping(_ntimes <= 0),
timer_no(-1), ntimes(_ntimes), delay(d)
#if UBIT_WITH_X11
, timeout(*new timeval)
#elif UBIT_WITH_GDK
, gid(0)
#endif
{}

Timer::~Timer() {
#if UBIT_WITH_X11
  delete &timeout;
#elif UBIT_WITH_GDK
  g_source_remove(gid);
#endif
}

void Timer::onAction(UCall& c) {
  Child* l = new Child(&c);
  l->setCond(UOn::action);
  _addAttr(*l);
}

// ceci corrige un bug bien connu...

static inline void FIX_TIME(timeval& time) {
  while (time.tv_usec >= 1000000) {
    time.tv_usec -= 1000000;                  
    time.tv_sec++;                            
  }
  while (time.tv_usec < 0) {
    if (time.tv_sec > 0) {
      time.tv_usec += 1000000;
      time.tv_sec--;
    }
    else {         
      time.tv_usec = 0;
      break;
    }      
  }
}

unsigned long UTimerImpl::getTime() {
  timeval time;
  gettimeofday(&time, null);
  FIX_TIME(time);
  return time.tv_sec * 1000 + time.tv_usec / 1000;
}

void UTimerImpl::getTime(timeval& time) {
  gettimeofday(&time, null);
  FIX_TIME(time);
}

void UTimerImpl::minTime(struct timeval& mintime, struct timeval& time) {
  if (mintime.tv_sec > time.tv_sec) {
    mintime.tv_sec  = time.tv_sec;
    mintime.tv_usec = time.tv_usec;
  }
  else if (mintime.tv_sec == time.tv_sec && mintime.tv_usec > time.tv_usec)
    mintime.tv_usec = time.tv_usec;
}

void UTimerImpl::addTime(struct timeval& time, unsigned long millisec_delay) {
  time.tv_sec  += millisec_delay / 1000;
  time.tv_usec += (millisec_delay % 1000) * 1000;
  FIX_TIME(time);
}

bool UTimerImpl::lessTime(struct timeval& t1, struct timeval& t2) {
  if (t1.tv_sec < t2.tv_sec)
    return true;
  else if (t1.tv_sec == t2.tv_sec && t1.tv_usec <= t2.tv_usec)
    return true;
  return false;  // sinon
}

#if UBIT_WITH_X11

bool UTimerImpl::resetTimers(struct timeval& delay) {
  if (timers.size() == 0) return false;
  
  struct timeval time, mintime;
  getTime(time);
  mintime.tv_sec  = LONG_MAX;
  mintime.tv_usec = LONG_MAX;
  bool timeout_found = false;
  
  for (unsigned int k = 0; k < timers.size(); ++k) {
    Timer* t = timers[k];
    if (t && t->is_running) {
      timeout_found = true;
      minTime(mintime, t->timeout);
    }
  }
  
  if (!timeout_found) return false;  // is no active timeout was found
  
  if (lessTime(mintime, time)) { // is mintime <= time ?
    delay.tv_sec  = 0;
    delay.tv_usec = 0;
  }
  else {
    delay.tv_sec = mintime.tv_sec  - time.tv_sec;
    if (mintime.tv_usec >= time.tv_usec)
      delay.tv_usec = mintime.tv_usec - time.tv_usec;
    else {
      delay.tv_sec--;
      delay.tv_usec = 1000000 - time.tv_usec + mintime.tv_usec;
    }
  }  
  // NB: delay can be (0,0)
  return (delay.tv_sec != LONG_MAX || delay.tv_usec != LONG_MAX);
}


void UTimerImpl::fireTimers() {
  if (timers.size() == 0) return;

  struct timeval curtime;
  getTime(curtime);
  
  for (unsigned int k = 0; k < timers.size(); ++k) {
    Timer* t = timers[k];
    if (t && !t->isDestructed() && t->is_running && lessTime(t->timeout, curtime)) {
      bool valid = t->timerCB();
      if (valid && t->is_running) {   // mettre a jour le prochain timeout
        t->timeout = curtime; // ne cherche pas a rattraper le temps perdu
        addTime(t->timeout, t->delay);
      }
      //else t->stop();  // ??? va modifier la liste et peut detruire le timer
    }
  }
}

#endif // UBIT_WITH_X11
  
}
