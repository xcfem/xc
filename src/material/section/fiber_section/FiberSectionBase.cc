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
//FiberSectionBase.cc

#include "FiberSectionBase.h"
#include "utility/matrix/Vector.h"



#include "material/section/ResponseId.h"
#include "material/section/repres/section/FiberSectionRepr.h"
#include "material/section/repres/geom_section/GeomSection.h"
#include "material/section/interaction_diagram/DeformationPlane.h"
#include "material/section/interaction_diagram/NormalStressStrengthParameters.h"
#include "material/section/interaction_diagram/InteractionDiagramData.h"
#include "material/section/interaction_diagram/ComputePivots.h"
#include "material/section/interaction_diagram/Pivots.h"
#include "material/section/interaction_diagram/InteractionDiagram.h"
#include "material/section/interaction_diagram/InteractionDiagram2d.h"
#include "material/section/interaction_diagram/NMPointCloud.h"
#include "material/section/interaction_diagram/NMyMzPointCloud.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Triang3dMesh.h"
#include "xc_utils/src/geom/d3/ConvexHull3d.h"
#include "xc_utils/src/geom/d2/ConvexHull2d.h"
#include "xc_utils/src/geom/d2/HalfPlane2d.h"
#include "xc_utils/src/geom/d2/2d_polygons/polygon2d_bool_op.h"
#include "xc_utils/src/geom/d1/Ray2d.h"
#include "xc_utils/src/geom/d1/Segment2d.h"


//! @brief Constructor.
XC::FiberSectionBase::FiberSectionBase(int tag,int num,int classTag,int dim,MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(tag, classTag,mat_ldr), eTrial(dim), eInic(dim), eCommit(dim), kr(dim), fibers(num), fiberTag(num+1), section_repres(nullptr)
  {}

//! @brief Constructor.
XC::FiberSectionBase::FiberSectionBase(int tag, int classTag,int dim,MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(tag, classTag,mat_ldr), eTrial(dim), eInic(dim), eCommit(dim), kr(dim), fibers(0), fiberTag(0), section_repres(nullptr)
  {}

// constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSectionBase::FiberSectionBase(int classTag,int dim,MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(0, classTag,mat_ldr), eTrial(dim), eInic(dim), eCommit(dim), kr(dim),fibers(0), fiberTag(0), section_repres(nullptr)
  {}

//! @brief Copy constructor.
XC::FiberSectionBase::FiberSectionBase(const FiberSectionBase &other)
  : PrismaticBarCrossSection(other), eTrial(other.eTrial), eInic(other.eInic), eCommit(other.eCommit), kr(other.kr), fibers(other.fibers), fiberTag(other.fiberTag), section_repres(nullptr)
  {
    if(other.section_repres)
      section_repres= other.section_repres->getCopy();
  }

//! @brief Assignment operator.
XC::FiberSectionBase &XC::FiberSectionBase::operator=(const FiberSectionBase &other)
  {
    PrismaticBarCrossSection::operator=(other);
    eTrial= other.eTrial;
    eInic= other.eInic;
    eCommit= other.eCommit;
    kr= other.kr;
    fibers= other.fibers;
    fiberTag= other.fiberTag;
    if(other.section_repres)
      section_repres= other.section_repres->getCopy();
    else
      {
        delete section_repres;
        section_repres= nullptr;
      }
    return *this;
  }

//! @brief Send data through the channel being passed as parameter.
int XC::FiberSectionBase::sendData(Communicator &comm)
  {
    int res= PrismaticBarCrossSection::sendData(comm);
    res+= comm.sendVector(eTrial,getDbTagData(),CommMetaData(5));
    res+= comm.sendVector(eInic,getDbTagData(),CommMetaData(6));
    res+= comm.sendVector(eCommit,getDbTagData(),CommMetaData(7));
    res+= comm.sendMovable(kr,getDbTagData(),CommMetaData(8));
    res+= comm.sendMovable(fibers,getDbTagData(),CommMetaData(9));
    res+= comm.sendInt(fiberTag,getDbTagData(),CommMetaData(10));
    res+= comm.sendMovable(fiber_sets,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::FiberSectionBase::recvData(const Communicator &comm)
  {    
    int res= PrismaticBarCrossSection::recvData(comm);
    res+= comm.receiveVector(eTrial,getDbTagData(),CommMetaData(5));
    res+= comm.receiveVector(eInic,getDbTagData(),CommMetaData(6));
    res+= comm.receiveVector(eCommit,getDbTagData(),CommMetaData(7));
    res+= comm.receiveMovable(kr,getDbTagData(),CommMetaData(8));
    res+= comm.receiveMovable(fibers,getDbTagData(),CommMetaData(9));
    res+= comm.receiveInt(fiberTag,getDbTagData(),CommMetaData(10));
    res+= comm.receiveMovable(fiber_sets,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Creates a new fiber section representation.
void XC::FiberSectionBase::setup_repres(void)
  {
    if(!section_repres)
      {
        section_repres= new FiberSectionRepr(0,getMaterialHandler());
        section_repres->set_owner(this);
      }
  }

//! @brief Returns fiber section representation.
XC::FiberSectionRepr *XC::FiberSectionBase::getFiberSectionRepr(void)
  {
    setup_repres();
    return section_repres;
  }

//! @brief Creare a new fiber set.
void XC::FiberSectionBase::create_fiber_set(const std::string &nmb)
  { fiber_sets.create_fiber_set(nmb); }

//! @brief Creates a fiber set which name is being passed as parameter.
XC::FiberSectionBase::fiber_set_iterator XC::FiberSectionBase::get_fiber_set(const std::string &set_name)
  { return fiber_sets.get_fiber_set(set_name); }


//! @brief Creates a fiber set which material has the tag being passed as parameter.
XC::FiberSectionBase::fiber_set_iterator XC::FiberSectionBase::sel_mat_tag(const std::string &set_name,const int &matTag)
  {
    fiber_set_iterator i= get_fiber_set(set_name);
    fibers.SelMatTag(matTag,(*i).second);
    return i;
  }

//! @brief Creates a fiber set with those that belongs to the set with the name set_name_org, and have the material indetified with the tag being passed as parameter.
//! @param set_name: new set name.
//! @param set_name_org: set that contains the fibers.
//! @param matTag: material tag.
XC::FiberSectionBase::fiber_set_iterator XC::FiberSectionBase::resel_mat_tag(const std::string &set_name,const std::string &set_name_org,const int &matTag)
  { return fiber_sets.resel_mat_tag(set_name,set_name_org,matTag); }
 

//! @brief Destructor:
XC::FiberSectionBase::~FiberSectionBase(void)
  {
    if(section_repres)
      delete section_repres;
    section_repres= nullptr;
  }


//! @brief Add a fiber to the section.
XC::Fiber *XC::FiberSectionBase::addFiber(const std::string &nmbMat,const double &area,const Vector &coo)
  {
    fiberTag++;
    return addFiber(fiberTag,*getMaterialHandler(),nmbMat,area,coo);
  }


//! @brief Sets generalized initial strains values.
int XC::FiberSectionBase::setInitialSectionDeformation(const Vector &deforms)
  {
    eInic= deforms;
    return 0;
  }

//! @brief Sets generalized trial strains values.
int XC::FiberSectionBase::setTrialSectionDeformation(const Vector &deforms)
  {
    eTrial= deforms;
    return 0;
  }

//! @brief Returns material's trial generalized strain.
const XC::Vector &XC::FiberSectionBase::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= eTrial-eInic;
    return retval;
  }

//! @brief Returns a const pointer to section geometry.
const XC::GeomSection *XC::FiberSectionBase::getGeomSection(void) const
  {
    const GeomSection *retval= nullptr;
    if(section_repres)
      {
        retval= section_repres->getGeom();
        if(!retval)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; section geometry not defined."
                    << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fiber section representation not defined."
                << std::endl;
    return retval;
  }

//! @brief Returns a pointer to section geometry.
XC::GeomSection *XC::FiberSectionBase::getGeomSection(void)
  {
    FiberSectionBase *this_no_const= const_cast<XC::FiberSectionBase *>(this);
    return this_no_const->getGeomSection();
  }

//! @brief Returns cross section contour.
Polygon2d XC::FiberSectionBase::getRegionsContour(void) const
  {
    Polygon2d retval;
    const GeomSection *geom= getGeomSection();
    if(geom)
      retval= geom->getRegionsContour();
    return retval;
  }

//! @brief Returns current section lever arm from the position of neutral axis.
double XC::FiberSectionBase::getLeverArm(void) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      retval= geom->getLeverArm(getBendingPlaneTrace());
    return retval;
  }

//! @brief Returns section depth from the line being passed as parameter
//! to the most compressed fiber.
double XC::FiberSectionBase::getCompressedZoneDepth(const Line2d &r) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      {
        const HalfPlane2d comp= getCompressedHalfPlane(r);
        if(comp.exists())
          retval= geom->getCompressedZoneDepth(comp);
        else
          {
            retval= NAN;
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; can't get the compressed half-plane."
                      << std::endl;
	  }
      }
    return retval;
  }

//! @brief Returns section depth from the neutral axis to the
//! most compressed fiber.
double XC::FiberSectionBase::getCompressedZoneDepth(void) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      {
        const HalfPlane2d comp= getCompressedHalfPlane();
        if(comp.exists())
          retval= geom->getCompressedZoneDepth(comp);
        else
          {
            retval= NAN;
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; can't get the compressed half-plane."
                      << std::endl;
          }
       }
    return retval;
  }

//! @brief Returns section depth from the neutral axis to the
//! most tensioned fiber.
double XC::FiberSectionBase::getTensionedZoneDepth(void) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      {
        const HalfPlane2d comp= getCompressedHalfPlane();
        if(comp.exists())
          retval= geom->getTensionedZoneDepth(comp);
        else //Full section is in tension.
          retval= geom->getLeverArm(getBendingPlaneTrace());
      }
    return retval;
  }

//! @brief Returns section depth from the line being passed as parameter
//! to the most tensioned fiber.
double XC::FiberSectionBase::getTensionedZoneDepth(const Line2d &r) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      {
        const HalfPlane2d comp= getCompressedHalfPlane(r);
        if(comp.exists())
          retval= geom->getTensionedZoneDepth(comp);
        else
          {
            retval= NAN;
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; can't get the compressed half-plane."
                      << std::endl;
          }
      }
    return retval;
  }

//! @brief Returns neutral axis depth.
double XC::FiberSectionBase::getNeutralAxisDepth(void) const
  {
    return getCompressedZoneDepth();
  }

//! @brief Returns the distance from the neutral axis
//! to the point whose coordinates are being passed as parameters.
double XC::FiberSectionBase::getNeutralAxisDist(const double &y,const double &) const
  {
    double retval= 0.0;
    const Line2d fn= getNeutralAxis();
    if(fn.exists())
      retval= fn.dist(Pos2d(y,0));
    return retval;
  }

//! @brief Returns the line that limits the concrete effective area $A_{c,ef}$
//! as in article 49.2.4 from EHE-08 (hatched area in figure 49.2.4b).
//! See also figures 47.5 y 47.6 from volume II of the
//! book: "Proyecto y cálculo de estructuras de hormigón" author: J. Calavera.
Line2d XC::FiberSectionBase::getEffectiveConcreteAreaLimitLine(const double &hEfMax) const
  {
    Line2d retval;
    Line2d fn= getNeutralAxis(); //Neutral axis computed from deformation plane.
    if(!fn.exists()) //It this doesn't work.
      fn= fibers.getNeutralAxis(); //Neutral axis computed from fiber model. 
    if(fn.exists())
      {
        const double hef= getTensionedZoneDepth(); //Lever arm of cross-section tensioned zone.
        assert(!std::isnan(hef));
        if(hef<hEfMax)
          retval= fn;
        else
          {
	    //Compute a parallel line at (hef-fEfMax) distance over the tensioned zone.
            const double d= -(hef-hEfMax); //Sign changed to move over tensioned zone.
	    const Vector lav= normalize(getLeverArmVector());
            const Vector v= lav*d;
	    const Vector2d v2d= Vector2d(v[0],v[1]);
            retval= fn.Offset(v2d);
          }
      }
    return retval;
  }

//! @brief Returns the contours of the concrete effective area \f$A_{c,ef}\f$
//! the area that CAN "collaborate" with rebars to resist tension in SLS.
//! 
//! Concrete effective area as in article 49.2.4 from EHE-08 (hatched area
//! in figure 49.2.4b). See also figures 47.5 y 47.6 from volume II of the
//! book: "Proyecto y cálculo de estructuras de hormigón" author: J. Calavera.
std::list<Polygon2d> XC::FiberSectionBase::getGrossEffectiveConcreteAreaContour(const double &hEfMax) const
  {
    std::list<Polygon2d> retval;
    Polygon2d contour= getRegionsContour(); //Computes cross-section contour.

    const double epsMin= fibers.getStrainMin(); //Minimal strain.
    const double epsMax= fibers.getStrainMax(); //Maximal strain.
    if(epsMin>0) //Full section is in tension.
      retval.push_back(contour); //XXX Is this an error?.
    else if(epsMax>0) //Some (epsMin<=0) but not all the section is compressed.
      {
        if(hEfMax>1e-6)
          {
	    // Compute the line that limits the effective area contour
            const Line2d limit= getEffectiveConcreteAreaLimitLine(hEfMax);
            if(limit.exists())
              {
		if(contour.Overlap(limit))
		  {
		    // Compute the half plane that is in tension.
		    const HalfPlane2d tensionedArea= getTensionedHalfPlane(limit);
		    if(tensionedArea.exists())
		      {
			retval= contour.getIntersection(tensionedArea);
			if(retval.empty())
			  std::cerr << getClassName() << "::" << __FUNCTION__
				    << "; intersection of section contour: "
				    << contour << " with tensioned area: "
				    << tensionedArea << std::endl;
		      }
		    else
		      std::cerr << getClassName() << "::" << __FUNCTION__
				<< "; can't compute tensioned area."
				<< " epsMin= " << epsMin << " epsMax= " << epsMax
				<< std::endl;
		  }
		else
		  std::cerr << getClassName() << "::" << __FUNCTION__
			    << "; effective concrete area limit line: "
			    << limit << " outside section contour: " << contour
			    << ";  hEfMax= " << hEfMax
			    << std::endl;
              }
            else
              retval.push_back(contour);
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; maximum effective height is zero." << std::endl;
      }
    if(retval.empty())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; can't get contour of gross effective concrete area."
	          << " epsMin= " << epsMin << " epsMax= " << epsMax
                  << std::endl;
      }
    return retval;
  }

double XC::FiberSectionBase::getGrossEffectiveConcreteArea(const double &hEfMax) const
  {
    double retval= 0.0;
    std::list<Polygon2d> tmp= getGrossEffectiveConcreteAreaContour(hEfMax);
    if(!tmp.empty())
      retval= area(tmp.begin(),tmp.end());
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; error when computing contour of"
                  << " gross effective concrete area."
                  << std::endl;
    return retval;
  }

//! @brief Returns the sum of the effective areas of rebars in tension.
double XC::FiberSectionBase::getNetEffectiveConcreteArea(const double &hEfMax,const std::string &rebarSetName,const double &factor) const
  {
    double retval= 0.0;
    std::list<Polygon2d> grossEffectiveConcreteAreaContour= getGrossEffectiveConcreteAreaContour(hEfMax);
    if(!grossEffectiveConcreteAreaContour.empty())
      {
	//Iterate over the rebar set.
        fiber_set_const_iterator i= fiber_sets.find(rebarSetName);
        if(i!=fiber_sets.end())
          {
            const FiberPtrDeque &rebars= (*i).second; //Rebar family.
            retval+= rebars.computeFibersEffectiveConcreteArea(grossEffectiveConcreteAreaContour,factor);
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; fiber set: "
                    << rebarSetName << " not found." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; can't compute effective area." << std::endl;
    return retval;
  }

//! @brief Computes crack effective concrete areas around the fibers.
double XC::FiberSectionBase::computeFibersEffectiveConcreteArea(const double &hEfMax,const std::string &rebarSetName,const double &factor) const
  {
    double retval= 0;
    std::list<Polygon2d> grossEffectiveConcreteAreaContour= getGrossEffectiveConcreteAreaContour(hEfMax);
    if(!grossEffectiveConcreteAreaContour.empty())
      {
        fiber_set_const_iterator i= fiber_sets.find(rebarSetName);
        if(i!=fiber_sets.end())
          {
            const FiberPtrDeque &rebars= (*i).second; //Rebars.
            retval= rebars.computeFibersEffectiveConcreteArea(grossEffectiveConcreteAreaContour,factor);
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; fiber set: "
                    << rebarSetName << " not found." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; can't compute effective area." << std::endl;
    return retval;
  }

//! @brief Computes concrete cover of the fibers.
void XC::FiberSectionBase::computeCovers(const std::string &rebarSetName) const
  {
    fiber_set_const_iterator i= fiber_sets.find(rebarSetName);
    if(i!=fiber_sets.end())
      {
        const FiberPtrDeque &rebars= (*i).second; //Rebars.
        const GeomSection *geom= getGeomSection();
        if(geom)
          rebars.computeCovers(*geom);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; fiber set: "
                << rebarSetName << " not found." << std::endl;
  }

//! @brief Computes spacing of the fibers.
void XC::FiberSectionBase::computeSpacement(const std::string &rebarSetName) const
  {
    fiber_set_const_iterator i= fiber_sets.find(rebarSetName);
    if(i!=fiber_sets.end())
      {
        const FiberPtrDeque &rebars= (*i).second; //Rebars.
        rebars.computeSpacement();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; fiber set: "
                << rebarSetName << " not found." << std::endl;
  }

//! @brief Returns the signed distance from the neutral axis
//! to the point whose coordinates are being passed as parameters.
double XC::FiberSectionBase::get_dist_to_neutral_axis(const double &y,const double &z) const
  {
    double retval= 0;
    const HalfPlane2d comp= getCompressedHalfPlane();
    if(comp.exists())
      retval= comp.DistSigno(Pos2d(y,z));
    else
      retval= NAN;
    return retval;
  }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::FiberSectionBase::getSectionTangent(void) const
  { return kr.Stiffness(); }

//! @brief Returns the resultant of section stresses.
const XC::Vector &XC::FiberSectionBase::getStressResultant(void) const
  { return kr.getResultant(); }

//! @brief Returns i-th component of the stress resultant.
double XC::FiberSectionBase::getStressResultant(const int &i) const
  { return PrismaticBarCrossSection::getStressResultant(i); }

//! @brief Commits state.
int XC::FiberSectionBase::commitState(void)
  {
    int err= fibers.commitState();
    eCommit= eTrial;
    return err;
  }

//! @brief Returns to the last committed state.
int XC::FiberSectionBase::revertToLastCommit(void)
  {
    // Last committed section deformations
    eTrial= eCommit;
    return 0;
  }

//! @brief Returns to the initial state.
int XC::FiberSectionBase::revertToStart(void)
  {
    eCommit.Zero();
    return 0;
  }

//! @brief Returns the point (N,My,Mz) that corresponds to the resultant
//! of normal stresses in the section.
Pos3d XC::FiberSectionBase::Esf2Pos3d(void) const
  { return Pos3d(getStressResultant(XC::SECTION_RESPONSE_P),getStressResultant(XC::SECTION_RESPONSE_MY),getStressResultant(XC::SECTION_RESPONSE_MZ)); }

//! @brief Returns the section normal stresses resultant for the
//! deformation plane being passed as parameter.
Pos3d XC::FiberSectionBase::getNMyMz(const DeformationPlane &def)
  {
    setTrialDeformationPlane(def);
    return Esf2Pos3d();
  }

//    ^ z
//    |   /
//    |  /
//    | / theta
//    +------->y
//
//! @brief Returns the points that define the interaction diagram
//! of the section for an angle \f$\theta\f$ with respect to the z axis.
void XC::FiberSectionBase::getInteractionDiagramPointsForTheta(NMyMzPointCloud &lista_esfuerzos,const InteractionDiagramData &diag_data,const FiberPtrDeque &fsC,const FiberPtrDeque &fsS,const double &theta)
  {
    ComputePivots cp(diag_data.getPivotsUltimateStrains(),fibers,fsC,fsS,theta);
    Pivots pivots(cp);
    if(pivots.Ok())
      {
        //Domains 1 and 2
        Pos3d P1= pivots.getAPivot(); //Pivot.
        Pos3d P2= P1+100.0*cp.GetK(); //Bending around local z axis.
        Pos3d P3;
        DeformationPlane def;
        const double inc_eps_B= diag_data.getIncEps();
        const double eps_agot_A= diag_data.getPivotsUltimateStrains().getUltimateStrainAPivot();
        const double eps_agot_B= diag_data.getPivotsUltimateStrains().getUltimateStrainBPivot();
        for(double e= eps_agot_A;e>=eps_agot_B;e-=inc_eps_B)
          {
            P3= pivots.getBPoint(e);
            lista_esfuerzos.append(getNMyMz(DeformationPlane(P1,P2,P3)));
          }
        //Domains 3 and 4
        P1= pivots.getBPivot(); //Pivot
        P2= P1+100.0*cp.GetK(); 
        const double inc_eps_A= diag_data.getIncEps();
        for(double e= eps_agot_A;e>=0.0;e-=inc_eps_A)
          {
            P3= pivots.getAPoint(e);
            lista_esfuerzos.append(getNMyMz(DeformationPlane(P1,P2,P3)));
          }
        //Domain 4a
        //Compute strain in D when the pivot point is B
        //and strain in A is zero (domain 4a starts).
        const Pos3d PA0= pivots.getAPoint(0.0); //Zero strain at rebars.
        const DeformationPlane def_lim_4a= DeformationPlane(P1,P2,PA0);
        const Pos2d PD= pivots.getPointDPosition();
        const double eps_D4a= def_lim_4a.Strain(PD);
        const double recorr_eps_D4a= eps_D4a;
        if(recorr_eps_D4a>(eps_agot_A/200.0)) //Si el recorrido es positivo y "apreciable"
          {
            const double inc_eps_D4a= diag_data.getIncEps(); //recorr_eps_D4a/n_div_domain; //Analysis intervals.
            for(double e= eps_D4a;e>=0.0;e-=inc_eps_D4a)
              {
                P3= pivots.getDPoint(e);
                lista_esfuerzos.append(getNMyMz(DeformationPlane(P1,P2,P3)));
              }
          }
        //Domain 5
        P1= pivots.getCPivot(); //Pivot
        P2= P1+100.0*cp.GetK(); 
        const double eps_agot_C= diag_data.getPivotsUltimateStrains().getUltimateStrainCPivot();
        const double inc_eps_D= diag_data.getIncEps();
        for(double e= 0.0;e>=eps_agot_C;e-=inc_eps_D)
          {
            P3= pivots.getDPoint(e);
            lista_esfuerzos.append(getNMyMz(DeformationPlane(P1,P2,P3)));
          }
      }
  }

//! @brief Returns the points that define the interaction diagram
//! on the plane defined by the \f$\theta\f$ angle being passed as parameter.
const XC::NMPointCloud &XC::FiberSectionBase::getInteractionDiagramPointsForPlane(const InteractionDiagramData &diag_data, const double &theta)
  {
    static NMPointCloud retval;
    retval.clear();
    retval.setThreshold(diag_data.getThreshold());
    const FiberPtrDeque &fsC= sel_mat_tag(diag_data.getConcreteSetName(),diag_data.getConcreteTag())->second;
    if(fsC.empty())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fibers for concrete material, identified by tag: "
		<< diag_data.getConcreteTag()
                << ", not found." << std::endl;
    const FiberPtrDeque &fsS= sel_mat_tag(diag_data.getRebarSetName(),diag_data.getReinforcementTag())->second;
    if(fsS.empty())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fibers for steel material, identified by tag: "
		<< diag_data.getReinforcementTag()
                << ", not found." << std::endl;
    if(!fsC.empty() && !fsS.empty())
      {
        static NMyMzPointCloud tmp;
        tmp.clear();
        tmp.setThreshold(diag_data.getThreshold());
        getInteractionDiagramPointsForTheta(tmp,diag_data,fsC,fsS,theta);
        getInteractionDiagramPointsForTheta(tmp,diag_data,fsC,fsS,theta+M_PI); //theta+M_PI
        retval= tmp.getNM(theta);
        revertToStart();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; can't compute interaction diagram." << std::endl;
    return retval;
  }

//! @brief Returns the points that define the interaction diagram of the section.
const XC::NMyMzPointCloud &XC::FiberSectionBase::getInteractionDiagramPoints(const InteractionDiagramData &diag_data)
  {
    static NMyMzPointCloud lista_esfuerzos;
    lista_esfuerzos.clear();
    lista_esfuerzos.setThreshold(diag_data.getThreshold());
    const FiberPtrDeque &fsC= sel_mat_tag(diag_data.getConcreteSetName(),diag_data.getConcreteTag())->second;
    if(fsC.empty())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fibers for concrete material, identified by tag: "
		<< diag_data.getConcreteTag()
                << ", not found." << std::endl;
    const FiberPtrDeque &fsS= sel_mat_tag(diag_data.getRebarSetName(),diag_data.getReinforcementTag())->second;
    if(fsS.empty())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fibers for steel material, identified by tag: "
		<< diag_data.getReinforcementTag()
                << ", not found." << std::endl;
    if(!fsC.empty() && !fsS.empty())
      {
        for(double theta= 0.0;theta<2*M_PI;theta+=diag_data.getIncTheta())
          getInteractionDiagramPointsForTheta(lista_esfuerzos,diag_data,fsC,fsS,theta);
        revertToStart();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; can't compute interaction diagram." << std::endl;
    return lista_esfuerzos;
  }

//! @brief Returns the interaction diagram.
XC::InteractionDiagram XC::FiberSectionBase::GetInteractionDiagram(const InteractionDiagramData &diag_data)
  {
    const NMyMzPointCloud lp= getInteractionDiagramPoints(diag_data);
    InteractionDiagram retval;
    if(!lp.empty())
      {
        retval= InteractionDiagram(Pos3d(0,0,0),Triang3dMesh(get_convex_hull(lp)));
        const double error= fabs(retval.getCapacityFactor(lp).Norm2()-lp.size())/lp.size();
        if(error>0.005)
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; error in computation of interaction diagram ("
                    << error << ") seems too big." << std::endl;
      }
    return retval;
  }

//! @brief Returns the interaction diagram.
XC::InteractionDiagram2d XC::FiberSectionBase::GetInteractionDiagramForPlane(const InteractionDiagramData &diag_data, const double &theta)
  {
    const NMPointCloud lp= getInteractionDiagramPointsForPlane(diag_data, theta);
    InteractionDiagram2d retval;
    if(!lp.empty())
      {
        retval= InteractionDiagram2d(get_convex_hull2d(lp));
        const double error= fabs(retval.getCapacityFactor(lp).Norm2()-lp.size())/lp.size();
        if(error>0.005)
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; error in computation of interaction diagram ("
                    << error << ") seems too big." << std::endl;
      }
    return retval;
  }

//! @brief Returns the interaction diagram on plane N-My.
XC::InteractionDiagram2d XC::FiberSectionBase::GetNMyInteractionDiagram(const InteractionDiagramData &diag_data)
  { return GetInteractionDiagramForPlane(diag_data,M_PI/2.0); }

//! @brief Returns the interaction diagram on plane N-Mz.
XC::InteractionDiagram2d XC::FiberSectionBase::GetNMzInteractionDiagram(const InteractionDiagramData &diag_data)
  { return GetInteractionDiagramForPlane(diag_data,0.0); }

//! @brief Returns a vector from the centroid of tensions to the centroid of compressions.
XC::Vector XC::FiberSectionBase::getLeverArmVector(void) const
  { return fibers.getLeverArmVector(); }

//! @brief Returns a vector oriented from the centroid of the area in tension
//! to the most compressed fiber.
XC::Vector XC::FiberSectionBase::getVectorCantoUtil(void) const
  {
    Vector retval(2);
    const Segment2d tmp= getEffectiveDepthSegment();
    const Pos2d p1= tmp.getFromPoint();
    const Pos2d p2= tmp.getToPoint();
    retval(0)= p2.x()-p1.x();
    retval(1)= p2.y()-p1.y();
    return retval;
  }

//! @brief Returns a segment from the centroid of the tensioned area
//! to the centroid of the compressed area.
Segment2d XC::FiberSectionBase::getLeverArmSegment(void) const
  {
    Segment2d retval= fibers.getLeverArmSegment();
    if(!retval.exists())
      {
        //Lever arm as 0.8 times total depth.
        const Line2d Xaxis= getInternalForcesAxis();
        const Pos2d center_of_mass= getCenterOfMass();
        const Line2d Yaxis= Xaxis.Perpendicular(center_of_mass);
        const Polygon2d contour= getRegionsContour();
        retval= contour.Clip(Yaxis);
        Pos2d org= retval.getFromPoint()+0.1*retval.GetVector();
        Pos2d dest= retval.getToPoint()-0.1*retval.GetVector();
        retval= Segment2d(org,dest);
      }
    return retval;
  }

//! @brief Returns the segment defined by the current effective
//! depth of the section. The segment is oriented from the centroid
//! of the tensioned fibers to the most compressed fiber.
Segment2d XC::FiberSectionBase::getEffectiveDepthSegment(void) const
  {
    Segment2d retval;
    const Segment2d bm= getLeverArmSegment();
    const Ray2d sr(bm.getFromPoint(),bm.getToPoint());
    const Polygon2d contour= getRegionsContour();
    retval= contour.Clip(sr);
    return retval;
  }

//! @brief Returns the intercept of the bending plane with the
//! plane that contains the cross section.
Line2d XC::FiberSectionBase::getBendingPlaneTrace(void) const
  {
    Line2d retval= fibers.getBendingPlaneTrace();
    if(!retval.exists())
      {
        Line2d axis= getInternalForcesAxis();
        Pos2d center_of_mass= getCenterOfMass();
        retval= axis.Perpendicular(center_of_mass);
      } 
    return retval;
  }

//! @brief Returns the intercept of a plane perpendicular to the bending
//! plane through the centroid of the tensioned fibers with the
//! plane that contains the cross section.
Line2d XC::FiberSectionBase::getTensionedPlaneTrace(void) const
  {
    Line2d retval= fibers.getTensionedPlaneTrace();
    if(!retval.exists())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; intercept of the tension plane not found." << std::endl;
    return retval;
  }

//! @brief Returns the intercept of a plane perpendicular to the bending
//! plane through the centroid of the compressed fibers with the
//! plane that contains the cross section.
Line2d XC::FiberSectionBase::getCompressedPlaneTrace(void) const
  {
    Line2d retval= fibers.getCompressedPlaneTrace();
    if(!retval.exists())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; intercept of the compression plane not found."
		<< std::endl;
    return retval;
  }

//! @brief Returns the width of the section for shear checking
//! see (figure 44.2.1.a in article 44 of EHE-08).
double XC::FiberSectionBase::getAnchoMecanico(void) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      retval= geom->getAnchoMecanico(getBendingPlaneTrace());
    return retval;
  }

//! @brief Returns the compressed strut width (shear analysis).
double XC::FiberSectionBase::getCompressedStrutWidth(void) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      retval= geom->getCompressedStrutWidth(getLeverArmSegment());
    return retval;
  }

//! @brief Returns the concrete cover for the position
//! being passed as parameter.
double XC::FiberSectionBase::getCover(const Pos2d &p) const
  {
    double retval= 0.0;
    const GeomSection *geom= getGeomSection();
    if(geom)
      retval= geom->getCover(p);
    return retval;
  }

//! @brief Returns the lever arm of the section.
double XC::FiberSectionBase::getMechanicLeverArm(void) const
  { return fibers.getMechanicLeverArm(); }

//! @brief Returns the effective depth of the section.
double XC::FiberSectionBase::getEffectiveDepth(void) const
  { return getVectorCantoUtil().Norm(); }

//! @brief Returns the section area.
double XC::FiberSectionBase::getArea(void) const
  { return fibers.getArea(); }

//! @brief Moment of inertia relative to bending axis.
double XC::FiberSectionBase::getHomogenizedI(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    const Line2d axis= getInternalForcesAxis();
    return fibers.getIHomogenizedSection(E0,axis);
  }

//! @brief Static moment relative to bending axis of area that rests over this axis.
double XC::FiberSectionBase::getSPosHomogeneizada(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    const Line2d axis= getInternalForcesAxis();
    return fibers.getSPosHomogenizedSection(E0,HalfPlane2d(axis));
  }

std::string XC::FiberSectionBase::getStrClaseEsfuerzo(const double &tol) const
  { return fibers.getStrClaseEsfuerzo(); }

int XC::FiberSectionBase::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // Initial declarations
    int retval= -1;
    const int argc= argv.size();
    if(argc>0)
      {
	// A material parameter
	if(argv[0] == "material")
	  {
	    // Get the tag of the material
	    const int paramMatTag= atoi(argv[1]);
	    // Loop over fibers to find the right material(s)
	    std::vector<std::string> argv2(argv);
	    argv2.erase(argv2.begin(),argv2.begin()+2);
	    const int ok= fibers.setParameter(paramMatTag,argv2, param);
	    if(ok<0)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; could not set parameter. " << std::endl;
		retval= -1;
	      }
	    else
	      retval= ok;
	  }
        // Check if it belongs to the section integration
	else if(argv[0]=="integration")
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; section integration (from OpenSees) not implemented yet." << std::endl;
	    // if(sectionIntegr!=nullptr)
	    //   retval= sectionIntegr->setParameter(&argv[1], argc-1, param);
	    // else
	    //   retval= -1;
	  }
	else
	  retval= PrismaticBarCrossSection::setParameter(argv,param);
        // Section integration (from OpenSees) not implemented yet.
	// if(sectionIntegr != 0)
	//   {
	//     int ok= sectionIntegr->setParameter(argv, argc, param);
	//     if(ok != -1)
	//       retval= ok;
	//   }
      }
    return retval;
  }
