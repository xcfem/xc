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
//DiagInteraccion.cc

#include "DiagInteraccion.h"
#include "xc_utils/src/geom/d2/MallaTriang3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d2/Triangulo3d.h"
#include "xc_basic/src/util/mchne_eps.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "utility/matrix/Vector.h"
#include "xc_utils/src/base/utils_any.h"
#include "material/section/fiber_section/FiberSectionBase.h"
#include "material/section/diag_interaccion/DatosDiagInteraccion.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "xc_utils/src/base/CmdStatus.h"

XC::DiagInteraccion::iterator XC::DiagInteraccion::begin(void) { return triedros.begin(); }
XC::DiagInteraccion::iterator XC::DiagInteraccion::end(void) { return triedros.end(); }
XC::DiagInteraccion::const_iterator XC::DiagInteraccion::begin() const { return triedros.begin(); }
XC::DiagInteraccion::const_iterator XC::DiagInteraccion::end() const { return triedros.end(); }
size_t XC::DiagInteraccion::size(void) const { return triedros.size(); }

size_t XC::DiagInteraccion::GetNumFacetas(void) const { return size(); }


void XC::DiagInteraccion::clasifica_triedro(const Triedro3d &tdro)
  {
    //Clasificamos el triedro por cuadrantes.
    for(int i= 0;i<8;i++)
      if(tdro.TocaCuadrante(i+1)) triedros_cuadrante[i].insert(&tdro);
  }

void XC::DiagInteraccion::clasifica_triedros(void)
  {
    //Clasificamos los triedros por cuadrantes.
    for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
      clasifica_triedro(*i);
  }

//! @brief Constructor por defecto.
XC::DiagInteraccion::DiagInteraccion(void)
  : GeomObj3d(), MovableObject(0), triedros(0), tol(0.0), rMax(0.0),rMin(0.0) {}

XC::DiagInteraccion::DiagInteraccion(const Pos3d &org,const MallaTriang3d &mll)
  : GeomObj3d(), MovableObject(0), triedros(mll.GetNumCaras(),Triedro3d()), tol(0.0), rMax(0.0), rMin(0.0)
  {
    const size_t nf= mll.GetNumCaras();
    static const Pos3d org3d;
    if(nf<4) return;
    for(MallaTriang3d::Point_const_iterator i= mll.points_begin();i!=mll.points_end();i++)
      rMax= std::max(rMax,dist(Pos3d(*i),org3d));
    rMin= 10*rMax;
    for(MallaTriang3d::Point_const_iterator i= mll.points_begin();i!=mll.points_end();i++)
      rMin= std::min(rMin,dist(Pos3d(*i),org3d));
    rMin/=3; //Radio de la esfera inscrita en el tetraedro cuya
             //esfera circunscrita pasa por el punto más cercano al origen.
    size_t cont= 0;
    for(MallaTriang3d::Facet_const_iterator i= mll.facets_begin();i!=mll.facets_end();i++)
      {
        const Triangulo3d tf= mll.GetTrianguloCara(i);
        triedros[cont]= Triedro3d(org,tf);
        clasifica_triedro(triedros[cont]);
        cont++;
      }
    //const double longDiagonalBND= Abs(Bnd().Diagonal());
    tol= 0.0;//longDiagonalBND/1e6;
  }

//! @brief Constructor de copia.
XC::DiagInteraccion::DiagInteraccion(const DiagInteraccion &otro)
  : GeomObj3d(otro), MovableObject(otro), triedros(otro.triedros), tol(otro.tol), rMax(otro.rMax),rMin(otro.rMin)
  {
    clasifica_triedros();
  }

//! @brief Operador de asignación.
XC::DiagInteraccion &XC::DiagInteraccion::operator=(const DiagInteraccion &otro)
  {
    GeomObj3d::operator=(otro);
    triedros= otro.triedros;
    tol= otro.tol;
    rMax= otro.rMax;
    rMin= otro.rMin;
    clasifica_triedros();
    return *this;
  }

void XC::DiagInteraccion::writeTo(const std::string &fName)
  {
    std::ofstream out(fName.c_str(), std::ios::out | std::ios::binary);
    write(out);
    out.close();
  }

void XC::DiagInteraccion::readFrom(const std::string &fName)
  {
    std::ifstream input(fName.c_str(), std::ios::in | std::ios::binary);
    if(input)
      {
        input.seekg(0);
        read(input);
        input.close();
      }
    else
      std::cerr << "No se pudo abrir el archivo: '"
                << fName << "'\n";
  }
  

//! @brief Lee un objeto DiagInteraccion desde archivo
bool XC::DiagInteraccion::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(DiagInteraccion) Procesando comando: " << cmd << std::endl;
    if(cmd=="write")
      {
	const std::string fname= interpretaString(status.GetString());
        writeTo(fname);
        return true;
      }
    else if(cmd=="read")
      {
	const std::string fname= interpretaString(status.GetString());
        readFrom(fname);
        return true;
      }
    else
      return GeomObj3d::procesa_comando(status);
  }

//! @brief Constructor virtual.
XC::DiagInteraccion *XC::DiagInteraccion::clon(void) const
  { return new DiagInteraccion(*this); }

double XC::DiagInteraccion::GetMax(short unsigned int i) const
  {
    double retval= NAN;
    if(size()<1) return retval;
    v_triedros::const_iterator j= triedros.begin();
    retval= j->GetMax(i);
    j++;
    for(;j!=triedros.end();j++)
      retval= std::max(retval,j->GetMax(i));
    return retval;
  }
double XC::DiagInteraccion::GetMin(short unsigned int i) const
  {
    double retval= NAN;
    if(size()<1) return retval;
    v_triedros::const_iterator j= triedros.begin();
    retval= j->GetMin(i);
    j++;
    for(;j!=triedros.end();j++)
      retval= std::min(retval,j->GetMin(i));
    return retval;
  }

Pos3d XC::DiagInteraccion::Cdg(void) const
  { return Pos3d(0,0,0); }
double XC::DiagInteraccion::Longitud(void) const
  { return 0.0; }
double XC::DiagInteraccion::Area(void) const
  { return 0.0; }
double XC::DiagInteraccion::Volumen(void) const
  { return 0.0; }
double XC::DiagInteraccion::Ix(void) const
  { return 0.0; }
double XC::DiagInteraccion::Iy(void) const
  { return 0.0; }
double XC::DiagInteraccion::Iz(void) const
  { return 0.0; }

//! @brief Busca el triedro que contiene al punto que se pasa como parámetro.
XC::DiagInteraccion::const_iterator XC::DiagInteraccion::BuscaTriedro(const Pos3d &p) const
  {
    XC::DiagInteraccion::const_iterator retval= end();
    for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
      {
        if((*i).In(p,tol))
          {
            retval= i;
            break;
          }
      }
    return retval;
  }

//! @brief Busca el triedro que contiene al punto que se pasa como parámetro.
const Triedro3d *XC::DiagInteraccion::BuscaPtrTriedro(const Pos3d &p) const
  {
    const Triedro3d *retval= nullptr;
    if(triedros.empty())
      {
	std::cerr << "DiagInteraccion::BuscaPtrTriedro; la lista de triedros está vacía."
                  << std::endl;
        return retval;
      }
    const int cuadrante= p.Cuadrante();
    const set_ptr_triedros &set_triedros= triedros_cuadrante[cuadrante-1];
    for(set_ptr_triedros::const_iterator i= set_triedros.begin();i!=set_triedros.end();i++)
      if((*i)->In(p,tol))
        {
          retval= *i;
          break;
        }
    if(!retval) //No lo encuentra, lo intentamos por fuerza bruta.
      for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
        {
          if((*i).In(p,tol))
            {
              retval= &(*i);
              break;
            }
        }
    if(!retval) //Sigue sin encontrarlo, buscamos aquel cuyo eje está más próximo.
      {
	XC::DiagInteraccion::const_iterator i= begin();
        const Triedro3d *tr= &(*i);
        SemiRecta3d rayo(tr->Cuspide(),p);
        Recta3d eje= tr->Eje();
        double angEjeRayo= angulo(eje,rayo);
        double angTmp= angEjeRayo;
        retval= tr;
        i++;
        for(;i!=end();i++)
          {
            tr= &(*i);
            rayo= SemiRecta3d(tr->Cuspide(),p);
            angTmp= angulo(tr->Eje(),rayo);
            if(angTmp<angEjeRayo)
              {
                angEjeRayo= angTmp;
                retval= tr;
              }
          }
      }
//     if(!retval) //Si sigue sin encontrarlo buscamos un punto en la misma línea.
//       {
//         const GEOM_FT denom= 100.0; //Abs(p.VectorPos())/100;
//         const Pos3d perturb= Pos3d(p.x()/denom,p.y()/denom,p.z()/denom); // + Vector3d(tol/1e2,tol/1e2,tol/1e2);
//         for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
//           {
//             if((*i).In(perturb,tol))
//               {
//                 retval= &(*i);
//                 break;
//               }
//           }
//       }
    return retval;
  }

//! @brief Devuelve la intersección de la semirrecta que une el origen (0,0,0) y el
//! el punto p con el diagrama de interacción.
GeomObj::list_Pos3d XC::DiagInteraccion::get_interseccion(const Pos3d &p) const
  {
    GeomObj::list_Pos3d lst_intersec;
    const Pos3d O= Pos3d(0.0,0.0,0.0);
    //Buscamos el triedro que contiene a p.
    const Triedro3d *i= BuscaPtrTriedro(p);
    if(!i)
      {
	std::cerr << "DiagInteraccion::get_interseccion: no se encontró un triedro que contuviera a:"
                  << p << " cuadrante: " << p.Cuadrante() << std::endl;
      }
    else
      {
        Triangulo3d triang(i->Vertice(1),i->Vertice(2),i->Vertice(3));
        //Calculamos el plano que contiene al triangulo.
        const Plano3d plano= triang.GetPlano();
        SemiRecta3d Op(O,p);
        //Calculamos la intersección entre la semirrecta y dicho plano.
        lst_intersec= interseccion(plano,Op);
        if(lst_intersec.empty())
          {
            
	    std::cerr << "No intersecan. " << std::endl
                      << " Area triangulo: " << triang.Area() << std::endl
                      << " vértice 1: " << i->Vertice(1)
                      << " vertice 2: " << i->Vertice(2)
                      << " vertice 3: " << i->Vertice(3) << std::endl
                      << " terna: " << p << std::endl;
            exit(-1);
          }
      }
    return lst_intersec;
  }

//! @brief Devuelve el factor de capacidad para la terna de esfuerzos que se pasan como parámetro.
double XC::DiagInteraccion::FactorCapacidad(const Pos3d &esf_d) const
  {
    assert(rMax>0.0);
    static const Pos3d O= Pos3d(0.0,0.0,0.0);
    const double d= dist(O,esf_d); //Distancia desde la terna de esfuerzos al origen.
    const double umbralMax= rMax*10.0;
    if(d<mchne_eps_dbl) //Si el punto está muy cerca del origen.
      return 0.0;//Devolvemos el máximo factor de capacidad que puede presentarse.
    if(d>umbralMax) //El punto está lejos de la superficie del diagrama.
      return d/rMax;
    const double umbralMin= rMin/10.0;
    if(d<umbralMin) //El punto está dentro de la superficie del diagrama.
      return d/rMin;
    const GeomObj::list_Pos3d lst_intersec= get_interseccion(esf_d);
    if(!lst_intersec.empty())
      {
        const Pos3d C= *(lst_intersec.begin());
        const Segmento3d sOC(O,C);
        return d/sOC.Longitud();
      }
    else
      {
	std::cerr << "DiagInteraccion::FactorCapacidad; no se encontró la intersección para la terna: "
                  << esf_d << std::endl;
        return d/rMin; //No ha encontrado la intersección.
      }
  }

XC::Vector XC::DiagInteraccion::FactorCapacidad(const GeomObj::list_Pos3d &lp) const
  {
    Vector retval(lp.size());
    int i= 0;
    for(GeomObj::list_Pos3d::const_iterator j= lp.begin();j!=lp.end(); j++, i++)
      retval[i]= FactorCapacidad(*j);
    return retval;
  }


void XC::DiagInteraccion::Print(std::ostream &os) const
  {
    std::cerr << "DiagInteraccion::Print no implementada." << std::endl;
  }

//! @brief Devuelve una matriz con las coordenadas de los puntos
//! que definen cada uno de los triedros.
void XC::DiagInteraccion::getMatrizPosiciones(Matrix &m)
  {
    const int sz= size();
    size_t fila= 0;
    m= Matrix(sz,12);
    for(const_iterator i= begin();i!=end();i++,fila++)
      {
        const Triedro3d &t= *i;
        const Pos3d &c= t.Cuspide();
        const Triangulo3d &b= t.Base();
        const Pos3d p1= b.Vertice(1);
        const Pos3d p2= b.Vertice(2);
        const Pos3d p3= b.Vertice(3);
        m(fila,0)= c.x(); m(fila,1)= c.y(); m(fila,2)= c.z();
        m(fila,3)= p1.x(); m(fila,4)= p1.y(); m(fila,5)= p1.z();
        m(fila,6)= p2.x(); m(fila,7)= p2.y(); m(fila,8)= p2.z();
        m(fila,9)= p3.x(); m(fila,10)= p3.y(); m(fila,11)= p3.z();
      }
  }

//! @brief Crea los triedros que definen el diagrama a partir
//! de una matriz con las coordenadas de los puntos
//! que definen cada uno de los triedros.
void XC::DiagInteraccion::setMatrizPosiciones(const Matrix &m)
  {
    const int nfilas= m.noRows();
    assert(m.noCols()==12);
    triedros.resize(nfilas);
    for(int i= 0;i<nfilas;i++)
      {
        const Pos3d c(m(i,0),m(i,1),m(i,2));
        const Pos3d p1(m(i,3),m(i,4),m(i,5));
        const Pos3d p2(m(i,6),m(i,7),m(i,8));
        const Pos3d p3(m(i,9),m(i,10),m(i,11));
        triedros[i]= Triedro3d(c,p1,p2,p3);
      }
    clasifica_triedros();   
  }

//! @brief Escribe la matriz en un archivo binario.
void XC::DiagInteraccion::write(std::ofstream &os)
  {
    os.write((char *) &tol,sizeof tol);
    os.write((char *) &rMax,sizeof rMax);
    os.write((char *) &rMin,sizeof rMin); 
    Matrix m;
    getMatrizPosiciones(m);
    m.write(os);
  }

//! @brief Lee la matriz de un archivo binario.
void XC::DiagInteraccion::read(std::ifstream &is)
  {
    is.read((char *) &tol,sizeof tol);
    is.read((char *) &rMax,sizeof rMax); 
    is.read((char *) &rMin,sizeof rMin); 
    Matrix m;
    m.read(is);
    setMatrizPosiciones(m);
    clasifica_triedros();
  }

//! @brief Envia los miembros del objeto a través del canal que se pasa como parámetro.
int XC::DiagInteraccion::sendData(CommParameters &cp)
  {
    int res= 0; //MovableObject::sendData(cp);
    res+= cp.sendDoubles(tol,rMax,rMin,getDbTagData(),CommMetaData(1));
    Matrix m;
    getMatrizPosiciones(m);
    res+= cp.sendMatrix(m,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::DiagInteraccion::recvData(const CommParameters &cp)
  {
    int res= 0;
    res+= cp.receiveDoubles(tol,rMax,rMin,getDbTagData(),CommMetaData(1));
    Matrix m;
    res+= cp.receiveMatrix(m,getDbTagData(),CommMetaData(2));
    setMatrizPosiciones(m);
    clasifica_triedros();   
    return res;
  }

//! @brief Envia el objeto a través del canal que se pasa como parámetro.
int XC::DiagInteraccion::sendSelf(CommParameters &cp)
  {
    inicComm(3);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "DiagInteraccion::sendSelf() - failed to send ID data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::DiagInteraccion::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    inicComm(3);
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "DiagInteraccion::recvSelf() - failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::DiagInteraccion::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "MaterialLoader::GetProp (" << nombre_clase() << "::GetProp) Buscando propiedad: " << cod << std::endl;

    if(cod=="getFactorCapacidad") //Devuelve factor de capacidad
      {                           //que corresponde a una terna de esfuerzos.
        tmp_gp_dbl= -1;
        double n= 0.0;
        double my= 0.0;
        double mz= 0.0;
        if(InterpreteRPN::Pila().size()>2)
          {
            mz= convert_to_double(InterpreteRPN::Pila().Pop());
            my= convert_to_double(InterpreteRPN::Pila().Pop());
            n= convert_to_double(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,3,"GetProp",cod);
        tmp_gp_dbl= FactorCapacidad(Pos3d(n,my,mz));
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return GeomObj3d::GetProp(cod);
  }

XC::DiagInteraccion XC::calc_diag_interaccion(const FiberSectionBase &scc,const DatosDiagInteraccion &datos= DatosDiagInteraccion())
  {
    DiagInteraccion retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetDiagInteraccion(datos);
        delete tmp;
      }
    else
      std::cerr << "XC::calc_diag_interaccion, no se pudo obtener una copia de la sección."
                << std::endl;
    return retval;
  }

