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
// $Date: 2003/02/25 23:33:36 $
// $Source: material/section/fiber_section/fiber/Fiber.h,v $
                                                                        
                                                                        
// File: ~/fiber/Fiber.h
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the class definition for 
// Fiber. Fiber is an abstract base class and thus no objects of 
// it's type can be instatiated. It has pure virtual functions which
// must be implemented in it's derived classes.
//
// What: "@(#) Fiber.h, revA"


#ifndef Fiber_h
#define Fiber_h

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"

class Pos2d;

namespace XC {
class Vector;
class Matrix;
class ResponseId;
class UniaxialMaterial;
class Information;
class Response;

//! @ingroup MATSCCModeloFibras
//
//! @defgroup MATSCCFibras Clases que definen la representación de las fibras de la sección.
//
//! @ingroup MATSCCFibras
//
//! @brief Representa una fibra de una sección.
class Fiber: public TaggedObject, public MovableObject
  {
    bool dead; //!< Verdadero si la fibra está desactivada.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    Fiber(int tag, int classTag);

    bool interpreta(const std::string &,const int &numValEsperados= -1) const;

    virtual int setTrialFiberStrain(const Vector &vs)=0;
    virtual Vector &getFiberStressResultants(void) =0;
    virtual Matrix &getFiberTangentStiffContr(void) =0;
	
    virtual int commitState(void)=0;
    virtual int revertToLastCommit(void)=0;    
    virtual int revertToStart(void)=0;
    
    virtual Fiber *getCopy(void) const= 0;
    virtual int getOrder(void) const= 0;
    virtual const ResponseId &getType(void) const= 0;

    virtual const bool isDead(void) const
      { return dead; }
    virtual const bool isAlive(void) const
      { return !dead; }
    virtual void kill(void)
      { dead= true; }
    virtual void alive(void)
      { dead= false; }

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &info);
    virtual int getResponse(int responseID, Information &info);

    virtual void getFiberLocation(double &y, double &z) const=0;
    virtual double getLocY(void) const= 0;
    virtual double getLocZ(void) const
      { return 0.0; }
    Pos2d getPos(void) const;

    virtual UniaxialMaterial *getMaterial(void) =0;
    virtual const UniaxialMaterial *getMaterial(void) const= 0;
    virtual double getArea(void) const=0;
    double getDeformacion(void) const;
    double getFuerza(void) const;
    double getMz(const double &y0= 0.0) const;
    double getMy(const double &z0= 0.0) const;
  };

//! @brief Devuelve el momento de la fuerza ejercida por la fibra
//! respecto al eje paralelo al «z» que pasa por y0.
inline double Fiber::getMz(const double &y0) const
  { return getFuerza()*(getLocY() - y0); }

//! @brief Devuelve el momento de la fuerza ejercida por la fibra
//! respecto al eje paralelo al «y» que pasa por z0.
inline double Fiber::getMy(const double &z0) const
  { return getFuerza()*(getLocZ() - z0); }

} // fin namespace XC


#endif

