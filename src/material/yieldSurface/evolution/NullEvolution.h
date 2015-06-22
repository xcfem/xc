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
//! \ingroup MATYS
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
} // fin namespace XC

#endif
