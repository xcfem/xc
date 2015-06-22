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

//##################################################################################
//# COPYRIGHT (C):     :-))                                                        #
//# PROJECT:           Object Oriented Finite Element Program                      #
//# PURPOSE:           General platform for elaso-plastic constitutive model       #
//#                    implementation                                              #
//# CLASS:             PotentialSurface(the base class for all potential surfaces) #
//#                                                                                #
//# VERSION:                                                                       #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
//# TARGET OS:         DOS || UNIX || . . .                                        #
//# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
//#                                                                                #
//#                                                                                #
//# DATE:              08-03-2000                                                  #
//# UPDATE HISTORY:                                                                #
//#                                                                                #
//#                                                                                #
//#                                                                                #
//#                                                                                #
//# SHORT EXPLANATION: The goal is to create a platform for efficient and easy     #
//#                    implemetation of any elasto-plastic constitutive model!     #
//#                                                                                #
//##################################################################################
//

#ifndef PS_H
#define PS_H

#include <iostream>

namespace XC {
  class BJtensor;
  class EPState;

//! @ingroup 3DEPMat
//
//! @defgroup MatPS Potential surface.
//
//! @ingroup MatPS
//
//! @brief The goal is to create a platform for efficient and easy
//! implemetation of any elasto-plastic constitutive model.
class PotentialSurface
  {
  public:
    virtual PotentialSurface *newObj()  = 0;  //create a colne of itself
    virtual ~PotentialSurface() {};	      // Codewizard requires virtual destructor
    double  q() const { return 0.0; }; // Codewizard does not like function defined in class definition
    virtual BJtensor dQods(const EPState *EPS ) const =  0;  //pure virtual func
    virtual BJtensor d2Qods2(const EPState *EPS ) const = 0; //pure virtual func   
    virtual void print() = 0; //pure virtual func
    
    // Added for Consistent Algorithm, Z. Cheng, Jan 2004
    // Ref. Jeremic & Sture, Mechanics of Cohesive-Frictional Materials, Vol.2 165-183 (1997) 
    virtual BJtensor d2Qodsds1(const EPState *EPS ) const;
    virtual BJtensor d2Qodsds2(const EPState *EPS ) const;
    virtual BJtensor d2Qodsds3(const EPState *EPS ) const;
    virtual BJtensor d2Qodsds4(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt1(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt2(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt3(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt4(const EPState *EPS ) const;
                                 
    //================================================================================
    // Overloaded Insertion Operator
    // prints an PotentialSurface's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const PotentialSurface & PS)
      {
        os << "Potential Surface Parameters: " << std::endl;
        return os;
      }
};
} // fin namespace XC


#endif

