/*
*
*  usource.h: file and socket data management
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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

#ifndef UBIT_NET_SOURCE_H_
#define	UBIT_NET_SOURCE_H_

#include <ubit/core/object.h>

namespace ubit {

/** a Source object fires callbacks when a file or a socket gets data.
* Example:
* <pre>
*    int source = ...;
*    unique_ptr<Source> i = new Source(source);
*    i->onAction( ucall(x, y, printArgs) );
*    i->onAction( ucall(obj, val, &Demo::doIt) );
* </pre>
* - callbacks printArgs(x, y) and obj->doIt(val) are fired when 
*   the source receives data
*
* See also: Socket (Ubit simple sockets).
*/
class Source : public Node {    // !!!@@@ devrait deriver de UObject !!!
public:
  UCLASS(Source)
    
  Source(int source =-1);
  ///< creates a new Source listener and start listening to this source if >= 0.
  
  virtual ~Source();
  
  virtual void onAction(UCall& callback);
  /**< adds a callback that is fired when data is received.
   * the argument is a ucall<> expression (see UCall) that is destroyed when the
   * Source is destroyed (except if "referenced elsewhere"; see Node).
   */
  
  virtual void onInput(UCall& callback) {onAction(callback);}
  ///< synonym for onAction().

  virtual void onClose(UCall& callback);
  ///< adds a callback that is fired when the source is closed.

  virtual void open(int source);
  ///< starts listening to the source (a file, a socket, a pipe ID).

  virtual void close();
  ///< stops listening to the source.
     
  int getSource() const {return source;}
  ///< returns the OS ID of the the source.

  bool isOpened() const {return is_opened;}
  ///< returns true if the Source is opened.
        
  // - - - impl - - -    
  
  virtual void fireInput();
  virtual void fireClose();

private:
  friend class AppImpl;
  bool is_opened;
  int source;
  int gid;  // input ID when GDK is used
};

}
#endif // UBIT_NET_SOURCE_H_
