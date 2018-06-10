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
//SectionMatrices.h

#ifndef SectionMatrices_h
#define SectionMatrices_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {

//! @ingroup OneDimensionalElem
//
//! @brief Matrices for each section (used in BeamColumnWithSectionFD)
class SectionMatrices: public MovableObject
  {
  private:
    std::vector<Vector> vsSubdivide;
    std::vector<Vector> SsrSubdivide;
    std::vector<Matrix> fsSubdivide;

  public:
    static const size_t maxNumSections= 10;
  private:
    void alloc(const size_t &sz);
    void free(void);

  public:
    SectionMatrices(const size_t &sz= 10);

    static const size_t &getMaxNumSections(void)
      { return maxNumSections; }

    const std::vector<Vector> &getVsSubdivide(void) const
      { return vsSubdivide; }
    const std::vector<Vector> &getSsrSubdivide(void) const
      { return SsrSubdivide; }
    const std::vector<Matrix> &getFsSubdivide(void) const
      { return fsSubdivide; }
    std::vector<Vector> &getVsSubdivide(void)
      { return vsSubdivide; }
    std::vector<Vector> &getSsrSubdivide(void)
      { return SsrSubdivide; }
    std::vector<Matrix> &getFsSubdivide(void)
      { return fsSubdivide; }

    void resize(const size_t &sz);

    void initialize(const size_t &i,const int &order);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} //end of XC namespace
#endif
