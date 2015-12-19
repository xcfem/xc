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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:23 $
// $Source: /usr/local/cvs/OpenSees/SRC/modelbuilder/PartitionedModelBuilder.h,v $
                                                                        
                                                                        
// File: ~/modelbuilder/PartitionedModelBuilder.h
// 
// Written: fmk 
// Created: 03/98
// Revision: A
//
// Description: This file contains the class definition for 
// PartitionedModelBuilder. PartitionedModelBuilder is an abstract class,
// A PartitionedModelBuilder creates the discritization of the
// structure, placing the components into the appropriate subdomains
// within the PartitionedDomain.
//
// What: "@(#) PartitionedModelBuilder.h, revA"

#ifndef PartitionedModelBuilder_h
#define PartitionedModelBuilder_h

#include "ModelBuilder.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class PartitionedDomain;
class Subdomain;

class PartitionedModelBuilder: public ModelBuilder, public MovableObject
{
  public:
    PartitionedModelBuilder(PartitionedDomain &aPartitionedDomain, int classTag);
    PartitionedModelBuilder(Subdomain &aSubdomain, int classTag);

    virtual int buildFE_Model(void);
    virtual int buildInterface(int numSubdomains) =0;
    virtual int buildSubdomain(int partition, int numPartitions, Subdomain &theSubdomain) =0;
    
  protected:
    PartitionedDomain *getPartitionedDomainPtr(void) const;
    
  private:
    PartitionedDomain *thePartitionedDomain;
};
} // end of XC namespace

#endif

