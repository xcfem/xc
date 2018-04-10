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
//MeshComponent.h                                                         
                                                                        
#ifndef MeshComponent_h
#define MeshComponent_h

#include "domain/component/ContinuaReprComponent.h"
#include "xc_basic/src/text/LabelContainer.h"

class Pos3d;

namespace XC {
class Matrix;

//! @brief Base class for nodes and elements (mesh components).
class MeshComponent: public ContinuaReprComponent
  {
  private:
    void check_matrices(const std::deque<Matrix> &,const int &) const;
    void nueva_matriz(std::deque<Matrix> &,const int &) const;
  protected:
    mutable int index; //!< Index for VTK arrays.
    LabelContainer labels; //!< Label container.

    void setup_matrices(std::deque<Matrix> &,const int &) const;
    int sendIdsEtiquetas(int posDbTag,CommParameters &);
    int recvIdsEtiquetas(int posDbTag,const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    // constructors
    MeshComponent(int classTag);
    MeshComponent(int tag, int classTag);
  };

} // end of XC namespace

#endif

