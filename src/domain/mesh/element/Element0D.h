// -*-c++-*-
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
//Element0D.h

#ifndef Element0D_h
#define Element0D_h

#include <domain/mesh/element/ElementBase.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class Node;
class Material;

//! @ingroup Elem
//
//! @brief Element of dimension 0 (both nodes have the
//! same position).
class Element0D: public ElementBase<2>
  {
  protected:
    static const double LenTol; //!< Tolerance for zero length of element.
    class Vxy: public CommandEntity
      {
        Vector x,y;
      public:
        Vxy(void);
        Vxy(const Vector &,const Vector &);
        inline const Vector &getX(void)
	  { return x; }
        inline const Vector &getY(void)
	  { return y; }
        bool check(void) const;
      };
    int dimension; //!< = 2 or 3 dimensions
    int numDOF;	   //!< number of dof for ZeroLengthSection
    Matrix transformation; //!< transformation matrix for orientation

    virtual void setUp(int Nd1, int Nd2,const Vector &x,const Vector &y);
    ElemPtrArray3d sew(const SetEstruct &f1,const SetEstruct &f2) const;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    Element0D(int tag, int classTag,int Nd1,int Nd2);
    Element0D(int tag, int classTag,int Nd1,int Nd2, int dim);
    Element0D(int tag, int classTag,int Nd1,int Nd2, int dim, const Vector &, const Vector &);

    BoolArray3d getNodePattern(void) const;
    int getNumDOF(void) const;
    size_t getDimension(void) const;
    int getVtkCellType(void) const;
    const Vector &getX(void) const;
    const Vector &getY(void) const;
    const Vector &getZ(void) const;
    Matrix getLocalAxes(bool) const;
    Rect3d3dCooSys getCooSys(bool) const;    
    inline virtual const Matrix &getTransformation(void) const
      { return transformation; }

    virtual void setUpVectors(const Vector &, const Vector &);
  };


} //end of XC namespace
#endif
