/*
 *  number.hpp
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


#ifndef _unumber_hpp_
#define	_unumber_hpp_ 1

#include <iostream>
#include <ubit/core/node.h>

namespace ubit {
 
// !!! should not inherit from Node

/**
 * Active Number base class.
 */
class Number: public Node {
public:
  //UCLASS("#number", Number, null)
  UABSTRACT_CLASS(Number)
    
  virtual int    intValue()    const = 0;
  virtual float  floatValue()  const = 0;
  virtual double doubleValue() const = 0;
  virtual String   toString()    const = 0;

  virtual Number& onChange(UCall&);
  ///< adds callbacks that are activated when the objects' value changes.

  virtual void changed(bool = true);
  ///< called when object's content is changed.
};
  
  
/**
 * Active Integer
 */
class Int : public Number {
public:
  UCLASS(Int)
  
  Int(int v = 0) : value(v) {}
  Int(const Int& v) : value(v.value) {}
  Int(const String&);
  Int(const std::string&);

  operator int() const {return value;}
  ///< type conversion: converts Int to int.

  friend std::istream& operator>>(std::istream&, Int&);
  ///< prints value on output stream.

  int    intValue()    const {return value;}
  float  floatValue()  const {return float(value);}
  double doubleValue() const {return double(value);}
  String   toString()    const;
  //static int parseInt(String s);
  //static int parseInt(String s, int radix);
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Int& operator=(int v)          {return setImpl(v);}
  Int& operator=(const Int& v)  {return setImpl(v.value);}

  Int& operator=(const char* s)  {return setImpl(s);}
  Int& operator=(const String& s);
  Int& operator=(const std::string& s);

  friend std::ostream& operator<<(std::ostream&, const Int&);
  ///< reads value from input stream.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool operator==(int v)         const {return value == v;}
  bool operator==(double v)      const {return value == v;}
  bool operator==(const Int& v) const {return value == v.value;}

  bool operator!=(int v)         const {return value != v;}
  bool operator!=(double v)      const {return value != v;}
  bool operator!=(const Int& v) const {return value != v.value;}

  bool operator<(int v)          const {return value < v;}
  bool operator<(double v)       const {return value < v;}
  bool operator<(const Int& v)  const {return value < v.value;}

  bool operator<=(int v)         const {return value <= v;}
  bool operator<=(double v)      const {return value <= v;}
  bool operator<=(const Int& v) const {return value <= v.value;}

  bool operator>(int v)          const {return value > v;}
  bool operator>(double v)       const {return value > v;}
  bool operator>(const Int& v)  const {return value > v.value;}

  bool operator>=(int v)         const {return value >= v;}
  bool operator>=(double v)      const {return value >= v;}
  bool operator>=(const Int& v) const {return value >= v.value;}
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Int& operator++() {return setImpl(value+1);}
  Int  operator++(int);
  Int& operator--() {return setImpl(value-1);}
  Int  operator--(int);

  Int& operator+=(int v) {return setImpl(value+v);}
  Int& operator+=(const Int& v) {return setImpl(value+v.value);}
  Int& operator-=(int v) {return setImpl(value-v);}
  Int& operator-=(const Int& v) {return setImpl(value-v.value);}
  Int& operator*=(int v) {return setImpl(value*v);}
  Int& operator*=(const Int& v) {return setImpl(value*v.value);}
  Int& operator/=(int v) {return setImpl(value/v);}
  Int& operator/=(const Int& v) {return setImpl(value/v.value);}
  

  virtual Int& setImpl(int, bool call_callbacks = true);
  virtual Int& setImpl(const char*, bool call_callbacks = true);
  ///< [impl] assignment.
  
private:
  friend class Float;
  friend class Double;
  int value;
};

  
/** Active Float.
*/
class Float : public Number {
public:
  UCLASS(Float)
  
  Float(float v = 0.0)     : value(v) {}
  Float(const Float& v)   : value(v.value) {}
  Float(const Int& v) : value(v.value) {}
  //Float(const char*);  ambigu
  Float(const String&);
  Float(const std::string&);

  operator float() const {return value;}
  ///< type conversion: converts Float to float.

  friend std::ostream& operator<<(std::ostream&, const Float&);
  ///< prints value on output stream.

  int    intValue()    const {return int(value);}
  float  floatValue()  const {return value;}
  double doubleValue() const {return double(value);}
  String   toString()    const;
  
   // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Float& operator=(float v)           {return setImpl(v);}
  Float& operator=(const Float& v)   {return setImpl(v.value);}
  Float& operator=(const Int& v) {return setImpl(v.value);}
  Float& operator=(const char* s)     {return setImpl(s);}
  Float& operator=(const String& s);
  Float& operator=(const std::string& s);

  friend std::istream& operator>>(std::istream&, Float&);
  ///< reads value from input stream.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool operator==(float v)          const {return value == v;}
  bool operator==(double v)         const {return value == v;}
  bool operator==(const Float& v)  const {return value == v.value;}

  bool operator!=(float v)          const {return value != v;}
  bool operator!=(double v)         const {return value != v;}
  bool operator!=(const Float& v)  const {return value != v.value;}

  bool operator<(float v)           const {return value < v;}
  bool operator<(double v)          const {return value < v;}
  bool operator<(const Float& v)   const {return value < v.value;}

  bool operator<=(float v)          const {return value <= v;}
  bool operator<=(double v)         const {return value <= v;}
  bool operator<=(const Float& v)  const {return value <= v.value;}

  bool operator>(float v)           const {return value > v;}
  bool operator>(double v)          const {return value > v;}
  bool operator>(const Float& v)   const {return value > v.value;}

  bool operator>=(float v)          const {return value >= v;}
  bool operator>=(double v)         const {return value >= v;}
  bool operator>=(const Float& v)  const {return value >= v.value;}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Float& operator++() {return setImpl(value+1);}
  Float  operator++(int);
  Float& operator--() {return setImpl(value-1);}
  Float  operator--(int);

  Float& operator+=(int v) {return setImpl(value+v);}
  Float& operator+=(const Float& v) {return setImpl(value+v.value);}
  Float& operator-=(int v) {return setImpl(value-v);}
  Float& operator-=(const Float& v) {return setImpl(value-v.value);}
  Float& operator*=(int v) {return setImpl(value*v);}
  Float& operator*=(const Float& v) {return setImpl(value*v.value);}
  Float& operator/=(int v) {return setImpl(value/v);}
  Float& operator/=(const Float& v) {return setImpl(value/v.value);}


  virtual Float& setImpl(float, bool call_callbacks = true);
  virtual Float& setImpl(const char*, bool call_callbacks = true);
  ///< [impl] assignment.
  
private:
  friend class Int;
  friend class Double;
  float value;
};

  
/** Active Double.
*/
class Double : public Number {
public:
  UCLASS(Double)

  Double(double v = 0.0) : value(v) {}
  Double(const Double& v) : value(v.value) {}
  Double(const Float& v) : value(v.value) {}
  Double(const Int& v) : value(v.value) {}
  Double(const String&);
  Double(const std::string&);

  operator double() const {return value;}
  ///< type conversion: converts Double to double.

  friend std::ostream& operator<<(std::ostream&, const Double&);
  ///< prints value on output stream.

  int    intValue()    const {return int(value);}
  float  floatValue()  const {return float(value);}
  double doubleValue() const {return value;}
  String   toString()    const;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Double& operator=(double v)           {return setImpl(v);}
  Double& operator=(const Double& v)   {return setImpl(v.value);}
  Double& operator=(const Float& v)    {return setImpl(v.value);}
  Double& operator=(const Int& v)  {return setImpl(v.value);}

  Double& operator=(const char* s)      {return setImpl(s);}
  Double& operator=(const String& s);
  Double& operator=(const std::string& s);

  friend std::istream& operator>>(std::istream&, Double&);
  ///< reads value from input stream.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool operator==(float v)          const {return value == v;}
  bool operator==(double v)         const {return value == v;}
  bool operator==(const Double& v) const {return value == v.value;}

  bool operator!=(float v)          const {return value != v;}
  bool operator!=(double v)         const {return value != v;}
  bool operator!=(const Double& v) const {return value != v.value;}

  bool operator<(float v)           const {return value < v;}
  bool operator<(double v)          const {return value < v;}
  bool operator<(const Double& v)  const {return value < v.value;}

  bool operator<=(float v)          const {return value <= v;}
  bool operator<=(double v)         const {return value <= v;}
  bool operator<=(const Double& v) const {return value <= v.value;}

  bool operator>(float v)           const {return value > v;}
  bool operator>(double v)          const {return value > v;}
  bool operator>(const Double& v)  const {return value > v.value;}

  bool operator>=(float v)          const {return value >= v;}
  bool operator>=(double v)         const {return value >= v;}
  bool operator>=(const Double& v) const {return value >= v.value;}
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Double& operator++() {return setImpl(value+1);}
  Double  operator++(int);
  Double& operator--() {return setImpl(value-1);}
  Double  operator--(int);

  Double& operator+=(int v) {return setImpl(value+v);}
  Double& operator+=(const Double& v) {return setImpl(value+v.value);}
  Double& operator-=(int v) {return setImpl(value-v);}
  Double& operator-=(const Double& v) {return setImpl(value-v.value);}
  Double& operator*=(int v) {return setImpl(value*v);}
  Double& operator*=(const Double& v) {return setImpl(value*v.value);}
  Double& operator/=(int v) {return setImpl(value/v);}
  Double& operator/=(const Double& v) {return setImpl(value/v.value);}


  virtual Double& setImpl(double, bool call_callbacks = true);
  virtual Double& setImpl(const char*, bool call_callbacks = true);
  ///< [impl] assignment.

private:
  friend class Int;
  friend class Float;
  double value;
};

}
#endif

