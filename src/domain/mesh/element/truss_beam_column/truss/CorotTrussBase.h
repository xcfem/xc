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
//CorotTrussBase.h

#ifndef CorotTrussBase_h
#define CorotTrussBase_h

#include "ProtoTruss.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
//! \ingroup ElemBarraArt
//
//! @brief Clase base para los elementos
//! de tipo barra articulada con formulación corrotacional.
class CorotTrussBase: public ProtoTruss
  {
  protected:
    double Lo;	        //!< initial length of truss
    double Ln;		//!< current length of truss
    double d21[3];	//!< current displacement offsets in basic system

    Matrix R;//!< Rotation matrix

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    CorotTrussBase(int tag, int classTag, int dim,int Nd1, int Nd2);
    CorotTrussBase(int tag, int classTag,int dimension);
    CorotTrussBase(const CorotTrussBase &);
    CorotTrussBase &operator=(const CorotTrussBase &);
    const double &getLo(void) const;
    const double &getLn(void) const;
  };
} // fin namespace XC

#endif




