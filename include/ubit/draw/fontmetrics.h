/*
 *  ufontmetrics.hpp
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


#ifndef _ufontmetrics_hpp_
#define	_ufontmetrics_hpp_ 1
namespace ubit {

class UFontMetrics {
public:
  UFontMetrics();

  UFontMetrics(const UFont&, Display*);

  UFontMetrics(const UFontDesc&, Display*);
  ///< Note: the UFontDesc argument must NOT be destroyed while this UFontMetrics is used.

  UFontMetrics(UpdateContext&);

  ~UFontMetrics();
    
  void set(const UFont&, Display* = null);
  void set(const UFontDesc&, Display* = null);

  float getAscent() const;  
  float getDescent() const;
  float getHeight() const;

  float getWidth(char) const;  
  float getWidth(const String&) const;
  float getWidth(const String&, int from_char, int len = -1) const;
  float getWidth(const char* str, int str_len = -1) const;

  int getCharPos(const char* str, int str_len, float x) const;
  ///< converts x pos to char pos.

  float getXPos(const char* str, int str_len, int char_pos) const;
  ///< converts char pos to x pos.
  
  bool getSubTextSize(const char* s, int s_len, Dimension&, float available_width, 
                      int& s_sublen, int& change_line) const;
  /**< returns font metrics of a substring that verifies certain conditions.
    * Notes:
    * - the substring starts from the beginning of 'str' and ends at 
    *   the 1st \n  or 1st " " that is before 'available_width'.
    * - it is equal to 'str' otherwise.
    * - returns w and h that this substring occupies and its length 'sublen'
    * - change_line = 1 si line must be changed because of size
    *   and 2 if a \n was found
    */
  
  static bool 
    getClippedText(const UHardFont*, float clip_x, float clip_width,
                   const char* str, int str_len, 
                   float x0, float y0, int& charpos_begin, int& charpos_end,
                   float& xpos_begin, float& xpos_end);
  /**< returns the substring that is enclosed in the current clip.
    * utilise par drawString en mode OpenGL pour eviter de dessiner
    * ce qui sort de la zone de clipping
    */

protected:
  Display* disp;
  const UFontDesc* fd;
  bool own_fd;
};

}
#endif
