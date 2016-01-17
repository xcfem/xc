//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//VectorSeccionesBarraPrismatica.cc

#include "VectorSeccionesBarraPrismatica.h"
#include "SeccionBarraPrismatica.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "boost/any.hpp"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"
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

//! @brief Solicita a cada sección que ejecute el bloque de código que se pasa como parámetro.
void XC::VectorSeccionesBarraPrismatica::for_each(const std::string &bloque)
  {
    const std::string nmbBlq= nombre_clase()+":for_each";
    for(iterator i= begin();i!=end();i++)
      {
        if(*i)
          (*i)->EjecutaBloque(bloque,nmbBlq);
      }
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
    //EntCmd::clearProps();
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
        const Vector e1= (*this)[0]->getVectorDeformacion(strainLoad.getPlanoDeformacion1())*loadFactor;
        const Vector e2= (*this)[nSections-1]->getVectorDeformacion(strainLoad.getPlanoDeformacion1())*loadFactor;
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

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
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

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
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

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
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

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
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
