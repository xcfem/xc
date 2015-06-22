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
//Element1D.h

#ifndef Element1D_h
#define Element1D_h

#include <domain/mesh/element/ElementBase.h>

class Segmento3d;

namespace XC {
class Node;
class Material;
class Domain;
class CrdTransf;
class CrdTransf2d;
class CrdTransf3d;
 class PlanoDeformacion;

//! \ingroup Elem
//
//! @brief Elemento unidimensional (beam,truss,...)
class Element1D : public ElementBase<2>
  {
    void malla_unidimensional(const TritrizPtrNod &,TritrizPtrElem &) const;
  protected:
    mutable std::vector<double> longsTributarias;

    TritrizPtrElem coloca_en_malla(const TritrizPtrNod &,dir_mallado dm) const;
    TritrizPtrElem cose(const SetEstruct &f1,const SetEstruct &f2) const;

    int sendCoordTransf(int posFlag,const int &,const int &,CommParameters &);
    CrdTransf *recvCoordTransf(int posFlag,const int &,const int &,const CommParameters &);
    CrdTransf2d *recvCoordTransf2d(int posFlag,const int &,const int &,const CommParameters &);

    CrdTransf3d *recvCoordTransf3d(int posFlag,const int &,const int &,const CommParameters &);

    bool procesa_element_load(const std::string &cmd,CmdStatus &status);
    bool procesa_comando(CmdStatus &status);
  public:
    Element1D(int tag, int classTag);
    Element1D(int tag, int classTag,int Nd1,int Nd2);
    void setDomain(Domain *theDomain);
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;
    virtual const Matrix &getCooPuntos(const size_t &ndiv) const;
    virtual const Vector &getCooPunto(const double &xrel) const;
    Segmento3d getSegmento(bool geomInicial= true) const;
    double getDist2(const Pos2d &p,bool geomInicial= true) const;
    double getDist(const Pos2d &p,bool geomInicial= true) const;
    double getDist2(const Pos3d &p,bool geomInicial= true) const;
    double getDist(const Pos3d &p,bool geomInicial= true) const;

    size_t getDimension(void) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    virtual void calculaLongsTributarias(bool geomInicial= true) const;
    double getLongTributaria(const Node *) const;

    void vector2dUniformLoadGlobal(const Vector &);
    void vector2dUniformLoadLocal(const Vector &);
    void vector2dPointByRelDistLoadGlobal(const double &,const Vector &);
    void vector2dPointByRelDistLoadLocal(const double &,const Vector &);
    void vector2dPointLoadGlobal(const Vector &,const Vector &);
    void vector2dPointLoadLocal(const Vector &,const Vector &);
    void vector3dUniformLoadGlobal(const Vector &);
    void vector3dUniformLoadLocal(const Vector &);
    void vector3dPointByRelDistLoadGlobal(const double &,const Vector &);
    void vector3dPointByRelDistLoadLocal(const double &,const Vector &);
    void vector3dPointLoadGlobal(const Vector &,const Vector &);
    void vector3dPointLoadLocal(const Vector &,const Vector &);
    void strainLoad(const PlanoDeformacion &p1, const PlanoDeformacion &p2);

    any_const_ptr GetProp(const std::string &cod) const;
  };

} //fin namespace XC
#endif
