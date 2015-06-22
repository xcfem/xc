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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/25 23:33:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/Material.cpp,v $
                                                                        
                                                                        
// File: ~/material/Material.C
//
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class implementation for MaterialModel.
//
// What: "@(#) MaterialModel.C, revA"

#include "Material.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "modelador/loaders/MaterialLoader.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"

//! @brief Constructor.
XC::Material::Material(int tag, int clasTag)
  :TaggedObject(tag), MovableObject(clasTag) {}


//! @brief Lee un objeto XC::Material desde archivo
bool XC::Material::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Material) Procesando comando: " << cmd << std::endl;
    if(cmd == "update")
      {
        status.GetString();
        update();
        return true;
      }
    else if(cmd == "revert_to_start")
      {
        status.GetBloque(); //Ignoramos argumentos.
        revertToStart();
        return true;
      }
    else if(cmd == "commit")
      {
	std::clog << "El comando: " << cmd << " está pensado para pruebas." << std::endl;
        status.GetBloque(); //Ignoramos argumentos.
        commitState();
        return true;
      }
    else
      return TaggedObject::procesa_comando(status);
  }

//! @brief Devuelve (si puede) un puntero al modelador.
const XC::MaterialLoader *XC::Material::GetMaterialLoader(void) const
  {
    const XC::MaterialLoader *retval= dynamic_cast<const MaterialLoader *>(Owner());
    if(!retval)
      std::cerr << "Material::GetMaterialLoader; el puntero al cargador de materiales es nulo." << std::endl;
    return retval;
  }

//! @brief Devuelve (si puede) un puntero al cargador de materiales.
XC::MaterialLoader *XC::Material::GetMaterialLoader(void)
  {
    XC::MaterialLoader *retval= dynamic_cast<MaterialLoader *>(Owner());
    if(!retval)
      std::cerr << "Material::GetMaterialLoader; el puntero al cargador de materiales es nulo." << std::endl;
    return retval;
  }

int XC::Material::setVariable(const std::string &argv)
  { return -1; }

int XC::Material::getVariable(int variableID, double &info)
  { return -1; }

int XC::Material::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::Material::updateParameter(int responseID, Information &eleInformation)
  { return -1; }

XC::Response* XC::Material::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  { return nullptr; }

int XC::Material::getResponse(int responseID, Information &info)
  { return -1; }

//! @brief Actualiza las variables de estado del material
//! de acuerdo con los nuevos valores de los parámetros del
//! mismo.
void XC::Material::update(void)
   {return;}

void XC::Material::addInitialGeneralizedStrain(const Vector &incS)
  {
    Vector tmp= getInitialGeneralizedStrain();
    tmp+= incS;
    setInitialGeneralizedStrain(getInitialGeneralizedStrain()+incS);
  }

//! @brief Envía un puntero a material a través del canal que se pasa como parámetro.
//! @param posClassTag: Posición de ID del identificador de la clase del material.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::sendMaterialPtr(Material *ptr,DbTagData &dt,CommParameters &cp,const BrokedPtrCommMetaData &md)
  {
    int res= 0;
    if(ptr)
      {
        dt.setDbTagDataPos(md.getPosClassTag(),ptr->getClassTag());
        res= cp.sendMovable(*ptr,dt,md);
      }
    if(res < 0)
      std::cerr <<"WARNING sendMaterialPtr - "
                << " failed to send material\n";
    return res;
  }

//! @brief Recibe un puntero a material a través del canal que se pasa como parámetro.
//! @param posClassTag: Posición de ID del identificador de la clase del material.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::Material *XC::receiveMaterialPtr(Material* ptr,DbTagData &dt,const CommParameters &cp,const BrokedPtrCommMetaData &md)
  {
    Material *retval= nullptr;
    const int matClass= dt.getDbTagDataPos(md.getPosClassTag());
    if(ptr && (ptr->getClassTag() == matClass))
      retval= ptr;
    else 
      {

        // check if we have a material object already &
        // if we do if of right type
        if(ptr) // if old one .. delete it
          delete ptr;
        ptr= nullptr;
        retval= cp.getBrokedMaterial(ptr,dt,md);
      }
    return retval;
  }

//! @brief Devuelve la propiedad cuyo código se pasa como parámetro.
any_const_ptr XC::Material::GetProp(const std::string &cod) const
  {
    if(cod=="getGeneralizedStrain")
      return get_prop_vector(getGeneralizedStrain());
    else if(cod=="getInitialGeneralizedStrain")
      return get_prop_vector(getInitialGeneralizedStrain());
    else if(cod=="getGeneralizedStress")
      return get_prop_vector(getGeneralizedStress());
    else if(cod=="getGeneralizedStrainComp")
      {
        const size_t i= popSize_t(cod);
        tmp_gp_dbl= getGeneralizedStrain()[i-1];
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getIntialGeneralizedStrainComp")
      {
        const size_t i= popSize_t(cod);
        tmp_gp_dbl= getInitialGeneralizedStrain()[i-1];
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getGeneralizedStressComp")
      {
        const size_t i= popSize_t(cod);
        tmp_gp_dbl= getGeneralizedStress()[i-1];
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return TaggedObject::GetProp(cod);
  }
