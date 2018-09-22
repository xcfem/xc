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
//PrismaticBarCrossSectionsVector.cc

#include "PrismaticBarCrossSectionsVector.h"
#include "PrismaticBarCrossSection.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "boost/any.hpp"


#include "material/section/ResponseId.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "utility/actor/actor/MovableID.h"

XC::PrismaticBarCrossSection *copy_section(const XC::SectionForceDeformation *theSec)
  {
    XC::PrismaticBarCrossSection *retval= nullptr;
    if(theSec)
      {
	XC::SectionForceDeformation *tmp= theSec->getCopy();
        if(tmp)
          {
            retval= dynamic_cast<XC::PrismaticBarCrossSection *>(tmp);
            if(!retval)
              {
                std::cerr << __FUNCTION__
			  << "; not a suitable material.\n";
                delete(tmp);
                exit(-1);
              }
          }
        else
          std::cerr << __FUNCTION__
		    << "; failed to get copy of the section.\n";
      }
    return retval;
  }

//! @brief Constructor.
XC::PrismaticBarCrossSectionsVector::PrismaticBarCrossSectionsVector(const size_t &sz)
  : std::vector<PrismaticBarCrossSection *>(sz,nullptr), MovableObject(0)
  {}

//! @brief Default constructor.
XC::PrismaticBarCrossSectionsVector::PrismaticBarCrossSectionsVector(const size_t &sz,const PrismaticBarCrossSection *matModel)
  : std::vector<PrismaticBarCrossSection *>(sz,nullptr), MovableObject(0)
  {
    if(matModel)
      {
        for(iterator i= begin();i!=end();i++)
          {
            (*i)= copy_section(matModel);
            if(!(*i))
              std::cerr<<" PrismaticBarCrossSectionsVector -- failed allocate material model pointer\n";
          }
      }
  }

//! @brief Constructor.
XC::PrismaticBarCrossSectionsVector::PrismaticBarCrossSectionsVector(const size_t &sz,const Material *m)
  : std::vector<PrismaticBarCrossSection *>(sz,nullptr), MovableObject(0)
  {
    if(m)
      setupSection(m);
  }

//! @brief Copy constructor.
XC::PrismaticBarCrossSectionsVector::PrismaticBarCrossSectionsVector(const PrismaticBarCrossSectionsVector &other)
  : CommandEntity(other), std::vector<PrismaticBarCrossSection *>(other.size(),nullptr), MovableObject(other)
  {
    const size_t sz= other.size();
    for(size_t i= 0;i<sz;i++)
      {
        if(other[i])
          {
            (*this)[i]= copy_section(other[i]);
            if(!(*this)[i])
              std::cerr << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate material model pointer\n";
          }
      }
  }

//! @brief Assignment operator.
XC::PrismaticBarCrossSectionsVector &XC::PrismaticBarCrossSectionsVector::operator=(const PrismaticBarCrossSectionsVector &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << ": assignment operator must not be called."
              << std::endl;
    return *this;
  }

XC::PrismaticBarCrossSectionsVector::~PrismaticBarCrossSectionsVector(void)
  { clearAll(); }

void XC::PrismaticBarCrossSectionsVector::setSection(const PrismaticBarCrossSection *nueva_secc)
  {
    clear_sections();
    if(nueva_secc)
      {
        for(iterator i= begin();i!=end();i++)
          {
            (*i)= copy_section(nueva_secc);
            if(!(*i))
              std::cerr<<" PrismaticBarCrossSectionsVector -- failed allocate material model pointer\n";
          }
      }
  }

void XC::PrismaticBarCrossSectionsVector::setupSection(const Material *sec)
  {
    if(!sec)
      std::cerr << "XC::PrismaticBarCrossSectionsVector::setupSection; material pointer is null." << std::endl;
    else
      {
        const PrismaticBarCrossSection *ptr= dynamic_cast<const PrismaticBarCrossSection *>(sec);
        if(!ptr)
          std::cerr << "XC::PrismaticBarCrossSectionsVector::setupSection; material type not valid." 
                    << std::endl;
        else
          setSection(ptr);
      }
  }

void XC::PrismaticBarCrossSectionsVector::setSectionCopy(size_t i,PrismaticBarCrossSection *nueva_secc)
  {
    if((*this)[i]) delete (*this)[i];
    (*this)[i]= copy_section(nueva_secc);
    if(!(*this)[i])
      std::cerr << "XC::PrismaticBarCrossSectionsVector::setSectionCopy -- failed to get copy of section.\n";
  }

void XC::PrismaticBarCrossSectionsVector::clear_sections(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        if(*i) delete (*i);
        (*i)= nullptr;
      }
  }

void XC::PrismaticBarCrossSectionsVector::clearAll(void)
  {
    clear_sections();
    std::vector<PrismaticBarCrossSection *>::clear();
    CommandEntity::clearPyProps();
  }


//! @brief Commits sections state.
int XC::PrismaticBarCrossSectionsVector::commitState(void)
  {
    int retVal= 0;

    for(iterator i=begin();i!=end();i++)
      retVal+= (*i)->commitState();
    return retVal;
  }

//! @brief Returns the sections to its last commited state.
int XC::PrismaticBarCrossSectionsVector::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i=begin();i!=end();i++)
      retVal+= (*i)->revertToLastCommit() ;
    return retVal;
  }


//! @brief Returns the sections to its initial state.
int XC::PrismaticBarCrossSectionsVector::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i=begin();i!=end();i++)
      retVal+= (*i)->revertToStart() ;
    return retVal;
  }

//! @brief Returns true if the sections have torsional stiffness.
bool XC::PrismaticBarCrossSectionsVector::isTorsion(void) const
  {
    bool isTorsion= false;
    for(const_iterator i= begin();i!=end();i++)
      {
        int order= (*i)->getOrder();
        const XC::ID &code= (*i)->getType();
        for(int j= 0;j<order;j++)
          {
            if(code(j) == SECTION_RESPONSE_T)
              isTorsion= true;
          }
      }
    return isTorsion;
  }

//! @brief Set the sections of the vector.
bool XC::PrismaticBarCrossSectionsVector::setSections(const std::vector<PrismaticBarCrossSection *> &sectionPtrs)
  {
    bool isTorsion= false;
    // get copy of the sections
    const size_t ptrsSize= sectionPtrs.size();
    const size_t nSections= size();
    if(ptrsSize<nSections)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; error: not enough sections (" << ptrsSize
	        << "), " << nSections << " needed.";
    for(size_t i= 0;i<nSections;i++)
      {
        if(!sectionPtrs[i])
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; null section pointer at " << i << std::endl;
            exit(-1);
          }  
        (*this)[i]= copy_section(sectionPtrs[i]);
        if(!(*this)[i])
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; could not create copy of section " << i << std::endl;
            exit(-1);
          }
        int order= (*this)[i]->getOrder();
        const ID &code= (*this)[i]->getType();
        for(int j= 0;j<order;j++)
          {
            if(code(j) == SECTION_RESPONSE_T)
              isTorsion = true;
          }
      }
    return isTorsion;
  }

//! @brief Zeroes initial strains.
void XC::PrismaticBarCrossSectionsVector::zeroInitialSectionDeformations(void)
  {
    const size_t nSections= size();
    for(size_t i= 0;i<nSections;i++)
      (*this)[i]->zeroInitialSectionDeformation();
  }

//! @brief Set initial strains.
void XC::PrismaticBarCrossSectionsVector::setInitialSectionDeformations(const std::vector<Vector> &vs)
  {
    const size_t nSections= std::min(size(),vs.size());
    if(vs.size()<nSections)
      std::cerr << "Error en PrismaticBarCrossSectionsVector::setInitialSectionDeformations" << std::endl;
    for(size_t i= 0;i<nSections;i++)
      (*this)[i]->setInitialSectionDeformation(vs[i]);
  }

//! @brief Set initial strains interpolating between e1 and e2.
void XC::PrismaticBarCrossSectionsVector::addInitialSectionDeformations(const BeamStrainLoad &strainLoad,const double &loadFactor,const Matrix &xi, const double &L)
  {
    const size_t numAbcisas= xi.noRows();
    const size_t nSections= std::min(size(),numAbcisas);
    if(numAbcisas<nSections)
      std::cerr << "Error en PrismaticBarCrossSectionsVector::setInitialSectionDeformations" << std::endl;
    if(nSections>0)
      {
        const Vector e1= (*this)[0]->getGeneralizedStrainVector(strainLoad.getDeformationPlane1())*loadFactor;
        const Vector e2= (*this)[nSections-1]->getGeneralizedStrainVector(strainLoad.getDeformationPlane1())*loadFactor;
        const Vector a= (e2-e1)/L;
        Vector tmp;
        for(size_t i= 0;i<nSections;i++)
          {
            tmp= a*xi(i,1)+e1;
            (*this)[i]->addInitialSectionDeformation(tmp);
          }
      }
  }

//! @brief Set trial strains.
void XC::PrismaticBarCrossSectionsVector::setTrialSectionDeformations(const std::vector<Vector> &vs)
  {
    const size_t nSections= std::min(size(),vs.size());
    if(vs.size()<nSections)
      std::cerr << "Error en PrismaticBarCrossSectionsVector::setTrialSectionDeformations" << std::endl;
    for(size_t i= 0;i<nSections;i++)
      (*this)[i]->setTrialSectionDeformation(vs[i]);
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::PrismaticBarCrossSectionsVector::sendData(CommParameters &cp)
  {
    const size_t sz= size();
    setDbTagDataPos(0,sz);
    DbTagData cpMat(sz*2);
    inicComm(sz*2);
    int res= 0;
    for(size_t i= 0;i<sz;i++)
      res+= cp.sendBrokedPtr((*this)[i],cpMat,BrokedPtrCommMetaData(i,0,i+sz));
    res+= cpMat.send(getDbTagData(),cp,CommMetaData(1));
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PrismaticBarCrossSectionsVector::recvData(const CommParameters &cp)
  {
    const size_t sz= getDbTagDataPos(0);
    DbTagData cpMat(sz*2);
    inicComm(sz*2);
    int res= cpMat.receive(getDbTagData(),cp,CommMetaData(1));
    for(size_t i= 0;i<sz;i++)
      {
        // Receive the material
        (*this)[i]= cp.getBrokedMaterial((*this)[i],cpMat,BrokedPtrCommMetaData(i,0,i+sz));
        if(!(*this)[i])
          std::cerr << "PrismaticBarCrossSectionsVector::recvData() - material "
                    << i << " failed to recv itself\n";
      }
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PrismaticBarCrossSectionsVector::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PrismaticBarCrossSectionsVector::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
