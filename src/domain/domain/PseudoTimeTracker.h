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
//PseudoTimeTracker.h

// Written: lcpt

#ifndef PseudoTimeTracker_h
#define PseudoTimeTracker_h

#include "utility/actor/actor/MovableObject.h"
#include <iostream>

namespace XC {

//! @brief Registro del tiempo.
class PseudoTimeTracker: public MovableObject
  {
  private:
    double currentTime; //!< current pseudo time
    double committedTime; //!< the committed pseudo time
    double dT; //!< difference between committed and current time
    double eigenvalueTimeSet;
  protected:
    friend class Domain;
    PseudoTimeTracker(void);
    void setCurrentTime(const double &newTime);
    void setCommittedTime(const double &newTime);
    void setEigenvalueTime(void);    
    void Inic(const double &newTime);
    void Zero(void);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    const double &getCurrentTime(void) const;
    const double &getCommittedTime(void) const;
    const double &getDt(void) const;
    const double &getEigenValueTimeSet(void) const;

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;
  };

std::ostream &operator<<(std::ostream &s,const PseudoTimeTracker &tt);
} // fin namespace XC
#endif
