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
//CrossSectionProperties3d.cc

#include <material/section/repres/CrossSectionProperties3d.h>
#include "xc_utils/src/utils/misc_utils/inertia.h"
#include "xc_utils/src/geom/sis_ref/PrincipalAxesOfInertia2D.h"
#include <domain/mesh/element/utils/Information.h>

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include "domain/component/Parameter.h"
#include "material/section/SectionForceDeformation.h"
#include "material/section/ResponseId.h"

XC::Matrix XC::CrossSectionProperties3d::ks4(4,4);
XC::Matrix XC::CrossSectionProperties3d::ks6(6,6);

//! @brief Check values of inertia values.
bool XC::CrossSectionProperties3d::check_values(void)
  {
    bool retval= true;
    if(iy <= 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Input Iy <= 0.0 ... setting Iy to 1.0\n";
        iy= 1.0;
        retval= false;
      }
    if(j <= 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Input J <= 0.0 ... setting J to 1.0\n";
        j= 1.0;
        retval= false;
      }
    if(retval) //it's ok so far.
      retval= CrossSectionProperties2d::check_values();
    return retval;
  }

//! @brief Constructor.
XC::CrossSectionProperties3d::CrossSectionProperties3d(void)
  :CrossSectionProperties2d(), iy(0), iyz(0), j(0) {}

//! @brief Constructor.
XC::CrossSectionProperties3d::CrossSectionProperties3d(double E_in, double A_in, double Iz_in, double Iy_in, double G_in, double J_in)
  : CrossSectionProperties2d(E_in,A_in,Iz_in,G_in), iy(Iy_in), iyz(0), j(J_in)
  { check_values(); }

//! @brief Constructor.
XC::CrossSectionProperties3d::CrossSectionProperties3d(double EA_in, double EIz_in, double EIy_in, double GJ_in)
  : CrossSectionProperties2d(EA_in,EIz_in), iy(EIy_in), iyz(0), j(GJ_in)
  { check_values(); }

//! @brief Constructor.
XC::CrossSectionProperties3d::CrossSectionProperties3d(const SectionForceDeformation &section)
  : CrossSectionProperties2d(section), iy(0.0), iyz(0.0), j(0.0)
  {
    const Matrix &sectTangent= section.getInitialTangent();
    const ResponseId &sectCode= section.getType();
    for (int i=0; i<sectCode.Size(); i++)
      {
	int code = sectCode(i);
	switch(code)
	  {
	  case SECTION_RESPONSE_MY:
	    iy= sectTangent(i,i);
	    break;
	  case SECTION_RESPONSE_T:
	    j= sectTangent(i,i);
	    break;
	  default:
	    break;
	  }
      }
  }


//! @brief Returns the angle between the principal axes and the local system.
double XC::CrossSectionProperties3d::getTheta(void) const
  { return theta_inertia(Iy(),Iz(),Iyz()); }

//! @brief Returns the major principal axis of inertia.
double XC::CrossSectionProperties3d::getI1(void) const
  { return I1_inertia(Iy(),Iz(),Iyz()); }

//! @brief Returns the minor principal axis of inertia.
double XC::CrossSectionProperties3d::getI2(void) const
  { return I2_inertia(Iy(),Iz(),Iyz()); }

//! @brief Returns the principal axis of inertia.
PrincipalAxesOfInertia2D XC::CrossSectionProperties3d::getInertiaAxes(void) const
  {
    const Pos2d center_of_mass(0,0);
    return PrincipalAxesOfInertia2D(center_of_mass,Iy(),Iz(),Iyz());
  }
//! @brief Returns principal axis I (strong).
Vector2d XC::CrossSectionProperties3d::getAxis1VDir(void) const
  { return getInertiaAxes().getAxis1VDir(); }
//! @brief Returns principal axis I (strong).
Vector2d XC::CrossSectionProperties3d::getVDirStrongAxis(void) const
  { return getInertiaAxes().getAxis1VDir(); }
//! @brief Returns principal axis II (weak).
Vector2d XC::CrossSectionProperties3d::getAxis2VDir(void) const
  { return getInertiaAxes().getAxis2VDir(); }
//! @brief Returns principal axis II (weak).
Vector2d XC::CrossSectionProperties3d::getVDirWeakAxis(void) const
  { return getInertiaAxes().getAxis2VDir(); }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getSectionTangent4x4(void) const
  {
    ks4(0,0)= EA(); //Axial stiffness.
    ks4(1,1)= EIz(); //z bending stiffness.
    ks4(1,2)= ks4(2,1)= -EIyz(); //Contribution of the product of inertia.
    ks4(2,2)= EIy(); //y bending stiffness.
    ks4(3,3)= GJ(); //Torsional stiffness.
    return ks4;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getInitialTangent4x4(void) const
  { return getSectionTangent4x4(); }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getSectionFlexibility4x4(void) const
  {
    const double eiyz= EIyz();
    const double eimax= std::max(EIz(),EIy());
    if(std::abs(eiyz/eimax)<1e-5) //product of inertia nulo.
      {
        ks4(0,0)= 1.0/(EA());
        ks4(1,1)= 1.0/EIz();
        ks4(2,2)= 1.0/(EIy());
        ks4(3,3)= 1.0/(GJ());
      }
    else //product of inertia NO nulo.
      {
        getSectionTangent4x4();
        ks4(0,0)= 1.0/ks4(0,0);
        const double a= ks4(1,1); const double b= ks4(1,2);
        const double c= ks4(2,2);
        const double d= 1/(a*c-b*b);
        ks4(1,1)= c/d; ks4(1,2)=ks4(2,1)= -b/d;
        ks4(2,2)= a/d;
        ks4(3,3)= 1.0/ks4(3,3);
      }
    return ks4;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getInitialFlexibility4x4(void) const
  { return getSectionFlexibility4x4(); }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getSectionTangent6x6(void) const
  {
    ks6(0,0) = EA(); //Axial stiffness.
    ks6(1,1) = EIz(); //z bending stiffness.
    ks4(1,3)= ks4(3,1)= -EIyz(); //Contribution of the product of inertia.
    ks6(3,3) = EIy(); //y bending stiffness.
    ks6(5,5) = GJ(); //Torsional stiffness.

    const double GA = GAAlpha();
    ks6(2,2)= GA;
    ks6(4,4)= GA;
    return ks6;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getInitialTangent6x6(void) const
  { return getSectionTangent6x6(); }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getSectionFlexibility6x6(void) const
  {
    const double eiyz= EIyz();
    const double eimax= std::max(EIz(),EIy());
    if(std::abs(eiyz/eimax)<1e-5) //product of inertia nulo.
      {
        ks6(0,0)= 1.0/(EA());
        ks6(1,1)= 1.0/EIz();
        ks6(3,3)= 1.0/(EIy());
        ks6(5,5)= 1.0/(GJ());
  
        const double GA= 1.0/GAAlpha();
        ks6(2,2)= 1/GA;
        ks6(4,4)= 1/GA;
      }
    else //product of inertia NO nulo.
      {
        getSectionTangent6x6();
        ks6(0,0)= 1.0/ks6(0,0);
        const double a= ks6(1,1); const double b= ks6(1,3);
        const double c= ks6(3,3);
        const double d= 1/(a*c-b*b);
        ks6(1,1)= c/d; ks6(1,3)=ks6(3,1)= -b/d;
        ks6(3,3)= a/d;

        ks6(5,5)= 1.0/ks6(5,5);

        ks6(2,2)= 1/ks6(2,2);
        ks6(4,4)= 1/ks6(4,4);
      }

    return ks6;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties3d::getInitialFlexibility6x6(void) const
  { return getSectionFlexibility6x6(); }


//! @brief Couterclockwise rotation of the section by the angle being passed as parameter.
void XC::CrossSectionProperties3d::rotate(const double &theta)
  {
    const double &iiy= Iy();
    const double &iiz= Iz();
    const double &iiyz= Iyz();
    const double media_suma= (iiy+iiz)/2;
    const double media_resta= (iiy-iiz)/2;
    const double dosTheta= 2*theta;
    const double cos2theta= cos(dosTheta);
    const double sin2theta= sin(dosTheta);
    setIy(media_suma+media_resta*cos2theta-iiyz*sin2theta);
    setIz(media_suma-media_resta*cos2theta+iiyz*sin2theta);
    setIyz(media_resta*sin2theta+iiyz*cos2theta);
  }


int XC::CrossSectionProperties3d::setParameter(const std::vector<std::string> &argv,Parameter &param,SectionForceDeformation *scc)
  {
    if(argv[0] == "Iz")
      {
        param.setValue(Iz());
        return param.addObject(3,scc);
      }
    else if(argv[0] == "Iy")
      {
        param.setValue(Iy());
        return param.addObject(4,scc);
      }
    else if(argv[0] == "J")
      {
        param.setValue(J());
        return param.addObject(6,scc);
      }
    else 
      return CrossSectionProperties2d::setParameter(argv,param,scc);
  }

int XC::CrossSectionProperties3d::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 4:
        iy= info.theDouble;
        return 0;
      case 6:
        j= info.theDouble;
        return 0;
      default:
        return CrossSectionProperties2d::updateParameter(parameterID,info);
      }
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::CrossSectionProperties3d::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::CrossSectionProperties3d::sendData(CommParameters &cp)
  {
    int res= CrossSectionProperties2d::sendData(cp);
    res+= cp.sendDoubles(iy,iyz,j,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::CrossSectionProperties3d::recvData(const CommParameters &cp)
  {
    int res= CrossSectionProperties2d::recvData(cp); 
    res+= cp.receiveDoubles(iy,iyz,j,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::CrossSectionProperties3d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::CrossSectionProperties3d::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::CrossSectionProperties3d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      {}
    else
      {
        s << "CrossSectionProperties3d, E: " << E() << std::endl;
        s << "\t A: " << A() << std::endl;
        s << "\t Iz: " << Iz() << std::endl;
        s << "\t Iy: " << Iy() << std::endl;
        s << "\t Iyz: " << Iyz() << std::endl;
        s << "\t J: " << J();
      }
  }

