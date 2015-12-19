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
//PQyzBase.h

#ifndef PQYZBASE_H
#define PQYZBASE_H

#include <material/uniaxial/PY/PYBase.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Clase base para PySimple1 y QzSimple1.
class PQyzBase: public PYBase
  {
  protected:
    // Generated parameters or constants (not user input)
    double NFkrig; // stiffness of the "rigid" portion of Near Field spring
    
    InternalParamsLRIn CNF; // Committed internal parameters for the NearField rigid-plastic component
    InternalParamsLRIn TNF; // Trial internal parameters for the NearField rigid-plastic component
    
    InternalParamsA CGap; //!< Committed internal parameters for the Gap (Drag + Closure)
    InternalParamsA TGap; //!< Trial internal parameters for the Gap (Drag + Closure) 

    // Material parameters
    double Elast; // (p/matCapacity) when yielding first occurs in virgin loading
    double nd; // exponent for hardening shape of drag component

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    PQyzBase(int tag, int classtag, int soilType, double rult, double v50,double dashpot);
    PQyzBase(int tag, int classtag);
    PQyzBase(void);

    int commitState(void);
    int revertToLastCommit(void);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace


#endif
