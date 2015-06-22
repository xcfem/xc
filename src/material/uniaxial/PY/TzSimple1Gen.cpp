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
//////////////////////////////////////////////////////////////////////
//  This file contains the constructor, destructor, and member        //  
//  functions for the XC::TzSimple1Gen class.  The purpose of the        //
//  class is to create XC::TzSimple1 materials associated with            //
//  pre-defined    zeroLength elements, beam column elements, and        //
//    nodes.                                                            //
//                                                                    //
//  Written by: Scott Brandenberg                                    //
//              Graduate Student, UC Davis                            //
//              December 2, 2003                                    //
//////////////////////////////////////////////////////////////////////

//$Revision: 1.4 $
//$Date: 2004/06/30 00:27:40 $
//$Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PY/TzSimple1Gen.cpp,v $

#include "material/uniaxial/PY/TzSimple1Gen.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <utility/matrix/ID.h> 

//! @brief Libera la memoria reservada.
void XC::TzSimple1Gen::libera(void)
  {
    if(TzEleNum)
      {
        delete[] TzEleNum;
        TzEleNum= nullptr;
      }
    if(TzNode1)
      {
        delete[] TzNode1;
        TzNode1= nullptr;
      }
    if(TzNode2)
      {
        delete[] TzNode2;
        TzNode2= nullptr;
      }
    if(TzMat)
      {
        delete[] TzMat;
        TzMat= nullptr;
      }
    if(TzDir)
      {
        delete[] TzDir;
        TzDir= nullptr;
      }
    if(p_t)
      {
        delete[] p_t;
        p_t= nullptr;
      }
    if(p_b)
      {
        delete[] p_b;
        p_b= nullptr;
      }
    if(ca_t)
      {
        delete[] ca_t;
        ca_t= nullptr;
      }
    if(ca_b)
      {
        delete[] ca_b;
        ca_b= nullptr;
      }
    if(delta_t)
      {
        delete[] delta_t;
        delta_t= nullptr;
      }
    if(delta_b)
      {
        delete[] delta_b;
        delta_b= nullptr;
      }
    if(zMt_t)
      {
        delete[] zMt_t;
        zMt_t= nullptr;
      }
    if(zMt_b)
      {
        delete[] zMt_b;
        zMt_b= nullptr;
      }
    if(mt_val_t)
      {
        delete[] mt_val_t;
        mt_val_t= nullptr;
      }
    if(mt_val_b)
      {
        delete[] mt_val_b;
        mt_val_b= nullptr;
      }
    if(Sa_b)
      {
        delete[] Sa_b;
        Sa_b= nullptr;
      }
    if(Sa_t)
      {
        delete[] Sa_t;
        Sa_t= nullptr;
      }
    if(z50_t)
      {
        delete[] z50_t;
        z50_t= nullptr;
      }
    if(z50_b)
      {
        delete[] z50_b;
        z50_b= nullptr;
      }
    if(tult_t)
      {
        delete[] tult_t;
        tult_t= nullptr;
      }
    if(tult_b)
      {
        delete[] tult_b;
        tult_b= nullptr;
      }
    if(tzType)
      {
        delete[] tzType;
        tzType= nullptr;
      }
    Simple1GenBase::libera();
  }

////////////////////////////////////////////////////////////////////////
// Default Constructor
XC::TzSimple1Gen::TzSimple1Gen(void)
  {
    NumTzEle = 0;
    NPile = 0;
    NumMtLoadSp = 0;
    NumMt = 0;
    p = 0.0;
    zground = 0.0;
    TULT = 0.0;
    Z50 = 0.0;
    ca = 0.0;
    delta = 0.0;
    Sa = 0.0;
  }

/////////////////////////////////////////////////////////////////////////
// Destructor deletes dynamically allocated arrays
XC::TzSimple1Gen::~TzSimple1Gen(void)
  {
    libera();
  }

///////////////////////////////////////////////////////////////////////////////////////
// Function to call appropriate subroutines given input from XC::main
void XC::TzSimple1Gen::WriteTzSimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5)
  { GetTzSimple1(file1, file2, file3, file4, file5); }

void XC::TzSimple1Gen::WriteTzSimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5, const std::string &file6)
  {
    GetTzSimple1(file1, file2, file3, file4, file5);
    GetPattern(file6);
  }

///////////////////////////////////////////////////////////////////////////////////////
// Function to write an output file containing tz materials
// given nodes, pile elements and tz elements
void XC::TzSimple1Gen::GetTzSimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5)
  {

    // Define local variables
    int tzelenum, TzIndex, tzmat= -1, stype= -1, NODENUM= -1;
    double z, maxz, c= -1, mt;
    double ztrib1, ztrib2, dzsub, zsub, depthsub, sublength, tult, z50, numtzshared;
    mt = 1.0;
    std::string mattype= nullptr;

    // Initialize output stream
    std::ofstream TzOut;
    TzOut.open(file5.c_str(), std::ios::out);

    // Write headers for output file
    TzOut << "#######################################################################################" << std::endl;
    TzOut << "##" << std::endl;
    TzOut << "## This file contains XC::TzSimple1 materials associated with pre-defined nodes, zeroLength" << std::endl;
    TzOut << "## elements and pile beam column elements.  The file was created using the program" << std::endl;
    TzOut << "## TzSimple1Gen.cpp written by Scott Brandenberg (sjbrandenberg@ucdavis.edu)" << std::endl;
    TzOut << "##" << std::endl;
    TzOut << "#######################################################################################" << std::endl << std::endl;
    TzOut << "########################################################################################" << std::endl;
    TzOut << "## Material Properties for tz Elements" << std::endl << std::endl;

    // Call functions to open input files
    
    GetSoilProperties(file1);
    GetNodes(file2);
    GetTzElements(file3);
    GetPileElements(file4);
    
    // Loop over nodes
    for(int i=0;i<NumTzEle;i++)
      {
        // Initialize variables to zero.  Note that elements and nodes must be assigned numbers larger than zero
        tzelenum = 0;
        z = 0;
        TzIndex = -1;

        // Find number of tz element that shares the node
        for(int j=0;j<NumNodes;j++)
          {
            if(NodeNum[j] == TzNode1[i]) // Only calculate values for TzNode that also attaches to pile node
              {
                for(int k=0;k<NumPileEle;k++)
                  {
                    if(PileNode1[k] == TzNode1[i] || PileNode2[k] == TzNode1[i])
                      {
                        tzelenum = TzEleNum[i];
                        TzIndex = i;
                        tzmat = TzMat[i];
                        z = Nodey[j];
                        NODENUM = NodeNum[j];
                      }
                  }
              }
            else if(NodeNum[j] == TzNode2[i])
              {
                for(int k=0;k<NumPileEle;k++)
                  {
                    if(PileNode1[k] == TzNode2[i] || PileNode2[k] == TzNode2[i])
                      {
                        tzelenum = TzEleNum[i];
                        TzIndex = i;
                        tzmat = TzMat[i];
                        z = Nodey[j];
                        NODENUM = NodeNum[j];
                      }
                  }
              }
            if(TzIndex == -1)
                continue;
          }

        // Find depth of node
        maxz = z_t[0];
        for(int j=0;j<NumMat;j++)
          {
            if(z_t[j] > maxz)
                maxz = z_t[j];
          }

        depth = maxz - z;

        GetTributaryCoordsTz(NODENUM);
        ztrib1 = tribcoord[0];
        ztrib2 = tribcoord[1];

        // make sure coordinates of tributary length lie within soil layer for assigning tributary lengths
        // for the tz elements
        if(ztrib1 > maxz)
            ztrib1 = maxz;
        if(ztrib2 > maxz)
            ztrib2 = maxz;

        // Calculate tz material properties and write to file
        if(TzIndex != -1)
        {
            // subdivide tributary length into 10 sublayers, and integrate pult over tributary length
            dzsub = (ztrib2 - ztrib1)/10.0; // sublayer incremental depth change
            sublength = fabs(dzsub);  // thickness of sublayer
            tult = 0.0;
            for(int k=0;k<10;k++)
            {            
                zsub = ztrib1 + dzsub/2.0 + k*dzsub; // z-coordinate at sublayer center
                depthsub = maxz - zsub;
            
                // Find properties at node location
                for(int j=0;j<NumMat;j++)
                  {
                    if(zsub<=z_t[j] && zsub>=z_b[j])
                      {
                        mattype = MatType[j];
                        // linearly interpolate parameters at z
                        p = linterp(z_t[j], z_b[j], p_t[j], p_b[j], zsub);
                        ca = linterp(z_t[j], z_b[j], ca_t[j], ca_b[j], zsub);
                        delta = linterp(z_t[j], z_b[j], delta_t[j], delta_b[j], zsub);
                        c= linterp(z_t[j], z_b[j], c_t[j], c_b[j], zsub);
                        TULT = linterp(z_t[j], z_b[j], tult_t[j], tult_b[j], zsub);
                        Z50 = linterp(z_t[j], z_b[j], z50_t[j], z50_b[j], zsub);
                        ru = linterp(z_t[j], z_b[j], ru_t[j], ru_b[j], zsub);
                        Sa = linterp(z_t[j], z_b[j], Sa_t[j], Sa_b[j], zsub);
                        if(mattype == "tz1")
                            stype = 1;
                        else if(mattype == "tz2" || mattype == "tz3")
                            stype = 2;
                        else if(mattype == "tz4")
                            stype = tzType[j];
                        else                    
                          {
                            std::cerr << "MatType must be tz1, tz2, tz3 or tz4.  " << mattype << " is not supported." << std::endl;
                            exit(0);
                          }
                        break;
                      }
                  }
                                
                for(int j=0;j<NumMt;j++)
                  {
                    if(zsub<=zMt_t[j] && zsub>=zMt_b[j])
                        mt = linterp(zMt_t[j], zMt_b[j], mt_val_t[j], mt_val_b[j], zsub);
                    else mt = 1.0;
                  }

                // calculate vertical effective stress and integrate over tributary length
                stress = GetVStress(zsub);
                tult = GetTult(mattype)*sublength*mt + tult;
            }

            z50 = GetZ50(mattype);

            // Calculate the number of t-z elements that share nodes with the current t-z element
            numtzshared = 1.0;
            for(int j=0;j<NumTzEle;j++)
            {
                if(j!=i)
                {
                    if(TzNode1[j] == TzNode1[i] || TzNode1[j] == TzNode2[i])
                        numtzshared += 1.0;
                }
            }

            TzOut << "uniaxialMaterial TzSimple1 " << tzmat << " " << stype << " " << tult/numtzshared << " " << z50 << " " << c << std::endl;
        }
    }

    // Write footer for output file
    TzOut << std::endl << "## End XC::Material Properties for tz Elements" << std::endl;
    TzOut << "########################################################################################" << std::endl;

    TzOut.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Function to get applied constraints
void XC::TzSimple1Gen::GetPattern(const std::string &file6)
{
    double ztrib1, ztrib2, maxz, minz, dzsub, sublength, zsub, depthsub;
    int node;
    double patternvalue, z, load, sp= 0.0;
    std::string patterntype= "trash";
    
    // Now open a stream to construct the constraints
    std::ofstream PatternOut;
    PatternOut.open(file6.c_str(),std::ios::out);

    if(!PatternOut)
      {
        std::cerr << "Error opening " << file6 << " in XC::TzSimple1Gen.cpp.  Must Exit." << std::endl;
        exit(-1);
      }

    patternvalue = 0.0;
    z = 0.0;

        // Write header for constraint file
    PatternOut << "#######################################################################################" << std::endl;
    PatternOut << "##" << std::endl;
    PatternOut << "## This file contains load patterns applied to pile nodes, and/or displacement" << std::endl;
    PatternOut << "## patterns applied to the free ends of tz elements.  The file was created using" << std::endl;
    PatternOut << "## TzSimple1Gen.cpp written by Scott Brandenberg (sjbrandenberg@ucdavis.edu)" << std::endl;
    PatternOut << "##" << std::endl;
    PatternOut << "#######################################################################################" << std::endl << std::endl;
    PatternOut << "#######################################################################################" << std::endl;
    PatternOut << "## Begin Pattern File" << std::endl << std::endl;
    
    // If loads are applied (i.e. PatternType = "load"), then the appropriate loads must be assigned to
    // the pile nodes.  The free ends of the tz elements below the nodal loads (i.e. in the soil
    // that is not spreading) must already be fixed when the mesh is generated in GiD.    
    // Write constraints file for pushover analyses
    
    for(int i=0;i<NumNodes;i++)
      {
        z = Nodey[i];
        GetTributaryCoordsPile(NodeNum[i]);
        ztrib1 = tribcoord[0];
        ztrib2 = tribcoord[1];
    
        // Find depth of node
        maxz = z_t[0];  // initialize maxz to some value in the domain
        minz = z_b[0];
        for(int j=0;j<NumMat;j++)
          {
            if(z_t[j] > maxz)
                maxz = z_t[j];
            if(z_b[j] < minz)
                minz = z_b[j];
          }

            // subdivide tributary length into 10 sublayers, and integrate distributed load over tributary length
        dzsub = (ztrib2 - ztrib1)/10.0; // sublayer incremental depth change
        sublength = fabs(dzsub);  // thickness of sublayer
        load = 0.0;
        for(int k=0;k<10;k++)
          {    
            zsub = ztrib1 + dzsub/2.0 + k*dzsub; // z-coordinate at sublayer center
            depthsub = maxz - zsub;

            for(int j=0;j<NumLoad;j++)
              {                
                if(zsub<=zLoad_t[j] && zsub>=zLoad_b[j])
                  {
                    load = linterp(zLoad_t[j], zLoad_b[j], load_val_t[j], load_val_b[j], zsub)*sublength + load;
                    patterntype= "load";
                  }
                
              }
          }
        node = -1;
        if(patterntype == "load")
          {
            for(int j=0;j<NumPileEle;j++)
              {                    
                if(NodeNum[i] == PileNode1[j] || NodeNum[i] == PileNode2[j])
                  { node = NodeNum[i]; }
              }
            if(node!=-1)
            PatternOut << "load " << node << " 0.0 " << load << " 0.0" << std::endl;
          }
    
        for(int j=0;j<NumSp;j++)
          {
            if(z<=zSp_t[j] && z>=zSp_b[j])
            {
              sp = linterp(zSp_t[j], zSp_b[j], sp_val_t[j], sp_val_b[j], z);
              patterntype= "sp";
            }
          }        
    
        node = -1;
        if(patterntype == "sp")
          {
            for(int k=0;k<NumTzEle;k++)
              {
                if(NodeNum[i] == TzNode1[k] || NodeNum[i] == TzNode2[k])
                  {
                    node = NodeNum[i];
                    // Check if node is free or attached to pile
                    for(int j=0;j<NumPileEle;j++)
                      {
                        if(PileNode1[j] == NodeNum[i] || PileNode2[j] == NodeNum[i])
                          {
                            node = -1;
                            break;
                          }
                      }
                  }
              }
                
        // write to file
            if(node != -1)
                PatternOut << "sp " << node << " 2 " << sp << std::endl;
          }
        
      }

    PatternOut << std::endl << std::endl;
    PatternOut << "## End Tz Pattern File" << std::endl;
    PatternOut << "#######################################################################################" << std::endl;
    PatternOut.close();
  }



//////////////////////////////////////////////////////////////////////////////////
// Function to get tz element numbers, material numbers, and direction tags
void XC::TzSimple1Gen::GetTzElements(const std::string &file)
  {
    int i = 0;
    std::string trash;
    char ch;

    std::ifstream in_file;
    in_file.open(file.c_str(), std::ios::in);

    if(!in_file)
      {
        std::cerr << "File " << file << "does not exist.  Must exit." << std::endl;
        exit(-1);
      }

    NumTzEle = NumRows(file,"element");
    TzEleNum = new int[NumTzEle];
    TzNode1 = new int[NumTzEle];
    TzNode2 = new int[NumTzEle];
    TzMat = new int[NumTzEle];
    TzDir = new int[NumTzEle];

    while(!in_file.eof())
      {
        if(in_file.peek()=='e')
          {
            //in_file.get(trash,8);
            in_file >> trash;
            if(trash == "element")
              {
                in_file >> trash >> TzEleNum[i] >> TzNode1[i] >> TzNode2[i] >> trash >> TzMat[i] >> trash >> TzDir[i];
                i+=1;
              }
            continue;
          }
        while(in_file.get(ch))
          {
            if(ch=='\n')
                break;
          }
      }
    in_file.close();
    return;
  }

//! @brief Function to get soil properties
void XC::TzSimple1Gen::GetSoilProperties(const std::string &file)
  {
    int i = 0;
    int I = 0;
    int J = 0;
    int K = 0;
    std::string OptionalTag;
    
    std::ifstream in1;
    in1.open(file.c_str(), std::ios::in);    
    
    if(!in1)
      {
        std::cerr << "File " << file << "does not exist.  Must exit." << std::endl;
        exit(0);
      }

    // Define number of rows containing properties to define XC::TzSimple1 materials
    NumMat = NumRows(file, "tz1") + NumRows(file, "tz2") + NumRows(file,"tz3") + NumRows(file,"tz4");  // Number of tz materials defined in file
    NumMt = NumRows(file,"mt"); // Number of t-multiplier terms defined in file
    NumSp = NumRows(file,"sp");            // number of applied displacements defined in file
    NumLoad = NumRows(file,"load");     // number of applied distributed loads defined in file
    NumMtLoadSp = NumMt + NumSp + NumLoad + NumRows(file,"Pattern") + NumRows(file,"pattern");        // total number of applied patterns defined in file

    // Dynamically allocate memory for arrays containing information for each soil layer.
    // Arguments general to all layers
    MatType.resize(4);

    z_t = new double[NumMat];
    z_b = new double[NumMat];
    gamma_t = new double[NumMat];
    gamma_b = new double[NumMat];
    p_t = new double[NumMat];
    p_b = new double[NumMat];
    c_t = new double[NumMat];
    c_b = new double[NumMat];
    ca_t = new double[NumMat];
    ca_b = new double[NumMat];
    delta_t = new double[NumMat];
    delta_b = new double[NumMat];
    Sa_t = new double[NumMat];
    Sa_b = new double[NumMat];
    ru_t = new double[NumMat];
    ru_b = new double[NumMat];
    tzType = new int[NumMat];
    tult_t = new double[NumMat];
    tult_b = new double[NumMat];
    z50_t = new double[NumMat];
    z50_b = new double[NumMat];

    // Dynamically allocate memory for arrays containing information for p-multipliers
    zMt_t = new double[NumMt];
    zMt_b = new double[NumMt];
    mt_val_t = new double[NumMt];
    mt_val_b = new double[NumMt];
    
    // Dynamically allocate memory for arrays containing information for load pattern
    zLoad_t = new double[NumLoad];
    zLoad_b = new double[NumLoad];
    load_val_t = new double[NumLoad];
    load_val_b = new double[NumLoad];

    // Dynamically allocate memory for arryas containing information for displacement pattern
    zSp_t = new double[NumSp];
    zSp_b = new double[NumSp];
    sp_val_t = new double[NumSp];
    sp_val_b = new double[NumSp];

    for(i=0;i<NumMat;i++)
      {
        // initialize variables to zero, then redefine later
        c_t[i] = 0;
        c_b[i] = 0;
        ca_t[i] = 0;
        ca_b[i] = 0;
        delta_t[i] = 0;
        delta_b[i] = 0;

        // read in arguments that are common to all material types
        in1 >> MatType[i] >> z_t[i] >> z_b[i] >> gamma_t[i] >> gamma_b[i];
    
        // read in arguments that are specific to certain material types
        if(MatType[i] == "tz1")
          {
            in1 >> p_t[i] >> p_b[i] >> ca_t[i] >> ca_b[i];
            if(in1.peek() != '\n')
                in1 >> c_t[i] >> c_b[i];
          }
        else if(MatType[i] == "tz2")
          {
            in1 >> p_t[i] >> p_b[i] >> delta_t[i] >> delta_b[i];
            if(in1.peek() != '\n')
                in1 >> c_t[i] >> c_b[i];
          }        
        else if(MatType[i] == "tz3")
          {
            in1 >> p_t[i] >> p_b[i] >> delta_t[i] >> delta_b[i] >> Sa_t[i] >> Sa_b[i] >> ru_t[i] >> ru_b[i];
            if(in1.peek() != '\n')    
                in1 >> c_t[i] >> c_b[i];
          }
        else if(MatType[i] == "tz4")
          {
            in1 >> tzType[i] >> tult_t[i] >> tult_b[i] >> z50_t[i] >> z50_b[i];
            if(in1.peek() != '\n')
                in1 >> c_t[i] >> c_b[i];
          }
        else
          {
            std::cerr << "MatType " << MatType[i] << "Is not supported in XC::TzSimple1Gen.cpp.";
            exit(0);
          }
        // read to next line or next character
        if(in1.peek()=='\n')
            in1.ignore(100000,'\n');
        if(in1.peek()==' ')
            in1.ignore(100000,' ');
      }
    
    // Read in values that define patterns (either loads applied directly to the pile nodes, or free-field
    // displacements applied to the backs of the tz elements).
    // Read in values that define patterns (either loads applied directly to the pile nodes, or free-field
    // displacements applied to the backs of the py elements).
    for(int i=0;i<NumMtLoadSp;i++)
      {
        in1 >> OptionalTag;
        if(OptionalTag == "load")
          {
            in1 >> zLoad_t[I] >> zLoad_b[I] >> load_val_t[I] >> load_val_b[I];
            I+=1;
          }
        if(OptionalTag == "sp")
          {
            in1 >> zSp_t[J] >> zSp_b[J] >> sp_val_t[J] >> sp_val_b[J];
            J+=1;
          }
        if(OptionalTag == "mt")
          {
            in1 >> zMt_t[K] >> zMt_b[K] >> mt_val_t[K] >> mt_val_b[K];
            K+=1;
          }
        if(in1.peek()=='\n')
          in1.ignore(100000,'\n');
        if(in1.peek()==' ')
          in1.ignore(100000,' ');
      }
    in1.close();
  }

    
///////////////////////////////////////////////////////////////////////////////////////
// Member function to calculate pult
double XC::TzSimple1Gen::GetTult(const std::string &type)
  {
    double tult_0, tult_1, tult_ru;
    
    if(type == "tz1")// Calculate tult for clay 
      { return ca*p; }
    else if(type == "tz2") // Calculate tult for sand
      {
        if(depth == 0)
            return 0.00001;  // TzSimple1 does not support tult = 0;

        double Ko;
        double deg = 3.141592654/180.0;
        Ko = 0.4; // Use 0.4 like LPile
        return Ko*stress*tan(delta*deg)*p;
      }
    else if(type == "tz3")
      {
        double Ko;
        double deg = 3.141592654/180.0;
        // convert phi, alpha and beta from degrees to radians
        Ko = 0.4; // Use 0.4 like LPile
        tult_0 = Ko*stress*tan(delta*deg)*p;

        tult_1 = Sa*p*stress;

        tult_ru = linterp(0.0, 1.0, tult_0, tult_1, ru);

        return tult_ru;
      }
    else if(type == "tz4")
      { return TULT; }
    else
      {
        std::cerr << "TzType " << type << " is not supported in XC::TzSimple1GenPushover::GetTult.  Setting tult = 0.00000001";
        return 0.00000001;
      }
  }

///////////////////////////////////////////////////////////////////////////////////////
// Member function to return y50
double XC::TzSimple1Gen::GetZ50(const std::string &type)
  {
    if(type == "tz4")
      return Z50;
    else // Set z50 such that zult = 0.5% of pile diameter.  Calculate pile diameter from perimeter for a circular section.
        return 0.005*p/3.14159/8.0;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Member function to calculate vertical effective stress at a depth given the unit weight and depth arrays already read in.
double XC::TzSimple1Gen::GetVStress(double z)
  {
    double stress, maxz, minz, z_top, z_bot, gamma_top, gamma_bot, gamma_z;
    int i;
    stress = 0;
    maxz = z_t[0];
    minz = z_b[0];
    z_top = 0;
    z_bot = 0;
    gamma_top = 0;
    gamma_bot = 0;
    
    // Find maximum and minimum of depth range specified in z_t and z_b
    for(i=0;i<NumMat;i++)
      {
        if(z_t[i] >= maxz)
            maxz = z_t[i];
        if(z_b[i] <= minz)
            minz = z_b[i];
    }

    // Check that z lies within range of z_t and z_b
    if(z > maxz || z < minz)
    {
        std::cerr << "Depth lies out of range of specified depth vectors in function 'vstress' in PySimple1GenPushover. Setting stress = 0." << std::endl;
        return 0.0;
    }


    // Extract coordinates of top and bottom of layer
    for(i=0;i<NumMat;i++)
    {
        if(z >= z_b[i] && z <= z_t[i])
        {
            z_top = z_t[i];
            z_bot = z_b[i];
            gamma_top = gamma_t[i];
            gamma_bot = gamma_b[i];
        }
    }
    


    // Linearly interpolate unit weight at z
    gamma_z = linterp(z_top, z_bot, gamma_top, gamma_bot, z);

    // calculate stress
    for (i=0;i<NumMat;i++)
    {
        if(z <= z_b[i])
            stress = stress + 0.5*(gamma_t[i] + gamma_b[i])*(z_t[i] - z_b[i]);
        if(z > z_b[i] && z < z_t[i])
            stress = stress + 0.5*(gamma_t[i] + gamma_z)*(z_t[i] - z);
    }
    
    return stress;
}

/////////////////////////////////////////////////////////////////////////////////
// Function that returns the coordinates of the ends of the tributary length
// based on t-z element locations.  Tributary length is based on 1/2 of the pile
// length above nodenum1 and 1/2 of the pile length below nodenum1 as long as
// the pile elements above and below nodenum1 both attach to t-z elements at both nodes.
void XC::TzSimple1Gen::GetTributaryCoordsTz(int nodenum1)
  {
    double coordnodenum1= 0.0;
    int tzeletag= 0;

    // initialize tribcoord to the coordinate of nodenum1
    for(int i=0; i<NumNodes; i++)
      {
        if(nodenum1 == NodeNum[i])
          {
            coordnodenum1 = Nodey[i];
            tribcoord[0] = Nodey[i];
            tribcoord[1] = Nodey[i];
          }
      }
    for(int i=0; i<NumPileEle; i++)
      {
        if(PileNode1[i] == nodenum1)
          {
            tzeletag = 0;
            for(int j=0; j<NumTzEle; j++)
              {
                if(TzNode1[j] == PileNode1[i] || TzNode2[j] == PileNode1[i])
                  {
                    for(int k=0; k<NumTzEle; k++)
                      {
                        if(TzNode1[k] == PileNode2[i] || TzNode2[k] == PileNode2[i])
                            tzeletag = 1;  // set pyeletag = 1 if PileNode1 is attached to a py element
                      }
                  }
              }
            if(tzeletag==1)
              {
                for(int j=0; j<NumNodes; j++)
                  {
                    if(PileNode2[i] == NodeNum[j])
                      {
                        tribcoord[0] = coordnodenum1 + 0.5*(Nodey[j] - coordnodenum1);
                      }
                  }
              }
          }
        if(PileNode2[i] == nodenum1)
          {
            tzeletag = 0;
            for(int j=0;j<NumTzEle;j++)
              {
                if(TzNode1[j] == PileNode2[i] || TzNode2[j] == PileNode2[i])
                  {
                    for(int k=0; k<NumTzEle; k++)
                      {
                        if(TzNode1[k] == PileNode1[i] || TzNode2[k] == PileNode1[i])
                            tzeletag = 1;  // set pyeletag = 1 if PileNode2 is attached to a py element
                      }
                  }
              }
            if(tzeletag==1)
              {
                for(int j=0; j<NumNodes; j++)
                  {
                    if(PileNode1[i] == NodeNum[j])
                      { tribcoord[1] = coordnodenum1 + 0.5*(Nodey[j] - coordnodenum1); }
                  }
              }
          }
      }
    return;
  }

/////////////////////////////////////////////////////////////////////////////////
// Function that returns the coordinates of the ends of the tributary length
// based on pile element locations.  Tributary length is based on 1/2 of the pile
// length above nodenum1 and 1/2 of the pile length below nodenum1 even if
// the pile elements above and below nodenum1 do not both attach to p-y elements 
// at both nodes.
void XC::TzSimple1Gen::GetTributaryCoordsPile(int nodenum1)
  {
    
    double coordnodenum1= 0.0;

    // initialize tribcoord to the coordinate of nodenum1
    for(int i=0; i<NumNodes; i++)
      {
        if(nodenum1 == NodeNum[i])
          {
            coordnodenum1 = Nodey[i];
            tribcoord[0] = Nodey[i];
            tribcoord[1] = Nodey[i];
          }
      }
    for(int i=0; i<NumPileEle; i++)
      {
        if(PileNode1[i] == nodenum1)
          {
            for(int j=0; j<NumNodes; j++)
              {
                if(PileNode2[i] == NodeNum[j])
                  { tribcoord[0] = coordnodenum1 + 0.5*(Nodey[j] - coordnodenum1); }
              }
          }
        if(PileNode2[i] == nodenum1)
          {
            for(int j=0; j<NumNodes; j++)
              {
                if(PileNode1[i] == NodeNum[j])
                  { tribcoord[1] = coordnodenum1 + 0.5*(Nodey[j] - coordnodenum1); }
              }
          }
      }
    return;
  }

