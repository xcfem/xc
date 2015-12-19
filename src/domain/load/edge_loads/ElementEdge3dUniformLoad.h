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
                                                                        
#ifndef ElementEdge3dUniformLoad_h
#define ElementEdge3dUniformLoad_h

#include "ElementEdgeUniformLoad.h"

namespace XC {
class FVectorElementEdgeColumn3d;

//! @ingroup ElemLoads
//
//! @brief Carga uniforme sobre elementos barra de tres dimensiones.
class ElementEdge3dUniformLoad: public ElementEdgeUniformLoad
  {
  private:
    double wz;  // Transverse
    static Vector data;
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    ElementEdge3dUniformLoad(int tag, const double &, const double &, const double &, const ID &, const ID &);
    ElementEdge3dUniformLoad(int tag, const Vector &Fxyz, const ID &, const ID &);
    ElementEdge3dUniformLoad(int tag);    
    ElementEdge3dUniformLoad(void);

    int getType(void);
    inline const double &Wx(void) const
      { return Axial; }
    inline const double &Wy(void) const
      { return Trans; }
    inline const double &Wz(void) const
      { return wz; }

    size_t getDimVectorFuerza(void) const;
    size_t getDimVectorMomento(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    void addReactionsInBasicSystem(const ID &,const Vector &,const double &,const double &,FVector &);
    void addFixedEndForcesInBasicSystem(const ID &,const Vector &,const double &L,const double &loadFactor,FVector &);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
    any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif

