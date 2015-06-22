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
// $Date: 2005/11/30 23:47:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/subdomain/ActorSubdomain.h,v $
                                                                        
                                                                        
#ifndef ActorSubdomain_h
#define ActorSubdomain_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ActorSubdomain.
// ActorSubdomain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SP_Constraints 
// and MP_Constraints that have been added to the ActorSubdomain.
//
// What: "@(#) ActorSubdomain.h, revA"

#include "Subdomain.h"
#include "utility/actor/actor/Actor.h"
#include "utility/matrix/ID.h"

namespace XC {
class ActorSubdomain: public Subdomain, public Actor
  {
  private:
    ID msgData;
    Vector lastResponse;
  public:
    ActorSubdomain(Channel &, FEM_ObjectBroker &,DataOutputHandler::map_output_handlers *,EntCmd *);
    
    virtual int run(void);
    virtual const Vector &getLastExternalSysResponse(void);

    virtual int  update(void);    
    virtual int  updateTimeDt(void);    
    virtual int  barrierCheck(int res);    
  };
} // fin namespace XC
	
		   
#endif
