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

#ifndef IntPtrWrapper_h
#define IntPtrWrapper_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <boost/python/list.hpp>

namespace XC {
//! @ingroup Matrix
//
class IntPtrWrapper: public EntCmd
  {
  private:
    static int ID_NOT_VALID_ENTRY;
    int sz;
    int *data;
    int fromFree;
    void free_mem(void);
    void check_sizes(void);
    IntPtrWrapper(const IntPtrWrapper &);
    IntPtrWrapper &operator=(const IntPtrWrapper  &V);
  public:
    // constructors and destructor
    IntPtrWrapper(void);
    IntPtrWrapper(int *data, int size, bool cleanIt= false);
    virtual ~IntPtrWrapper();
 
    // utility methods
    inline int Size(void) const
      { return sz; }
    void Zero(void);
    inline const int *getDataPtr(void) const
      { return data; }
    inline int *getDataPtr(void)
      { return data; }
    bool isEmpty(void) const;
    const int &max(void) const;
    const int &min(void) const;

    bool checkRange(const int &) const;
    int &at(const int &); 
    const int &at(const int &) const; 
    int &operator()(const int &);
    const int &operator()(const int &) const;
    int &operator[](const int &i)    	    
      { return at(i); }
    const int &operator[](const int &i) const    	    
      { return at(i); }
    
    int setData(int *newData, int size, bool cleanIt = false);
    int getLocation(int value) const;
    int getLocationOrdered(int value) const; // for when insert was used to add elements
    int removeValue(int value);

    friend std::ostream &operator<<(std::ostream &, const IntPtrWrapper &);


    friend class UDP_Socket;
    friend class TCP_Socket;
    friend class TCP_SocketNoDelay;
    friend class MPI_Channel;
  };

std::ostream &operator<<(std::ostream &, const IntPtrWrapper &);

std::vector<int> id_to_std_vector(const IntPtrWrapper &);

inline bool IntPtrWrapper::isEmpty(void) const
  { return (data== nullptr); }

//! @brief check if argument is inside range [0,sz-1]
inline bool IntPtrWrapper::checkRange(const int &i) const
  {
    if((i < 0) || (i >= sz)) //Range checking.
      {
        std::cerr << "IntPtrWrapper::(loc) - loc "
		  << i << " outside range 0 - " <<  sz-1 << std::endl;
        return false;
      }
    else
      return true;
  }
 
inline int &IntPtrWrapper::at(const int &i) 
  {
    if(checkRange(i))
      return data[i];
    else
      return ID_NOT_VALID_ENTRY;
  }

inline const int &IntPtrWrapper::at(const int &i) const 
  {
    if(checkRange(i))
      return data[i];
    else
      return ID_NOT_VALID_ENTRY;
  }

 
inline int &IntPtrWrapper::operator()(const int &i) 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif
    return data[i];
  }

inline const int &IntPtrWrapper::operator()(const int &i) const 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif

    return data[i];
  }

} // end of XC namespace

#endif



