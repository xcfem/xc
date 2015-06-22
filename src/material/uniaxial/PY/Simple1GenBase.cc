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
//Simple1GenBase.cc

#include "material/uniaxial/PY/Simple1GenBase.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <utility/matrix/ID.h>

//! @brief Libera la memoria reservada.
void XC::Simple1GenBase::libera(void)
  {
    if(Nodex)
      {
        delete[] Nodex;
        Nodex= nullptr;
      }
    if(Nodey)
      {
        delete[] Nodey;
        Nodey= nullptr;
      }
    if(NodeNum)
      {
        delete[] NodeNum;
        NodeNum= nullptr;
      }
    if(PileEleNum)
      {
        delete[] PileEleNum;
        PileEleNum= nullptr;
      }
    if(PileNode1)
      {
        delete[] PileNode1;
        PileNode1= nullptr;
      }
    if(PileNode2)
      {
        delete[] PileNode2;
        PileNode2= nullptr;
      }
    if(gamma_t)
      {
        delete[] gamma_t;
        gamma_t= nullptr;
      }
    if(gamma_b)
      {
        delete[] gamma_b;
        gamma_b= nullptr;
      }
    if(z_t)
      {
        delete[] z_t;
        z_t= nullptr;
      }
    if(z_b)
      {
        delete[] z_b;
        z_b= nullptr;
      }
    if(c_t)
      {
        delete[] c_t;
        c_t= nullptr;
      }
    if(c_b)
      {
        delete[] c_b;
        c_b= nullptr;
      }
    if(zLoad_t)
      {
        delete[] zLoad_t;
        zLoad_t= nullptr;
      }
    if(zLoad_b)
      {
        delete[] zLoad_b;
        zLoad_b= nullptr;
      }
    if(load_val_t)
      {
        delete[] load_val_t;
        load_val_t= nullptr;
      }
    if(load_val_b)
      {
        delete[] load_val_b;
        load_val_b= nullptr;
      }
    if(zSp_t)
      {
        delete[] zSp_t;
        zSp_t= nullptr;
      }
    if(zSp_b)
      {
        delete[] zSp_b;
        zSp_b= nullptr;
      }
    if(sp_val_t)
      {
        delete[] sp_val_t;
        sp_val_t= nullptr;
      }
    if(sp_val_b)
      {
        delete[] sp_val_b;
        sp_val_b= nullptr;
      }
    if(zMp_t)
      {
        delete[] zMp_t;
        zMp_t= nullptr;
      }
    if(zMp_b)
      {
        delete[] zMp_b;
        zMp_b= nullptr;
      }
    if(mp_val_t)
      {
        delete[] mp_val_t;
        mp_val_t= nullptr;
      }
    if(mp_val_b)
      {
        delete[] mp_val_b;
        mp_val_b= nullptr;
      }
    if(ru_t)
      {
        delete[] ru_t;
        ru_t= nullptr;
      }
    if(ru_b)
      {
        delete[] ru_b;
        ru_b= nullptr;
      }
  }

//! @brief Constructor initializes global variables to zero
XC::Simple1GenBase::Simple1GenBase(void)
  {    
    NumNodes = 0;
    NumPileEle = 0;
    NumLayer = 0;
    NumLoad = 0;
    NumSp = 0;
    NumMat = 0;
    b = 0.0;
    depth = 0.0;
    stress = 0.0;
    ru = 0.0;
  }


//! @brief Destructor deletes dynamically allocated arrays
XC::Simple1GenBase::~Simple1GenBase(void)
  { libera(); }


/////////////////////////////////////////////////////////////////////////
// Function to get node numbers and coordinates
void XC::Simple1GenBase::GetNodes(const std::string &file)
{
    int i = 0;
    std::string trash2;
    char ch;
    
    std::ifstream in2(file.c_str(), std::ios::in);
    
    if(!in2)
    {
        std::cerr << "File " << file << "does not exist.  Must exit." << std::endl;
        exit(-1);
    }
    
    NumNodes = NumRows(file,"node");
    NodeNum = new int[NumNodes];
    Nodex = new double[NumNodes];
    Nodey = new double[NumNodes];

    while(in2)
      {
        if(char(in2.peek())=='n')
          {
	     //in2.getline(trash2,5,' ');
            getline(in2,trash2,' ');
            if(trash2=="node")
              {
                in2 >>  NodeNum[i] >> Nodex[i] >>  Nodey[i];
                i+=1;
              }
          }
        while(in2.get(ch))
          {
            if(ch=='\n')
                break;
          }
      }
    in2.close();
    return;
}

//////////////////////////////////////////////////////////////////////////////////
// Function to get pile element numbers and node numbers
void XC::Simple1GenBase::GetPileElements(const std::string &file)
  {
    int i = 0;
    std::string trash;
    char ch;
    
    std::ifstream in4;
    in4.open(file.c_str(), std::ios::in);

    if(!in4)
    {
        std::cerr << "File " << file << "does not exist.  Must exit." << std::endl;
        exit(-1);
    }

    NumPileEle = NumRows(file,"element");
    PileEleNum = new int[NumPileEle];
    PileNode1 = new int[NumPileEle];
    PileNode2 = new int[NumPileEle];
    
    while(in4)
      {
        if(in4.peek()=='e')
          {
            //in4.get(trash,8);
            in4 >> trash;
            if(trash=="element")
              {
                in4 >> trash >> PileEleNum[i] >> PileNode1[i] >> PileNode2[i];
                i+=1;
              }
            continue;
          }
        while(in4.get(ch))
          {
            if(ch=='\n')
                break;
          }
      }

    in4.close();
    return;
  }

    
///////////////////////////////////////////////////////////////////////////////////////
// Member function to get the number of rows in a file that begin with a certain string
int XC::Simple1GenBase::NumRows(const std::string &file, const std::string &begin)
  {
    std::ifstream in_file;
    in_file.open(file.c_str(), std::ios::in);
    int i = 0;
    std::string filein;
    
    while(!in_file.eof())
      {
        // check for blank lines
        while(in_file.peek()=='\n')
          getline(in_file,filein);
        // Read first character string
        getline(in_file,filein,' ');
        if(filein==begin)
          i = i+1;

        // Read remainder of line
        in_file.ignore(1000,'\n');
      }
    
    in_file.close();
    return i;

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function to linearly interpolate a y value for a given x value, and two given
// x values and two given y values at 
double XC::Simple1GenBase::linterp(double x1, double x2, double y1, double y2, double x3)
  { return y1 + (x3-x1)*(y2-y1)/(x2-x1); }
