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
//NewmarkBase2.h
                                                                        
                                                                        
#ifndef NewmarkBase2_h
#define NewmarkBase2_h

#include <solution/analysis/integrator/transient/NewmarkBase.h>

namespace XC {

//! @ingroup NewmarkIntegrator
//
//! @brief ??.
class NewmarkBase2 : public NewmarkBase
  {
  protected:
    double beta;
    double c1;  // some constants we need to keep

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);

    NewmarkBase2(SoluMethod *,int classTag);
    NewmarkBase2(SoluMethod *,int classTag,double gamma, double beta);
    NewmarkBase2(SoluMethod *,int classTag,double gamma, double beta,const RayleighDampingFactors &rF); 
  public:
    virtual any_const_ptr GetProp(const std::string &cod) const;    
  };
} // fin namespace XC

#endif

