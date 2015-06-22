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
//ProtoTruss.cc

#include "ProtoTruss.h"
#include "xc_utils/src/base/CmdStatus.h"
#include <utility/matrix/Matrix.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

// initialise the class wide variables
 XC::Matrix XC::ProtoTruss::trussM2(2,2);
 XC::Matrix XC::ProtoTruss::trussM3(3,3);
 XC::Matrix XC::ProtoTruss::trussM4(4,4);
 XC::Matrix XC::ProtoTruss::trussM6(6,6);
 XC::Matrix XC::ProtoTruss::trussM12(12,12);
 XC::Vector XC::ProtoTruss::trussV2(2);
 XC::Vector XC::ProtoTruss::trussV3(3);
 XC::Vector XC::ProtoTruss::trussV4(4);
 XC::Vector XC::ProtoTruss::trussV6(6);
 XC::Vector XC::ProtoTruss::trussV12(12);

//! Constructor por defecto.
XC::ProtoTruss::ProtoTruss(int tag, int classTag,int Nd1,int Nd2,int ndof,int ndim)
  : Element1D(tag,classTag,Nd1,Nd2),numDOF(ndof),dimSpace(ndim), theMatrix(nullptr), theVector(nullptr)
  {}


//! @brief Constructor de copia.
XC::ProtoTruss::ProtoTruss(const ProtoTruss &otro)
  : Element1D(otro),numDOF(otro.numDOF),dimSpace(otro.dimSpace),theMatrix(otro.theMatrix), theVector(otro.theVector)
  {}

//! @brief Operador asignación.
XC::ProtoTruss &XC::ProtoTruss::operator=(const ProtoTruss &otro)
  {
    Element1D::operator=(otro);
    numDOF= otro.numDOF;
    dimSpace= otro.dimSpace;
    theMatrix= otro.theMatrix;
    theVector= otro.theVector;
    return *this;
  }

//! @brief Lee un objeto ProtoTruss desde archivo
bool XC::ProtoTruss::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ProtoTruss) Procesando comando: " << cmd << std::endl;
    if(cmd == "dimSpace")
      {
        dimSpace= interpretaInt(status.GetString());
        return true;
      }
    if(cmd == "material")
      {
        Material *ptrMat= getMaterial();
        if(ptrMat)
          ptrMat->LeeCmd(status);
        else
          {
            status.GetBloque(); //Ignoramos entrada.
	    std::cerr << "(TrussBase) Procesando comando: " << cmd
                      << " el material no está asignado. Se ignora la entrada."
                      << std::endl;
          }
        return true;
      }
    else
      return Element1D::procesa_comando(status);
  }

//! @brief Devuelve el número de grados de libertad.
int XC::ProtoTruss::getNumDOF(void) const 
  { return numDOF; }

//! @brief Devuelve la dimensión del espacio en el que se define el elemento (2D ó 3D).
int XC::ProtoTruss::getNumDIM(void) const 
  { return dimSpace; }

//! @brief Returns a reference to element's material.
XC::Material &XC::ProtoTruss::getMaterialRef(void)
  {
    Material *ptr= getMaterial();
    assert(ptr);
    return *ptr;
  }

//! @brief Set the number of dof for element and set matrix and vector pointers.
void XC::ProtoTruss::setup_matrix_vector_ptrs(int dofNd1)
  {
    if(getNumDIM() == 1 && dofNd1 == 1)
      {
        numDOF = 2;
        theMatrix = &trussM2;
        theVector = &trussV2;
      }
    else if(getNumDIM() == 2 && dofNd1 == 2)
      {
        numDOF = 4;
        theMatrix = &trussM4;
        theVector = &trussV4;
      }
    else if(getNumDIM() == 2 && dofNd1 == 3)
      {
        numDOF = 6;
        theMatrix = &trussM6;
        theVector = &trussV6;
      }
    else if(getNumDIM() == 3 && dofNd1 == 3)
      {
        numDOF = 6;
        theMatrix = &trussM6;
        theVector = &trussV6;
      }
    else if(getNumDIM() == 3 && dofNd1 == 6)
      {
        numDOF = 12;
        theMatrix = &trussM12;
        theVector = &trussV12;
      }
    else
      {
        std::cerr <<"WARNING XC::ProtoTruss::setup_matrix_vector_ptrs cannot handle " << getNumDIM() << " dofs at nodes in " << 
	  dofNd1  << " problem\n";

        // fill this in so don't segment fault later
        numDOF = 6;
        theMatrix = &trussM6;
        theVector = &trussV6;
        return;
      }
  }

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::ProtoTruss::sendData(CommParameters &cp)
  {
    int res= Element1D::sendData(cp);
    res+= cp.sendInts(numDOF,dimSpace,getDbTagData(),CommMetaData(7));
    res+= cp.sendMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(8,9,10,11)); 
    res+= cp.sendVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(12,13,14)); 
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::ProtoTruss::recvData(const CommParameters &cp)
  {
    int res= Element1D::recvData(cp);
    res+= cp.receiveInts(numDOF,dimSpace,getDbTagData(),CommMetaData(7));
    theMatrix= cp.receiveMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(8,9,10,11)); 
    theVector= cp.receiveVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(12,13,14)); 
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::ProtoTruss::GetProp(const std::string &cod) const
  {
    if(cod=="getDimSpace")
      return any_const_ptr(dimSpace);
    else if(cod=="rho")
      {
        tmp_gp_dbl= getRho();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getMaterial")
      {
        const Material *ptrMat= getMaterial();
        if(ptrMat)
          return any_const_ptr(ptrMat);
        else
          {
	    std::cerr << "Error en TrussBase::GetProp; el puntero a material es nulo." << std::endl;
            return any_const_ptr();
          }
      }
    else
      return Element1D::GetProp(cod);
  }

