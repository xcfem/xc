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
// $Date: 2003/08/29 07:17:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MPI_MachineBroker.h,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for MPI_MachineBroker.
// MPI_MachineBroker is the broker responsible for monitoring the usage of
// the processes in an mpi run.
//
// What: "@(#) MPI_MachineBroker.h, revA"

#ifndef MPI_MachineBroker_h
#define MPI_MachineBroker_h

#include "MachineBroker.h"
#include <cstdlib>

namespace XC {
class ID;
class MPI_Channel;
class FEM_ObjectBroker;

class MPI_MachineBroker : public MachineBroker
  {
  private:
    size_t rank;
    ID *usedChannels;
    std::vector<MPI_Channel *> theChannels;

    void libera(void);
    void alloc(const std::size_t &);
    MPI_MachineBroker(const MPI_MachineBroker &);
    MPI_MachineBroker &operator=(const MPI_MachineBroker &);
  public:
    MPI_MachineBroker(FEM_ObjectBroker *theBroker, int argc,char *argv[]);
    ~MPI_MachineBroker(void);

    // methods to return info about local process id and num processes
    int getPID(void);
    int getNP(void);

    // methods to get and free Channels (processes)
    Channel *getMyChannel(void);
    Channel *getRemoteProcess(void);
    int freeProcess(Channel *);
  };
} // fin namespace XC

#endif
