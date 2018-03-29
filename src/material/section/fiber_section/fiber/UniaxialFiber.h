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
//UniaxialFiber.h

#ifndef UniaxialFiber_h
#define UniaxialFiber_h

#include "Fiber.h"

namespace XC {
class UniaxialMaterial;
class MaterialHandler;

//! \ingroup MATSCCFibers
//
//! @brief Fiber of uniaxial material.
class UniaxialFiber: public Fiber
  {
    void free_mem(void);
    void alloc(const UniaxialMaterial &theMat);
  protected:
    UniaxialMaterial *theMaterial; //!< pointer to a material
    double area; //!< area of the fiber 

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    UniaxialFiber(int classTag);
    UniaxialFiber(int tag, int classTag,double Area);
    UniaxialFiber(int tag, int classTag,const UniaxialMaterial &,const double &);
    UniaxialFiber(int tag, int classTag,const MaterialHandler &,const std::string &,const double &);
    UniaxialFiber(const UniaxialFiber &);
    UniaxialFiber &operator=(const UniaxialFiber &);
    ~UniaxialFiber(void);

    void setMaterial(const UniaxialMaterial *theMat);
    void setMaterial(const MaterialHandler &ldr,const std::string &nmbMat);

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
    
    inline UniaxialMaterial *getMaterial(void)
      { return theMaterial; }
    inline const UniaxialMaterial *getMaterial(void) const
      { return theMaterial; }
    //! @brief Return the fiber area.
    inline double getArea(void) const
      { return area; }
  };
} // end of XC namespace


#endif






