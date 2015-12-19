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
// $Date: 2005/11/22 19:44:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/MultiSupportPattern.h,v $
                                                                        
#ifndef MultiSupportPattern_h
#define MultiSupportPattern_h

// Written: fmk 11/00
// Revised:
//
// Purpose: This file contains the class definition for MultiSupportPattern.
// MultiSupportPattern is an abstract class.

#include "EQBasePattern.h"
#include <utility/matrix/ID.h>

namespace XC {
class GroundMotion;
class Vector;

//! @ingroup LPatterns
//
//! @brief Pauta de cargas correspondiente a un terremoto.
//! El movimiento sísmico es distinto en cada apoyo.
class MultiSupportPattern: public EQBasePattern
  {
  private:
    ID theMotionTags;
    int dbMotions;
    MultiSupportPattern(const MultiSupportPattern &otro);    
    MultiSupportPattern &operator=(const MultiSupportPattern &otro);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    MultiSupportPattern(int tag, int classTag);
    MultiSupportPattern(int tag);    
    MultiSupportPattern(void);    

    virtual void applyLoad(double time);
    virtual bool addNodalLoad(NodalLoad *);
    virtual bool addElementalLoad(ElementalLoad *);
    
    // methods for o/p
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    virtual void Print(std::ostream &s, int flag =0);        

    int addMotion(GroundMotion &theMotion, int tag);    
    GroundMotion *getMotion(int tag);        
  };
} // end of XC namespace

#endif
