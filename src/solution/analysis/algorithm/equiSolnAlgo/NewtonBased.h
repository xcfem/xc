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
//NewtonBased.h                                                                        
                                                                        
#ifndef NewtonBased_h
#define NewtonBased_h

#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>

namespace XC {
class ConvergenceTest;

//! @ingroup EQSolAlgo
//
//! @brief Emplea la matriz de rigidez tangente en la
//! primera iteración para iterar hasta alcanzar la convengencia.
class NewtonBased: public EquiSolnAlgo
  {
  protected:
    int tangent;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    NewtonBased(SoluMethod *,int classTag,int tangent = CURRENT_TANGENT);
  public:
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif


