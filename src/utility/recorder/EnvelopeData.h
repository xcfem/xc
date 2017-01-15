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
                                                                        
#ifndef EnvelopeData_h
#define EnvelopeData_h

#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Vector;
class Matrix;

class EnvelopeData: public MovableObject
  {
  private:	
    Matrix *data;
    Vector *currentData;
    bool first;
    friend class EnvelopeElementRecorder;
    friend class EnvelopeNodeRecorder;
    void alloc(const size_t &);
    void free(void);
  protected:
    int sendData(CommParameters &);  
    int receiveData(const CommParameters &);
  public:
    EnvelopeData(void);
    ~EnvelopeData(void);

    inline const Matrix *getData(void) const
      { return data; }
    inline Matrix *getData(void)
      { return data; }
    inline const Vector *getCurrentData(void) const
      { return currentData; }
    inline Vector *getCurrentData(void)
      { return currentData; }
    inline const bool &First(void) const
      { return first; }
    inline bool &First(void)
      { return first; }

    int restart(void); 

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif
