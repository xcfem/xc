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
//
// COPY LEFT and RIGHT:
// Commercial    use    of    this  program without express permission of the
// University, is strictly encouraged. Copyright and Copyleft are covered
// by the following clause:
//
// Woody's (Guthrie) license:																																		
// ``This    source    code is Copyrighted in U.S., by the The University and 
// by the Authors,  for  an indefinite period, and anybody caught
// using  it  without  our  permission,  will be mighty good friends of ourn,
// cause  we  don't give a darn. Hack it. Compile it. Debug it. Run it. Yodel
// it. Enjoy it. We wrote it, that's all we wanted to do.'' bj
//
//
//
// PROJECT:           Object Oriented Finite Element Program
// PURPOSE:           Rounded Mohr Coulomb Potential Surface
// CLASS:
//
// VERSION:
// LANGUAGE:          C++
// TARGET OS:         DOS || UNIX || . . .
// DESIGNER(S):       Boris Jeremic jeremic@ucdavis.edu
//                    Zhao Cheng,
// PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//
//
// DATE:              12 Feb. 2003
// UPDATE HISTORY:
//
//
//
//
// SHORT EXPLANATION: Functions for rounded Mohr-Coulomb potential function
//
//================================================================================

#ifndef RMC01_PS_H
#define RMC01_PS_H

#include "material/nD/Template3Dep/PS.h"


namespace XC {
//! @ingroup MatPS
//
//! @brief Functions for rounded Mohr-Coulomb potential function.
class RMC01PotentialSurface : public PotentialSurface
  {
  public:
    RMC01PotentialSurface( ){ };   // Default constructor
    ~RMC01PotentialSurface() { }; //Not all virtual functions  redefined
    PotentialSurface *newObj(); //create a colne of itself

    BJtensor dQods(const EPState *EPS  ) const;
    BJtensor d2Qods2(const EPState *EPS) const;

    BJtensor d2Qodsds1(const EPState *EPS) const; // For Consistent Algorithm, Z Cheng, Jan 2004

    void print() { std::cerr << *this; };

    //================================================================================
    // Overloaded Insertion Operator
    // prints an RMC01-PotentialSurface's contents
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const RMC01PotentialSurface &PS);
  };
} // end of XC namespace

#endif

