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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/10/07 18:57:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/DomainComponent.h,v $
                                                                        
                                                                        
#ifndef DomainComponent_h
#define DomainComponent_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for DomainComponent.
// The DomainComponent class is an abstract class, example subclasses include
// Element, Node, SP_Constraint, MP_Constraint, NodalLoad, ElementalLoad. 
// Each of these objects forms part of Domain and has methods to set and obtain
// the associated Domain object.
//
// What: "@(#) DomainComponent.h, revA"

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Domain;
class Renderer;
class Information;
class Preprocessor;

//! @brief Objeto que forma parte de un dominio.
class DomainComponent: public TaggedObject, public MovableObject
  {
  private:    
    Domain *theDomain; // a pointer to the enclosing Domain object
    size_t idx; //!< @brief Índice del objeto (se emplea sólo para numerar entidades para VTK).
  protected:
    DomainComponent(int tag, int classTag);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ~DomainComponent(void);
    //! @brief Devuelve el índice (NO EL TAG) del objeto.
    inline size_t getIdx(void) const
      { return idx; }
    virtual void setDomain(Domain *theDomain);
    virtual Domain *getDomain(void) const;
    const Preprocessor *GetPreprocessor(void) const;
    Preprocessor *GetPreprocessor(void);

    void set_indice(const size_t &i);

  };
} // fin namespace XC

#endif


