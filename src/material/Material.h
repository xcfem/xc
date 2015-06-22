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
// $Source: /usr/local/cvs/OpenSees/SRC/material/Material.h,v $
                                                                        
                                                                        
#ifndef Material_h
#define Material_h

// File: Material.h
//
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class definition for Material.
// Material is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) Material.h, revA"

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/BrokedPtrCommMetaData.h"

namespace XC {
class Information;
class Response;
class MaterialLoader;
class ID;

//!  @defgroup Mat Modelización de materiales.

//! @ingroup Mat
//
//! @brief Clase base para los materiales del modelo.
class Material: public TaggedObject, public MovableObject
  {
  protected:
    bool procesa_comando(CmdStatus &status);

  public:
    Material(int tag, int classTag);

    const MaterialLoader *GetMaterialLoader(void) const;
    MaterialLoader *GetMaterialLoader(void);

    virtual int setVariable(const std::string &argv);
    virtual int getVariable(int variableID, double &info);

    // methods for sensitivity studies
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int responseID, Information &eleInformation);	

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    virtual int getResponse(int responseID, Information &info);

    virtual void update(void);

    virtual const Vector &getGeneralizedStress(void) const= 0;
    virtual const Vector &getGeneralizedStrain(void) const= 0;
    virtual const Vector &getInitialGeneralizedStrain(void) const= 0;

    virtual void setInitialGeneralizedStrain(const Vector &)= 0;
    virtual void addInitialGeneralizedStrain(const Vector &);

    virtual int commitState(void) = 0;
    virtual int revertToLastCommit(void) = 0;
    virtual int revertToStart(void) = 0;

    any_const_ptr GetProp(const std::string &cod) const;
  };

int sendMaterialPtr(Material *,DbTagData &,CommParameters &cp,const BrokedPtrCommMetaData &);
Material *receiveMaterialPtr(Material *,DbTagData &,const CommParameters &cp,const BrokedPtrCommMetaData &);

} // fin namespace XC


#endif

