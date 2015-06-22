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
//BidimStrainLoad.h
                                                                        
#ifndef BidimStrainLoad_h
#define BidimStrainLoad_h

#include "BidimLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Carga debida a deformación inicial en un elemento Truss.
class BidimStrainLoad: public BidimLoad
  {
  private:
    std::vector<Vector> deformaciones; //!< Deformaciones impuestas en cada punto de Gauss 1.	

    void set_comp(const std::string &cmd,CmdStatus &status);
    void set_strain(const std::string &cmd,CmdStatus &status);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    BidimStrainLoad(int tag, const std::vector<Vector> &, const ID &theElementTags);
    BidimStrainLoad(int tag, const size_t &, const Vector &, const ID &theElementTags);
    BidimStrainLoad(int tag, const size_t &, const ID &theElementTags);
    BidimStrainLoad(int tag, const size_t &, const Vector &);
    BidimStrainLoad(int tag, const size_t &);
    BidimStrainLoad(const size_t &s= 4);
  
    inline const std::vector<Vector> &getDeformaciones(void) const
      { return deformaciones; }
    inline std::vector<Vector> &Deformaciones(void)
      { return deformaciones; }
    inline const Vector &getDeformacion(const size_t &i) const
      { return deformaciones[i]; }
    inline Vector &Deformacion(const size_t &i)
      { return deformaciones[i]; }
    void setDeformaciones(const Matrix &);
    void setStrainComp(const size_t &,const size_t &,const double &);

  
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;       
    any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif

