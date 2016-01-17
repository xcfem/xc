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
//ShellUniformLoad.h
                                              
#ifndef ShellUniformLoad_h
#define ShellUniformLoad_h

#include "ShellMecLoad.h"

namespace XC {

//! @ingroup ElemLoads
//
//! @brief Carga uniforme sobre elementos shell.
class ShellUniformLoad : public ShellMecLoad
  {
  protected:
    DbTagData &getDbTagData(void) const;
  public:
    ShellUniformLoad(int tag,const double &,const double &,const double &,const ID &theElementTags);
    ShellUniformLoad(int tag, const Vector &Fxyz, const ID &theElementTags);
    ShellUniformLoad(int tag= 0);

    std::string Categoria(void) const;
    int getType(void);
    inline const double &Wx(void) const
      { return Axial1; }
    inline const double &Wy(void) const
      { return Axial2; }
    inline const double &Wz(void) const
      { return Trans; }

    size_t getDimVectorFuerza(void) const;
    size_t getDimVectorMomento(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    void addReactionsInBasicSystem(const double &,const double &,FVectorShell &);
    void addFixedEndForcesInBasicSystem(const double &,const double &loadFactor,FVectorShell &);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

