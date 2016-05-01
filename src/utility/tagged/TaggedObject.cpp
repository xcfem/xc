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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:02:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/TaggedObject.cpp,v $
                                                                        
                                                                        
// File: ~/taggedt/TaggedObject.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation for XC::TaggedObject.
// A XC::TaggedObject is an object with an integer identifier. It is used as
// a base class by XC::DomainComponent, Graph and other classes in the framework.
//
// What: "@(#) TaggedObject.C, revA"

#include "utility/tagged/TaggedObject.h"
#include "xc_basic/src/matrices/m_double.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "boost/lexical_cast.hpp"


//! @brief Constructor.
XC::TaggedObject::TaggedObject(int tag,EntCmd *owr)
  : EntCmd(owr), theTag(tag) {}

//! @brief Constructor virtual.
XC::TaggedObject *XC::TaggedObject::getCopy(void) const
  {
    std::cerr << "No se ha definido getCopy para la clase: '"
              << nombre_clase() << "'\n";
    return nullptr;
  }

//! @brief Establece el valor del tag.
void XC::TaggedObject::setTag(int newTag) 
  { theTag = newTag; }

//! @brief Establece el valor del tag.
void XC::TaggedObject::assignTag(int newTag) 
  {
    std::clog << "Call assignTag is not a good idea (usually)." << std::endl;
    setTag(newTag);
  }

//! @brief Imprime el objeto.
void XC::TaggedObject::Print(std::ostream &s, int flag)
  {
    s << nombre_clase() << "::Print(std::ostream,flag) sin implementar."
      << std::endl;
  }

//! @brief Inserta el objeto en el stream de salida.
std::ostream &XC::operator<<(std::ostream &s, XC::TaggedObject &m)
  {
    m.Print(s);
    return s;
  }
