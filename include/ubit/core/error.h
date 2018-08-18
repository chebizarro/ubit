/*
 *  error.h: Error Handling
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

#ifndef UBIT_CORE_ERROR_H
#define	UBIT_CORE_ERROR_H

#include <exception>

namespace ubit {

  /**
   * Ubit exception.
   */
  class Error : public std::exception {
  public:
    enum {
      FATAL_ERROR = -1, WARNING, ERROR, INTERNAL_ERROR, STYLE_ERROR, CSS_ERROR, XML_ERROR
    };
    
    Error(int errnum, const UObject*, const char* funcname);
    virtual ~Error() throw ();
    
    virtual const char* what() const throw();

    static const int message_capacity = 2000;
    int errnum;
    const UObject* object;
    const char* function_name;
    char message[message_capacity];
  };
  
  /**
   * prints out an error message.
   * this method displays an error message (on std::cerr by default) :
   * - 'function_name' = the name of the function where the error occured
   * - 'format' = a printf-like format
   * - '...'    = a variadic list of arguments that correspond to 'format'.
   * @see also: uwarning() and UAppli::error().
   */
  void uerror(const char* function_name, const char* format, ...);
  
  /// prints out a warning message: @see uerror().
  void uwarning(const char* function_name, const char* format, ...);

}

#endif // UBIT_CORE_ERROR_H
