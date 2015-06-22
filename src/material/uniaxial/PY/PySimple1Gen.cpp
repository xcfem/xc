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
//  functions for the PySimple1Gen class.  The purpose of the        //
//  class is to create PySimple1 materials associated with            //
//  pre-defined    zeroLength elements, beam column elements, and        //
//    nodes.                                                            //
//                                                                    //
//  Written by: Scott Brandenberg                                    //
//              Graduate Student, UC Davis                            //
//              December 2, 2003                                    //
//////////////////////////////////////////////////////////////////////

#include "material/uniaxial/PY/PySimple1Gen.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <utility/matrix/ID.h>

//! @brief Libera la memoria reservada.
void XC::PySimple1Gen::libera(void)
  {
    if(PyEleNum)
      {
        delete[] PyEleNum;
        PyEleNum= nullptr;
      }
    if(PyNode1)
      {
        delete[] PyNode1;
        PyNode1= nullptr;
      }
    if(PyNode2)
      {
        delete[] PyNode2;
        PyNode2= nullptr;
      }
    if(PyMat)
      {
        delete[] PyMat;
        PyMat= nullptr;
      }
    if(PyDir)
      {
        delete[] PyDir;
        PyDir= nullptr;
      }
    if(Cd_t)
      {
        delete[] Cd_t;
        Cd_t= nullptr;
      }
    if(Cd_b)
      {
        delete[] Cd_b;
        Cd_b= nullptr;
      }
    if(cu_t)
      {
        delete[] cu_t;
        cu_t= nullptr;
      }
    if(cu_b)
      {
        delete[] cu_b;
        cu_b= nullptr;
      }
    if(e50_t)
      {
        delete[] e50_t;
        e50_t= nullptr;
      }
    if(e50_b)
      {
        delete[] e50_b;
        e50_b= nullptr;
      }
    if(phi_t)
      {
        delete[] phi_t;
        phi_t= nullptr;
      }
    if(phi_b)
      {
        delete[] phi_b;
        phi_b= nullptr;
      }
    if(Sr_t)
      {
        delete[] Sr_t;
        Sr_t= nullptr;
      }
    if(Sr_b)
      {
        delete[] Sr_b;
        Sr_b= nullptr;
      }
    if(pult_t)
      {
        delete[] pult_t;
        pult_t= nullptr;
      }
    if(pult_b)
      {
        delete[] pult_b;
        pult_b= nullptr;
      }
    if(y50_t)
      {
        delete[] y50_t;
        y50_t= nullptr;
      }
    if(y50_b)
      {
        delete[] y50_b;
        y50_b= nullptr;
      }
    if(b_t)
      {
        delete[] b_t;
        b_t= nullptr;
      }
    if(b_b)
      {
        delete[] b_b;
        b_b= nullptr;
      }
    if(pyType)
      {
        delete[] pyType;
        pyType= nullptr;
      }
    Simple1GenBase::libera();
  }

////////////////////////////////////////////////////////////////////////
// Constructor initializes global variables to zero
XC::PySimple1Gen::PySimple1Gen(void)
  { 
    NumPyEle = 0;
    NumMpLoadSp = 0;
    NumMp = 0;
    pult = 0.0;
    maxz = 0.0;
    minz = 0.0;
    cu = 0.0;
    e50 = 0.0;
    phi = 0.0;
    sr = 0.0;
    PULT = 0.0;
    Y50 = 0.0;
  }


/////////////////////////////////////////////////////////////////////////
// Destructor deletes dynamically allocated arrays
XC::PySimple1Gen::~PySimple1Gen()
  { libera(); }

///////////////////////////////////////////////////////////////////////////////////////
// Function to call appropriate subroutines given input from XC::main
void XC::PySimple1Gen::WritePySimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5)
  {
    GetPySimple1(file1, file2, file3, file4, file5);
    return;
  }

void XC::PySimple1Gen::WritePySimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5, const std::string &file6)
  {

    GetPySimple1(file1, file2, file3, file4, file5);
    GetPattern(file6);
    return;
  }

///////////////////////////////////////////////////////////////////////////////////////
// Function to write an output file containing the py materials
// given nodes, pile elements and py elements
void XC::PySimple1Gen::GetPySimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5)
  {
    // Define local variables
    int pyelenum, PyIndex, pymat= 0, NODENUM= 0;
    double z, Cd= 0.0, c= 0.0, mp;
    double ztrib1, ztrib2, dzsub, zsub, depthsub, sublength, numpyshared;
    mp = 1.0;
    std::string mattype;
    const std::string py2= "py2";

    // Initialize output stream
    std::ofstream PyOut;
    PyOut.open(file5.c_str(), std::ios::out);

    if(!PyOut)
      {
        std::cerr << "Error opening output stream for :" << file5 << ". Must Exit.";
        exit(-1);
      }

    // Write headers for output file
    PyOut << "#######################################################################################" << std::endl;
    PyOut << "##" << std::endl;
    PyOut << "## This file contains XC::PySimple1 materials associated with pre-defined nodes, zeroLength" << std::endl;
    PyOut << "## elements and pile beam column elements.  The file was created using the program" << std::endl;
    PyOut << "## PySimple1Gen.cpp written by Scott Brandenberg (sjbrandenberg@ucdavis.edu)" << std::endl;
    PyOut << "##" << std::endl;
    PyOut << "#######################################################################################" << std::endl << std::endl;
    PyOut << "########################################################################################" << std::endl;
    PyOut << "## Material Properties for py Elements" << std::endl << std::endl;

    // Call functions to open input files
    GetSoilProperties(file1);
    GetNodes(file2);
    GetPyElements(file3);
    GetPileElements(file4);

    // Loop over nodes
    for(int i=0;i<NumPyEle;i++)
      {
        // Initialize variables to zero.  Note that elements and nodes must be assigned numbers larger than zero
        pyelenum = 0;
        z = 0;
        PyIndex = -1;

        // Find number of py element that shares the node
        for(int j=0;j<NumNodes;j++)
          {
            if(NodeNum[j] == PyNode1[i]) // Only calculate values for PyNode that also attaches to pile node
              {
                for(int k=0;k<NumPileEle;k++)
                  {
                    if(PileNode1[k] == PyNode1[i] || PileNode2[k] == PyNode1[i])
                      {
                                    pyelenum = PyEleNum[i];
                        PyIndex = i;
                        pymat = PyMat[i];
                        z = Nodey[j];
                        NODENUM = NodeNum[j];
                      }
                  }
              }
            else if(NodeNum[j] == PyNode2[i])
              {
                for(int k=0;k<NumPileEle;k++)
                  {
                    if(PileNode1[k] == PyNode2[i] || PileNode2[k] == PyNode2[i])
                      {
                                    pyelenum = PyEleNum[i];
                        PyIndex = i;
                        pymat = PyMat[i];
                        z = Nodey[j];
                        NODENUM = NodeNum[j];
                      }
                  }
              }
          }
        
        if(PyIndex == -1)
          continue;

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

        depth = maxz - z;

        GetTributaryCoordsPy(NODENUM);
        ztrib1 = tribcoord[0];
        ztrib2 = tribcoord[1];

        // make sure coordinates of tributary length lie within soil layer for assigning tributary lengths
        // for the py elements
        if(ztrib1 > maxz)
          ztrib1 = maxz;
        if(ztrib2 > maxz)
          ztrib2 = maxz;
        if(ztrib1 < minz)
          ztrib1 = minz;
        if(ztrib2 < minz)
          ztrib2 = minz;

        // Calculate py material properties and write to file
        if(PyIndex != -1)
          {            
            // Calculate y50 at coordinate z.  This requires calculating pult at z, however, pult
            // will be integrated over the tributary length in sublayers following the calculation of y50.
            for(int j=0;j<NumMat;j++)
              {
                if(z<=z_t[j] && z>=z_b[j])
                  {
                    mattype= MatType[j];
                    if(MatType[j]=="py1")
                      stype = 1;
                    else if((MatType[j]=="py2") || (MatType[j]=="py3"))
                      stype = 2;
                    else if(MatType[j]=="py4")
                      stype = pyType[j];
                    else
                      {
                        std::cerr << "MatType must be py1, py2, py3 or py4.  " << MatType[j] << " is not supported." << std::endl;
                        exit(0);
                      }
                    // linearly interpolate parameters at z
                    b = linterp(z_t[j], z_b[j], b_t[j], b_b[j], z);
                    cu = linterp(z_t[j], z_b[j], cu_t[j], cu_b[j], z);
                    e50 = linterp(z_t[j], z_b[j], e50_t[j], e50_b[j], z);
                    phi = linterp(z_t[j], z_b[j], phi_t[j], phi_b[j], z);
                    sr = linterp(z_t[j], z_b[j], Sr_t[j], Sr_b[j], z);
                    Cd = linterp(z_t[j], z_b[j], Cd_t[j], Cd_b[j], z);
                    c = linterp(z_t[j], z_b[j], c_t[j], c_b[j], z);
                    PULT = linterp(z_t[j], z_b[j], pult_t[j], pult_b[j], z);
                    Y50 = linterp(z_t[j], z_b[j], y50_t[j], y50_b[j], z);
                    ru = linterp(z_t[j], z_b[j], ru_t[j], ru_b[j], z);

                    break;
                  }
              }    
            stress = GetVStress(z);
            pult = GetPult(mattype);

            if(mattype=="py3")
              pult = GetPult(py2);  // use sand py curve to develop y50
            y50 = GetY50(mattype);            

            // subdivide tributary length into 10 sublayers, and integrate pult over tributary length
            dzsub = (ztrib2 - ztrib1)/10.0; // sublayer incremental depth change
            sublength = fabs(dzsub);  // thickness of sublayer
            pult = 0.0;
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
                        if(MatType[j]=="py1")
                          stype = 1;
                        else if((MatType[j]=="py2") || (MatType[j]=="py3"))
                          stype = 2;
                        else if(MatType[j]=="py4")
                          stype = pyType[j];
                        else                    
                          {
                            std::cerr << "MatType must be py1, py2, py3 or py4.  " << mattype << " is not supported." << std::endl;
                            exit(0);
                          }
                        // linearly interpolate parameters at z
                        b = linterp(z_t[j], z_b[j], b_t[j], b_b[j], zsub);
                        cu = linterp(z_t[j], z_b[j], cu_t[j], cu_b[j], zsub);
                        e50 = linterp(z_t[j], z_b[j], e50_t[j], e50_b[j], zsub);
                        phi = linterp(z_t[j], z_b[j], phi_t[j], phi_b[j], zsub);
                        sr = linterp(z_t[j], z_b[j], Sr_t[j], Sr_b[j], zsub);
                        Cd = linterp(z_t[j], z_b[j], Cd_t[j], Cd_b[j], zsub);
                        c = linterp(z_t[j], z_b[j], c_t[j], c_b[j], zsub);
                        PULT = linterp(z_t[j], z_b[j], pult_t[j], pult_b[j], zsub);
                        Y50 = linterp(z_t[j], z_b[j], y50_t[j], y50_b[j], zsub);
                        ru = linterp(z_t[j], z_b[j], ru_t[j], ru_b[j], zsub);
                        break;
                      }
                  }
                for(int j=0;j<NumMp;j++)
                  {
                    if(zsub<=zMp_t[j] && zsub>=zMp_b[j])
                      mp = linterp(zMp_t[j], zMp_b[j], mp_val_t[j], mp_val_b[j], zsub);
                    else mp = 1.0;
                  }
                // calculate vertical effective stress and tributary length
                stress = GetVStress(zsub);

                // integrate pult over each sublayer
                pult = GetPult(mattype)*sublength*mp + pult;
              }

            // calculate the number of p-y elements that share the node
            numpyshared = 1;
            for(int j=0;j<NumPyEle;j++)
              {
                if(j!=i)
                {
                  if(PyNode1[j] == PyNode1[i] || PyNode1[j] == PyNode2[i])
                    numpyshared += 1.0;
                }
              }
            PyOut << "uniaxialMaterial XC::PySimple1 " << pymat << " " << stype << " " << pult/numpyshared << " " << y50 << " " << Cd << " " << c << std::endl;
          }
      }
    // Write footer for output file
    PyOut << std::endl << "## End XC::Material Properties for py Elements" << std::endl;
    PyOut << "########################################################################################" << std::endl;
    PyOut.close();
    return;
  }

///////////////////////////////////////////////////////////////////////////////////////////////
// Function to get applied constraints
void XC::PySimple1Gen::GetPattern(const std::string &file6)
{
    double ztrib1, ztrib2, maxz, minz, dzsub, sublength, zsub, depthsub;
    int node, i, j, k;
    double patternvalue, z, load, sp= 0.0;
    std::string patterntype= "trash";

    // Now open a stream to construct the constraints
    std::ofstream PatOut(file6.c_str(), std::ios::out);
    if(!PatOut)
      {
        std::cerr << "Error opening " << file6 << " in XC::PySimple1Gen.cpp.  Must Exit." << std::endl;
        exit(-1);
      }    

    patternvalue = 0.0;
    z = 0.0;

    // Write header for constraint file
    PatOut << "#######################################################################################" << std::endl;
    PatOut << "##" << std::endl;
    PatOut << "## This file contains either load patterns applied to pile nodes, or displacement" << std::endl;
    PatOut << "## patterns applied to the free ends of py elements.  The file was created using" << std::endl;
    PatOut << "## PySimple1Gen.cpp written by Scott Brandenberg (sjbrandenberg@ucdavis.edu)" << std::endl;
    PatOut << "##" << std::endl;
    PatOut << "#######################################################################################" << std::endl << std::endl;
    PatOut << "#######################################################################################" << std::endl;
    PatOut << "## Begin Pattern File" << std::endl << std::endl;
    
    // If loads are applied (i.e. PatternType = "load"), then the appropriate loads must be assigned to
    // the pile nodes.  The free ends of the py elements below the nodal loads (i.e. in the soil
    // that is not spreading) must already be fixed when the mesh is generated in GiD.            
    
    for(i=0;i<NumNodes;i++)
    {
        z = Nodey[i];
        GetTributaryCoordsPile(NodeNum[i]);
        ztrib1 = tribcoord[0];
        ztrib2 = tribcoord[1];

        // Find depth of node
        maxz = z_t[0];  // initialize maxz to some value in the domain
        minz = z_b[0];
        for (j=0;j<NumMat;j++)
        {
            if(z_t[j] > maxz)
                maxz = z_t[j];
            if(z_b[j] < minz)
                minz = z_b[j];
        }
            
        // subdivide tributary length into 10 sublayers, and integrate distributed load over tributary length
        
        load = 0.0;
        dzsub = (ztrib2 - ztrib1)/10.0; // sublayer incremental depth change
        sublength = fabs(dzsub);  // thickness of sublayer
        for(k=0;k<10;k++)
        {                
            zsub = ztrib1 + dzsub/2.0 + k*dzsub; // z-coordinate at sublayer center
            depthsub = maxz - zsub;

            for(j=0;j<NumLoad;j++)
            {                
                if(zsub<=zLoad_t[j] && zsub>=zLoad_b[j])
                {
                    load = linterp(zLoad_t[j], zLoad_b[j], load_val_t[j], load_val_b[j], zsub)*sublength + load;
                    patterntype="load";
                }
                
            }
        }
        node = -1;
        if(patterntype=="load")
        {
            for(j=0;j<NumPileEle;j++)
            {                    
                if(NodeNum[i] == PileNode1[j] || NodeNum[i] == PileNode2[j])
                {
                    node = NodeNum[i];
                }
            }
            if(node!=-1)
            PatOut << "load " << node << " " << load << " 0.0 0.0" << std::endl;
        }
    
        for(j=0;j<NumSp;j++)
        {
            if(z<=zSp_t[j] && z>=zSp_b[j])
            {
                sp = linterp(zSp_t[j], zSp_b[j], sp_val_t[j], sp_val_b[j], z);
                patterntype="sp";
            }
        }        
    
        node = -1;
        if(patterntype=="sp")
        {
            for(k=0;k<NumPyEle;k++)
            {
                if(NodeNum[i] == PyNode1[k] || NodeNum[i] == PyNode2[k])
                {
                    node = NodeNum[i];
                    // Check if node is free or attached to pile
                    for(j=0;j<NumPileEle;j++)
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
                PatOut << "sp " << node << " 1 " << sp << std::endl;
        }
        
    }                    

    PatOut << std::endl << std::endl;        

    PatOut << "## End Pattern File" << std::endl;
    PatOut << "#######################################################################################" << std::endl;

    PatOut.close();
    return;
    
}

//////////////////////////////////////////////////////////////////////////////////
// Function to get py element numbers, material numbers, and direction tags
void XC::PySimple1Gen::GetPyElements(const std::string &file)
{
    int i = 0;
    std::string trash;
    char ch;

    std::ifstream in3;
    in3.open(file.c_str(), std::ios::in);

    if(!in3)
    {
        std::cerr << "File " << file << "does not exist.  Must exit." << std::endl;
        exit(-1);
    }

    NumPyEle = NumRows(file,"element");
    PyEleNum = new int[NumPyEle];
    PyNode1 = new int[NumPyEle];
    PyNode2 = new int[NumPyEle];
    PyMat = new int[NumPyEle];
    PyDir = new int[NumPyEle];

    while(in3)
      {
        if(in3.peek()=='e')
          {
	    //in3.get(trash,8);
            in3 >> trash;
            if(trash=="element")
              {
                in3 >> trash >> PyEleNum[i] >> PyNode1[i] >> PyNode2[i]
                    >> trash >> PyMat[i] >> trash >> PyDir[i];
                i+=1;
              }
            continue;
        }
        while(in3.get(ch))
        {
            if(ch=='\n')
                break;
        }
    }

    in3.close();
    return;
  }

//////////////////////////////////////////////////////////////////////////////////
// Function to get soil properties
void XC::PySimple1Gen::GetSoilProperties(const std::string &file)
{
    int i = 0;
    int I = 0;
    int J = 0;
    int K = 0;
    std::string OptionalTag;
    char ch;

    std::ifstream in1;
    in1.open(file.c_str(), std::ios::in);

    if(!in1)
    {
        std::cerr << "File " << file << "does not exist.  Must exit." << std::endl;
        exit(0);
    }

    // Define number of rows containing properties to define XC::PySimple1 materials
    NumMat = NumRows(file, "py1") + NumRows(file, "py2") + NumRows(file, "py3") + NumRows(file, "py4");

    // Dynamically allocate memory for arrays containing information for each soil layer.
    // Arguments general to all layers

    MatType.resize(NumMat);
    z_t = new double[NumMat];
    z_b = new double[NumMat];
    gamma_t = new double[NumMat];
    gamma_b = new double[NumMat];
    b_t = new double[NumMat];
    b_b = new double[NumMat];
    Cd_t = new double[NumMat];
    Cd_b = new double[NumMat];
    c_t = new double[NumMat];
    c_b = new double[NumMat];
    cu_t = new double[NumMat];
    cu_b = new double[NumMat];
    e50_t = new double[NumMat];
    e50_b = new double[NumMat];
    phi_t = new double[NumMat];
    phi_b = new double[NumMat];
    Sr_t = new double[NumMat];
    Sr_b = new double[NumMat];
    pyType = new int[NumMat];
    pult_t = new double[NumMat];
     pult_b = new double[NumMat];
    y50_t = new double[NumMat];
    y50_b = new double[NumMat];
    ru_t = new double[NumMat];
    ru_b = new double[NumMat];

    // Calculate number of p-multipliers, load patterns and displacement patterns
    NumMp = NumRows(file,"mp");
    NumLoad = NumRows(file,"load");
    NumSp = NumRows(file,"sp");
    NumMpLoadSp = NumMp + NumLoad + NumSp;

    // Dynamically allocate memory for arrays containing information for p-multipliers
    zMp_t = new double[NumMp];
    zMp_b = new double[NumMp];
    mp_val_t = new double[NumMp];
    mp_val_b = new double[NumMp];

    // Dynamically allocate memory for arrays containing information for load patterns
    zLoad_t = new double[NumLoad];
    zLoad_b = new double[NumLoad];
    load_val_t = new double[NumLoad];
    load_val_b = new double[NumLoad];

    // Dynamically allocate memory for arrays containing information for displacement patterns
    zSp_t = new double[NumSp];
    zSp_b = new double[NumSp];
    sp_val_t = new double[NumSp];
    sp_val_b = new double[NumSp];

    for(i=0;i<NumMat;i++)
    {
        // initialize variables to zero, then redefine later
        z_t[i] = 0;
        z_b[i] = 0;
        gamma_t[i] = 0;
        gamma_b[i] = 0;
        b_t[i] = 0;
        b_t[i] = 0;
        Cd_t[i] = 0;
        Cd_b[i] = 0;
        c_t[i] = 0;
        c_b[i] = 0;
        cu_t[i] = 0;
        cu_b[i] = 0;
        e50_t[i] = 0;
        e50_b[i] = 0;
        phi_t[i] = 0;
        phi_b[i] = 0;
        Sr_t[i] = 0;
        Sr_b[i] = 0;
        pyType[i] = 0;
        pult_t[i] = 0;
        pult_b[i] = 0;
        y50_t[i] = 0;
        y50_b[i] = 0;
        ru_t[i] = 0;
        ru_b[i] = 0;

        // read in arguments that are common to all material types
        //in1.get(MatType[i],4);
        in1 >> MatType[i];
        in1 >> z_t[i] >> z_b[i] >> gamma_t[i] >> gamma_b[i];

        // read in arguments that are specific to certain material types
    
        if(MatType[i]=="py1")
        {
            in1 >> b_t[i] >> b_b[i] >> cu_t[i] >> cu_b[i] >> e50_t[i] >> e50_b[i] >> Cd_t[i] >> Cd_b[i];
            if(in1.peek() != '\n')
                in1 >> c_t[i] >> c_b[i];
        }
        
        else if(MatType[i]=="py2")
        {
            in1 >> b_t[i] >> b_b[i] >> phi_t[i] >> phi_b[i] >> Cd_t[i] >> Cd_b[i];
            if(in1.peek() != '\n')
                in1 >> c_t[i] >> c_b[i];
        }
        
        else if(MatType[i]=="py3")
        {
            in1 >> b_t[i] >> b_b[i] >> phi_t[i] >> phi_b[i] >> Sr_t[i] >> Sr_b[i] >> ru_t[i] >> ru_b[i] >> Cd_t[i] >> Cd_b[i];
            if(in1.peek() != '\n')
                in1 >> c_t[i] >> c_b[i];
        }
                
        else if(MatType[i]=="py4")
        {
            in1 >> pyType[i] >> pult_t[i] >> pult_b[i] >> y50_t[i] >> y50_b[i] >> Cd_t[i] >> Cd_b[i];
            if(in1.peek() != '\n')
                in1 >> c_t[i] >> c_b[i];
        }
        
        else
        {
            std::cerr << "Invalid MatType in XC::PySimple1Gen.cpp.";
            exit(0);
        }
        while(in1.get(ch))
        {
            if(ch=='\n')
                break;
        }
    }

    // Read in values that define patterns (either loads applied directly to the pile nodes, or free-field
    // displacements applied to the backs of the py elements).

    for(i=0;i<NumMpLoadSp;i++)
    {
        in1 >> OptionalTag;
        if(OptionalTag=="load")
        {
            in1 >> zLoad_t[I] >> zLoad_b[I] >> load_val_t[I] >> load_val_b[I];
            I+=1;
        }
        if(OptionalTag=="sp")
        {
            in1 >> zSp_t[J] >> zSp_b[J] >> sp_val_t[J] >> sp_val_b[J];
            J+=1;
        }
        if(OptionalTag=="mp")
        {
            in1 >> zMp_t[K] >> zMp_b[K] >> mp_val_t[K] >> mp_val_b[K];
            K+=1;
        }
        while(in1.get(ch))
        {
            if(ch=='\n')
                break;
        }
    }

    in1.close();
    return;
}

    
///////////////////////////////////////////////////////////////////////////////////////
// Member function to calculate pult
double XC::PySimple1Gen::GetPult(const std::string &type)
  {
    double alpha, beta, Ko, Ka, pu1, pu2, A= 0.0;
    double deg = 3.141592654/180;
    double pult_0, pult_1, pult_ru;
    
    // Calculate pult for Matlock soft clay
    // note: strength = cu for clay
    if(type=="py1")
      {
        if(3 + stress/cu + 0.5/b*depth > 9)
          return 9*cu*b;
        else
          return (3 + stress/cu + 0.5/b*depth)*cu*b;
      }
    // Calculate pult for API sand
    // node: strength = phi (deg) for sand
    else if(type=="py2")
      {
        if(depth == 0)
          return 0.00001;
        
        alpha = phi/2;
        beta = 45 + phi/2;
        // convert phi, alpha and beta from degrees to radians
        phi = phi;
        alpha = alpha;
        beta = beta;
        Ko = 0.4; // Use 0.4 like LPile
        Ka = pow(tan(45*deg - alpha*deg),2.0);
        pu1 = stress*(Ko*depth*tan(phi*deg)*sin(beta*deg)/(tan(beta*deg-phi*deg)*cos(alpha*deg))+tan(beta*deg)/(tan(beta*deg-phi*deg))*(b+depth*tan(beta*deg)*tan(alpha*deg))+Ko*depth*tan(beta*deg)*(tan(phi*deg)*sin(beta*deg) - tan(alpha*deg)) - Ka*b);
        pu2 = Ka*b*stress*(pow(tan(beta*deg),8.0) - 1.0) + Ko*b*stress*tan(phi*deg)*pow(tan(beta*deg),4.0);

/////////////////////////////////////////////////////////////////////////////
//      Curve fitting of Figure 3.25 in LPile+4m technical manual results
//      in a smooth pult vs. depth curve
//
        if(depth < 5*b)
          A = 0.032*pow((5-depth/b),2.6)+0.88;
        else if(depth >= 5*b)
          A = 0.88;

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//        Original equations used in LPile generate a kink in pult vs. depth 
//
//        A = (3.0 - 0.8*depth/b);
//        if (A < 0.9)
//            A = 0.9;
//
//////////////////////////////////////////////////////////////////////////////
    
        if (pu1 > pu2)
            return pu2*A;
        else
            return pu1*A;
    }

    // Calculate pult for liquefied sand
    // note: strength = ?? for liquefied sand
    else if(type=="py3")
    {
        if(depth == 0)
        return 0.00001;
        
        alpha = phi/2;
        beta = 45 + phi/2;
        // convert phi, alpha and beta from degrees to radians
        phi = phi;
        alpha = alpha;
        beta = beta;
        Ko = 0.4; // Use 0.4 like LPile
        Ka = pow(tan(45*deg - alpha*deg),2.0);
        pu1 = stress*(Ko*tan(phi*deg)*sin(beta*deg)/(tan(beta*deg-phi*deg)*cos(alpha*deg))+tan(beta*deg)/(tan(beta*deg-phi*deg))*(b+depth*tan(beta*deg)*tan(alpha*deg))+Ko*depth*tan(beta*deg)*(tan(phi*deg)*sin(beta*deg) - tan(alpha*deg)) - Ka*b);
        pu2 = Ka*b*stress*(pow(tan(beta*deg),8.0) - 1.0) + Ko*b*stress*tan(phi*deg)*pow(tan(beta*deg),4.0);

/////////////////////////////////////////////////////////////////////////////
//      Curve fitting of Figure 3.25 in LPile+4m technical manual results
//      in a smooth pult vs. depth curve
//

        if(depth < 5*b)
            A = 0.032*pow((5-depth/b),2.6)+0.88;
        else if(depth >= 5*b)
            A = 0.88;

//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//        Original equations used in LPile generate a kink in pult vs. depth 
//
//        A = (3.0 - 0.8*depth/b);
//        if (A < 0.9)
//            A = 0.9;
//
//////////////////////////////////////////////////////////////////////////////
        
        if(pu1 > pu2)
            pult_0 = pu2*A;
        else
            pult_0 = pu1*A;
        pult_1 = 9.0*sr*stress*b;
        
        pult_ru = linterp(0.0, 1.0, pult_0, pult_1, ru);

        return pult_ru;
    }

    // Calculate pult for pile cap
    // note: strength = total load on pile cap divided by pile cap height
    else if(type=="py4")
    {
        return PULT;
    }

    else
    {
        std::cerr << "Invalid py type in XC::PySimple1GenPushover::GetPult.  Setting pult = 0";
        return 0.0;
    }

}

///////////////////////////////////////////////////////////////////////////////////////
// Member function to return y50
double XC::PySimple1Gen::GetY50(const std::string &type)
{
    double csigma = sqrt(50/stress);  // correction factor for overburden
    if(depth == 0)
        csigma = 1;  // avoid divide by zero stress error

    // Calculate y50 for clay (strain = e50 for clay)
    if(type=="py1")
        return 2.5*b*e50;

    // Calculate y50 for API sand
    else if(type=="py2")
    {
        // Avoid divide by zero error
        if (depth == 0)
        {
            return 0.00001;
        }

        double k;
        // Curve fitting was performed based on figure 3.29 in Reese et al. (2000).  Conversion from pci to kN/m3 is 271.447.
        k = (0.3141*pow(phi,3) - 32.114*pow(phi,2) + 1109.2*phi - 12808)*271.447;
        k = k*csigma;
        return 0.549*pult/k/depth;
    
    }
    // Calculate y50 for liquefied sand as the same as for API sand
    else if(type=="py3")
    {
        // Avoid divide by zero error
        if (depth == 0)
        {
            return 0.00001;
        }

        double k;
        // Curve fitting was performed based on figure 3.29 in Reese et al. (2000).  Conversion from pci to kN/m3 is 271.447.
        k = (0.3141*pow(phi,3) - 32.114*pow(phi,2) + 1109.2*phi - 12808)*271.447;
        k = k*csigma;

        return 0.549*pult/k/depth;
    
    }

    // Get y50 for pile cap
    else if(type=="py4")
        return Y50;

    // Return error message if py type is not found
    else
    {
        std::cerr << "Invalid py type in XC::PySimple1GenPushover::GetY50.  Setting y50 = 0";
        return 0.0;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
// Member function to calculate vertical effective stress at a depth given the unit weight and depth arrays already read in.
double XC::PySimple1Gen::GetVStress(double z)
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
    for (i=0;i<NumMat;i++)
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
// based on p-y element locations.  Tributary length is based on 1/2 of the pile
// length above nodenum1 and 1/2 of the pile length below nodenum1 as long as
// the pile elements above and below nodenum1 both attach to p-y elements at both nodes.
void XC::PySimple1Gen::GetTributaryCoordsPy(int nodenum1)
  {
    
    double coordnodenum1= 0.0;
    int pyeletag= 0;

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
            pyeletag = 0;
            for(int j=0; j<NumPyEle; j++)
              {
                if(PyNode1[j] == PileNode1[i] || PyNode2[j] == PileNode1[i])
                  {
                    for(int k=0; k<NumPyEle; k++)
                      {
                        if(PyNode1[k] == PileNode2[i] || PyNode2[k] == PileNode2[i])
                            pyeletag = 1;  // set pyeletag = 1 if PileNode1 is attached to a py element
                      }
                  }
              }
            if(pyeletag==1)
              {
                for(int j=0; j<NumNodes; j++)
                  {
                    if(PileNode2[i] == NodeNum[j])
                      { tribcoord[0] = coordnodenum1 + 0.5*(Nodey[j] - coordnodenum1); }
                  }
              }
          }
        if(PileNode2[i] == nodenum1)
          {
            pyeletag = 0;
            for(int j=0;j<NumPyEle;j++)
              {
                if(PyNode1[j] == PileNode2[i] || PyNode2[j] == PileNode2[i])
                  {
                    for(int k=0; k<NumPyEle; k++)
                      {
                        if(PyNode1[k] == PileNode1[i] || PyNode2[k] == PileNode1[i])
                            pyeletag = 1;  // set pyeletag = 1 if PileNode2 is attached to a py element
                      }
                  }
              }
            if(pyeletag==1)
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
void XC::PySimple1Gen::GetTributaryCoordsPile(int nodenum1)
  { 
    double coordnodenum1= 0.0;

    // initialize tribcoord to the coordinate of nodenum1
    for(int i=0; i<NumNodes; i++)
      {
        if(nodenum1 == NodeNum[i])
          {
            coordnodenum1= Nodey[i];
            tribcoord[0]= Nodey[i];
            tribcoord[1]= Nodey[i];
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
