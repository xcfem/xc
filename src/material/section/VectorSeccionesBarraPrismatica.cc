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
//VectorSeccionesBarraPrismatica.cc

#include "VectorSeccionesBarraPrismatica.h"
#include "SeccionBarraPrismatica.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "boost/any.hpp"


#include "material/section/ResponseId.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "utility/actor/actor/MovableID.h"

XC::SeccionBarraPrismatica *copia_seccion(const XC::SectionForceDeformation *theSec)
  {
    XC::SeccionBarraPrismatica *retval= nullptr;
    if(theSec)
      {
	XC::SectionForceDeformation *tmp= theSec->getCopy();
        if(tmp)
          {
            retval= dynamic_cast<XC::SeccionBarraPrismatica *>(tmp);
            if(!retval)
              {
                std::cerr << "copia_seccion; el material es inadecuado.\n";
                delete(tmp);
                exit(-1);
              }
          }
        else
          std::cerr << "copia_seccion -- failed to get copy of section.\n";
      }
    return retval;
  }

//! @brief Constructor.
XC::VectorSeccionesBarraPrismatica::VectorSeccionesBarraPrismatica(const size_t &sz)
  : std::vector<SeccionBarraPrismatica *>(sz,nullptr), MovableObject(0)
  {}

//! @brief Constructor por defecto.
XC::VectorSeccionesBarraPrismatica::VectorSeccionesBarraPrismatica(const size_t &sz,const SeccionBarraPrismatica *matModel)
  : std::vector<SeccionBarraPrismatica *>(sz,nullptr), MovableObject(0)
  {
    if(matModel)
      {
        for(iterator i= begin();i!=end();i++)
          {
            (*i)= copia_seccion(matModel);
            if(!(*i))
              std::cerr<<" VectorSeccionesBarraPrismatica -- failed allocate material model pointer\n";
          }
      }
  }

//! @brief Constructor.
XC::VectorSeccionesBarraPrismatica::VectorSeccionesBarraPrismatica(const size_t &sz,const Material *m)
  : std::vector<SeccionBarraPrismatica *>(sz,nullptr), MovableObject(0)
  {
    if(m)
      setupSection(m);
  }

//! @brief Constructor de copia.
XC::VectorSeccionesBarraPrismatica::VectorSeccionesBarraPrismatica(const VectorSeccionesBarraPrismatica &otro)
  : EntCmd(otro), std::vector<SeccionBarraPrismatica *>(otro.size(),nullptr), MovableObject(otro)
  {
    const size_t sz= otro.size();
    for(size_t i= 0;i<sz;i++)
      {
        if(otro[i])
          {
            (*this)[i]= copia_seccion(otro[i]);
            if(!(*this)[i])
              std::cerr<<" VectorSeccionesBarraPrismatica -- failed allocate material model pointer\n";
          }
      }
  }

//! @brief Operador asignación.
XC::VectorSeccionesBarraPrismatica &XC::VectorSeccionesBarraPrismatica::operator=(const VectorSeccionesBarraPrismatica &)
  {
    std::cerr << "VectorSeccionesBarraPrismatica: No se debe llamar al operador de asignación."
              << std::endl;
    return *this;
  }

XC::VectorSeccionesBarraPrismatica::~VectorSeccionesBarraPrismatica(void)
  { clearAll(); }

void XC::VectorSeccionesBarraPrismatica::setSection(const SeccionBarraPrismatica *nueva_secc)
  {
    borra_secciones();
    if(nueva_secc)
      {
        for(iterator i= begin();i!=end();i++)
          {
            (*i)= copia_seccion(nueva_secc);
            if(!(*i))
              std::cerr<<" VectorSeccionesBarraPrismatica -- failed allocate material model pointer\n";
          }
      }
  }

void XC::VectorSeccionesBarraPrismatica::setupSection(const Material *sec)
  {
    if(!sec)
      std::cerr << "XC::VectorSeccionesBarraPrismatica::setupSection; se pasó un puntero a material nulo." << std::endl;
    else
      {
        const SeccionBarraPrismatica *ptr= dynamic_cast<const SeccionBarraPrismatica *>(sec);
        if(!ptr)
          std::cerr << "XC::VectorSeccionesBarraPrismatica::setupSection; se pasó un puntero a material no válido." 
                    << std::endl;
        else
          setSection(ptr);
      }
  }

void XC::VectorSeccionesBarraPrismatica::setSectionCopy(size_t i,SeccionBarraPrismatica *nueva_secc)
  {
    if((*this)[i]) delete (*this)[i];
    (*this)[i]= copia_seccion(nueva_secc);
    if(!(*this)[i])
      std::cerr << "XC::VectorSeccionesBarraPrismatica::setSectionCopy -- failed to get copy of section.\n";
  }

void XC::VectorSeccionesBarraPrismatica::borra_secciones(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        if(*i) delete (*i);
        (*i)= nullptr;
      }
  }

void XC::VectorSeccionesBarraPrismatica::clearAll(void)
  {
    borra_secciones();
    std::vector<SeccionBarraPrismatica *>::clear();
    EntCmd::clearPyProps();
  }


//! @brief Consuma el estado de los secciones.
int XC::VectorSeccionesBarraPrismatica::commitState(void)
  {
    int retVal= 0;

    for(iterator i=begin();i!=end();i++)
      retVal+= (*i)->commitState();
    return retVal;
  }

//! @brief Devuelve el estado de los secciones al del último consumado.
int XC::VectorSeccionesBarraPrismatica::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i=begin();i!=end();i++)
      retVal+= (*i)->revertToLastCommit() ;
    return retVal;
  }


//! @brief Devuelve el estado de los secciones al inicial.
int XC::VectorSeccionesBarraPrismatica::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i=begin();i!=end();i++)
      retVal+= (*i)->revertToStart() ;
    return retVal;
  }

//! @brief Devuelve verdadero si las secciones tienen rigidez a torsión.
bool XC::VectorSeccionesBarraPrismatica::isTorsion(void) const
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

bool XC::VectorSeccionesBarraPrismatica::setSections(const std::vector<SeccionBarraPrismatica *> &sectionPtrs)
  {
    bool isTorsion= false;
    // get copy of the sections
    if(sectionPtrs.size()<size())
      std::cerr << "Error:  XC::VectorSeccionesBarraPrismatica::setSections; secciones insuficientes.";
    const size_t nSections= size();
    for(size_t i= 0;i<nSections;i++)
      {
        if(!sectionPtrs[i])
          {
            std::cerr << "Error: XC::NLBeamColumn3d::NLBeamColumn3d: section pointer " << i << std::endl;
            exit(-1);
          }  
        (*this)[i]= copia_seccion(sectionPtrs[i]);
        if(!(*this)[i])
          {
            std::cerr << "Error: XC::NLBeamColumn3d::NLBeamColumn3d: could not create copy of section " << i << std::endl;
            exit(-1);
          }
        int order= (*this)[i]->getOrder();
        const XC::ID &code= (*this)[i]->getType();
        for(int j= 0;j<order;j++)
          {
            if(code(j) == SECTION_RESPONSE_T)
              isTorsion = true;
          }
      }
    return isTorsion;
  }

//! @brief Asigna valores a las deformaciones iniciales.
void XC::VectorSeccionesBarraPrismatica::setInitialSectionDeformations(const std::vector<Vector> &vs)
  {
    const size_t nSections= std::min(size(),vs.size());
    if(vs.size()<nSections)
      std::cerr << "Error en VectorSeccionesBarraPrismatica::setInitialSectionDeformations" << std::endl;
    for(size_t i= 0;i<nSections;i++)
      (*this)[i]->setInitialSectionDeformation(vs[i]);
  }

//! @brief Asigna valores a las deformaciones iniciales interpolando entre e1 y e2.
void XC::VectorSeccionesBarraPrismatica::addInitialSectionDeformations(const BeamStrainLoad &strainLoad,const double &loadFactor,const Matrix &xi, const double &L)
  {
    const size_t numAbcisas= xi.noRows();
    const size_t nSections= std::min(size(),numAbcisas);
    if(numAbcisas<nSections)
      std::cerr << "Error en VectorSeccionesBarraPrismatica::setInitialSectionDeformations" << std::endl;
    if(nSections>0)
      {
        const Vector e1= (*this)[0]->getVectorDeformacion(strainLoad.getDeformationPlane1())*loadFactor;
        const Vector e2= (*this)[nSections-1]->getVectorDeformacion(strainLoad.getDeformationPlane1())*loadFactor;
        const Vector a= (e2-e1)/L;
        Vector tmp;
        for(size_t i= 0;i<nSections;i++)
          {
            tmp= a*xi(i,1)+e1;
            (*this)[i]->addInitialSectionDeformation(tmp);
          }
      }
  }

//! @brief Asigna valores a las deformaciones de prueba.
void XC::VectorSeccionesBarraPrismatica::setTrialSectionDeformations(const std::vector<Vector> &vs)
  {
    const size_t nSections= std::min(size(),vs.size());
    if(vs.size()<nSections)
      std::cerr << "Error en VectorSeccionesBarraPrismatica::setTrialSectionDeformations" << std::endl;
    for(size_t i= 0;i<nSections;i++)
      (*this)[i]->setTrialSectionDeformation(vs[i]);
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::VectorSeccionesBarraPrismatica::sendData(CommParameters &cp)
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
int XC::VectorSeccionesBarraPrismatica::recvData(const CommParameters &cp)
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
          std::cerr << "VectorSeccionesBarraPrismatica::recvData() - material "
                    << i << " failed to recv itself\n";
      }
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::VectorSeccionesBarraPrismatica::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::VectorSeccionesBarraPrismatica::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
