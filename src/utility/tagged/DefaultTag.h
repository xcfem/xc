//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
                                                                        
#ifndef DefaultTag_h
#define DefaultTag_h


namespace XC {
//! @ingroup Tagged
//
//! @brief Default tag
class DefaultTag
  {
  private:
    int default_tag; //<! tag para el nuevo nodo.
  public:
    // constructors
    inline DefaultTag(int tag= 0)
      : default_tag(tag) {}

    inline DefaultTag &operator=(const int &i)
      {
        default_tag= i;
        return *this;
      }
    inline void setTag(const int &tag)
      { default_tag= tag; }
    inline const int &getTag(void) const
      { return default_tag; }
    inline void incTag(void)
      { default_tag++; }
    inline int operator++(int)
      {
	int retval= default_tag;
        default_tag++;
        return retval;
      }
    inline int operator++(void)
      { return ++default_tag; }
  };


} // end of XC namespace

#endif

