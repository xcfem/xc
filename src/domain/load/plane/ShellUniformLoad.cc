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
//ShellUniformLoad.cpp

#include "ShellUniformLoad.h"
#include "domain/mesh/element/utils/fvectors/FVectorShell.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/plane/shell/ShellMITC4Base.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "xc_utils/src/geom/pos_vec/SlidingVector3d.h"



XC::ShellUniformLoad::ShellUniformLoad(int tag,const double &wt,const double &wa1,const double &wa2,const ID &theElementTags)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad,wt,wa1,wa2,theElementTags) {}
XC::ShellUniformLoad::ShellUniformLoad(int tag, const Vector &Fxyz, const ID &theElementTags)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad,Fxyz[2],Fxyz[0],Fxyz[1],theElementTags) {}

XC::ShellUniformLoad::ShellUniformLoad(int tag)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad) {}

std::string XC::ShellUniformLoad::Categoria(void) const
  { return "uniforme"; }


//! @brief Return the dimension of the force vector.
size_t XC::ShellUniformLoad::getForceVectorDimension(void) const
  { return 3; }

//! @brief Return the dimension of the moment vector.
size_t XC::ShellUniformLoad::getMomentVectorDimension(void) const
  { return 3; }

//! @brief Returns the components of the force vectors.
const XC::Matrix &XC::ShellUniformLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Wx();
        retval(i,1)= Wy();
        retval(i,2)= Wz();
      }
    return retval;
  }

//! @brief Returns the components of the moment vectors.
const XC::Matrix &XC::ShellUniformLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= 0.0;
        retval(i,1)= 0.0;
        retval(i,2)= 0.0;
      }
    return retval;
  }


const XC::Vector &XC::ShellUniformLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    static const Vector trash;
    return trash;
  }

//! @brief Adds the load al consistent load vector (see page 108 libro Eugenio Oñate).
//! @param area Area of the element.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::ShellUniformLoad::addReactionsInBasicSystem(const double &area,const double &loadFactor,FVectorShell &p0) const
  {
    //Loads over nodes.
    const double Px= Wx()*loadFactor*area/4;
    const double Py= Wy()*loadFactor*area/4;
    const double Pz= Wz()*loadFactor*area/4; //Axil.

    // Reactions in basic system
    p0.addForce(0,Px,Py,Pz);

    //p0[3]-= 0.0;
    //p0[4]-= 0.0;
    //p0[5]-= 0.0;

    p0.addForce(1,Px,Py,Pz);

    //p0[9]-= 0.0;
    //p0[10]-= 0.0;
    //p0[11]-= 0.0;

    p0.addForce(2,Px,Py,Pz);

    //p0[15]-= 0.0;
    //p0[16]-= 0.0;
    //p0[17]-= 0.0;

    p0.addForce(3,Px,Py,Pz);

    //p0[21]-= 0.0;
    //p0[22]-= 0.0;
    //p0[23]-= 0.0;
  }

//! @brief ??
//! @param area Area of the element.
//! @param loadFactor Load factor.
//! @param q0 
void XC::ShellUniformLoad::addFixedEndForcesInBasicSystem(const double &area,const double &loadFactor,FVectorShell &q0) const
  {
    std::cerr << "ShellUniformLoad::addFixedEndForcesInBasicSystem not implemented." << std::endl;
  }

//! brief Returns load resultant (force and moment integration over the elements).
SlidingVectorsSystem3d XC::ShellUniformLoad::getResultant(const Pos3d &centro, bool initialGeometry) const
  {
    SlidingVectorsSystem3d retval(centro);
    Matrix pressures= getGlobalPressures();
    const Domain *ptrDom= getDomain();
    if(ptrDom)
      {
        const size_t sz= pressures.noRows();
        for(size_t i=0; i<sz; i++)
          {
            const size_t elemTag= getElementTags()(i);
            const Element *ptrElem= ptrDom->getElement(elemTag);
            if(const ShellMITC4Base *ptrShell= dynamic_cast<const ShellMITC4Base *>(ptrElem))
	      {
  	        const double area= ptrShell->getArea();
		const Vector3d force(area*pressures(i,0),area*pressures(i,1),area*pressures(i,2));
		retval+= SlidingVector3d(ptrShell->getCenterOfMassPosition(),force);
              }
            else
	      std::cerr << "ShellMecLoad::getResultant; the element: "
                        << elemTag << " is not a shell element." << std::endl;
          }
      }
    else
      std::cerr << "ShellUniformLoad::getResultant; pointer to domain is NULL." << std::endl;
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ShellUniformLoad::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

int XC::ShellUniformLoad::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6);
    int result= sendData(cp);

    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "ShellUniformLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

int XC::ShellUniformLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; data could not be received.\n" ;
    else
      res+= recvData(cp);
    return res;
  }

void  XC::ShellUniformLoad::Print(std::ostream &s, int flag)
  {
    s << "ShellUniformLoad - Reference load" << std::endl;
    s << "  Transverse:  " << Trans << std::endl;
    s << "  Axial1:      " << Axial1 << std::endl;
    s << "  Axial2:      " << Axial2 << std::endl;
    s << "  Elements acted on: " << getElementTags();
  }
