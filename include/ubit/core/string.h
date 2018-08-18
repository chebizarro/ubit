/*
 *  str.h: active strings (can have callbacks)
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

#ifndef UBIT_CORE_STRING_H_
#define	UBIT_CORE_STRING_H_

#include <ubit/core/data.h>

namespace ubit {
  
  /**
   * Ubit String.
   *
   * Strings can have callback functions that are fired when their value is
   * changed (see an example below). String parents can also be notified when
   * string are modified by using the UOn::strChange condition:
   *
   * Example:
   * <pre> 
   *    class Demo {
   *    public:
   *       void strCB(String* s);   // this method can have an optional Event& parameter
   *       void textCB(Event&);  // the Event& parameter is optional
   *       ....
   *    };
   *
   *    Demo* d = new Demo();
   *    String* s = new String("Hello");
   *
   *    // d->strCB(s) will be called when the content of 's' is changed
   *    s->onChange( ucall(d, s, &Demo::strCB) );
   *
   *    // 's' is displayed in (and edited by) the textfield
   *    // (as expected, the strCB() callback will be called when 's' is modified)
   *    Box* tf = new TextField(*s);
   *    ....
   * </pre>
   *
   * Alternatively:
   * <pre>
   *    // utextfield(...) is a shortcut for *new TextField(...)
   *    Box& tf = utextfield(s + UOn::strChange / ucall(d, &Demo::textCB));
   * </pre>
   *
   * d->textCB(Event&) will be called when the content of 's' is changed. The Event&
   * parameter is optional. It makes it possible to retrieve the string as follows:
   * <pre>
   *    void Demo::textCB(Event& e) {
   *      String* s = 0;
   *      if (e.getTarget()) s = e.getTarget()->toStr();
   *      ...etc...
   *      // NB: one could also write:
   *      // if (e.getTarget()) s = dynamic_cast<String*>(e.getTarget());
   *   }
   * </pre>
   *
   * Note strCB() could also have a Event& parameter that would make it possible
   * to retrieve the string in the same way.
   *  
   * @see: UCall for more info on callback methods/functions and their parameters
   */
  class String: public Data {
  public:
    UCLASS(String)
    static const unsigned int npos = static_cast<unsigned int>(-1);
    
    virtual int getNodeType() const {return TEXT_NODE;}
    virtual const String& getNodeName() const;
    virtual String getNodeValue() const {return *this;}
    virtual const String& getData() const {return *this;}
    
    virtual String* toStr() {return this;}
    virtual const String* toStr() const {return this;}
    
    virtual String& onChange(UCall& c);
    /**< adds a callback that is fired when the string is modified.
     * @see an example in the String doc.
     */
    
    
    String();
    ///< creates a new empty string; @see also shortcut ustr().
    
    String(const char*);
    ///< creates a new string; @see also shortcut ustr(const char*).
    
    String(const String&);
    ///< creates a new string; @see also shortcut ustr(const String&).
    
    String(const std::string&);
    ///< creates a new string; @see also shortcut ustr(const std::string&).
    
    virtual ~String();
    
    int length() const {return len;}
    ///< returns the char count of the string.

    bool empty() const {return (len == 0);}
    ///< returns true if the string is empty.
    
    bool isEmpty() const {return (len == 0);}
    ///< returns true if the string is empty.
        
    virtual String& clear();
    ///< clears the string's content.
    
    virtual String& operator=(const char*);
    virtual String& operator=(const String&);
    virtual String& operator=(const std::string&);
    
    String& setInt(int);          // NB: a numeral, such as 111 is an int
    String& setLong(long);
    String& setFloat(float);
    String& setDouble(double);    // NB: un numeral 1.1 est double  
    
    int    toInt() const;
    long   toLong() const;
    float  toFloat() const;
    double toDouble() const;
    
    int  scan(const char* format, ...) const;
    
    bool scanValue(float& val, String& unit) const;
    bool scanValue(float& val, const char*& unit) const;

    const char* c_str() const {return s;}
    ///< returns an unmutable C string.
    
    char at(int pos) const;
    /**< returns the character at this position.
     * - the last char is returned if pos = -1
     * - 0 is returned if the position is invalid (no exception thrown)
     */
    
    char operator[](int pos) const;
    ///< same as at(int).
    
    char setCharAt(int pos, char c, bool update = true);
    /**< changes the character at offset 'pos'.
     * - pos = -1 refers to the last char of the string
     * - returns the character or 0 if the position is invalid
     * - does not throw exception (check the returned value)
     */
    
    friend std::ostream& operator<<(std::ostream&, const String&);
    ///< prints the string on an output stream (e.g. cout << str).
    
    friend std::istream& operator>>(std::istream&, String&);
    ///< reads the string from an input stream (eg. cin >> str).
    
    // comparisons
    
    bool operator==(const char* s2) const        {return equals(s2);}
    bool operator==(const String& s2) const        {return equals(s2);}
    bool operator==(const std::string& s2) const {return equals(s2);}
    
    bool operator!=(const char* s2) const        {return !equals(s2);}
    bool operator!=(const String& s2) const        {return !equals(s2);}
    bool operator!=(const std::string& s2) const {return !equals(s2);}
    
    virtual bool equals(const char*, bool ignore_case =false) const;
    ///< checks if string contents are the same, ignores case if last argument is true.
    
    virtual bool equals(const String&, bool ignore_case =false) const;
    ///< checks if string contents are the same, ignores case if last argument is true.
    
    virtual bool equals(const std::string&, bool ignore_case =false) const;
    ///< checks if string contents are the same, ignores case if last argument is true.
    
    virtual int compare(const char*, bool ignore_case =false) const;
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    virtual int compare(const String&, bool ignore_case =false) const;
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    virtual int compare(const std::string&, bool ignore_case =false) const;
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    virtual int compareN(const char*, unsigned int n, bool ignore_case =false) const;
    ///< compare the N first chars of strings, ignores case if last argument is true.
    
    virtual int compareN(const String&, unsigned int n, bool ignore_case =false) const;
    ///< compare the N first chars of strings, ignores case if last argument is true.
    
    virtual int compareN(const std::string&, unsigned int n, bool ignore_case=false) const;
    ///< compare the N first chars of strings, ignores case if last argument is true.
    
    
    virtual int find(char s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int find(const char* s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int find(const String& s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int find(const std::string& s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int rfind(char s) const;
    ///< search for the last occurrence of 's', return -1 if not found. 
    
    String substring(int from_pos=0, unsigned int nbchars = npos) const;
    ///< returns a substring.
    
    std::string toString(int pos=0, unsigned int nbchars = npos) const;
    /**< String to std::string conversion.
     * nbchars = npos means "copy to the end of *this string".
     * the second variant appends the substring to the 'to' argument.
     */
    
    bool toString(std::string& to, int pos=0, unsigned int nbchars = npos)const;
    /**< String to std::string conversion.
     * nbchars = npos means "copy to the end of *this string".
     * the second variant appends the substring to the 'to' argument.
     */
    
    String toUpper() const;
    ///< returns a upercase copy.
    
    String toLower() const;
    ///< returns a lowercase copy.
    
    void upper();
    ///< converts to uppercase characters.

    void lower();
    ///< converts to lowercase characters.

    void capitalize();
    ///< capitalizes this string.
    
    String split(int pos, bool delete_char_at_pos = false);
    /**< splits this String
     * removes chars after 'pos' in this String and returns the remaining chars
     * in another String
     */
    
    void trim(bool trim_beginning = true, bool trim_end = true);
    ///< removes white spaces, tabs and newlines at the beginning and/or the end of this String.
    
    int tokenize(std::vector<String*>& tokens, const String& separators,
                 bool trim_begin = true, bool trim_end = true) const;
    /**< parses this String and stores the tokens separated by a separator in the vector.
     * - return the number of valid tokens, which may differ from 
     * tokens.size() as explained below.
     * - the separator is one character from 'separators'
     * - this function augments the 'tokens' vector and creates its String elements.
     *   NOTE THAT if the vector is not empty the existing String elements are 
     *   REUSED or unchanged if tokens.size() > the number of  valid tokens.
     *   (this may be potentially dangerous but CPU efficient for tokenizing 
     *   a large number of strings)
     * - trim() is called for each token with arguments trim_begin and trim_end.
     * - the content of this string is unchanged
     */
    
    
    virtual void append(char);
    virtual void append(const char*);
    virtual void append(const char*, unsigned int nbchars);
    virtual void append(const String&);
    virtual void append(const String&, unsigned int nbchars);
    virtual void append(const std::string&);
    virtual void append(const std::string&, unsigned int nbchars);
    virtual void append(int);
    virtual void append(double);
    
    String& operator&=(const char c)           {append(c); return *this;}
    String& operator&=(const char* s2)         {append(s2); return *this;}
    String& operator&=(const String& s2)         {append(s2); return *this;}
    String& operator&=(const std::string& s2)  {append(s2); return *this;}
    String& operator&=(int i)                  {append(i); return *this;}
    String& operator&=(double d)               {append(d); return *this;}
    /**< appends the argument to the end of this String.
     * usage: str &= arg;
     * see also: append() and operator &
     */
    
    friend String operator&(const String& s1, const String& s2);
    friend String operator&(const String& s1, const char* s2);
    friend String operator&(const char* s1, const String& s2);
    friend String operator&(const String& s1, const std::string& s2);
    friend String operator&(const std::string& s1, const String& s2);
    friend String operator&(const String&, int);
    friend String operator&(int, const String&);
    friend String operator&(const String&, double);
    friend String operator&(double, const String&);
    /**< creates a new string that collates the 2 arguments.
     * usage: str = arg1 & arg2;
     * NB: the + operator has another meaning: it is used to create widget child lists
     */
    
    
    virtual bool insert(int to_pos, char);
    virtual bool insert(int to_pos, const char* from,
                        int from_pos = 0, unsigned int from_nbchars = npos);
    virtual bool insert(int to_pos, const String& from,
                        int from_pos = 0, unsigned int from_nbchars = npos);
    virtual bool insert(int to_pos, const std::string& from,
                        int from_pos = 0, unsigned int from_nbchars = npos);
    /**< inserts 'from' string at offset 'to_pos' in *this string.
     * details:
     * - pos = 0 specifies the beginning of the string (BEFORE the first char)
     * - pos = -1 specifies the end of the string (AFTER the last char)
     * - from_nbchars = npos means "to the end of the string"
     */
    
    
    virtual void remove(int pos, unsigned int nbchars);
    /**< removes 'nbchars' characters from offset 'pos'
     * details:
     * - pos = 0 specifies the first char of the string
     * - pos = -1 specifies the last char of the string
     */
    
    
    virtual void replace(int pos, unsigned int nbchars, const String&);
    virtual void replace(int pos, unsigned int nbchars, const char*);
    virtual void replace(int pos, unsigned int nbchars, const std::string&);
    ///< replaces 'nbchars' characters from offset 'pos' in this string by the argument string.
    
    // filenames.
    
    virtual String basename(bool with_suffix = true) const;
    /**< returns the base name of a pathname (omitting the directory).
     * this string is supposed to contain the pathname.
     * the suffix (= the pathname extension) is removed if 'with_suffix' is false.
     */
    
    virtual String dirname(bool with_final_slash = true) const;
    /**< returns the directory of a pathname (omitting the file name).
     * this string is supposed to contain the pathname.
     * the returned dirname is ended by a / if 'with_final_slash' is true.
     */
    
    virtual String suffix() const;
    ///< returns the suffix of a pathname.
    
    virtual String expand() const;
    ///< expands a pathname that contains a ~ into an absolute path.
    
    // I/O.
    
    virtual int read(const String& pathname);
    /**< reads this file and loads its content into this String.
     * pathname can start with a ~
     * returns the reading status (see UFilestat)
     */
    
    virtual int write(const String& pathname);
    /**< writes the content of this String in this file.
     * pathname can start with a ~
     * returns the writing status (see UFilestat)
     */
    
    static int readLines(const String& filename, std::vector<String*>& lines,
                         bool trim_beginning = true, bool trim_end = true);
    /**< reads this file and store its lines as a String vector.
     * filename can start with  a ~
     * returns the reading status (see UFilestat).
     * apply trim() to each line with arguments trim_beginning and trim_end
     */
    
    
    virtual void changed(bool update = true);
    /**< [impl] called when object's content is changed.
     * This function updates graphics (if 'update' is true) then fires 
     * parents' UOn::strChange callbacks
     */
    
    virtual void update();
    ///< updates grahics.
    
    
    String(const char*, UConst);
    ///< creates a new CONSTANT string; @see UObject::UCONST.
    
    virtual void syncVals(char*,int);
    
    virtual void addingTo(Child&, Element& parent);
    virtual void removingFrom(Child&, Element& parent);
    ///< NOTE that this function require a specific destructor.
    
    virtual bool checkFormat(int pos, int newchar);
    virtual bool checkFormat(int pos, const char*);
    ///< checks input conformance to format model (if any).
    
    virtual void initImpl(const char*, int len);
    virtual void setImpl(const char*, int len);
    virtual void setImplNoCopy(char *_s, int _len);
    virtual bool insertImpl(int to_pos, char newchar, bool upd);
    virtual bool insertImpl(int to_pos, const char* s2, int from_pos,
                            unsigned int from_len, bool upd);
    virtual bool replaceImpl(int pos, unsigned int nbchars,
                             const char* s2, bool upd);
    
    // prototype for non-warped text (View)
    virtual void getSize(UpdateContext&, Dimension&) const;
    
    // prototype for warped text (UFlowview)
    virtual void getSize(UpdateContext&, Dimension&, float available_width, 
                         int offset, int& str_sublen, int& change_line) const;
    
    // prototype for non-warped text (View)
    virtual void paint(Graph&, UpdateContext&, const Rectangle&) const;
    
    // prototype for warped text (UFlowview)
    virtual void paint(Graph&, UpdateContext&, const Rectangle&, int offset, int cellen) const;
  private:
    char* s;
    int len;  
  };
  
  
  std::ostream& operator<<(std::ostream&, const String&);
  ///< prints the string on an output stream (e.g. cout << str).
  
  std::istream& operator>>(std::istream&, String&);
  ///< reads the string from an input stream (eg. cin >> str).  
  
  inline String& ustr() {return *new String();}
  ///< creator shortcut that return a new empty String string.  
  
  inline String& ustr(const char* s) {return *new String(s);}
  ///< creator shortcut that return a new String string.
  
  inline String& ustr(const String& s) {return *new String(s);}
  ///< creator shortcut that return a new String string.
  
  inline String& ustr(const std::string& s) {return *new String(s);}
  ///< creator shortcut that return a new String string.
  
  
  /**
   * useful functions for C strings.
   * these functions may already exist on some OS.
   */
  struct UCstr {
    static char* dup(const char* s);
    ///< creates a duplicate of s.
    
    static char* dup(const char* s1, const char* s2);
    ///< creates a duplicate of s1 + s2.
    
    static char* dup(const char* s1, char sep, const char* s2);
    ///< creates a duplicate of s1 + sep + s2.
    
    static bool equals(const char* s1, const char* s2, bool ignore_case =false);
    ///< checks if string contents are the same, ignores case if last argument is true.

    static int compare(const char* s1, const char* s2, bool ignore_case =false);
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    static int compareN(const char* s1, const char* s2, unsigned int n, bool ignore_case =false);
    ///< compare the N first chars of strings, ignores case if last argument is true.
        
    static const char* suffix(const char *pathname);
    /**< finds the suffix in a pathname.
     * the returned value is null or points inside the original string
     *  (its is not duplicated and thus, can't be freed)
     */
    
    static void parsePath(char* path_name, int path_len,
                          char*& dir_name, int& dir_len,
                          char*& base_name, int& base_len,
                          char*& suffix, int& suffix_len);
    /**< transforms a path name into its dirname, basename and suffix components.
     * the returned pointers points inside the path name and should not be freed.
     */
  };
  
}
#endif // UBIT_CORE_STRING_H_
