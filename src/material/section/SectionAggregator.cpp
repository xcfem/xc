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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.11 $
// $Date: 2003/05/15 21:34:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionAggregator.cpp,v $
                                                                        
                                                                        
// File: ~/section/SectionAggregator.C
//
// Written: MHS
// Created: June 2000
// Revision: A
//
// Purpose: This file contains the implementation for the XC::SectionAggregator class. 

#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <classTags.h>
#include <material/section/SectionAggregator.h>
#include <utility/recorder/response/MaterialResponse.h>
#include "preprocessor/loaders/MaterialLoader.h"


#include "xc_utils/src/nucleo/python_utils.h"
#include "classTags.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

const size_t maxOrder= 10;

// Assumes section order is less than or equal to maxOrder.
// Can increase if needed!!!
double XC::SectionAggregator::workArea[2*maxOrder*(maxOrder+1)];
int    XC::SectionAggregator::codeArea[maxOrder];

//! @brief Comprueba que los punteros no sean nulos.
void XC::SectionAggregator::check_ptrs(void) const
  {
    if(!theCode || !def || !defzero || !s || !ks || !fs )
      {
        std::cerr << "SectionAggregator::SectionAggregator -- out of memory\n";
        exit(-1);
      }        
  }

void XC::SectionAggregator::alloc_storage_ptrs(void)
  {
    libera_storage_ptrs();
    const size_t order= getOrder();
    if(order > maxOrder)
      {
        std::cerr << "XC::SectionAggregator::alloc_storage_ptrs -- order too big, need to modify the maxOrder value in XC::SectionAggregator.cpp to %d\n";
        exit(-1);
      }
    theCode= new ResponseId(codeArea, order);
    def= new Vector(workArea, order);
    defzero= new Vector(workArea, order);
    s= new Vector(&workArea[maxOrder], order);
    ks= new Matrix(&workArea[2*maxOrder], order, order);
    fs= new Matrix(&workArea[maxOrder*(maxOrder+2)], order, order);
    check_ptrs();
  }

void XC::SectionAggregator::libera_storage_ptrs(void)
  {
    if(def)
      {
        delete def;
        def= nullptr;
      }
    if(defzero)
      {
        delete defzero;
        defzero= nullptr;
      }
    if(s)
      {
        delete s;
        s= nullptr;
      }
    if(ks)
      {
        delete ks;
        ks= nullptr;
      }
    if(fs)
      {
        delete fs;
        fs= nullptr;
      }
    if(theCode)
      {
        delete theCode;
        theCode= nullptr;
      }
  }

void XC::SectionAggregator::libera(void)
  {
    if(theSection)
      {
        delete theSection;
        theSection= nullptr;
      }
    libera_storage_ptrs();
  }

void XC::SectionAggregator::copia_seccion(const SectionForceDeformation *theSec)
  {
    if(theSection)
      {
        delete theSection;
        theSection= nullptr;
      }
    if(theSec)
      {
        SectionForceDeformation *tmp= theSec->getCopy();
        if(tmp)
          {
            theSection= dynamic_cast<SeccionBarraPrismatica *>(tmp);
            if(!theSection)
              {
                std::cerr << "SectionAggregator::copia_seccion; el material es inadecuado.\n";
                delete(tmp);
                exit(-1);
              }
          }
        else
          std::cerr << "SectionAggregator::copia_seccion -- failed to get copy of section.\n";
      }
  }

//! @brief Constructor.
XC::SectionAggregator::SectionAggregator(int tag, SeccionBarraPrismatica &theSec,const AggregatorAdditions &theAdds,MaterialLoader *mat_ldr)
  : SeccionBarraPrismatica(tag, SEC_TAG_Aggregator,mat_ldr), theSection(nullptr),
    theAdditions(theAdds,this), def(nullptr), defzero(nullptr), s(nullptr), ks(nullptr), fs(nullptr), theCode(nullptr)
  {
    copia_seccion(&theSec);
    alloc_storage_ptrs();
  }

//! @brief Constructor de copia.
XC::SectionAggregator::SectionAggregator(const SectionAggregator &otro)
  : SeccionBarraPrismatica(otro), theSection(nullptr), theAdditions(otro.theAdditions),
    def(nullptr), defzero(nullptr), s(nullptr), ks(nullptr), fs(nullptr), theCode(nullptr)
   {
     copia_seccion(otro.theSection);
     alloc_storage_ptrs();
   }

//! @brief Constructor.
XC::SectionAggregator::SectionAggregator(int tag, const AggregatorAdditions &theAdds,MaterialLoader *mat_ldr)
  : SeccionBarraPrismatica(tag, SEC_TAG_Aggregator,mat_ldr), theSection(nullptr), theAdditions(theAdds,this),
    def(nullptr), defzero(nullptr), s(nullptr), ks(nullptr), fs(nullptr), theCode(nullptr)
  { alloc_storage_ptrs(); }

XC::SectionAggregator::SectionAggregator(int tag, SeccionBarraPrismatica &theSec, UniaxialMaterial &theAddition, int c,MaterialLoader *mat_ldr)
  : SeccionBarraPrismatica(tag, SEC_TAG_Aggregator,mat_ldr), theSection(nullptr), theAdditions(this,theAddition,c),
    def(nullptr), defzero(nullptr), s(nullptr), ks(nullptr), fs(nullptr), theCode(nullptr)
  {
    copia_seccion(&theSec);
    alloc_storage_ptrs();
  }

XC::SectionAggregator::SectionAggregator(int tag,MaterialLoader *mat_ldr)
  : SeccionBarraPrismatica(tag, SEC_TAG_Aggregator,mat_ldr), theSection(nullptr), theAdditions(this), 
    def(nullptr), defzero(nullptr), s(nullptr), ks(nullptr), fs(nullptr), theCode(nullptr){}

//! @brief Constructor por defecto.
XC::SectionAggregator::SectionAggregator(MaterialLoader *mat_ldr)
  : SeccionBarraPrismatica(0, SEC_TAG_Aggregator,mat_ldr), theSection(nullptr), theAdditions(this),
    def(nullptr), defzero(nullptr), s(nullptr), ks(nullptr), fs(nullptr), theCode(nullptr) {}

//! @brief Operador asignación.
XC::SectionAggregator &XC::SectionAggregator::operator=(const SectionAggregator &otro)
  {
    libera();
    SeccionBarraPrismatica::operator=(otro);
    copia_seccion(otro.theSection);
    theAdditions= otro.theAdditions;
    theAdditions.set_owner(this);
    if(otro.theAdditions.check_ptrs())
      {
        alloc_storage_ptrs();
        (*def)= (*otro.def);
        (*defzero)= (*otro.defzero);
        (*s)= (*otro.s);
        (*ks)= (*otro.ks);
        (*fs)= (*otro.fs);
        (*theCode)= (*otro.theCode);
      }
    else
      std::cerr << "XC::SectionAggregator::operator=; puntero nulo en matCodes." << std::endl;
    return *this;
  }

void XC::SectionAggregator::setSection(const std::string &sectionName)
  {
    assert(material_loader);
    const Material *ptr_mat= material_loader->find_ptr(sectionName);
    if(ptr_mat)
      {
        const SeccionBarraPrismatica *tmp= dynamic_cast<const SeccionBarraPrismatica *>(ptr_mat);
        if(tmp)
          theSection= dynamic_cast<SeccionBarraPrismatica *>(tmp->getCopy());
        else
          std::cerr << "XC::SectionAggregator::setSection" 
                    << "; el material de código: '" << sectionName
                    << "' no corresponde a una sección.\n";
        alloc_storage_ptrs();
      }
    else
      std::cerr << "XC::SectionAggregator::setSection "
                << "no se encontró el material de código: '" << sectionName
                << "'.\n";
  }

void XC::SectionAggregator::setAddtions(const std::vector<std::string> &responseCodes,const std::vector<std::string> &nmbMats)
  {
    ResponseId codes(responseCodes);
    theAdditions.putMatCodes(codes);
    const size_t n= nmbMats.size();
    if(n!= responseCodes.size())
      std::cerr << "Error en el número de materiales; número de índices: " << responseCodes.size()
                << " número de materiales: " << n << std::endl;
    for(size_t i= 0;i<n;i++)
      {
        const Material *ptr_mat= material_loader->find_ptr(nmbMats[i]);
        if(ptr_mat)
          {
            const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
            if(tmp)
              theAdditions.push_back(tmp);
            else
              std::cerr << "SectionAggregator::setAddition; "
                        << "el material de código: '" << nmbMats[i]
                        << "' no corresponde a un material uniaxial.\n";
          }
        else
          std::cerr << "SectionAggregator::setAddition; "
                    << "no se encontró el material de código: '" << nmbMats[i]
                    << "'.\n";
      }
    alloc_storage_ptrs();
  }

void XC::SectionAggregator::setAddtionsPyList(const boost::python::list &responseCodes,const boost::python::list &nmbMats)
  { setAddtions(vector_string_from_py_list(responseCodes),vector_string_from_py_list(nmbMats)); }


//! @brief destructor:
XC::SectionAggregator::~SectionAggregator(void)
  { libera(); }

//! @brief Asigna la deformación inicial.
int XC::SectionAggregator::setInitialSectionDeformation(const Vector &def)
  {
    int ret= 0;
    int theSectionOrder= 0;

    if(theSection)
      {
        theSectionOrder= theSection->getOrder();
        Vector v(workArea, theSectionOrder);
        for(register int i= 0;i<theSectionOrder;i++)
          v(i)= def(i);
        ret= theSection->setInitialSectionDeformation(v);
      }
    theAdditions.setInitialStrain(def,theSectionOrder);
    return ret;
  }

//! @brief Asigna la deformación de prueba.
int XC::SectionAggregator::setTrialSectionDeformation(const Vector &def)
  {
    int ret= 0;
    int theSectionOrder= 0;

    if(theSection)
      {
        theSectionOrder= theSection->getOrder();
        Vector v(workArea, theSectionOrder);
        for(register int i= 0;i<theSectionOrder;i++)
          v(i)= def(i);
        ret= theSection->setTrialSectionDeformation(v);
      }
    theAdditions.setTrialStrain(def,theSectionOrder);
    return ret;
  }

//! @brief Devuelve la deformación correspondiente a la posición que se pasa como parámetro.
double XC::SectionAggregator::getStrain(const double &y,const double &z) const
  {
    double retval= 0;
    if(theSection)
      retval= theSection->getStrain(y,z);
    return retval;
  }

//! @brief Devuelve la deformación inicial de la sección.
const XC::Vector &XC::SectionAggregator::getInitialSectionDeformation(void) const
  {
    int theSectionOrder= 0;
    if(theSection)
      {
        const Vector &eSec= theSection->getInitialSectionDeformation();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          (*defzero)(i)= eSec(i);
      }
    theAdditions.getInitialStrain(*defzero,theSectionOrder);
    return *defzero;
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::SectionAggregator::getSectionDeformation(void) const
  {
    int theSectionOrder= 0;
    if(theSection)
      {
        const Vector &eSec= theSection->getSectionDeformation();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          (*def)(i)= eSec(i);
      }
    theAdditions.getStrain(*def,theSectionOrder);
    static Vector retval;
    retval= (*def)-(*defzero);
    return retval;
  }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::SectionAggregator::getSectionTangent(void) const
  {
    int theSectionOrder= 0;

    // Zero before assembly
    ks->Zero();

    if(theSection)
      {
        const XC::Matrix &kSec= theSection->getSectionTangent();
        theSectionOrder= theSection->getOrder();

        for(register int i= 0; i < theSectionOrder; i++)
          for(register int j= 0; j < theSectionOrder; j++)
            (*ks)(i,j)= kSec(i,j);
      }
    theAdditions.getTangent(*ks,theSectionOrder);
    return *ks;
  }

//! @brief Devuelve la matriz de rigidez noval.
const XC::Matrix &XC::SectionAggregator::getInitialTangent(void) const
  {
    int theSectionOrder= 0;

    // Zero before assembly
    ks->Zero();

    if(theSection)
      {
        const Matrix &kSec= theSection->getInitialTangent();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          for(register int j= 0; j < theSectionOrder; j++)
            (*ks)(i,j)= kSec(i,j);
      }
    theAdditions.getInitialTangent(*ks,theSectionOrder);
    return *ks;
  }

//! @brief Devuelve la matriz de flexibilidad.
const XC::Matrix &XC::SectionAggregator::getSectionFlexibility(void) const
  {
    int theSectionOrder= 0;
    // Zero before assembly
    fs->Zero();

    if(theSection)
      {
        const XC::Matrix &fSec= theSection->getSectionFlexibility();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          for(register int j= 0; j < theSectionOrder; j++)
            (*fs)(i,j)= fSec(i,j);
      }
    theAdditions.getFlexibility(*fs,theSectionOrder);
    return *fs;
  }

//! @brief Devuelve la matriz de flexibilidad noval.
const XC::Matrix &XC::SectionAggregator::getInitialFlexibility(void) const
  {
    int theSectionOrder= 0;

    // Zero before assembly
    fs->Zero();

    if(theSection)
      {
        const Matrix &fSec= theSection->getInitialFlexibility();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0;i<theSectionOrder;i++)
          for(register int j= 0;j<theSectionOrder;j++)
            (*fs)(i,j)= fSec(i,j);
      }
    theAdditions.getInitialFlexibility(*fs,theSectionOrder);
    return *fs;
  }

//! @brief Devuelve la resultante de tensiones.
const XC::Vector &XC::SectionAggregator::getStressResultant(void) const
  {
    int theSectionOrder= 0;
    
    if(theSection)
      {
        const Vector &sSec= theSection->getStressResultant();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          (*s)(i)= sSec(i);
      }
    theAdditions.getStress(*s,theSectionOrder);
    return *s;
  }

XC::SectionForceDeformation *XC::SectionAggregator::getCopy(void) const
  { return new SectionAggregator(*this); }

//! @brief Devuelve los códigos de los esfuerzos para los que la
//! sección aporta rigidez.
const XC::ResponseId &XC::SectionAggregator::getType(void) const
  {
    int theSectionOrder= 0;
    if(theSection)
      {
        const XC::ResponseId &secType= theSection->getType();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          (*theCode)(i)= secType(i);
      }
    theAdditions.getType(*theCode,theSectionOrder);
    return *theCode;
  }

int XC::SectionAggregator::getOrder(void) const
  {
    int order= theAdditions.size();
    if(theSection)
      order+= theSection->getOrder();
    return order;
  }

//! @brief Consuma el estado de los materiales tras alcanzar la convergencia.
int XC::SectionAggregator::commitState(void)
  {
    int err= 0;
    if(theSection)
      err+= theSection->commitState();
    err+= theAdditions.commitState();
    return err;
  }

//! @brief Devuelve el estado de la sección al último consumado.
int XC::SectionAggregator::revertToLastCommit(void)
  {
    int err= 0;
    // Revert the section
    if(theSection)
      err += theSection->revertToLastCommit();
    // Do the same for the uniaxial materials
    err+= theAdditions.revertToLastCommit();
    return err;
  }        

int XC::SectionAggregator::revertToStart(void)
  {
    int err= 0;
    // Revert the section
    if(theSection)
      err += theSection->revertToStart();
    // Do the same for the uniaxial materials
    err+= theAdditions.revertToStart();
    return err;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SectionAggregator::sendData(CommParameters &cp)
  {
    int res= SeccionBarraPrismatica::sendData(cp);
    res+= cp.sendBrokedPtr(theSection,getDbTagData(),BrokedPtrCommMetaData(5,6,7));
    res+= cp.sendMovable(theAdditions,getDbTagData(),CommMetaData(8));
    res+= cp.sendVectorPtr(def,getDbTagData(),ArrayCommMetaData(9,10,11));
    res+= cp.sendVectorPtr(defzero,getDbTagData(),ArrayCommMetaData(12,13,14));
    res+= cp.sendVectorPtr(s,getDbTagData(),ArrayCommMetaData(15,16,17));
    res+= cp.sendMatrixPtr(ks,getDbTagData(),MatrixCommMetaData(18,19,20,21));
    res+= cp.sendMatrixPtr(fs,getDbTagData(),MatrixCommMetaData(22,23,24,25));
    res+= cp.sendIDPtr(theCode,getDbTagData(),ArrayCommMetaData(26,27,28));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SectionAggregator::recvData(const CommParameters &cp)
  {
    int res= SeccionBarraPrismatica::recvData(cp);
    theSection= cp.getBrokedMaterial(theSection,getDbTagData(),BrokedPtrCommMetaData(5,6,7));
    res+= cp.receiveMovable(theAdditions,getDbTagData(),CommMetaData(8));
    def= cp.receiveVectorPtr(def,getDbTagData(),ArrayCommMetaData(9,10,11));
    defzero= cp.receiveVectorPtr(defzero,getDbTagData(),ArrayCommMetaData(12,13,14));
    s= cp.receiveVectorPtr(s,getDbTagData(),ArrayCommMetaData(15,16,17));
    ks= cp.receiveMatrixPtr(ks,getDbTagData(),MatrixCommMetaData(18,19,20,21));
    fs= cp.receiveMatrixPtr(fs,getDbTagData(),MatrixCommMetaData(22,23,24,25));
    theCode= cp.receiveResponseIdPtr(theCode,getDbTagData(),ArrayCommMetaData(26,27,28));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::SectionAggregator::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(29);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::SectionAggregator::recvSelf(const CommParameters &cp)
  {
    inicComm(29);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Imprime el objeto.
void XC::SectionAggregator::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      { theSection->Print(s, flag); }
    else
      {
        s << "\nSection Aggregator, tag: " << this->getTag() << std::endl;
        if(theSection)
          {
            s << "\tSection, tag: " << theSection->getTag() << std::endl;
            theSection->Print(s, flag);
          }
        s << "\tUniaxial Additions" << std::endl;
        theAdditions.Print(s,flag);
      }
  }

XC::Response *XC::SectionAggregator::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    // See if the response is one of the defaults
    Response *res= XC::SeccionBarraPrismatica::setResponse(argv,info);
    if(res != 0)
      return res;
  
    // If not, forward the request to the section (need to do this to get fiber response)
    // CURRENTLY NOT SENDING ANYTHING OFF TO THE UniaxialMaterials ... Probably
    // don't need anything more from them than stress, strain, and stiffness, 
    // which are covered in base class method ... can change if need arises
    else if(theSection)
      return theSection->setResponse(argv, info);
    else
      return 0;
  }

int XC::SectionAggregator::getResponse(int responseID, Information &info)
  {
    // Just call the base class method ... don't need to define
    // this function, but keeping it here just for clarity
    return XC::SeccionBarraPrismatica::getResponse(responseID, info);
  }

//! @brief Devuelve el identificador de la variable cuyo nombre se pasa como parámetro.
int XC::SectionAggregator::setVariable(const std::string &argv)
  {
    // Axial strain
    if(argv =="axialStrain")
      return 1;
    else if (argv == "curvatureZ") //Curvature about the section z-axis
      return 2;
    else if(argv == "curvatureY") // Curvature about the section y-axis
      return 3;
    else
      return -1;
  }

//! @brief Devuelve el valor de la variable cuyo nombre se pasa como parámetro.
int XC::SectionAggregator::getVariable(int variableID, double &info)
  {
    int i;
    info= 0.0;

    const int order= getOrder();

    const XC::Vector &e= getSectionDeformation();
    const XC::ResponseId &code= this->getType();

    switch (variableID)
      {
      case 1: // Axial strain
        // Series model ... add all sources of deformation
        for(i= 0; i < order; i++)
          if(code(i) == SECTION_RESPONSE_P)
            info+= e(i);
        return 0;
      case 2: // Curvature about the section z-axis
        for(i= 0; i < order; i++)
          if(code(i) == SECTION_RESPONSE_MZ)
            info += e(i);
          return 0;
      case 3:// Curvature about the section y-axis
        for(i= 0;i<order;i++)
          if(code(i) == SECTION_RESPONSE_MY)
            info += e(i);
        return 0;
      default:
        return -1;
      }
  }
