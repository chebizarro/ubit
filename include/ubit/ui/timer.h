/*
 *  timer.hpp
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


#ifndef _utimer_hpp_
#define	_utimer_hpp_ 1

extern "C" {
  struct timeval;
}

#include <ubit/uobject.hpp>

namespace ubit {
  
  /** a Timer object fires callbacks after a given delay. Callbacks can be
   * fired repeatedly (each time after the delay) or only one time.
   * Timers can also be used to postpone operations that take a long amount
   * of time to execute by using a delay of 0 msec.
   *
   * Exemple:
   * <pre>
   *    unique_ptr<Timer> t = new Timer();
   *    t->onAction( ucall(x, y, printArgs) );
   *    t->onAction( ucall(obj, val, &Demo::doIt) );
   *    t->start(1000);
   * </pre>
   * - starts the timer with a delay of 1000 msec in repeat mode. The 'onAction'
   *   callbacks are fired when the delay has passed. In this example, they call
   *   printArgs(x, y) and obj->doIt(val)
   * - by default these callbacks are fired until stop() is called. start() can 
   *   have an optional arg that specifies how many times they must be called.
   */
  class Timer : public Node {
  public:
    UCLASS(Timer)
    
    Timer(bool auto_delete = true);
    /**< creates a new timer.
     * @see Timer(unsigned long delay, int ntimes, bool auto_delete);
     * 'delay' and 'ntimes' are set to 0 by default.
     */
    
    Timer(unsigned long delay, int ntimes, bool auto_delete = true);
    /**< creates a new timer.
     * - 'delay' is in milliseconds. if 'delay' = 0, the timer is immediately fired
     *    when the main loop becomes idle.
     * - 'ntimes' specifies how many times the callbacks are fired (each time after
     *   the specified 'delay'). If ntimes = -1 the callbacks are fired repeatedly
     *   until stop() is called or the timer is destroyed.
     * - if 'auto_delete' is true, the timer is automatically deleted on completion
     *   if was created in the heap and is not pointed by a unique_ptr<> (@see UPtr).
     *   This feature avoids memory leaks and is especially useful if the timer
     *   is only used once
     */
    
    virtual ~Timer();
    
    virtual void onAction(UCall& callback);
    /**< adds a callback that is fired when the time is up.
     * 'callback' is a ucall<> expression (see UCall). It is destroyed when the timer
     *  is destroyed except if it is referenced by a unique_ptr<> or another node (see Node).
     */  
    
    virtual void onTimeout(UCall& callback) {onAction(callback);}
    ///< synonym for onAction().
    
    virtual void start();
    /**< (re)starts the timer.
     */
    
    virtual void start(unsigned long delay, int ntimes);
    /**< starts the timer.
     * @see Timer(unsigned long delay, int ntimes);
     */
    
    virtual void stop();
    /**< stops the timer.
     * the Timer is destroyed if it was started in auto_delete mode (see start())
     * and if it is not referenced elsewhere (see Node).
     */
    
    unsigned long getDelay() const {return delay;}
    
    void setDelay(unsigned long time);
    
    bool isRunning() const {return is_running;}
    ///< returns true if the timer is currently running.
    
    // - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    
    int getTimerNo() const {return timer_no;}
    bool timerCB();
    
  private:
    friend class Application;
    friend class UAppliImpl;
    friend class UTimerImpl;
    bool auto_delete, is_running, is_looping;
    int  timer_no, ntimes; 
    unsigned long delay;
    
    void removeTimer();
    
#if UBIT_WITH_X11
    struct timeval& timeout;
#elif UBIT_WITH_GDK
    unsigned int gid;  // timer ID when GDK is used
#endif
#endif
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UTimerImpl {
  public:
    static unsigned long getTime();
    static void getTime(timeval& time);
    static void minTime(struct timeval& mintime, struct timeval& time);
    static void addTime(struct timeval& time, unsigned long millisec_delay);
    static bool lessTime(struct timeval& time, struct timeval& t2);
    
    bool resetTimers(struct timeval& delay);
    void fireTimers();
    
    typedef std::vector<Timer*> Timers;
    Timers timers;
  };
  
}
#endif

