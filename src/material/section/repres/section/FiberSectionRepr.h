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
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/FiberSectionRepr.h,v $
                                                                        
                                                                        
// File: FiberSectionRepr.h
// Written by Remo M. de Souza (November 1998)
// modified by rms (July 1999) - doesn't prespecify number of regions and reinf. layers
//             rms (August 1999) - add fibers to section for the analysis
// modificado por lcpt (Septiembre 2006) - modificación grande. 
		    
#ifndef FiberSectionRepr_h 
#define FiberSectionRepr_h 

#include <material/section/repres/section/SectionRepres.h>
#include <material/section/repres/section/contenedor_fibras.h>
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include <list>

namespace XC {
class Fiber;
class FiberSection2d;
class FiberSection3d;
class FiberSectionGJ;
class FiberData;


//! @ingroup MATSCCRepresGeom
//
//! @brief Representación de una sección formada por fibras
class FiberSectionRepr: public SectionRepres
  {
  private:
    int sectID;
    contenedor_fibras fibras; //!< Punteros a fibras sueltas de la sección.

  protected:
    void vacia_fibras(void);
    void copia_fibras(const FiberSectionRepr &otro);
  public:
    // constructor and destructor
    FiberSectionRepr(int sectionID,MaterialLoader *ml);
    FiberSectionRepr(const FiberSectionRepr &otro);
    FiberSectionRepr &operator=(const FiberSectionRepr &otro);
    virtual FiberSectionRepr *getCopy(void) const;
    ~FiberSectionRepr(void);
        
    // edition functions
    int addFiber(Fiber &theFiber);

    // inquiring functions
    int getType(void) const;
      
    int getNumFibers(void) const;   
    FiberData getFiberData(void) const;
    contenedor_fibras getFibras2d(void) const;
    FiberSection2d getFiberSection2d(int secTag) const;
    contenedor_fibras getFibras3d(void) const;
    FiberSection3d getFiberSection3d(int secTag) const;
    FiberSectionGJ getFiberSectionGJ(int secTag,const double &GJ) const;
    
    void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, FiberSectionRepr &fiberSectionRepr);    
  };
} // fin namespace XC
#endif

