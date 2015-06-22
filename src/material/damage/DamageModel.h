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
  
                             
// $Revision: 1.1 $
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/damage/DamageModel.h,v $
                                                                        
#ifndef DamageModel_h
#define DamageModel_h         
                                                               
// Written: Arash Altoontash, Gregory Deierlein
// Created: 08/02
// Revision: AA
//
// Description: This file contains the class definition for 
// damage model Damage modelis a base class and 
// thus no objects of it's type can be instantiated. It has pure virtual 
// functions which must be implemented in it's derived classes. 

#include <utility/actor/actor/MovableObject.h>
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>


#include <utility/handler/ErrorHandler.h>
namespace XC {
class Response;
class DamageResponse;
 class Element;


enum DamageType
  {
    NotSpecified,
    Force,
    Deformation,
    PlasticDefo,
    TotalEnergy,
    PlasticEnergy
  };


class DamageModel :  public TaggedObject, public MovableObject
  {
  public:
    DamageModel(int tag, int classTag);    

    virtual int setTrial(const Vector &trialVector) = 0;
    virtual double getDamage (void) = 0;
    virtual double getPosDamage (void) = 0;
    virtual double getNegDamage (void) = 0;
    
    virtual int commitState (void) = 0;
    virtual int revertToLastCommit (void) = 0;    
    virtual int revertToStart (void) = 0;        
    
    virtual DamageModel *getCopy(void) const= 0;
    
    virtual int setVariable(const std::string &argv)  { return -1; }
    virtual int getVariable(int variableID, double &info)  { return -1; }
    
    virtual int setParameter(const std::vector<std::string> &argv, Information &theInformation);
    virtual int updateParameter(int responseID, Information &theInformation);	
    virtual Response *setResponse(const std::vector<std::string> &argv, Information &theInformation) = 0;
    virtual int getResponse(int responseID, Information &info) = 0;
    
    
    virtual int sendSelf(CommParameters &) = 0;  
    virtual int recvSelf(const CommParameters &) = 0;
    virtual void Print(std::ostream &s, int flag =0) =0;
  };

} // fin namespace XC

#endif
