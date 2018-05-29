//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/***************************************************************************
                          NullEvolution.h  -  description
                             -------------------
    begin                : Thu Aug 1 2002
    email                : rkaul@ce-blume215-pent-2.stanford.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef NULLEVOLUTION_H
#define NULLEVOLUTION_H

#include "material/yieldSurface/evolution/YS_Evolution.h"

/**Useful for declaring inner-surfaces or pinching surfaces or just
plain elastic-perfectly plastic surfaces that do not evolve
  *@author rkaul
  */

namespace XC {
//! @ingroup MATYS
//
//! @brief Useful for declaring inner-surfaces or pinching surfaces or just
//! plain elastic-perfectly plastic surfaces that do not evolve.
class NullEvolution: public YS_Evolution
  {
  private:
    static Vector vec_dim_1;
    static Vector vec_dim_2;  
    static Vector vec_dim_3;
  public: 

    NullEvolution(int tag, double isox);
    NullEvolution(int tag, double isox, double isoy);
    NullEvolution(int tag, double isox, double isoy, double isoz);

  int evolveSurface(YieldSurface_BC *ys, double magPlasticDefo,
                    Vector & G, Vector & F_Surface, int flag);

  const Vector &getEquiPlasticStiffness();

  YS_Evolution* getCopy();

  int getResponse(int responseID, Information & info);
  Response* setResponse(const std::vector<std::string> &argv, Information & info);
  
  virtual int sendSelf(CommParameters &) {return -1;}
  virtual int recvSelf(const CommParameters &){return -1;}
  /** No descriptions */
  int revertToLastCommit();
  int commitState(int status);
  double getTrialPlasticStrains(int dof);
  double getCommitPlasticStrains(int dof);
  };
} // end of XC namespace

#endif
