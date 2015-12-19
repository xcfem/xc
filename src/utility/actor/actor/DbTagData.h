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
                                                                        
                                                                        
#ifndef DbTagData_h
#define DbTagData_h

#include "utility/matrix/ID.h"
#include <set>

namespace XC {

class CommParameters;
class CommMetaData;

//! @ingroup ACTOR
//
//! @brief Vector que almacena los dbTags de los miembros de la clase.
class DbTagData
  {
  private:
    ID data;
    mutable std::set<size_t> visitados;

    friend class CommParameters;
    int sendIdData(CommParameters &,const int &) const;
    int receiveIdData(const CommParameters &,const int &);
  public:
    DbTagData(const int &dataSize= 0);
    explicit DbTagData(const ID &);
    void inicComm(const int &dataSize);

    inline int Size(void) const
      { return data.Size(); }
    const int &getDbTagDataPos(const size_t &i) const;
    void setDbTagDataPos(const size_t &i,const int &v);
    int send(DbTagData &,CommParameters &, const CommMetaData &) const;
    int receive(DbTagData &,const CommParameters &,const CommMetaData &);
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const DbTagData &);

} // end of XC namespace

#endif
