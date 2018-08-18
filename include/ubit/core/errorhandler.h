/*
 *  errorhandler.h
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

#ifndef UBIT_CORE_ERRORHANDLER_H_
#define	UBIT_CORE_ERRORHANDLER_H_


namespace ubit {
  
  /** Error management.
   */
  class ErrorHandler : public Object {
  public:    
    ErrorHandler(const String& label, std::ostream* out_stream = &std::cerr);
    /**< creates a new error handler.
     * - 'label' identifies the handler (it is the name of the application for the
     *    default Application error handler).
     * - 'out_stream' specifies on which output stream errors are written. std::cerr is
     *    used by default. No errors are written if this argument is null.
     */
    
    virtual ~ErrorHandler();
    
    virtual void setOutputStream(std::ostream*);
    ///< specifies on which output stream errors are written (errors not written on a stream if argument is null).
    
    virtual void setOutputBuffer(String*);
    ///< specifies a string buffer on which errors are written (errors not written on a buffer if argument is null).
    
    String& label() {return *plabel;}
    ///< returns the label property of this ErrorHandler.
    
    virtual void warning(const char* funcname, const char* format, ...) const;
    ///< raises a warning; shortcut for raiseError(UError::WARNING, null, ...).
    
    virtual void error(const char* funcname, const char* format, ...) const;
    ///< raises an error; shortcut for raiseError(UError::ERROR, null, ...).
    
    virtual void error(int errnum, const Object*, const char* funcname, 
                       const char* format, ...) const;
    ///< raises an error; shortcut for raiseError(UError::ERROR, ...).
    
    virtual void parserError(int errnum, const Char* text_buffer,
                                  const char* msg_start, const String& name,
                                  const char* msg_end, const Char* line) const;
    ///< used by XML and Style parsers to raise a formatted error.
    
    virtual void raiseError(int errnum, const Object*, const char* funcname, 
                            const char* format, va_list) const;
    /**< raises an error: prints out a message and/or generate an exception.
     * this method:
     * - displays an error message (on std::cerr by default, see below)
     * - throws a UError exception if 'errnum' is < 0.
     *   Predefined errnums are UError::WARNING, ERROR, FATAL_ERROR, INTERNAL_ERROR 
     *   but other values may be used if needed.
     * - 'funcname' should be the name of the function where the error occured
     * - 'format'is a printf-like format
     * - '...' is a variadic list of arguments that correspond to 'format'.
     */
            
    virtual void raiseError(int errnum, String* msg) const;

    virtual const char* getErrorName(int errnum) const;
    virtual void formatMessage(UError&, const char* format, va_list) const;
    virtual void printOnStream(const UError&) const;
    virtual void printOnBuffer(const UError&) const;
    
  protected:
    unique_ptr<String> plabel;
    unique_ptr<String> pbuffer;
    std::ostream* fout;
  };
  
}
#endif // UBIT_CORE_ERRORHANDLER_H_

