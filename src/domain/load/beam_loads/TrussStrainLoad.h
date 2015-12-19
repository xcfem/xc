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
//TrussStrainLoad.h
                                                                        
#ifndef TrussStrainLoad_h
#define TrussStrainLoad_h

#include "domain/load/ElementBodyLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Carga debida a deformación impuesta en un elemento Truss.
class TrussStrainLoad : public ElementBodyLoad
  {
  private:
    double e1; //!< Deformación impuesta en el nodo 1	
    double e2; //!< Deformación impuesta en el nodo 2
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
  // Constructors based on 2 temperature changes given
    TrussStrainLoad(int tag, const double &e1, const double &e2, const ID &theElementTags);
    TrussStrainLoad(int tag, const double &e_uniform, const ID &theElementTags);
    TrussStrainLoad(int tag, const ID &theElementTags);
    TrussStrainLoad(int tag);
    TrussStrainLoad(void);    
  
    inline const double &E1(void) const
      { return e1; }
    inline void setE1(const double &d)
      { e1= d; }
    inline const double &E2(void) const
      { return e2; }
    inline void setE2(const double &d)
      { e2= d; }

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;       
    any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif

