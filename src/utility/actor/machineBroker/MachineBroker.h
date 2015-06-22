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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/08/29 07:16:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MachineBroker.h,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for MachineBroker.
// MachineBroker is an abstract base class, a subclass of which must
// be written for each parallel machine. A MachineBroker is responsible
// for getting an actor process running on the parallel machine.
//
// What: "@(#) MachineBroker.h, revA"

#ifndef MachineBroker_h
#define MachineBroker_h

#include <vector>

namespace XC {
class Channel;
class FEM_ObjectBroker;
class ID;

class MachineBroker
  {
  private:
    FEM_ObjectBroker *theObjectBroker;

    std::vector<Channel *> actorChannels; // channels owned with running actor processes
    int numActorChannels;
    int numActiveChannels;
    ID *activeChannels;

    void libera(void);
    void alloc(const std::size_t &);
    MachineBroker(const MachineBroker &);
    MachineBroker &operator=(const MachineBroker &);
  public:
    MachineBroker(FEM_ObjectBroker *theObjectBroker);
    virtual ~MachineBroker();

    // methods to return info about local process id and num processes
    virtual int getPID(void) = 0;
    virtual int getNP(void)  = 0;

    // methods to get and free Actors
    virtual int shutdown(void);    
    virtual int runActors(void);
    virtual Channel *startActor(int actorType, int compDemand = 0);
    virtual int finishedWithActor(Channel *);

    // methods to get and free Channels (processes)
    virtual Channel *getMyChannel(void)        =0;
    virtual Channel *getRemoteProcess(void)    =0;
    virtual int freeProcess(Channel *)         =0;

  };
} // fin namespace XC

#endif
