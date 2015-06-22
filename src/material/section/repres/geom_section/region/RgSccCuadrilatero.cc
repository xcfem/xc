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
//RgSccCuadrilatero.cpp                                                                        
                                                                        
// File: RgSccCuadrilatero.C
// Written by Remo M. de Souza
// December 1998

#include <material/section/repres/geom_section/region/RgSccCuadrilatero.h>
#include <material/section/repres/cell/QuadCell.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_basic/src/texto/cadena_carac.h"
#include "xc_utils/src/geom/d2/Rejilla2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Cuadrilatero2d.h"
#include <utility/matrix/Matrix.h>

const int I= 0, J=1, K= 2, L= 3;
const int Y= 0, Z=1;

XC::RgSccCuadrilatero::RgSccCuadrilatero(Material *mat)
  : RgQuadCell(mat), vertCoord(4,2) {}


XC::RgSccCuadrilatero::RgSccCuadrilatero(Material *mat, int numSubdivIJ, int numSubdivJK,const XC::Matrix &vertexCoords)
  : RgQuadCell(mat,numSubdivIJ,numSubdivJK), vertCoord(vertexCoords)
  {}
  

//! @brief Lee un objeto XC::RgSccCuadrilatero desde archivo
bool XC::RgSccCuadrilatero::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(RgSccCuadrilatero) Procesando comando: " << cmd << std::endl;
    if(cmd == "yVertI")
      {
        vertCoord(I,Y)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "zVertI")
      {
        vertCoord(I,Z)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "yVertJ")
      {
        vertCoord(J,Y)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "zVertJ")
      {
        vertCoord(J,Z)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "yVertK")
      {
        vertCoord(K,Y)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "zVertK")
      {
        vertCoord(K,Z)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "yVertL")
      {
        vertCoord(L,Y)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "zVertL")
      {
        vertCoord(L,Z)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "posVertI")
      {
	std::deque<std::string> str_coord= separa_cadena(status.GetString(),",");
        if(str_coord.size()<2)
	  std::cerr << "Error; " << cmd 
		    << " se requieren dos coordenadas." << std::endl;
        else
          {
            const double y= interpretaDouble(str_coord[0]);
            vertCoord(I,Y)= y;
            const double z= interpretaDouble(str_coord[1]);
            vertCoord(I,Z)= z;
	  }
        return true;
      }
    else if(cmd == "posVertJ")
      {
	std::deque<std::string> str_coord= separa_cadena(status.GetString(),",");
        if(str_coord.size()<2)
	  std::cerr << "Error; " << cmd 
		    << " se requieren dos coordenadas." << std::endl;
        else
          {
            const double y= interpretaDouble(str_coord[0]);
            vertCoord(J,Y)= y;
            const double z= interpretaDouble(str_coord[1]);
            vertCoord(J,Z)= z;
	  }
        return true;
      }
    else if(cmd == "posVertK")
      {
	std::deque<std::string> str_coord= separa_cadena(status.GetString(),",");
        if(str_coord.size()<2)
	  std::cerr << "Error; " << cmd 
		    << " se requieren dos coordenadas." << std::endl;
        else
          {
            const double y= interpretaDouble(str_coord[0]);
            vertCoord(K,Y)= y;
            const double z= interpretaDouble(str_coord[1]);
            vertCoord(K,Z)= z;
	  }
        return true;
      }
    else if(cmd == "posVertL")
      {
	std::deque<std::string> str_coord= separa_cadena(status.GetString(),",");
        if(str_coord.size()<2)
	  std::cerr << "Error; " << cmd 
		    << " se requieren dos coordenadas." << std::endl;
        else
          {
            const double y= interpretaDouble(str_coord[0]);
            vertCoord(L,Y)= y;
            const double z= interpretaDouble(str_coord[1]);
            vertCoord(L,Z)= z;
	  }
        return true;
      }
    else if(cmd == "pMin")
      {
	std::deque<std::string> str_coord= separa_cadena(status.GetString(),",");
        if(str_coord.size()<2)
	  std::cerr << "Error; " << cmd 
		    << " se requieren dos coordenadas." << std::endl;
        else
          {
            const double y= interpretaDouble(str_coord[0]);
            const double z= interpretaDouble(str_coord[1]);
            setPMin(Pos2d(y,z));
 	  }
        return true;
      }
    else if(cmd == "pMax")
      {
	std::deque<std::string> str_coord= separa_cadena(status.GetString(),",");
        if(str_coord.size()<2)
	  std::cerr << "Error; " << cmd 
		    << " se requieren dos coordenadas." << std::endl;
        else
          {
            const double y= interpretaDouble(str_coord[0]);
            const double z= interpretaDouble(str_coord[1]);
            setPMax(Pos2d(y,z));
	  }
        return true;
      }
    else if(cmd == "swap")
      {
	status.GetString();
        swap();
        return true;
      }
    else
      return RgQuadCell::procesa_comando(status);
  }

double XC::RgSccCuadrilatero::getMaxY(void) const
  {
    double retval= vertCoord(I,Y);
    retval= std::max(retval,vertCoord(J,Y));
    retval= std::max(retval,vertCoord(K,Y));
    retval= std::max(retval,vertCoord(L,Y));
    return retval;
  }
double XC::RgSccCuadrilatero::getMaxZ(void) const
  {
    double retval= vertCoord(I,Z);
    retval= std::max(retval,vertCoord(J,Z));
    retval= std::max(retval,vertCoord(K,Z));
    retval= std::max(retval,vertCoord(L,Z));
    return retval;
  }
double XC::RgSccCuadrilatero::getMinY(void) const
  {
    double retval= vertCoord(I,Y);
    retval= std::min(retval,vertCoord(J,Y));
    retval= std::min(retval,vertCoord(K,Y));
    retval= std::min(retval,vertCoord(L,Y));
    return retval;
  }
double XC::RgSccCuadrilatero::getMinZ(void) const
  {
    double retval= vertCoord(I,Z);
    retval= std::min(retval,vertCoord(J,Z));
    retval= std::min(retval,vertCoord(K,Z));
    retval= std::min(retval,vertCoord(L,Z));
    return retval;
  }


//! @brief Returns upper rigth corner.
Pos2d XC::RgSccCuadrilatero::getPMax(void) const
  { return Pos2d(getMaxY(),getMaxZ()); }

//! @brief Sets upper rigth corner.
void XC::RgSccCuadrilatero::setPMax(const Pos2d &p)
  {
    const double y= p.x();
    vertCoord(J,Y)= y;
    vertCoord(K,Y)= y;
    const double z= p.y();
    vertCoord(L,Z)= z;
    vertCoord(K,Z)= z;
  }

//! @brief Returns lower left corner.
Pos2d XC::RgSccCuadrilatero::getPMin(void) const
  { return Pos2d(getMinY(),getMinZ()); }

//! @brief Sets upper rigth corner.
void XC::RgSccCuadrilatero::setPMin(const Pos2d &p)
  {
    const double y= p.x();
    vertCoord(I,Y)= y;
    vertCoord(L,Y)= y;
    const double z= p.y();
    vertCoord(I,Z)= z;
    vertCoord(J,Z)= z;
  }

Cuadrilatero2d XC::RgSccCuadrilatero::getCuadrilatero(void) const
  {
    Cuadrilatero2d retval= Cuadrilatero2d( Pos2d(vertCoord(I,Y),vertCoord(I,Z)),
                                           Pos2d(vertCoord(J,Y),vertCoord(J,Z)),
                                           Pos2d(vertCoord(K,Y),vertCoord(K,Z)),
                                           Pos2d(vertCoord(L,Y),vertCoord(L,Z)));
    return retval;
  }

void XC::RgSccCuadrilatero::swap(void)
  {
    Matrix tmp(vertCoord);
    tmp(I,Y)= vertCoord(L,Y); tmp(I,Z)= vertCoord(L,Z);
    tmp(J,Y)= vertCoord(K,Y); tmp(J,Z)= vertCoord(K,Z);
    tmp(K,Y)= vertCoord(J,Y); tmp(K,Z)= vertCoord(J,Z);
    tmp(L,Y)= vertCoord(I,Y); tmp(L,Z)= vertCoord(I,Z);
    vertCoord= tmp;
  }

//! @brief Assigns vertex coordinates.
void XC::RgSccCuadrilatero::setQuad(const Cuadrilatero2d &quad)
  {
    Pos2d vI= quad.Vertice0(I); Pos2d vJ= quad.Vertice0(J);
    Pos2d vK= quad.Vertice0(K); Pos2d vL= quad.Vertice0(L);
    vertCoord(I,Y)= vI.x(); vertCoord(I,Z)= vI.y();
    vertCoord(J,Y)= vJ.x(); vertCoord(J,Z)= vJ.y();
    vertCoord(K,Y)= vK.x(); vertCoord(K,Z)= vK.y();
    vertCoord(L,Y)= vL.x(); vertCoord(L,Z)= vL.y();
  }

Poligono2d XC::RgSccCuadrilatero::getPoligono(void) const
  { return getCuadrilatero(); }

void XC::RgSccCuadrilatero::setVertCoords(const XC::Matrix &vertexCoords)
  {
    vertCoord = vertexCoords;
  }

const XC::Matrix &XC::RgSccCuadrilatero::getVertCoords(void) const
  { return vertCoord; }

const Rejilla2d &XC::RgSccCuadrilatero::Malla(void) const
  { return alloc(Rejilla2d(getCuadrilatero().MallaBilin(nDivIJ,nDivJK))); }

const XC::VectorCells &XC::RgSccCuadrilatero::getCells(void) const
  {
    if(nDivIJ > 0  && nDivJK > 0)
      {
        const int numCells= this->getNumCells();

        cells.resize(numCells);

        const double deltaXi= 2.0 / nDivIJ;
        const double deltaEta= 2.0 / nDivJK;

        Matrix cellVertCoord(4,2);
        double xi, eta;
        Vector N(4);
        int k = 0;
        for(int j= 0;j<nDivJK;j++)
          for(int i= 0;i< nDivIJ;i++)
            {
              // compute natural coordinates
              cellVertCoord(I,Y) = -1.0 + deltaXi  * i;
              cellVertCoord(I,Z) = -1.0 + deltaEta * j;
              cellVertCoord(J,Y) = -1.0 + deltaXi  * (i+1);
              cellVertCoord(J,Z) = cellVertCoord(I,Z);
              cellVertCoord(K,Y) = cellVertCoord(J,Y);
              cellVertCoord(K,Z) = -1.0 + deltaEta * (j+1);
              cellVertCoord(L,Y) = cellVertCoord(I,Y);
              cellVertCoord(L,Z) = cellVertCoord(K,Z);

              // map to cartesian coordinates using bilinear
              // shape functions

              for(int r= 0;r<4;r++)
                {
                  xi= cellVertCoord(r,Y);
                  eta= cellVertCoord(r,Z);
 
                  N(0) = (1.0 - xi)*(1.0 - eta)/4.0;
                  N(1) = (1.0 + xi)*(1.0 - eta)/4.0;
                  N(2) = (1.0 + xi)*(1.0 + eta)/4.0;
                  N(3) = (1.0 - xi)*(1.0 + eta)/4.0;

                  cellVertCoord(r,Y) = 0.0;
                  cellVertCoord(r,Z) = 0.0;

                  for(int s=0;s<4;s++)
                    {
                      cellVertCoord(r,Y) += N(s) * vertCoord(s,Y);
                      cellVertCoord(r,Z) += N(s) * vertCoord(s,Z);
                    }
                }
              cells.put(k,QuadCell(cellVertCoord)); 
              //std::cerr << "\ncreating cells Cell " << k << " :" << cells[k];
              k++;
             }
      }
    return cells;
  }


XC::RegionSecc *XC::RgSccCuadrilatero::getCopy (void) const
  { return new XC::RgSccCuadrilatero(*this); }
 
//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::RgSccCuadrilatero::GetProp(const std::string &cod) const
  {
    if(cod=="yVertI") //Coordenada y del primer vértice.
      return any_const_ptr(vertCoord(I,Y));
    if(cod=="zVertI") //Coordenada z del primer vértice.
      return any_const_ptr(vertCoord(I,Z));
    if(cod=="yVertJ") //Coordenada y del segundo vértice.
      return any_const_ptr(vertCoord(J,Y));
    if(cod=="zVertJ") //Coordenada z del segundo vértice.
      return any_const_ptr(vertCoord(J,Z));
    if(cod=="yVertK") //Coordenada y del tercer vértice.
      return any_const_ptr(vertCoord(K,Y));
    if(cod=="zVertK") //Coordenada z del tercer vértice.
      return any_const_ptr(vertCoord(K,Z));
    if(cod=="yVertL") //Coordenada y del cuarto vértice.
      return any_const_ptr(vertCoord(L,Y));
    if(cod=="zVertL") //Coordenada z del cuarto vértice.
      return any_const_ptr(vertCoord(L,Z));
    else
      return RgQuadCell::GetProp(cod);
  }

void XC::RgSccCuadrilatero::Print(std::ostream &s, int flag) const
  {
    s << "\nRgQuadCell Type: RgSccCuadrilatero";
    //s << "\nMaterial Id: " << getMaterialID();
    s << "\nNumber of subdivisions in the IJ direction: " << nDivIJ;
    s << "\nNumber of subdivisions in the JK direction: " << nDivJK;
    s << "\nVertex Coordinates: " << vertCoord;
  }


std::ostream &XC::operator<<(std::ostream &s, XC::RgSccCuadrilatero &rg_scc_cuad)
  {
    rg_scc_cuad.Print(s);
    return s;
  }
