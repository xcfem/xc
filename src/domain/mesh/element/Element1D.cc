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
//Element1D.cc

#include "Element1D.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/loaders/LoadLoader.h"
#include "domain/mesh/node/Node.h"
#include "domain/load/beam_loads/Beam3dUniformLoad.h"
#include "domain/load/beam_loads/Beam3dPointLoad.h"
#include "domain/load/beam_loads/Beam2dUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPointLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "preprocessor/SetEstruct.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/matrix/Matrix.h"
#include "xc_basic/src/matrices/m_int.h"
#include "domain/mesh/element/coordTransformation/CrdTransf2d.h"
#include "domain/mesh/element/coordTransformation/CrdTransf3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::Element1D::Element1D(int tag, int classTag)
  : ElementBase<2>(tag,classTag), longsTributarias(2,0.0) {}

//! @brief Constructor.
XC::Element1D::Element1D(int tag, int classTag,int Nd1,int Nd2)
  : ElementBase<2>(tag,classTag), longsTributarias(2,0.0)
  { theNodes.set_id_nodos(Nd1,Nd2); }


void XC::Element1D::vector2dUniformLoadGlobal(const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
    vector2dUniformLoadLocal(vTrf);
  }

void XC::Element1D::vector2dUniformLoadLocal(const Vector &v)
  {
    Preprocessor *preprocessor= GetPreprocessor();
    MapLoadPatterns &casos= preprocessor->getLoadLoader().getLoadPatterns();
    static ID eTags(1);
    eTags[0]= getTag(); //Carga para éste elemento.
    const int &loadTag= casos.getCurrentElementLoadTag(); //Identificador de la carga.

    const size_t sz= v.Size();
    if(sz>1)
      {
        Beam2dUniformLoad *tmp= new Beam2dUniformLoad(loadTag,v[1],v[0],eTags);
        LoadPattern *lp= casos.getCurrentLoadPatternPtr();
        if(lp)
          {
            lp->addElementalLoad(tmp);
            casos.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << "There is no current load pattern. Load ignored."
                    << std::endl; 
      }
    else
      std::cerr << "vector2dUniformLoadLocal; el vector debe ser de dimensión 2" << std::endl;
  }

void XC::Element1D::vector2dPointByRelDistLoadGlobal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>1)
      {
        const CrdTransf *crd_trf= getCoordTransf();
        const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
        vector2dPointByRelDistLoadLocal(x,vTrf);
      }
    else
      std::cerr << "vector2dPointByRelDistLoadGlobal; el vector debe ser de dimensión 2" << std::endl;
  }

void XC::Element1D::vector2dPointByRelDistLoadLocal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>1)
      {
        Preprocessor *preprocessor= GetPreprocessor();
        MapLoadPatterns &casos= preprocessor->getLoadLoader().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Carga para éste elemento.
        const int &loadTag= casos.getCurrentElementLoadTag(); //Identificador de la carga.

        Beam2dPointLoad *tmp= new Beam2dPointLoad(loadTag,v[1],x,eTags,v[0]);
        LoadPattern *lp= casos.getCurrentLoadPatternPtr();
        if(lp)
          {
            lp->addElementalLoad(tmp);
            casos.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << "There is no current load pattern. Load ignored."
                    << std::endl; 
      }
    else
      std::cerr << "vector2dPointByRelDistLoadLocal; el vector debe ser de dimensión 2" << std::endl;
  }

void XC::Element1D::vector2dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    const double x= crd_trf->getPointBasicCoordFromGlobal(p);
    if(x<0.0 || x>1.0)
      std::cerr << "Error vector2dPointLoadGlobal; la distancia relativa x= "
                << x << " obtenida del punto: " << p
                << " debe estar comprendida entre 0 y 1." << std::endl;
    vector2dPointByRelDistLoadGlobal(x,v);
  }

void XC::Element1D::vector2dPointLoadLocal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    const double x= crd_trf->getPointBasicCoordFromGlobal(p);
    if(x<0.0 || x>1.0)
      std::cerr << "Error vector2dPointLoadGlobal; la distancia relativa x= "
                << x << " obtenida del punto: " << p
                << " debe estar comprendida entre 0 y 1." << std::endl;
    vector2dPointByRelDistLoadLocal(x,v);
  }

void XC::Element1D::vector3dUniformLoadGlobal(const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        const CrdTransf *crd_trf= getCoordTransf();
        const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
        vector3dUniformLoadLocal(vTrf);
      }
    else
      std::cerr  << "vector3dUniformLoadGlobal; el vector debe ser de dimensión 3" << std::endl;
  }

void XC::Element1D::vector3dUniformLoadLocal(const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        Preprocessor *preprocessor= GetPreprocessor();
        MapLoadPatterns &casos= preprocessor->getLoadLoader().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Carga para éste elemento.
        const int &loadTag= casos.getCurrentElementLoadTag(); //Identificador de la carga.

        LoadPattern *lp= casos.getCurrentLoadPatternPtr();
        if(lp)
          {
            Beam3dUniformLoad *tmp= new Beam3dUniformLoad(loadTag,v,0.0,eTags);
            lp->addElementalLoad(tmp);
            casos.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << "There is no current load pattern. Load ignored."
                    << std::endl; 
      }
    else
      std::cerr << "vector3dUniformLoadLocal; el vector debe ser de dimensión 3" << std::endl;
  }

void XC::Element1D::vector3dPointByRelDistLoadGlobal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        const CrdTransf *crd_trf= getCoordTransf();
        const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
        vector3dPointByRelDistLoadLocal(x,vTrf);
      }
    else
      std::cerr << "vector3dPointByRelDistLoadGlobal; el vector debe ser de dimensión 2" << std::endl;
  }

void XC::Element1D::vector3dPointByRelDistLoadLocal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        Preprocessor *preprocessor= GetPreprocessor();
        MapLoadPatterns &casos= preprocessor->getLoadLoader().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Carga para éste elemento.
        const int &loadTag= casos.getCurrentElementLoadTag(); //Identificador de la carga.

        Beam3dPointLoad *tmp= new Beam3dPointLoad(loadTag,v[1],v[2],x,eTags,v[0]);
        LoadPattern *lp= casos.getCurrentLoadPatternPtr();
        if(lp)
          {
            lp->addElementalLoad(tmp);
            casos.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << "There is no current load pattern. Load ignored."
                    << std::endl; 

      }
    else
      std::cerr << "vector3dPointByRelDistLoadLocal; el vector debe ser de dimensión 3" << std::endl;
  }

void XC::Element1D::vector3dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    const double x= crd_trf->getPointBasicCoordFromGlobal(p);
    if(x<0.0 || x>1.0)
      std::cerr << "Error vector3dPointLoadGlobal; la distancia relativa x= "
                << x << " obtenida del punto: " << p
                << " debe estar comprendida entre 0 y 1." << std::endl;
    vector3dPointByRelDistLoadGlobal(x,v);
  }

void XC::Element1D::vector3dPointLoadLocal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    const double x= crd_trf->getPointBasicCoordFromGlobal(p);
    if(x<0.0 || x>1.0)
      std::cerr << "Error vector3dPointLoadGlobal; la distancia relativa x= "
                << x << " obtenida del punto: " << p
                << " debe estar comprendida entre 0 y 1." << std::endl;
    vector3dPointByRelDistLoadLocal(x,v);
  }

void XC::Element1D::strainLoad(const PlanoDeformacion &p1,const PlanoDeformacion &p2)
  {
    Preprocessor *preprocessor= GetPreprocessor();
    MapLoadPatterns &casos= preprocessor->getLoadLoader().getLoadPatterns();
    static ID eTags(1);
    eTags[0]= getTag(); //Carga para éste elemento.
    const int &loadTag= casos.getCurrentElementLoadTag(); //Identificador de la carga.

    BeamStrainLoad *tmp= new BeamStrainLoad(loadTag,eTags);
    tmp->setPlanoDeformacion1(p1);
    tmp->setPlanoDeformacion2(p2);
    LoadPattern *lp= casos.getCurrentLoadPatternPtr();
    if(lp)
      {
        lp->addElementalLoad(tmp);
        casos.setCurrentElementLoadTag(loadTag+1);
      }
    else
      std::cerr << "There is no current load pattern. Load ignored."
                << std::endl; 
  }

//! @brief Procesa los comandos correspondientes a cargas sobre elemento
bool XC::Element1D::procesa_element_load(const std::string &cmd,CmdStatus &status)
  {
    static double x= 0.0;
    Preprocessor *preprocessor= GetPreprocessor();
    const CrdTransf *crd_trf= getCoordTransf();
    if(preprocessor && crd_trf)
      {
        MapLoadPatterns &casos= preprocessor->getLoadLoader().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Carga para éste elemento.
        const int &loadTag= casos.getCurrentElementLoadTag(); //Identificador de la carga.

        if(cmd == "x")
          {
            x= interpretaDouble(status.GetBloque());
            if(x<0.0 || x>1.0)
	      std::cerr << "Error; " << cmd << " la distancia relativa x= "
                        << x << " debe estar comprendida entre 0 y 1." << std::endl;
            return true;
          }
        else if(cmd == "punto_aplic")
          {
            const Vector p= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            x= crd_trf->getPointBasicCoordFromGlobal(p);
            if(x<0.0 || x>1.0)
	      std::cerr << "Error; " << cmd << " la distancia relativa x= "
                        << x << " obtenida del punto: " << p
                        << " debe estar comprendida entre 0 y 1." << std::endl;
            return true;
          }
        else if(cmd == "vector2d_uniform_load_global")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector2dUniformLoadGlobal(v);
            return true;
          }
        else if(cmd == "vector2d_uniform_load_local")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector2dUniformLoadLocal(v);
            return true;
          }
        else if(cmd == "vector2d_point_load_global")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector2dPointByRelDistLoadGlobal(x,v);
            return true;
          }
        else if(cmd == "vector2d_point_load_local")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector2dPointByRelDistLoadLocal(x,v);
            return true;
          }
        else if(cmd == "vector3d_uniform_load_global")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector3dUniformLoadGlobal(v);
            return true;
          }
        else if(cmd == "vector3d_uniform_load_local")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector3dUniformLoadLocal(v);
            return true;
          }
        else if(cmd == "vector3d_point_load_global")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector3dPointByRelDistLoadGlobal(x,v);
            return true;
          }
        else if(cmd == "vector3d_point_load_local")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
            vector3dPointByRelDistLoadLocal(x,v);
            return true;
          }
        else if(cmd == "strain_load")
          {
            BeamStrainLoad *tmp= new BeamStrainLoad(loadTag,eTags);
            tmp->LeeCmd(status);
            LoadPattern *lp= casos.getCurrentLoadPatternPtr();
            lp->addElementalLoad(tmp);
            return true;
          }
        else
          return false;
      }
    return false;
  }

//! @brief Lee un objeto Element1D desde archivo
bool XC::Element1D::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Element1D) Procesando comando: " << cmd << std::endl;

    if(cmd == "coordTransf") //Transformación de coordenadas.
      {
        CrdTransf *tmp= getCoordTransf();
        if(tmp)
          tmp->LeeCmd(status);
        else
	  std::cerr << "El elemento no tiene asignada una transformación de coordenadas." << std::endl;
        return true;
      }
    else if(cmd == "puntos")
      {
        std::deque<boost::any> fnc_args= status.Parser().SeparaArgs(this);
        if(fnc_args.size()<1)
          std::cerr << "puntos - uso: puntos(numPuntos) " << std::endl;
        const size_t ndiv= convert_to_size_t(fnc_args[0])-1; //Número de puntos.
        Matrix tmp= getCooPuntos(ndiv);
        tmp.LeeCmd(status);
        return true;
      }
    else if(procesa_element_load(cmd,status))
      { return true; }
    else
      return ElementBase<2>::procesa_comando(status);
  }

//! @brief Devuelve la dimensión del elemento.
size_t XC::Element1D::getDimension(void) const
  { return 1; }

void malla_en_i(const XC::Element1D &e,const XC::TritrizPtrNod::const_ref_fila_i &nodos,XC::TritrizPtrElem::var_ref_fila_i &elementos)
  {
    const size_t ncapas= nodos.GetCapas();
    for(size_t i=1;i<ncapas;i++)
      {
	XC::Element *tmp= e.getCopy();
        const int Nd1= nodos(i)->getTag();
        const int Nd2= nodos(i+1)->getTag();
        tmp->getNodePtrs().set_id_nodos(Nd1,Nd2);
        elementos(i)= tmp;
      }
  }

void malla_en_j(const XC::Element1D &e,const XC::TritrizPtrNod::const_ref_fila_j &nodos,XC::TritrizPtrElem::var_ref_fila_j &elementos)
  {
    const size_t nfilas= nodos.getNumFilas();
    for(size_t j=1;j<nfilas;j++)
      {
	XC::Element *tmp= e.getCopy();
        const int Nd1= nodos(j)->getTag();
        const int Nd2= nodos(j+1)->getTag();
        tmp->getNodePtrs().set_id_nodos(Nd1,Nd2);
        elementos(j)= tmp;
      }
  }

void malla_en_k(const XC::Element1D &e,const XC::TritrizPtrNod::const_ref_fila_k &nodos,XC::TritrizPtrElem::var_ref_fila_k &elementos)
  {
    const size_t ncols= nodos.getNumCols();
    for(size_t k=1;k<ncols;k++)
      {
	XC::Element *tmp= e.getCopy();
        const int Nd1= nodos(k)->getTag();
        const int Nd2= nodos(k+1)->getTag();
        tmp->getNodePtrs().set_id_nodos(Nd1,Nd2);
        elementos(k)= tmp;
      }
  }

void XC::Element1D::malla_unidimensional(const XC::TritrizPtrNod &nodos,XC::TritrizPtrElem &elementos) const
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    if(nodos.EsFilaI())
      {
        elementos= TritrizPtrElem(ncapas-1,MatrizPtrElem(1,1));
        TritrizPtrElem::var_ref_fila_i fila_elem= elementos.GetVarRefFilaI(1,1);
        malla_en_i(*this,nodos.GetConstRefFilaI(1,1),fila_elem);
      }
    else if(nodos.EsFilaJ())
      {
        elementos= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas-1,ncols));
        TritrizPtrElem::var_ref_fila_j fila_elem= elementos.GetVarRefFilaJ(1,1);
        malla_en_j(*this,nodos.GetConstRefFilaJ(1,1),fila_elem);
      }
    else if(nodos.EsFilaK())
      {
        elementos= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas,ncols-1));
        TritrizPtrElem::var_ref_fila_k fila_elem= elementos.GetVarRefFilaK(1,1);
        malla_en_k(*this,nodos.GetConstRefFilaK(1,1),fila_elem);
      }
  }

XC::TritrizPtrElem XC::Element1D::coloca_en_malla(const TritrizPtrNod &nodos,dir_mallado dm) const
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    const size_t dim_malla= nodos.GetDim();
    TritrizPtrElem retval;
    if(dim_malla<1)
      std::cerr << "Existe un sólo nodo, no se pudieron crear elementos." << std::endl;
    else
      {
       if(dim_malla<2) //Malla unidimensional
          malla_unidimensional(nodos,retval);
        else
          {
            switch(dm)
              {
              case dirm_i:
                if(ncapas<2)
		  std::cerr << "Element1D::coloca_en_malla; número insuficiente de nodos en dirección i. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas,ncols));
                    for(size_t j=1;j<=nfilas;j++)
                      for(size_t k=1;k<=ncols;k++)
                        {
                          TritrizPtrElem::var_ref_fila_i fila_elem= retval.GetVarRefFilaI(j,k);
                          malla_en_i(*this,nodos.GetConstRefFilaI(j,k),fila_elem);
                        }
                  }
                break;
              case dirm_j:
                if(nfilas<2)
		  std::cerr << "Element1D::coloca_en_malla; número insuficiente de nodos en dirección j. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas-1,ncols));
                    for(size_t i=1;i<=ncapas;i++)
                      for(size_t k=1;k<=ncols;k++)
                        {
                          XC::TritrizPtrElem::var_ref_fila_j fila_elem= retval.GetVarRefFilaJ(i,k);
                          malla_en_j(*this,nodos.GetConstRefFilaJ(i,k),fila_elem);
                        }
                  }
                break;
              case dirm_k:
                if(ncols<2)
		  std::cerr << "Element1D::coloca_en_malla; número insuficiente de nodos en dirección k. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas,ncols-1));
                    for(size_t i=1;i<=ncapas;i++)
	              for(size_t j=1;j<=nfilas;j++)
                        {
                          XC::TritrizPtrElem::var_ref_fila_k fila_elem= retval.GetVarRefFilaK(i,j);
                          malla_en_k(*this,nodos.GetConstRefFilaK(i,j),fila_elem);
                        }
                  }
                break;
              default:
                break;
              }
          }  
      }
    return retval;
  }

XC::TritrizPtrElem XC::Element1D::cose(const SetEstruct &f1,const SetEstruct &f2) const
  {
    const size_t nelem= f1.NumNodos();
    TritrizPtrElem retval(nelem,1,1);
    const size_t ncapas= std::min(f1.GetNumCapasNodos(),f2.GetNumCapasNodos());
    const size_t nfilas= std::min(f1.GetNumFilasNodos(),f2.GetNumFilasNodos());
    const size_t ncols= std::min(f1.GetNumColsNodos(),f2.GetNumColsNodos());
    for(size_t i=1;i<=ncapas;i++)
      for(size_t j=1;j<=nfilas;j++)
        for(size_t k=1;k<=ncols;k++)
          {
            Element *tmp= getCopy();
            const int Nd1= f1.GetNodo(i,j,k)->getTag();
            const int Nd2= f2.GetNodo(i,j,k)->getTag();
            tmp->getNodePtrs().set_id_nodos(Nd1,Nd2);
            retval(i,1,1)= tmp;
          }
    return retval;
  }

void XC::Element1D::setDomain(Domain *theDomain)
  {
    ElementBase<2>::setDomain(theDomain);
  }

//! @brief Devuelve (si puede) un apuntador a la transformación de coordenadas.
XC::CrdTransf *XC::Element1D::getCoordTransf(void)
  { return nullptr; }

//! @brief Devuelve (si puede) un apuntador a la transformación de coordenadas.
const XC::CrdTransf *XC::Element1D::getCoordTransf(void) const
  { return nullptr; }

//! @brief Devuelve puntos distribuidos entre los nodos extremos.
const XC::Matrix &XC::Element1D::getCooPuntos(const size_t &ndiv) const
  {
    static Matrix retval;
    const CrdTransf *tmp= getCoordTransf();
    if(tmp)
      retval= tmp->getCooPuntos(ndiv);
    else
      {
        const Pos3d p0= theNodes[0]->getPosInicial3d();
        const Pos3d p1= theNodes[1]->getPosInicial3d();
        MatrizPos3d linea(p0,p1,ndiv);
        retval= Matrix(ndiv+1,3);
        Pos3d tmp;
        for(size_t i= 0;i<ndiv+1;i++)
          {
            tmp= linea(i+1,1);
            retval(i,0)= tmp.x();
            retval(i,1)= tmp.y();
            retval(i,2)= tmp.z();
          }
      }
    return retval;
  }

//! @brief Devuelve el punto correspondiente a la coordenada 0<=xrel<=1.
const XC::Vector &XC::Element1D::getCooPunto(const double &xrel) const
  {
    static Vector retval;
    const CrdTransf *tmp= getCoordTransf();
    if(tmp)
      retval= tmp->getCooPunto(xrel);
    else
      {
        const Pos3d p0= theNodes[0]->getPosInicial3d();
        const Pos3d p1= theNodes[1]->getPosInicial3d();
        const Vector3d v= p1-p0;
        retval= Vector(3);
        const Pos3d tmp= p0+xrel*v;
        retval(0)= tmp.x();
        retval(1)= tmp.y();
        retval(2)= tmp.z();
      }
    return retval;
  }

//! @brief Devuelve el segmento definido por el elemento.
Segmento3d XC::Element1D::getSegmento(bool geomInicial) const
  {
    if(geomInicial)
      return Segmento3d(theNodes[0]->getPosInicial3d(),theNodes[1]->getPosInicial3d());
    else
      return Segmento3d(theNodes[0]->getPosFinal3d(),theNodes[1]->getPosFinal3d());
  }

//! @brief Devuelve el cuadrado de la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element1D::getDist2(const Pos2d &p,bool geomInicial) const
  { return getDist2(To3dXY2d(p),geomInicial); }

//! @brief Devuelve la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element1D::getDist(const Pos2d &p,bool geomInicial) const
  { return getDist(To3dXY2d(p),geomInicial); }

//! @brief Devuelve el cuadrado de la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element1D::getDist2(const Pos3d &p,bool geomInicial) const
  {
    const Segmento3d sg(getSegmento(geomInicial));
    return sg.dist2(p);
  }

//! @brief Devuelve la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element1D::getDist(const Pos3d &p,bool geomInicial) const
  {
    const Segmento3d sg(getSegmento(geomInicial));
    return sg.dist(p);
  }

//! @brief Envía la transformación de coordenadas a través del canal que se pasa como parámetro.
int XC::Element1D::sendCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,CommParameters &cp)
  {
    int retval= 0;
    CrdTransf *ptr= getCoordTransf();
    DbTagData &dt= getDbTagData();    
    if(!ptr)
      setDbTagDataPos(posFlag,1);
    else
      {
        setDbTagDataPos(posFlag,0);
        setDbTagDataPos(posClassTag,ptr->getClassTag());
        cp.sendMovable(*ptr,dt,CommMetaData(posDbTag));
      }
    return retval;
  }

//! @brief Recibe la transformación de coordenadas a través del canal que se pasa como parámetro.
XC::CrdTransf *XC::Element1D::recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const CommParameters &cp)
  {
    CrdTransf *ptr= getCoordTransf();
    DbTagData &dt= getDbTagData();
    if(getDbTagDataPos(posFlag) == 0)
      {
        // make some room and read in the vector
        if(!ptr)
          {
            ptr= cp.getNewCrdTransf(getDbTagDataPos(posClassTag));
            if(!ptr)
              std::cerr << "receiveCoordTransf -- ran out of memory\n";
          }
        if(ptr)
          {
            int res= cp.receiveMovable(*ptr,dt,CommMetaData(posDbTag));
            ptr->revertToLastCommit();// Revert the crdtrasf to its last committed state
            if(res<0)
              std::cerr << "receiveCoordTransf - failed to receive vector data\n";
          }
      }
    return ptr;
  }

//! @brief Recibe una transformación de coordenadas 2d a través del canal que se pasa como parámetro.
XC::CrdTransf2d *XC::Element1D::recvCoordTransf2d(int posFlag,const int &posClassTag,const int &posDbTag,const CommParameters &cp)
  {
    CrdTransf2d *retval= nullptr;
    CrdTransf *tmp= recvCoordTransf(posFlag,posClassTag,posDbTag,cp);
    if(tmp)
      {
        retval= dynamic_cast<CrdTransf2d *>(tmp);
        if(!retval)
          {
	    std::cerr << "El tipo de la transformación no es adecuado"
                      << std::endl;
            delete tmp;
          }
      }
    return retval;
  }

//! @brief Recibe una transformación de coordenadas 3d a través del canal que se pasa como parámetro.
XC::CrdTransf3d *XC::Element1D::recvCoordTransf3d(int posFlag,const int &posClassTag,const int &posDbTag,const CommParameters &cp)
  {
    CrdTransf3d *retval= nullptr;
    CrdTransf *tmp= recvCoordTransf(posFlag,posClassTag,posDbTag,cp);
    if(tmp)
      {
        retval= dynamic_cast<CrdTransf3d *>(tmp);
        if(!retval)
          {
	    std::cerr << "El tipo de la transformación no es adecuado"
                      << std::endl;
            delete tmp;
          }
      }
    return retval;
  }


//! @brief Interfaz con VTK.
int XC::Element1D::getVtkCellType(void) const
  { return VTK_LINE; }

//! @brief Interfaz con el formato MED de Salome.
int XC::Element1D::getMEDCellType(void) const
  { return MED_SEG2; }


//! @brief Calcula las longitudes tributarias correspondientes a cada
//! nodo del elemento
void XC::Element1D::calculaLongsTributarias(bool geomInicial) const
  {
    const double lt= getSegmento(geomInicial).Longitud()/2.0;
    longsTributarias[0]= lt;
    longsTributarias[1]= lt;
    vuelcaTributarias(longsTributarias);
  }

//! @brief Devuelve la longitud tributaria correspondiente al nodo que se pasa
//! como parámetro.
double XC::Element1D::getLongTributaria(const Node *nod) const
  {
    double retval= 0;
    const int i= theNodes.find(nod);
    if(i>=0) //El nodo es de este elemento.
      retval= longsTributarias[i];
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
//!
//! Soporta las propiedades:
any_const_ptr XC::Element1D::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "Element1D::GetProp (" << nombre_clase() << "::GetProp) Buscando propiedad: " << cod << std::endl;
    if(cod=="getVertices")
      {
	static m_int retval(2,1,0);
        if(theNodes[0]) retval(1,1)= theNodes[0]->getIdx();
        if(theNodes[1]) retval(2,1)= theNodes[1]->getIdx();
        return any_const_ptr(retval);
      }
    else if(cod == "getPuntos")
      {
        const size_t ndiv= popSize_t(cod);
        static m_double retval;
        tmp_gp_mdbl= matrix_to_m_double(getCooPuntos(ndiv));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod == "getPunto")
      {
        const double xrel= popDouble(cod);
        tmp_gp_mdbl= vector_to_m_double(getCooPunto(xrel));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getCoordTransf")
      {
        const CrdTransf *tmp= getCoordTransf();
        if(tmp)
          return any_const_ptr(tmp);
        else
          {
	    std::cerr << "El elemento no tiene asignada una transformación de coordenadas." << std::endl;
            return any_const_ptr();
          }
      }
    else if(cod == "getLongTributaria")
      {
        const int iNod= popInt(cod);
        tmp_gp_dbl= getLongTributariaByTag(iNod);
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return ElementBase<2>::GetProp(cod);
  }
