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
//ProtoTruss.h

#ifndef ProtoTruss_h
#define ProtoTruss_h

#include <domain/mesh/element/Element1D.h>

namespace XC {
class Node;
class Material;

//! \ingroup OneDimensionalElem
//
//! @defgroup TrussElem Truss elements.
//
//! \ingroup TrussElem
//! @brief Base class for truss elements.
class ProtoTruss : public Element1D
  {
  protected:
    int numDOF; //!< number of dof for truss
    int dimSpace; //!< truss in 2 or 3d domain
    Matrix *theMatrix; //!< pointer to objects matrix (a class wide Matrix)
    Vector *theVector; //!< pointer to objects vector (a class wide Vector)

    // static data - single copy for all objects of the class	
    static Matrix trussM2;   // class wide matrix for 2*2
    static Matrix trussM3;   // class wide matrix for 3*3
    static Matrix trussM4;   // class wide matrix for 4*4
    static Matrix trussM6;   // class wide matrix for 6*6
    static Matrix trussM12;  // class wide matrix for 12*12
    static Vector trussV2;   // class wide Vector for size 2
    static Vector trussV3;   // class wide Vector for size 3
    static Vector trussV4;   // class wide Vector for size 44
    static Vector trussV6;   // class wide Vector for size 6
    static Vector trussV12;  // class wide Vector for size 12

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    void setup_matrix_vector_ptrs(int dofNd1);

  public:
    ProtoTruss(int tag, int classTag,int Nd1,int Nd2,int ndof,int dimSpace);
    ProtoTruss(const ProtoTruss &);
    ProtoTruss &operator=(const ProtoTruss &);

    virtual const Material *getMaterial(void) const= 0;
    virtual Material *getMaterial(void)= 0;
    Material &getMaterialRef(void);
    virtual double getRho(void) const= 0;

    // public methods to obtain inforrmation about dof & connectivity    
    int getNumDIM(void) const;	
    int getNumDOF(void) const;

  };

} //end of XC namespace
#endif
