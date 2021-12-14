//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//FiberSection3dBase.cc

#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include "material/section/fiber_section/fiber/UniaxialFiber3d.h"
#include <classTags.h>
#include <material/section/fiber_section/FiberSection3dBase.h>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>


#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"

//! @brief Constructor.
XC::FiberSection3dBase::FiberSection3dBase(int tag, int classTag, int dim, const fiber_list &fiberList,XC::MaterialHandler *mat_ldr)
  : FiberSectionBase(tag,classTag, fiberList.size(),dim,mat_ldr)
  {}

//! @brief Constructor.
XC::FiberSection3dBase::FiberSection3dBase(int tag, int classTag, int dim,XC::MaterialHandler *mat_ldr):
  FiberSectionBase(tag, classTag,dim,mat_ldr) {}

//! @brief constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSection3dBase::FiberSection3dBase(int classTag, int dim,XC::MaterialHandler *mat_ldr):
  FiberSectionBase(0, classTag,dim,mat_ldr) {}

//! @brief Copy constructor.
XC::FiberSection3dBase::FiberSection3dBase(const FiberSection3dBase &other)
  : FiberSectionBase(other) {}

//! @brief Assignment operator.
XC::FiberSection3dBase &XC::FiberSection3dBase::operator=(const FiberSection3dBase &other)
  {
    FiberSectionBase::operator=(other);
    kr= other.kr;
    return *this;
  }

double XC::FiberSection3dBase::get_strain(const double &y,const double &z) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1) + z*def(2));
  }

//! @brief Adds a fiber to the section.
XC::Fiber *XC::FiberSection3dBase::addFiber(int tag,const MaterialHandler &ldr,const std::string &nmbMat,const double &Area, const Vector &position)
  {
    Fiber *retval= fibers.findFiber(tag);
    if(retval)
      std::cerr << "(FiberSection3dBase::addFiber; fiber: " << tag << " already exists." << std::endl;
    else
      {
        if(position.Size()<2)
          std::cerr << "(FiberSection2d::addFiber; the position vector must be of dimension 2." << std::endl;
        UniaxialFiber3d f(tag,ldr,nmbMat,Area,position);
        retval= addFiber(f);
      }
    return retval;
  }

int XC::FiberSection3dBase::updateParameter(int parameterID, Information &info)
  {
    int ok= -1;

    switch(parameterID)
      {
      case 1:
        return -1;
      default:
        if(parameterID >= 100)
          {
            ID *paramIDPtr;
            paramIDPtr= info.theID;
            ID paramID= (*paramIDPtr);
            int paramMatrTag= paramID(1);
            ok= fibers.updateParameter(paramMatrTag,parameterID-100,info);
            if(ok<0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; could not update parameter. " << std::endl;
                return ok;
              }
            else
              { return ok; }
          }
        else
          return -1;
     }
  }

//! @brief Returns strain at position being passed as parameter.
double XC::FiberSection3dBase::getStrain(const double &y,const double &z) const
  { return get_strain(-y,z); } //XXX We must change sign of the y coordinate
                               //because of the mess due to the change of
                               //the coordinate sign in the private member
                               //of the object. See, for example,
                               //UniaxialFiber2d constructor

