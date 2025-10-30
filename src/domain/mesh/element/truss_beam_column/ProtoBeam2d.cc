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
//ProtoBeam2d.cc

#include "ProtoBeam2d.h"

#include "utility/actor/actor/MovableVector.h"
#include "material/section/elastic_section/BaseElasticSection2d.h"
#include "utility/geom/pos_vec/Vector2d.h"
#include "material/section/repres/CrossSectionProperties2d.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"

double proto_beam_2d_extrapolation_data[4]= {1.0,0.0, 0.0,1.0};

XC::Matrix XC::ProtoBeam2d::extrapolation_matrix(proto_beam_2d_extrapolation_data,2,2); // identity matrix.

//! @brief Set values to section mass properties.
void XC::ProtoBeam2d::set_material(const Material *m)
  {
    if(m)
      {
        const BaseElasticSection2d *scc= dynamic_cast<const BaseElasticSection2d *>(m);
        if(scc)
	  {
            physicalProperties.set(scc->getCrossSectionProperties());
	    physicalProperties.copyPropsFrom(scc);
	  }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; material type: '" << m->getClassName()
	              << "' is not valid.\n";
          }
      }
    else
      if(verbosity>0)
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; pointer to material is null." << std::endl;
  }

//! @brief Default constructor.
//! @param tag: element identifier.
//! @param class_tag: element class identifier.
//! @param m: element material.
XC::ProtoBeam2d::ProtoBeam2d(int tag,int class_tag,const Material *m)
  :BeamColumn(tag,class_tag,0,0), physicalProperties(1)
  { set_material(m); }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param class_tag: element class identifier.
//! @param a: section area.
//! @param e: elastic modulus.
//! @param i: moment of inertia.
//! @param Nd1: identifier of the first node.
//! @parma Nd2: idenfifier of the second node.
XC::ProtoBeam2d::ProtoBeam2d(int tag, int class_tag, double a, double e, double i, int Nd1, int Nd2)
  :BeamColumn(tag,class_tag,Nd1,Nd2), physicalProperties(1)
  { setSectionProperties(CrossSectionProperties2d(e,a,i)); }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param class_tag: element class identifier.
//! @param a: section area.
//! @param alpha: ratio between section area and section shear area.
//! @param e: elastic modulus.
//! @param g: shear modulus.
//! @param i: moment of inertia.
//! @param Nd1: identifier of the first node.
//! @param Nd2: idenfifier of the second node.
XC::ProtoBeam2d::ProtoBeam2d(int tag, int class_tag, double a, double alpha, double e, double g, double i, int Nd1, int Nd2)
  :BeamColumn(tag,class_tag,Nd1,Nd2), physicalProperties(1)
  { setSectionProperties(CrossSectionProperties2d(e,a,i,g,alpha)); }

//! @brief Return the number of degrees of freedom of the element.
int XC::ProtoBeam2d::getNumDOF(void) const
  { return 6; }

//! @brief Returns the value of the persistent (does not get wiped out by
//! zeroLoad) initial deformation of the section.
const XC::Vector &XC::ProtoBeam2d::getPersistentInitialSectionDeformation(void) const
  { return persistentInitialDeformation; }

//! @brief Increments the persistent (does not get wiped out by zeroLoad)
//! initial deformation of the section. It's used to store the deformation
//! of the material during the periods in which their elements are deactivated
//! (see for example XC::BeamColumnWithSectionFD::alive().
void XC::ProtoBeam2d::incrementPersistentInitialDeformationWithCurrentDeformation(void)
  {
    const Vector &v= getSectionDeformation();
    if(persistentInitialDeformation.isEmpty()) // Not yet initialized.
      persistentInitialDeformation= v; // not yet initialized.
    else
      persistentInitialDeformation+= v; // increment the current value.
  }

//! @brief Return section properties.
const XC::CrossSectionProperties2d &XC::ProtoBeam2d::getSectionProperties(void) const
  { return (*physicalProperties[0]).getCrossSectionProperties(); }

//! @brief Return section properties.
XC::CrossSectionProperties2d &XC::ProtoBeam2d::getSectionProperties(void)
  { return (*physicalProperties[0]).getCrossSectionProperties(); }

//! @brief Set section properties.
void XC::ProtoBeam2d::setSectionProperties(const CrossSectionProperties2d &csp)
  {
    physicalProperties.set(0,csp);
  }

//! @brief Returns a pointer to the i-th section of the element.
const XC::PrismaticBarCrossSection *XC::ProtoBeam2d::getSectionPtr(const size_t &i) const
  {
    const PrismaticBarCrossSection *retval(nullptr);
    if(physicalProperties.size()>0)
      retval= physicalProperties[i];
    return retval;
  }

//! @brief Set the element material.
void XC::ProtoBeam2d::setMaterial(const std::string &matName)
  {
    const Material *ptr_mat= get_material_ptr(matName);
    if(ptr_mat)
      { set_material(ptr_mat); }
  }

//! @brief Return linear density.
double XC::ProtoBeam2d::getRho(void) const
  { return getSectionProperties().getLinearRho(); }

//! @brief Sets linear density.
void XC::ProtoBeam2d::setRho(const double &r)
  { getSectionProperties().setLinearRho(r); }

//! @brief Return linear density.
double XC::ProtoBeam2d::getLinearRho(void) const
  { return getSectionProperties().getLinearRho(); }

//! @brief Send members through the communicator argument.
int XC::ProtoBeam2d::sendData(Communicator &comm)
  {
    DbTagData &dt= getDbTagData();
    int res= BeamColumn::sendData(comm);
    res+= comm.sendMovable(physicalProperties,dt,CommMetaData(7));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ProtoBeam2d::recvData(const Communicator &comm)
  {
    int res= BeamColumn::recvData(comm);
    res+= comm.receiveMovable(physicalProperties,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Returns the direction vector of element strong axis
//! expressed in the local coordinate system.
XC::Vector XC::ProtoBeam2d::getVDirStrongAxisLocalCoord(void) const
  {
    const Vector2d sectionStrongAxis= getSectionProperties().getVDirStrongAxis();
    Vector eF(3); eF(0)= 0.0; eF(1)= sectionStrongAxis.x(); eF(2)= sectionStrongAxis.y();
    return eF;
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the local coordinate system.
XC::Vector XC::ProtoBeam2d::getVDirWeakAxisLocalCoord(void) const
  {
    const Vector2d sectionWeakAxis= getSectionProperties().getVDirWeakAxis();
    Vector eD(3); eD(0)= 0.0; eD(1)= sectionWeakAxis.x(); eD(2)= sectionWeakAxis.y();
    return eD;
  }

//! @brief Compute the current strain.
const XC::Vector &XC::ProtoBeam2d::computeCurrentStrain(void) const
  {
    static Vector retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; not implemented yet.\n";
    return retval;
  }

//! @brief Set the element initial strain. The given vector is already
//! expressed as the section "sees" the deformations.
int XC::ProtoBeam2d::setInitialSectionDeformation(const Vector &def)
  {
    (*physicalProperties[0]).setInitialSectionDeformation(def);
    return 0;
  }

//! @brief Return the section generalized strain.
const XC::Vector &XC::ProtoBeam2d::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= computeCurrentStrain();
    if(!persistentInitialDeformation.isEmpty())  // Have being inactive.
      retval-= persistentInitialDeformation;
    const Vector &e0= getInitialSectionDeformation();
    // retval(0)= (dx2-dx1)/L: Element elongation/L.
    // retval(1)= (dy1-dy2)/L: Rotation about z/L.
    // retval(2)= (dy1-dy2)/L: Rotation about z/L.
    retval(0)-= e0(0);
    retval(1)-= e0(1);
    retval(2)+= e0(1); // reverse sign LP 02/08/2024.
    return retval;
  }

//! @brief Update element state.
int XC::ProtoBeam2d::update(void)
  {
    int retval= BeamColumn::update();
    // determine the current strain given trial displacements at nodes
    const Vector strain= this->computeCurrentStrain();
    const XC::CrdTransf *crdTransf= this->getCoordTransf();
    const double oneOverL= 1.0/crdTransf->getInitialLength();  
    ElasticSection2d *section= physicalProperties[0];
    const int order= section->getOrder();
    const ResponseId &code= section->getResponseType();
    Vector localStrain(order);
    for(int i= 0; i<order;i++)
      {
	switch(code(i))
	  {
	    case SECTION_RESPONSE_P:
	      localStrain(i)= strain(0);
	      break;
	    case SECTION_RESPONSE_MZ:
	      localStrain(i)= (strain(2)-strain(1));
	      break;
	    case SECTION_RESPONSE_VY:
	      localStrain(i) = oneOverL*(strain(2)-strain(1));
	      break;
	    default:
	      localStrain(i) = 0.0;
	      break;
	  }
      }
    retval+= section->setTrialSectionDeformation(localStrain);
    return retval;
  }

//! @brief Commit the element state.
int XC::ProtoBeam2d::commitState(void)
  {
    int retVal = BeamColumn::commitState();
    // call element commitState to do any base class stuff
    if(retVal != 0)
      { std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class."; }
    retVal+= physicalProperties.commitState();
    return retVal;
  }

//! @brief Revert the element to the its last commited state.
int XC::ProtoBeam2d::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    //int retval= BeamColumn::revertToLastCommit(); // pure virtual.
    int retval= physicalProperties.revertToLastCommit();
    return retval;
  }

//! @brief Revert the the element to the its initial state.
int XC::ProtoBeam2d::revertToStart()
  {
    int retval= BeamColumn::revertToStart();
    retval+= physicalProperties.revertToStart();
    return retval;
  }

//! @brief Returns the angle between element strong axis
//! and local XZ plane.
double XC::ProtoBeam2d::getStrongAxisAngle(void) const
  {
    Vector eF= getVDirStrongAxisLocalCoord();
    return atan2(eF(2),eF(1));
  }

//! @brief Returns the angle between element weak axis
//! and local XZ plane.
double XC::ProtoBeam2d::getWeakAxisAngle(void) const
  {
    Vector eD= getVDirWeakAxisLocalCoord();
    return atan2(eD(2),eD(1));
  }

//! @brief Reactivates the element.
void XC::ProtoBeam2d::alive(void)
  {
    if(isDead())
      {
	// Store the current deformation.
        this->incrementPersistentInitialDeformationWithCurrentDeformation();
	BeamColumn::alive(); // Not dead anymore.
      }
  }

//! @brief Removes the element loads.
void XC::ProtoBeam2d::zeroLoad(void)
  {
    BeamColumn::zeroLoad();
    (*physicalProperties[0]).zeroInitialSectionDeformation(); //Removes also initial strains.
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
XC::ElementalLoad *XC::ProtoBeam2d::createInertiaLoad(const Vector &accel)
  {
    const Vector load= -accel*getLinearRho();
    return this->vector2dUniformLoadGlobal(load);
  }

//! @brief Return a python list with the values of the argument property
//! at element nodes.
//!
//! When the property requested its located at the integration point this
//! function is responsible of the extrapolation of values from
//! Gauss points to nodes.
//! 
//! @param code: identifier of the requested value.
//! @param silent: if true, don't complaint about non-existent property.
boost::python::list XC::ProtoBeam2d::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    const int nNodes= getNumExternalNodes();
    if(code=="rho")
      {
	const double r= getRho();
	for(int i= 0;i<nNodes;i++)
	  retval.append(r);
      }
    else if(code=="linearRho")
      {
	const double r= getLinearRho();
	for(int i= 0;i<nNodes;i++)
	  retval.append(r);
      }
    else if(code=="strain")
      {
	std::clog << getClassName() << "::" << __FUNCTION__
	          << "; the strain value correspond to the"
	          << " average on the element." << std::endl;
	const Vector strain= getSectionDeformation();
	for(int i= 0;i<nNodes;i++)
	  retval.append(strain);
      }
    else if(code=="initial_strain")
      {
	const Vector initialStrain= getInitialSectionDeformation();
	for(int i= 0;i<nNodes;i++)
	  retval.append(initialStrain);
      }
    else if(code.rfind("e0")==0) // initial strain component (translation)
      {
	const Vector initialStrain= getInitialSectionDeformation();
	const char axis= code.back();
	double value= 0.0;
	if(axis=='x')
	  value= initialStrain[0];
	else if(axis=='y')
	  value= initialStrain[1];
	else if(axis=='z') // 2D element no shear on Z.
	  value= 0.0;
	if((axis=='x') || (axis=='y') || (axis=='z'))
	  for(int i= 0;i<nNodes;i++)
	    retval.append(value);
      }
    else if(code.rfind("k0")==0) // initial strain component (rotation)
      {
	const Vector initialStrain= getInitialSectionDeformation();
	const char axis= code.back();
	double value= 0.0;
	if(axis=='x') // 2D element no torsion.
	  value= 0.0;
	else if(axis=='y') // 2D element no bending on Y.
	  value= 0.0;
	else if(axis=='z')
	  value= initialStrain[2];
	if((axis=='x') || (axis=='y') || (axis=='z'))
	  for(int i= 0;i<nNodes;i++)
	    retval.append(value);
      }
    else
      retval= BeamColumn::getValuesAtNodes(code, silent); 
    return retval;
  }
