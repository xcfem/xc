//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// COPYRIGHT (C):     :-))
// PROJECT:           Object Oriented Finite XC::Element Program
// FILE:              TwentyNodeBrick.cpp
// CLASS:             TwentyNodeBrick
// MEMBER FUNCTIONS:
//
// MEMBER VARIABLES
//
// PURPOSE:           Finite XC::Element Class
// RETURN:
// VERSION:
// LANGUAGE:          C++
// TARGET OS:         DOS || UNIX || . . .
// DESIGNER:          Boris Jeremic, Zhaohui Yang and Xiaoyan Wu
// PROGRAMMER:        Boris Jeremic, Zhaohui Yang  and Xiaoyan Wu
// DATE:              Aug. 2001
// UPDATE HISTORY:    May 2004 Guanzhou added update()
//
//
//
///////////////////////////////////////////////////////////////////////////////
//

#ifndef TWENTYNODEBRICK_CPP
#define TWENTYNODEBRICK_CPP

#include <material/nD/NDMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include "utility/matrix/nDarray/BJmatrix.h"
#include <domain/domain/Domain.h>
#include <cstring>
#include <domain/mesh/element/utils/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include <domain/mesh/node/Node.h>

#include <domain/mesh/element/volumetric/20nbrick/TwentyNodeBrick.h>
#include <domain/load/ElementalLoad.h>
#include "domain/load/volumetric/BrickSelfWeight.h"
#include <material/nD/template_3d_ep/MatPoint3D.h>
#include <material/nD/template_3d_ep/Template3Dep.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>

#include <material/nD/NDMaterial.h>
#include <domain/mesh/element/utils/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussQuadRule1d.h>

#include <utility/matrix/nDarray/straint.h>
#include <utility/matrix/nDarray/stresst.h>
#include <cstring>
#include "material/nD/NDMaterialType.h"

#define FixedOrder 3


namespace XC{
 XC::Matrix XC::TwentyNodeBrick::K(60, 60);
 XC::Matrix XC::TwentyNodeBrick::C(60, 60);
 XC::Matrix XC::TwentyNodeBrick::M(60, 60);
 XC::Vector XC::TwentyNodeBrick::P(60);
 XC::Vector Info(109+3);  //For computing moment
 XC::Vector InfoPt(FixedOrder*FixedOrder*FixedOrder*4+1); //Plastic info
 XC::Vector InfoSt(FixedOrder*FixedOrder*FixedOrder*6+1); //Stress info
 XC::Vector Gsc(FixedOrder*FixedOrder*FixedOrder*3+1); //Gauss point coordinates
 XC::Vector InfoSpq2(2); //p and q of count/2
} //namespace XC


//! @brief Constructor
XC::TwentyNodeBrick::TwentyNodeBrick(int element_number,
                               int node_numb_1,  int node_numb_2,  int node_numb_3,  int node_numb_4,
                               int node_numb_5,  int node_numb_6,  int node_numb_7,  int node_numb_8,
                               int node_numb_9,  int node_numb_10, int node_numb_11, int node_numb_12,
                               int node_numb_13, int node_numb_14, int node_numb_15, int node_numb_16,
                               int node_numb_17, int node_numb_18, int node_numb_19, int node_numb_20,
                               NDMaterial *Globalmmodel, const BodyForces3D &bForces,
                               double r, double p)

  :ElementBase<20>(element_number, ELE_TAG_TwentyNodeBrick ),
  Ki(0), bf(bForces), rho(r), pressure(p)
  {
    load.reset(60);
    //elem_numb = element_number;
    determinant_of_Jacobian = 0.0;

    //r_integration_order = r_int_order;
    //s_integration_order = s_int_order;
    //t_integration_order = t_int_order;
    r_integration_order = FixedOrder; // Gauss-Legendre integration order in r direction
    s_integration_order = FixedOrder; // Gauss-Legendre integration order in s direction
    t_integration_order = FixedOrder; // Gauss-Legendre integration order in t direction

    //Not needed. Right now we have one XC::NDMaterial for each material point
    //mmodel = Globalmmodel->getCopy( type ); // One global mat model

    int total_number_of_Gauss_points = r_integration_order*s_integration_order*t_integration_order;


    if( total_number_of_Gauss_points != 0 )
      {
        matpoint  = new MatPoint3D * [total_number_of_Gauss_points];
      }
    else
      {
        matpoint  = 0;
      }
    ////////////////////////////////////////////////////////////////////
    short where = 0;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        double r = get_Gauss_p_c( r_integration_order, GP_c_r );
        double rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            double s = get_Gauss_p_c( s_integration_order, GP_c_s );
            double sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                double t = get_Gauss_p_c( t_integration_order, GP_c_t );
                double tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                //XC::DB::printf("\n\nBefore Initialization **************** where = %d \n",where);
                //XC::DB::printf("GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //DB            GP_c_r,GP_c_s,GP_c_t);
                //DB
                //DBGPstress[where].reportshort("stress within before Initialization");
                //DBGPstrain[where].reportshort("strain within before Initialization");
                //DB
                //DB// I suspect that [] should be overloaded so that compiler knows which
                //DB// material model is returning a pointer and for the purpose
                //DB//matpoint[where].report("mmodel within before Initialization");
                //DB//matpoint[where].report("mmodel within before Initialization"); // operator[] overloaded
                //DB(matpoint)->operator[](where).report("mmodel within before Initialization"); // operator[] overloaded
                //DB                                                               // for XC::NDMaterial and
                //DB                                                               // derived types!

                matpoint[where] = new MatPoint3D(GP_c_r,
                                                 GP_c_s,
                                                 GP_c_t,
                                                 r, s, t,
                                                 rw, sw, tw,
                                               //InitEPS,
                                                 Globalmmodel);
      //NMD);
      //&( GPstress[where] ), //&( GPiterative_stress[where] ), //IN_q_ast_iterative[where] ,//&( GPstrain[where] ),  //&( GPtangent_E[where] ),
                                         //&( (matpoint)->operator[](where) )
                                         // ugly syntax but it works! Still don't know what's wrong   // with the old style matpoint[where]
              }
          }
      }

      // Set connected external node IDs
    theNodes.set_id_nodes(node_numb_1,node_numb_2,node_numb_3,node_numb_4,node_numb_5,node_numb_6,node_numb_7,node_numb_8,node_numb_9,node_numb_10,node_numb_11,node_numb_12,node_numb_13,node_numb_14,node_numb_15,node_numb_16,node_numb_17,node_numb_18,node_numb_19,node_numb_20);

  }

//====================================================================
XC::TwentyNodeBrick::TwentyNodeBrick ():ElementBase<20>(0, ELE_TAG_TwentyNodeBrick ),
Ki(0), bf(), rho(0.0), pressure(0.0), mmodel(0)
  {
    load.reset(60);
    matpoint = 0;
  }


//#############################################################################

//! @brief Virtual constructor.
XC::Element* XC::TwentyNodeBrick::getCopy(void) const
  { return new TwentyNodeBrick(*this); }


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
XC::TwentyNodeBrick::~TwentyNodeBrick ()
{

    int total_number_of_Gauss_points = r_integration_order*s_integration_order*t_integration_order;

    for(int i = 0; i < total_number_of_Gauss_points; i++)
    {
 // Delete the NDMaterials at each integration point
 if(matpoint[i])
    delete matpoint[i];
    }

    // Delete the array of pointers to XC::NDMaterial pointer arrays
    if(matpoint)
     delete [] matpoint;

    if(Ki != 0)
      delete Ki;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void XC::TwentyNodeBrick::incremental_Update()
  {
    double r  = 0.0;
    // double rw = 0.0;
    double s  = 0.0;
    // double sw = 0.0;
    double t  = 0.0;
    // double tw = 0.0;

    short where = 0;
    //,,,,,    double weight = 0.0;

    //double this_one_PP = (matpoint)->operator[](where).IS_Perfect_Plastic();

    const std::vector<int> dh_dim({20,3});
    BJtensor dh(dh_dim, 0.0);


    static const std::vector<int> disp_dim({20,3});
    BJtensor incremental_displacements(disp_dim,0.0);

    straintensor incremental_strain;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    incremental_displacements = incr_disp();

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        //--        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            //--            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
            {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                //--                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");
                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");
                // determinant of Jacobian XC::BJtensor ( matrix )
                //--                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal = dh("ij") * JacobianINV("jk"); // Zhaohui 09-02-2001
                dhGlobal = dh("ij") * JacobianINV("kj");
                //....                dhGlobal.print("dh","dhGlobal");
                //weight
                //                weight = rw * sw * tw * det_of_Jacobian;
                //::::   ::printf("\n\nIN THE STIFFNESS TENSOR INTEGRATOR ----**************** where = %d \n", where);
                //::::   ::printf(" void XC::TwentyNodeBrick::incremental_Update()\n");
                //::::   ::printf(" GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d    --->>>  where = %d \n",
                //::::                      GP_c_r,GP_c_s,GP_c_t,where);
                //::::   ::printf("WEIGHT = %f", weight);
                //::::   ::printf("determinant of Jacobian = %f", determinant_of_Jacobian);
                //::::   matpoint[where].report("Gauss Point\n");
                // incremental strains at this Gauss point
                // now in Update we know the incremental displacements so let's find
                // the incremental strain
                incremental_strain =
                    (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                incremental_strain.null_indices();
                //incremental_strain.reportshort("\n incremental_strain XC::BJtensor at GAUSS point\n");

                // here comes the final_stress calculation actually on only needs to copy stresses
                // from the iterative data . . .
                //(GPstress+where)->reportshortpqtheta("\n stress START GAUSS \n");

  if( ! ( (matpoint[where]->matmodel)->setTrialStrainIncr( incremental_strain)) )
    std::cerr << "XC::TwentyNodeBrick::incremental_Update (tag: " << this->getTag() << "), not converged\n";
  //matpoint[where].setEPS( mmodel->getEPS() );
            }
          }
      }
  }


//#############################################################################
//#############################################################################
//***************************************************************
 XC::BJtensor XC::TwentyNodeBrick::H_3D(double r1, double r2, double r3)
  {

    const std::vector<int> dimension({60,3});

    BJtensor H(dimension, 0.0);

    // influence of the node number 20
        H(58,1)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
        H(59,2)=H(58,1); //(1.0+r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
        H(60,3)=H(58,1); //(1.0+r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 19
        H(55,1)=(1.0-r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
        H(56,2)=H(55,1); //(1.0-r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
        H(57,3)=H(55,1); //(1.0-r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 18
        H(52,1)=(1.0-r1)*(1.0+r2)*(1.0-r3*r3)*0.25;
        H(53,2)=H(52,1); //(1.0-r1)*(1.0+r2)*(1.0-r3*r3)*0.25;
        H(54,3)=H(52,1); //(1.0-r1)*(1.0+r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 17
        H(49,1)=(1.0+r1)*(1.0+r2)*(1.0-r3*r3)*0.25;
        H(50,2)=H(49,1); //(1.0+r1)*(1.0+r2)*(1.0-r3*r3)*0.25;
        H(51,3)=H(49,1); //(1.0+r1)*(1.0+r2)*(1.0-r3*r3)*0.25;

    // influence of the node number 16
        H(46,1)=(1.0+r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
        H(47,2)=H(46,1); //(1.0+r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
        H(48,3)=H(46,1); //(1.0+r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
    // influence of the node number 15
        H(43,1)=(1.0-r1*r1)*(1.0-r2)*(1.0-r3)*0.25;
        H(44,2)=H(43,1); //(1.0-r1*r1)*(1.0-r2)*(1.0-r3)*0.25;
        H(45,3)=H(43,1); //(1.0-r1*r1)*(1.0-r2)*(1.0-r3)*0.25;
    // influence of the node number 14
        H(40,1)=(1.0-r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
        H(41,2)=H(40,1); //(1.0-r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
        H(42,3)=H(40,1); //(1.0-r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
    // influence of the node number 13
        H(37,1)=(1.0-r1*r1)*(1.0+r2)*(1.0-r3)*0.25;
        H(38,2)=H(37,1); //(1.0-r1*r1)*(1.0+r2)*(1.0-r3)*0.25;
        H(39,3)=H(37,1); //(1.0-r1*r1)*(1.0+r2)*(1.0-r3)*0.25;

    // influence of the node number 12
        H(34,1)=(1.0+r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
        H(35,2)=H(34,1); //(1.0+r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
        H(36,3)=H(34,1); //(1.0+r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
    // influence of the node number 11
        H(31,1)=(1.0-r1*r1)*(1.0-r2)*(1.0+r3)*0.25;
        H(32,2)=H(31,1); //(1.0-r1*r1)*(1.0-r2)*(1.0+r3)*0.25;
        H(33,3)=H(31,1); //(1.0-r1*r1)*(1.0-r2)*(1.0+r3)*0.25;
    // influence of the node number 10
        H(28,1)=(1.0-r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
        H(29,2)=H(28,1); //(1.0-r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
        H(30,3)=H(28,1); //(1.0-r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
    // influence of the node number 9
        H(25,1)=(1.0-r1*r1)*(1.0+r2)*(1.0+r3)*0.25;
        H(26,2)=H(25,1); //(1.0-r1*r1)*(1.0+r2)*(1.0+r3)*0.25;
        H(27,3)=H(25,1); //(1.0-r1*r1)*(1.0+r2)*(1.0+r3)*0.25;


    // 9-20 nodes

    // influence of the node number 8
    H(22,1)=(1.0+r1)*(1.0-r2)*(1.0-r3)*0.125 - (H(43,1)+H(48,3)+H(60,3))*0.5;
    H(23,2)=H(22,1); //(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0 - (H(43,1)+H(48,3)+H(60,3))/2.0;
    H(24,3)=H(22,1); //(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0 - (H(43,1)+H(48,3)+H(60,3))/2.0;
    // influence of the node number 7
    H(19,1)=(1.0-r1)*(1.0-r2)*(1.0-r3)*0.125 - (H(42,3)+H(43,1)+H(57,3))*0.5;
    H(20,2)=H(19,1); //(1.0-r1)*(1.0-r2)*(1.0-r3)/8.0 - (H(42,3)+H(43,1)+H(57,3))/2.0;
    H(21,3)=H(19,1); //(1.0-r1)*(1.0-r2)*(1.0-r3)/8.0 - (H(42,3)+H(43,1)+H(57,3))/2.0;
    // influence of the node number 6
    H(16,1)=(1.0-r1)*(1.0+r2)*(1.0-r3)*0.125 - (H(39,3)+H(42,3)+H(54,3))*0.5;
    H(17,2)=H(16,1); //(1.0-r1)*(1.0+r2)*(1.0-r3)/8.0 - (H(39,3)+H(42,3)+H(54,3))/2.0;
    H(18,3)=H(16,1); //(1.0-r1)*(1.0+r2)*(1.0-r3)/8.0 - (H(39,3)+H(42,3)+H(54,3))/2.0;
    // influence of the node number 5
    H(13,1)=(1.0+r1)*(1.0+r2)*(1.0-r3)*0.125 - (H(39,3)+H(48,3)+H(51,3))*0.5;
    H(14,2)=H(13,1); //(1.0+r1)*(1.0+r2)*(1.0-r3)/8.0 - (H(39,3)+H(48,3)+H(51,3))/2.0;
    H(15,3)=H(13,1); //(1.0+r1)*(1.0+r2)*(1.0-r3)/8.0 - (H(39,3)+H(48,3)+H(51,3))/2.0;

    // influence of the node number 4
    H(10,1)=(1.0+r1)*(1.0-r2)*(1.0+r3)*0.125 - (H(33,3)+H(36,3)+H(60,3))*0.5;
    H(11,2)=H(10,1); //(1.0+r1)*(1.0-r2)*(1.0+r3)/8.0 - (H(33,3)+H(36,3)+H(60,3))/2.0;
    H(12,3)=H(10,1); //(1.0+r1)*(1.0-r2)*(1.0+r3)/8.0 - (H(33,3)+H(36,3)+H(60,3))/2.0;
    // influence of the node number 3
    H(7,1) =(1.0-r1)*(1.0-r2)*(1.0+r3)*0.125 - (H(30,3)+H(33,3)+H(57,3))*0.5;
    H(8,2) =H(7,1); //(1.0-r1)*(1.0-r2)*(1.0+r3)/8.0 - (H(30,3)+H(33,3)+H(57,3))/2.0;
    H(9,3) =H(7,1); //(1.0-r1)*(1.0-r2)*(1.0+r3)/8.0 - (H(30,3)+H(33,3)+H(57,3))/2.0;
    // influence of the node number 2
    H(4,1) =(1.0-r1)*(1.0+r2)*(1.0+r3)*0.125 - (H(30,3)+H(54,3)+H(27,3))*0.5;
    H(5,2) =H(4,1); //(1.0-r1)*(1.0+r2)*(1.0+r3)/8.0 - (H(30,3)+H(54,3)+H(27,3))/2.0;
    H(6,3) =H(4,1); //(1.0-r1)*(1.0+r2)*(1.0+r3)/8.0 - (H(30,3)+H(54,3)+H(27,3))/2.0;
    // influence of the node number 1
    H(1,1) =(1.0+r1)*(1.0+r2)*(1.0+r3)*0.125 - (H(36,3)+H(51,3)+H(27,3))*0.5;
    H(2,2) =H(1,1); //(1.0+r1)*(1.0+r2)*(1.0+r3)/8.0 - (H(36,3)+H(51,3)+H(27,3))/2.0;
    H(3,3) =H(1,1); //(1.0+r1)*(1.0+r2)*(1.0+r3)/8.0 - (H(36,3)+H(51,3)+H(27,3))/2.0;

    //         double sum = 0;
    //
    //  for(int i=1; i<=60 ; i++)
    //           {
    // //       sum+=H(i,1);
    //      for(int j=1; j<= 1; j++)
    //         {
    //                   sum+=H(i,1);
    //            ::printf( "  %+9.2e", H(i,j) );
    //          }
    //            // ::printf( "  %d \n", i);
    //     }
    //      ::printf( " \n sum= %+6.2e\n", sum );


    //    printf("r1 = %lf, r2 = %lf, r3 = %lf\n", r1, r2, r3);
    //    H.print("h");

    return H;
  }

//#############################################################################
//***************************************************************
 XC::BJtensor XC::TwentyNodeBrick::interp_poli_at(double r1, double r2, double r3)
  {

    const std::vector<int> dimension({20});
    BJtensor h(dimension, 0.0);


    // influence of the node number 20
    //    h(20)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
        h(20)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 19
        h(19)=(1.0-r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 18
        h(18)=(1.0-r1)*(1.0+r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 17
        h(17)=(1.0+r1)*(1.0+r2)*(1.0-r3*r3)*0.25;

    // influence of the node number 16
        h(16)=(1.0+r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
    // influence of the node number 15
        h(15)=(1.0-r1*r1)*(1.0-r2)*(1.0-r3)*0.25;
    // influence of the node number 14
        h(14)=(1.0-r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
    // influence of the node number 13
        h(13)=(1.0-r1*r1)*(1.0+r2)*(1.0-r3)*0.25;

    // influence of the node number 12
        h(12)=(1.0+r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
    // influence of the node number 11
        h(11)=(1.0-r1*r1)*(1.0-r2)*(1.0+r3)*0.25;
    // influence of the node number 10
        h(10)=(1.0-r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
    // influence of the node number 9
        h( 9)=(1.0-r1*r1)*(1.0+r2)*(1.0+r3)*0.25;

      // influence of the node number 8
    //h(8)=(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0 - (h(15)+h(16)+h(20))/2.0;
    h(8)=(1.0+r1)*(1.0-r2)*(1.0-r3)*0.125 - (h(15)+h(16)+h(20))*0.5;
      // influence of the node number 7
    h(7)=(1.0-r1)*(1.0-r2)*(1.0-r3)*0.125 - (h(14)+h(15)+h(19))*0.5;
      // influence of the node number 6
    h(6)=(1.0-r1)*(1.0+r2)*(1.0-r3)*0.125 - (h(13)+h(14)+h(18))*0.5;
      // influence of the node number 5
    h(5)=(1.0+r1)*(1.0+r2)*(1.0-r3)*0.125 - (h(13)+h(16)+h(17))*0.5;

      // influence of the node number 4
    h(4)=(1.0+r1)*(1.0-r2)*(1.0+r3)*0.125 - (h(11)+h(12)+h(20))*0.5;
      // influence of the node number 3
    h(3)=(1.0-r1)*(1.0-r2)*(1.0+r3)*0.125 - (h(10)+h(11)+h(19))*0.5;
      // influence of the node number 2
    h(2)=(1.0-r1)*(1.0+r2)*(1.0+r3)*0.125 - (h(10)+h(18)+h( 9))*0.5;
      // influence of the node number 1
    h(1)=(1.0+r1)*(1.0+r2)*(1.0+r3)*0.125 - (h(12)+h(17)+h( 9))*0.5;
    //    printf("r1 = %lf, r2 = %lf, r3 = %lf\n", r1, r2, r3);
    //    h.print("h");

    return h;
  }



 XC::BJtensor XC::TwentyNodeBrick::dh_drst_at(double r1, double r2, double r3)
  {

    const std::vector<int> dimensions({20,3});  // Changed from{20,3} to {8,3} Xiaoyan 07/12
    BJtensor dh(dimensions, 0.0);


    // influence of the node number 20
        dh(20,1) =   (1.0-r2)*(1.0-r3*r3)*0.25; ///4.0;
        dh(20,2) = - (1.0+r1)*(1.0-r3*r3)*0.25; ///4.0;
        dh(20,3) = - (1.0+r1)*(1.0-r2)*r3*0.50; ///2.0;
    // influence of the node number 19
        dh(19,1) = - (1.0-r2)*(1.0-r3*r3)*0.25; ///4.0;
        dh(19,2) = - (1.0-r1)*(1.0-r3*r3)*0.25; ///4.0;
        dh(19,3) = - (1.0-r1)*(1.0-r2)*r3*0.50; ///2.0;
    // influence of the node number 18
        dh(18,1) = - (1.0+r2)*(1.0-r3*r3)*0.25; ///4.0;
        dh(18,2) =   (1.0-r1)*(1.0-r3*r3)*0.25; ///4.0;
        dh(18,3) = - (1.0-r1)*(1.0+r2)*r3*0.50; ///2.0;
    // influence of the node number 17
        dh(17,1) =   (1.0+r2)*(1.0-r3*r3)*0.25; ///4.0;
        dh(17,2) =   (1.0+r1)*(1.0-r3*r3)*0.25; ///4.0;
        dh(17,3) = - (1.0+r1)*(1.0+r2)*r3*0.50; ///2.0;

    // influence of the node number 16
        dh(16,1) =   (1.0-r2*r2)*(1.0-r3)*0.25; ///4.0;
        dh(16,2) = - (1.0+r1)*r2*(1.0-r3)*0.50; ///2.0;
        dh(16,3) = - (1.0+r1)*(1.0-r2*r2)*0.25; ///4.0;
    // influnce of the node number 15
        dh(15,1) = - r1*(1.0-r2)*(1.0-r3)*0.50; ///2.0;
        dh(15,2) = - (1.0-r1*r1)*(1.0-r3)*0.25; ///4.0;
        dh(15,3) = - (1.0-r1*r1)*(1.0-r2)*0.25; ///4.0;
    // influence of the node number 14
        dh(14,1) = - (1.0-r2*r2)*(1.0-r3)*0.25; ///4.0;
        dh(14,2) = - (1.0-r1)*r2*(1.0-r3)*0.50; ///2.0;
        dh(14,3) = - (1.0-r1)*(1.0-r2*r2)*0.25; ///4.0;
    // influence of the node number 13
        dh(13,1) = - r1*(1.0+r2)*(1.0-r3)*0.50; ///2.0;
        dh(13,2) =   (1.0-r1*r1)*(1.0-r3)*0.25; ///4.0;
        dh(13,3) = - (1.0-r1*r1)*(1.0+r2)*0.25; ///4.0;

    // influence of the node number 12
        dh(12,1) =   (1.0-r2*r2)*(1.0+r3)*0.25; ///4.0;
        dh(12,2) = - (1.0+r1)*r2*(1.0+r3)*0.50; ///2.0;
        dh(12,3) =   (1.0+r1)*(1.0-r2*r2)*0.25; ///4.0;
    // influence of the node number 11
        dh(11,1) = - r1*(1.0-r2)*(1.0+r3)*0.50; ///2.0;
        dh(11,2) = - (1.0-r1*r1)*(1.0+r3)*0.25; ///4.0; // bug discovered 01 aug '95 2.0 -> 4.0
        dh(11,3) =   (1.0-r1*r1)*(1.0-r2)*0.25; ///4.0;
    // influence of the node number 10
        dh(10,1) = - (1.0-r2*r2)*(1.0+r3)*0.25; ///4.0;
        dh(10,2) = - (1.0-r1)*r2*(1.0+r3)*0.50; ///2.0;
        dh(10,3) =   (1.0-r1)*(1.0-r2*r2)*0.25; ///4.0;
    // influence of the node number 9
        dh(9,1)  = - r1*(1.0+r2)*(1.0+r3)*0.50; ///2.0;
        dh(9,2)  =   (1.0-r1*r1)*(1.0+r3)*0.25; ///4.0;
        dh(9,3)  =   (1.0-r1*r1)*(1.0+r2)*0.25; ///4.0;

      // influence of the node number 8
    //dh(8,1)= (1.0-r2)*(1.0-r3)/8.0 - (dh(15,1)+dh(16,1)+dh(20,1))/2.0;
    dh(8,1)= (1.0-r2)*(1.0-r3)*0.125 - (dh(15,1)+dh(16,1)+dh(20,1))*0.50; ///2.0;
    dh(8,2)=-(1.0+r1)*(1.0-r3)*0.125 - (dh(15,2)+dh(16,2)+dh(20,2))*0.50; ///2.0;
    dh(8,3)=-(1.0+r1)*(1.0-r2)*0.125 - (dh(15,3)+dh(16,3)+dh(20,3))*0.50; ///2.0;
      // influence of the node number 7
    dh(7,1)=-(1.0-r2)*(1.0-r3)*0.125 - (dh(14,1)+dh(15,1)+dh(19,1))*0.50; ///2.0;
    dh(7,2)=-(1.0-r1)*(1.0-r3)*0.125 - (dh(14,2)+dh(15,2)+dh(19,2))*0.50; ///2.0;
    dh(7,3)=-(1.0-r1)*(1.0-r2)*0.125 - (dh(14,3)+dh(15,3)+dh(19,3))*0.50; ///2.0;
      // influence of the node number 6
    dh(6,1)=-(1.0+r2)*(1.0-r3)*0.125 - (dh(13,1)+dh(14,1)+dh(18,1))*0.50; ///2.0;
    dh(6,2)= (1.0-r1)*(1.0-r3)*0.125 - (dh(13,2)+dh(14,2)+dh(18,2))*0.50; ///2.0;
    dh(6,3)=-(1.0-r1)*(1.0+r2)*0.125 - (dh(13,3)+dh(14,3)+dh(18,3))*0.50; ///2.0;
      // influence of the node number 5
    dh(5,1)= (1.0+r2)*(1.0-r3)*0.125 - (dh(13,1)+dh(16,1)+dh(17,1))*0.50; ///2.0;
    dh(5,2)= (1.0+r1)*(1.0-r3)*0.125 - (dh(13,2)+dh(16,2)+dh(17,2))*0.50; ///2.0;
    dh(5,3)=-(1.0+r1)*(1.0+r2)*0.125 - (dh(13,3)+dh(16,3)+dh(17,3))*0.50; ///2.0;

      // influence of the node number 4
    dh(4,1)= (1.0-r2)*(1.0+r3)*0.125 - (dh(11,1)+dh(12,1)+dh(20,1))*0.50; ///2.0;
    dh(4,2)=-(1.0+r1)*(1.0+r3)*0.125 - (dh(11,2)+dh(12,2)+dh(20,2))*0.50; ///2.0;
    dh(4,3)= (1.0+r1)*(1.0-r2)*0.125 - (dh(11,3)+dh(12,3)+dh(20,3))*0.50; ///2.0;
      // influence of the node number 3
    dh(3,1)=-(1.0-r2)*(1.0+r3)*0.125 - (dh(10,1)+dh(11,1)+dh(19,1))*0.50; ///2.0;
    dh(3,2)=-(1.0-r1)*(1.0+r3)*0.125 - (dh(10,2)+dh(11,2)+dh(19,2))*0.50; ///2.0;
    dh(3,3)= (1.0-r1)*(1.0-r2)*0.125 - (dh(10,3)+dh(11,3)+dh(19,3))*0.50; ///2.0;
      // influence of the node number 2
    dh(2,1)=-(1.0+r2)*(1.0+r3)*0.125 - (dh(10,1)+dh(18,1)+dh( 9,1))*0.50; ///2.0;
    dh(2,2)= (1.0-r1)*(1.0+r3)*0.125 - (dh(10,2)+dh(18,2)+dh( 9,2))*0.50; ///2.0;
    dh(2,3)= (1.0-r1)*(1.0+r2)*0.125 - (dh(10,3)+dh(18,3)+dh( 9,3))*0.50; ///2.0;
      // influence of the node number 1
    dh(1,1)= (1.0+r2)*(1.0+r3)*0.125 - (dh(12,1)+dh(17,1)+dh( 9,1))*0.50; ///2.0;
    dh(1,2)= (1.0+r1)*(1.0+r3)*0.125 - (dh(12,2)+dh(17,2)+dh( 9,2))*0.50; ///2.0;
    dh(1,3)= (1.0+r1)*(1.0+r2)*0.125 - (dh(12,3)+dh(17,3)+dh( 9,3))*0.50; ///2.0;

    return dh;
  }



////#############################################################################
 XC::TwentyNodeBrick & XC::TwentyNodeBrick::operator[](int subscript)
  {
    return ( *(this+subscript) );
  }

//Finite_Element & XC::TwentyNodeBrick::operator[](short subscript)
//  {
//    return ( *(this+subscript) );
//  }

//Finite_Element & XC::TwentyNodeBrick::operator[](unsigned subscript)
//  {
//    return ( *(this+subscript) );
//  }


////#############################################################################
////#############################################################################
////#############################################################################
////#############################################################################
 XC::BJtensor XC::TwentyNodeBrick::getStiffnessTensor(void) const
  {
    const std::vector<int> K_dim({20,3,3,20});
    BJtensor Kk(K_dim,0.0);
    BJtensor Kkt(K_dim,0.0);

//debugging
//    matrix Kmat;

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    const std::vector<int> dh_dim({20,3});
    BJtensor dh(dh_dim, 0.0);

    //    BJtensor Constitutive( def_dim_4, 0.0);
    BJtensor Constitutive;

    double det_of_Jacobian = 0.0;

    static const std::vector<int> disp_dim({20,3});
    BJtensor incremental_displacements(disp_dim,0.0); // \Delta u

    straintensor incremental_strain;
//    straintensor total_strain_at_GP;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor JacobianINVtemp;
    BJtensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh = dh_drst_at(r,s,t);
  //dh.print("dh");
                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //JacobianINVtemp = Jacobian.inverse();
                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                ////////!!!!!!! dhGlobal = dh("ij") * JacobianINV("jk");  //big bug found here Zhaohui 09-02-2001
                dhGlobal = dh("ij") * JacobianINV("kj");
                //        ::fprintf(stdout," # %d \n\n\n\n\n\n\n\n", El_count);
                //dhGlobal.print("dhGlobal");
                //weight
                weight = rw * sw * tw * det_of_Jacobian;
                //::::
                //printf("\n\nIN THE STIFFNESS TENSOR INTEGRATOR ----**************** where = %d \n", where);
                //printf("  Stifness_Tensor \n");
                //printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //                            GP_c_r,GP_c_s,GP_c_t);
                //printf("WEIGHT = %f", weight);
                //Jacobian.print("J");
                //JacobianINV.print("JINV");
                //JacobianINVtemp.print("JINVtemp");
                //BJtensor I = JacobianINV("ij")*Jacobian("jk");
                //I.print("I");

                //printf("determinant of Jacobian = %.6e", det_of_Jacobian);
                //matpoint[where].report("Gauss Point\n");

                // incremental strains at this Gauss point
                //GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor1\n");

  //09-02-2001 Zhaohui
  //              incremental_strain =
                //     (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();

  //incremental_strain.null_indices();
                //incremental_strain.report("\n incremental_strain XC::BJtensor at GAUSS point\n");

                // incremental_strain.reportshort("\n incremental_strain XC::BJtensor at GAUSS point\n");
                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor2\n");
                // initialize total strain with the strain at this Gauss point before
                // adding this increments strains!
                //                total_strain_at_GP.Initialize(*(GPstrain+where));
                //total_strain_at_GP.reportshort("\n total_strain XC::BJtensor at GAUSS point BEFORE\n");
                // this is the addition of incremental strains to the previous strain state at
                // this Gauss point
                //                total_strain_at_GP = total_strain_at_GP + incremental_strain;
                //total_strain_at_GP.reportshort("\n total_strain XC::BJtensor at GAUSS point AFTER\n");
                //..   dakle ovde posalji strain_increment jer se stari stress cuva u okviru svake
                //..   Gauss tacke a samo saljes strain_increment koji ce da se prenese
                //..   u integracionu rutinu pa ce ta da vrati krajnji napon i onda moze da
                //..   se pravi ConstitutiveStiffnessTensor.
                // here comes the final_stress calculation
                // this final stress after integration is used ONLY to obtain Constitutive XC::BJtensor
                // at this Gauss point.

                //final_stress_after_integration =
                //    (matpoint)->operator[](where).FinalStress(*(GPstress+where),
                //                                 incremental_strain,
                //                                 (matpoint)->operator[](where),
                //                                 number_of_subincrements,
                //                                 this_one_PP);
                //final_stress_after_integration.reportshortpqtheta("\n final_stress_after_integration in stiffness_tensor5\n");

                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor3\n");
                //final_stress_after_integration.reportshortpqtheta("\n final_stress_after_integration GAUSS \n");
                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor4\n");

                // this final stress after integration is used ONLY to obtain Constitutive XC::BJtensor
                // at this Gauss point AND to set up the iterative_stress that is used in calculting
                // internal forces during iterations!!

                //GPiterative_stress[where].Initialize(final_stress_after_integration);
                //GPiterative_stress[where].reportshortpqtheta("\n iterative_stress at GAUSS point in stiffness_tensor5\n");


                // Stress state at Gauss point will be updated ( in the
                // sense of updating stresses ( integrating them ) ) in function Update (...) ! ! ! ! !
                // calculate the constitutive XC::BJtensor
                //......         Constitutive =  GPtangent_E[where];

  //Constitutive =  (matpoint)->operator[](where).ConstitutiveTensor(final_stress_after_integration,
                //                                         *(GPstress+where),
                //                                          incremental_strain,
                //                                          (matpoint)->operator[](where),
                //                                          this_one_PP);
                //Constitutive.print("C","\n\n C XC::BJtensor \n");

         //EPState *tmp_eps = (matpoint[where]).getEPS();
         //NDMaterial *tmp_ndm = (matpoint[where]).getNDMat();

                Constitutive = (matpoint[where]->matmodel)->getTangentTensor();
//                Constitutive.print("C","\n\n C XC::BJtensor \n");

  //    matpoint[where].setEPS( mmodel->getEPS() );
  //}
  //else if( tmp_ndm ) { //Elastic case
  //    (matpoint[where].p_matmodel)->setTrialStrainIncr( incremental_strain );
  //    Constitutive = (matpoint[where].p_matmodel)->getTangentTensor();
  //}
  //else {
                //   std::cerr << "XC::TwentyNodeBrick::incremental_Update (tag: %d), could not getTangentTensor", this->getTag();
  //   exit(1);
  //}

  //printf("Constitutive.trace = %12.6e\n", Constitutive.trace());
                //Kmat = this->stiffness_matrix(Constitutive);
                //printf("Constitutive XC::BJtensor max:= %10.3e\n", Kmat.mmax());

                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor5\n");
                // this is update of constitutive XC::BJtensor at this Gauss point
                //GPtangent_E[where].Initialize(Constitutive);
                //....GPtangent_E[where].print(" tangent E at GAUSS point");

                //GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor6\n");

                //K = K + temp2;

                Kkt = dhGlobal("ib")*Constitutive("abcd");
                Kk = Kk + Kkt("aicd")*dhGlobal("jd")*weight;

  //Kk = Kk + dhGlobal("ib")*Constitutive("abcd")*dhGlobal("jd")*weight;
                //....K.print("K","\n\n K XC::BJtensor \n");

  //Kmat = this->stiffness_matrix(Kk);
                //printf("K XC::BJtensor max= %10.3e\n", Kmat.mmax());

                //convert constitutive and K to matrix and find min and max and print!



              }
          }
      }
//    Kk.print("K","\n\n K XC::BJtensor \n");
    //K = Kk;
    return Kk;
  }


//#############################################################################

void XC::TwentyNodeBrick::set_strain_stress_tensor(FILE *fp, double * u)
  {
    const std::vector<int> dh_dim({20,3});
    BJtensor dh(dh_dim, 0.0);

//    BJtensor Constitutive( def_dim_4, 0.0);
    BJtensor Constitutive;
    double r  = 0.0;
    double s  = 0.0;
    double t  = 0.0;
    int where = 0;

    double det_of_Jacobian;

    straintensor strain;
    stresstensor stress;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;


    static std::vector<int> disp_dim({20,3});
    BJtensor total_displacements(disp_dim,0.0); //

    total_displacements = total_disp(fp, u);

    ::printf("\n  displacement(x-y-z) at GAUSS pt %d \n\n", where+1);
    for(int ii=1; ii<=20;ii++)
     {
      ::printf("Global# %d Local#%d  %+0.5e %+0.5e %+0.5e\n",
                     //G_N_numbs[ii-1],
       theNodes.getTagNode(ii-1),
       ii,total_displacements(ii,1),
              total_displacements(ii,2),
       total_displacements(ii,3));
     }

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh = dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal = dh("ij") * JacobianINV("jk");// Zhaohui 09-02-2001
                dhGlobal = dh("ij") * JacobianINV("kj");
                //weight
                // strains at this Gauss point from displacement
                strain = (dhGlobal("ib")*total_displacements("ia")).symmetrize11();
                strain.null_indices();
                // here comes the final_stress calculation
                // at this Gauss point.

                //Constitutive =  GPtangent_E[where];
                //Constitutive =  (matpoint->getEPS() )->getEep();
                // if set total displ, then it should be elstic material
                Constitutive =  ( matpoint[where]->matmodel)->getTangentTensor();

                stress = Constitutive("ijkl") * strain("kl");
                stress.null_indices();

                ::printf("\n  strain XC::BJtensor at GAUSS point %d \n", where+1);
                strain.reportshort("");
                ::printf("\n  stress XC::BJtensor at GAUSS point %d \n", where+1);
                stress.reportshort("");


              }
          }
      }
  }


////#############################################################################
//  BJtensor XC::TwentyNodeBrick::mass_tensor(Elastic  mmodel)
 XC::BJtensor XC::TwentyNodeBrick::getMassTensor(void) const
  {
    //std::vector<int> M_dim({8,3,3,8});
    std::vector<int> M_dim({60,60});
    BJtensor Mm(M_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    std::vector<int> dh_dim({20,3});

    BJtensor dh(dh_dim, 0.0);

    std::vector<int> h_dim({60,3}); // Xiaoyan changed from {60,3} to {24,3}
    BJtensor H(h_dim, 0.0);

    double det_of_Jacobian = 0.0;

    BJtensor Jacobian;

    double RHO;
    RHO= rho;   //global

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh = dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //   Jacobian.print("J","Jacobian");
                // Inverse of Jacobian XC::BJtensor ( matrix )
                //                JacobianINV = Jacobian_3Dinv(dh);
                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //   printf("det_of_Jacobian = %6.2e \n",det_of_Jacobian);
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //                dhGlobal = dh("ij") * JacobianINV("jk");
                // derivatives of local coordinates with respect to local coordinates


                // printf("\n\nIN THE MASS TENSOR INTEGRATOR ----**************** where = %d \n", where);
                // printf("  Mass_Tensor \n");
                // printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //                             GP_c_r,GP_c_s,GP_c_t);
                //
                H = H_3D(r,s,t);

                // double sum = 0.0;
                // for(int i=1; i<=60 ; i++)
                //           {
                // //       sum+=H(i,1);
                //      for(int j=1; j<= 3; j++)
                //         {
                //                   sum+=H(i,j);
                //            ::printf( "  %+9.2e", H(i,j) );
                //          }
                //             ::printf( "  %d \n", i);
                //     }
                //      ::printf( " \n sum= %+6.2e\n", sum );




                // matpoint GaPo = XC::MatPoint3D::GP()+where;

                weight = rw * sw * tw * RHO * det_of_Jacobian;

                Mm = Mm + H("ib")*H("kb")*weight;
        // printf("\n +++++++++++++++++++++++++ \n\n");
        //Mm.printshort("M");
              }
          }
      }
    //M = Mm;
    //Mm.printshort("M");
    return Mm;
  }


////#############################################################################

 XC::BJtensor XC::TwentyNodeBrick::stiffness_matrix(const BJtensor & K)
  {
//    std::vector<int> K_dim({20,3,3,20});
//    BJtensor K(K_dim,0.0);
    matrix Kmatrix(60,60,0.0);

    int Ki=0;
    int Kj=0;

    for( int i=1 ; i<=20 ; i++ )
      {
        for( int j=1 ; j<=20 ; j++ )
          {
            for( int k=1 ; k<=3 ; k++ )
              {
                for( int l=1 ; l<=3 ; l++ )
                  {
                    Ki = k+3*(i-1);
                    Kj = l+3*(j-1);
                    //::printf("i=%d k=%d  Ki=%d       j=%d l=%d  Kj=%d\n",i,k,Ki,j,l,Kj);

                    Kmatrix( Ki , Kj ) = K(i,k,l,j);
                  }
              }
          }
      }
    return BJtensor(nDarray(Kmatrix));
  }

//#############################################################################

////#############################################################################
XC::BJtensor XC::TwentyNodeBrick::mass_matrix(const BJtensor & M)
  {
    //    std::vector<int> K_dim({20,3,3,20});
    //    BJtensor K(K_dim,0.0);
    matrix Mmatrix(60,60,0.0);

    for( int i=1 ; i<=60 ; i++ )
      {
        for( int j=1 ; j<=60 ; j++ )
          {
             Mmatrix( i , j ) = M(i,j);
             //  ::printf("Mi Mj %d %d %+6.2e ",Mi,Mj,Mmatrix( Mi , Mj ) );
          }
      }
    return BJtensor(Mmatrix);
  }
////#############################################################################

////#############################################################################
XC::BJtensor XC::TwentyNodeBrick::Jacobian_3D(const BJtensor &dh) const
  {
     BJtensor N_C = Nodal_Coordinates();
     BJtensor Jacobian_3D = dh("ij") * N_C("ik");
     return Jacobian_3D;
  }

//#############################################################################
XC::BJtensor XC::TwentyNodeBrick::Jacobian_3Dinv(const BJtensor &dh) const
  {
     BJtensor N_C = Nodal_Coordinates();
     BJtensor Jacobian_3Dinv = (dh("ij") * N_C("ik")).inverse();
     return Jacobian_3Dinv;
  }


////#############################################################################
XC::BJtensor XC::TwentyNodeBrick::Nodal_Coordinates(void) const
  {
    const std::vector<int> dimensions({20,3});
    BJtensor N_coord(dimensions, 0.0);

    //Zhaohui using node pointers, which come from the XC::Domain
    const Vector &nd1Crds = theNodes[0]->getCrds();
    const Vector &nd2Crds = theNodes[1]->getCrds();
    const Vector &nd3Crds = theNodes[2]->getCrds();
    const Vector &nd4Crds = theNodes[3]->getCrds();
    const Vector &nd5Crds = theNodes[4]->getCrds();
    const Vector &nd6Crds = theNodes[5]->getCrds();
    const Vector &nd7Crds = theNodes[6]->getCrds();
    const Vector &nd8Crds = theNodes[7]->getCrds();

    const Vector &nd9Crds  =  theNodes[8]->getCrds();
    const Vector &nd10Crds = theNodes[9]->getCrds();
    const Vector &nd11Crds = theNodes[10]->getCrds();
    const Vector &nd12Crds = theNodes[11]->getCrds();

    const Vector &nd13Crds = theNodes[12]->getCrds();
    const Vector &nd14Crds = theNodes[13]->getCrds();
    const Vector &nd15Crds = theNodes[14]->getCrds();
    const Vector &nd16Crds = theNodes[15]->getCrds();


    const Vector &nd17Crds = theNodes[16]->getCrds();
    const Vector &nd18Crds = theNodes[17]->getCrds();
    const Vector &nd19Crds = theNodes[18]->getCrds();
    const Vector &nd20Crds = theNodes[19]->getCrds();

    N_coord(1,1)=nd1Crds(0); N_coord(1,2)=nd1Crds(1); N_coord(1,3)=nd1Crds(2);
    N_coord(2,1)=nd2Crds(0); N_coord(2,2)=nd2Crds(1); N_coord(2,3)=nd2Crds(2);
    N_coord(3,1)=nd3Crds(0); N_coord(3,2)=nd3Crds(1); N_coord(3,3)=nd3Crds(2);
    N_coord(4,1)=nd4Crds(0); N_coord(4,2)=nd4Crds(1); N_coord(4,3)=nd4Crds(2);
    N_coord(5,1)=nd5Crds(0); N_coord(5,2)=nd5Crds(1); N_coord(5,3)=nd5Crds(2);
    N_coord(6,1)=nd6Crds(0); N_coord(6,2)=nd6Crds(1); N_coord(6,3)=nd6Crds(2);
    N_coord(7,1)=nd7Crds(0); N_coord(7,2)=nd7Crds(1); N_coord(7,3)=nd7Crds(2);
    N_coord(8,1)=nd8Crds(0); N_coord(8,2)=nd8Crds(1); N_coord(8,3)=nd8Crds(2);

    N_coord(9 ,1)=nd9Crds(0);  N_coord(9 ,2)=nd9Crds(1);  N_coord(9 ,3)=nd9Crds(2);
    N_coord(10,1)=nd10Crds(0); N_coord(10,2)=nd10Crds(1); N_coord(10,3)=nd10Crds(2);
    N_coord(11,1)=nd11Crds(0); N_coord(11,2)=nd11Crds(1); N_coord(11,3)=nd11Crds(2);
    N_coord(12,1)=nd12Crds(0); N_coord(12,2)=nd12Crds(1); N_coord(12,3)=nd12Crds(2);

    N_coord(13,1)=nd13Crds(0); N_coord(13,2)=nd13Crds(1); N_coord(13,3)=nd13Crds(2);
    N_coord(14,1)=nd14Crds(0); N_coord(14,2)=nd14Crds(1); N_coord(14,3)=nd14Crds(2);
    N_coord(15,1)=nd15Crds(0); N_coord(15,2)=nd15Crds(1); N_coord(15,3)=nd15Crds(2);
    N_coord(16,1)=nd16Crds(0); N_coord(16,2)=nd16Crds(1); N_coord(16,3)=nd16Crds(2);

    N_coord(17,1)=nd17Crds(0); N_coord(17,2)=nd17Crds(1); N_coord(17,3)=nd17Crds(2);
    N_coord(18,1)=nd18Crds(0); N_coord(18,2)=nd18Crds(1); N_coord(18,3)=nd18Crds(2);
    N_coord(19,1)=nd19Crds(0); N_coord(19,2)=nd19Crds(1); N_coord(19,3)=nd19Crds(2);
    N_coord(20,1)=nd20Crds(0); N_coord(20,2)=nd20Crds(1); N_coord(20,3)=nd20Crds(2);

    return N_coord;

  }

////#############################################################################
XC::BJtensor XC::TwentyNodeBrick::incr_disp(void) const
  {
    const std::vector<int> dimensions({20,3});
    BJtensor increment_disp(dimensions, 0.0);

    //for( int i=0 ; i<20 ; i++ )
    //
    //  {
    //    // increment_disp(i+1,1) = nodes[ G_N_numbs[i] ].incremental_translation_x();
    //    // increment_disp(i+1,2) = nodes[ G_N_numbs[i] ].incremental_translation_y();
    //    // increment_disp(i+1,3) = nodes[ G_N_numbs[i] ].incremental_translation_z();
    //
    //    increment_disp(i+1,1) = IncremenDis(0);
    //    increment_disp(i+1,2) = IncremenDis(1);
    //    increment_disp(i+1,3) = IncremenDis(2);
    //
    //  }

    //Zhaohui using node pointers, which come from the XC::Domain
    //const Vector &TotDis1 = theNodes[0]->getTrialDisp();
    //const Vector &incrdelDis1 = theNodes[0]->getIncrDisp();
    //Have to get IncrDeltaDisp, not IncrDisp for cumulation of incr_disp
    const Vector &IncrDis1 = theNodes[0]->getIncrDeltaDisp();
    const Vector &IncrDis2 = theNodes[1]->getIncrDeltaDisp();
    const Vector &IncrDis3 = theNodes[2]->getIncrDeltaDisp();
    const Vector &IncrDis4 = theNodes[3]->getIncrDeltaDisp();
    const Vector &IncrDis5 = theNodes[4]->getIncrDeltaDisp();
    const Vector &IncrDis6 = theNodes[5]->getIncrDeltaDisp();
    const Vector &IncrDis7 = theNodes[6]->getIncrDeltaDisp();
    const Vector &IncrDis8 = theNodes[7]->getIncrDeltaDisp();

    const Vector &IncrDis9  = theNodes[8]->getIncrDeltaDisp();
    const Vector &IncrDis10 = theNodes[9]->getIncrDeltaDisp();
    const Vector &IncrDis11 = theNodes[10]->getIncrDeltaDisp();
    const Vector &IncrDis12 = theNodes[11]->getIncrDeltaDisp();

    const Vector &IncrDis13 = theNodes[12]->getIncrDeltaDisp();
    const Vector &IncrDis14 = theNodes[13]->getIncrDeltaDisp();
    const Vector &IncrDis15 = theNodes[14]->getIncrDeltaDisp();
    const Vector &IncrDis16 = theNodes[15]->getIncrDeltaDisp();

    const Vector &IncrDis17 = theNodes[16]->getIncrDeltaDisp();
    const Vector &IncrDis18 = theNodes[17]->getIncrDeltaDisp();
    const Vector &IncrDis19 = theNodes[18]->getIncrDeltaDisp();
    const Vector &IncrDis20 = theNodes[19]->getIncrDeltaDisp();

    increment_disp(1,1)=IncrDis1(0); increment_disp(1,2)=IncrDis1(1);increment_disp(1,3)=IncrDis1(2);
    increment_disp(2,1)=IncrDis2(0); increment_disp(2,2)=IncrDis2(1);increment_disp(2,3)=IncrDis2(2);
    increment_disp(3,1)=IncrDis3(0); increment_disp(3,2)=IncrDis3(1);increment_disp(3,3)=IncrDis3(2);
    increment_disp(4,1)=IncrDis4(0); increment_disp(4,2)=IncrDis4(1);increment_disp(4,3)=IncrDis4(2);
    increment_disp(5,1)=IncrDis5(0); increment_disp(5,2)=IncrDis5(1);increment_disp(5,3)=IncrDis5(2);
    increment_disp(6,1)=IncrDis6(0); increment_disp(6,2)=IncrDis6(1);increment_disp(6,3)=IncrDis6(2);
    increment_disp(7,1)=IncrDis7(0); increment_disp(7,2)=IncrDis7(1);increment_disp(7,3)=IncrDis7(2);
    increment_disp(8,1)=IncrDis8(0); increment_disp(8,2)=IncrDis8(1);increment_disp(8,3)=IncrDis8(2);

    increment_disp(9 ,1)=IncrDis9(0);  increment_disp(9 ,2)=IncrDis9(1); increment_disp(9 ,3)=IncrDis9(2);
    increment_disp(10,1)=IncrDis10(0); increment_disp(10,2)=IncrDis10(1);increment_disp(10,3)=IncrDis10(2);
    increment_disp(11,1)=IncrDis11(0); increment_disp(11,2)=IncrDis11(1);increment_disp(11,3)=IncrDis11(2);
    increment_disp(12,1)=IncrDis12(0); increment_disp(12,2)=IncrDis12(1);increment_disp(12,3)=IncrDis12(2);

    increment_disp(13,1)=IncrDis13(0); increment_disp(13,2)=IncrDis13(1);increment_disp(13,3)=IncrDis13(2);
    increment_disp(14,1)=IncrDis14(0); increment_disp(14,2)=IncrDis14(1);increment_disp(14,3)=IncrDis14(2);
    increment_disp(15,1)=IncrDis15(0); increment_disp(15,2)=IncrDis15(1);increment_disp(15,3)=IncrDis15(2);
    increment_disp(16,1)=IncrDis16(0); increment_disp(16,2)=IncrDis16(1);increment_disp(16,3)=IncrDis16(2);

    increment_disp(17,1)=IncrDis17(0); increment_disp(17,2)=IncrDis17(1);increment_disp(17,3)=IncrDis17(2);
    increment_disp(18,1)=IncrDis18(0); increment_disp(18,2)=IncrDis18(1);increment_disp(18,3)=IncrDis18(2);
    increment_disp(19,1)=IncrDis19(0); increment_disp(19,2)=IncrDis19(1);increment_disp(19,3)=IncrDis19(2);
    increment_disp(20,1)=IncrDis20(0); increment_disp(20,2)=IncrDis20(1);increment_disp(20,3)=IncrDis20(2);


    return increment_disp;
  }

////#############################################################################
XC::BJtensor XC::TwentyNodeBrick::total_disp(void) const
  {
    const std::vector<int> dimensions({20,3});
    BJtensor total_disp(dimensions, 0.0);

    //Zhaohui using node pointers, which come from the XC::Domain
    const Vector &TotDis1 = theNodes[0]->getTrialDisp();
    std::cerr<<"\ntot node " << theNodes[0]->getTag() <<" x "<< TotDis1(0) <<" y "<< TotDis1(1) << " z "<< TotDis1(2) << std::endl;
    const Vector &TotDis2 = theNodes[1]->getTrialDisp();
    std::cerr << "tot node " << theNodes[1]->getTag() << " x " << TotDis2(0) <<" y "<< TotDis2(1) << " z "<< TotDis2(2) << std::endl;
    const Vector &TotDis3 = theNodes[2]->getTrialDisp();
    std::cerr << "tot node " << theNodes[2]->getTag() << " x " << TotDis3(0) <<" y "<< TotDis3(1) << " z "<< TotDis3(2) << std::endl;
    const Vector &TotDis4 = theNodes[3]->getTrialDisp();
    std::cerr << "tot node " << theNodes[3]->getTag() << " x " << TotDis4(0) <<" y "<< TotDis4(1) << " z "<< TotDis4(2) << std::endl;
    const Vector &TotDis5 = theNodes[4]->getTrialDisp();
    std::cerr << "tot node " << theNodes[4]->getTag() << " x " << TotDis5(0) <<" y "<< TotDis5(1) << " z "<< TotDis5(2) << std::endl;
    const Vector &TotDis6 = theNodes[5]->getTrialDisp();
    std::cerr << "tot node " << theNodes[5]->getTag() << " x " << TotDis6(0) <<" y "<< TotDis6(1) << " z "<< TotDis6(2) << std::endl;
    const Vector &TotDis7 = theNodes[6]->getTrialDisp();
    std::cerr << "tot node " << theNodes[6]->getTag() << " x " << TotDis7(0) <<" y "<< TotDis7(1) << " z "<< TotDis7(2) << std::endl;
    const Vector &TotDis8 = theNodes[7]->getTrialDisp();
    std::cerr << "tot node " << theNodes[7]->getTag() << " x " << TotDis8(0) <<" y "<< TotDis8(1) << " z "<< TotDis8(2) << std::endl;

    const Vector &TotDis9 = theNodes[8]->getTrialDisp();
    std::cerr << "tot node " << theNodes[8]->getTag() << " x " << TotDis9(0) <<" y "<< TotDis9(1) << " z "<< TotDis9(2) << std::endl;
    const Vector &TotDis10 = theNodes[9]->getTrialDisp();
    std::cerr << "tot node " << theNodes[9]->getTag() << " x " << TotDis10(0) <<" y "<< TotDis10(1) << " z "<< TotDis10(2) << std::endl;
    const Vector &TotDis11 = theNodes[10]->getTrialDisp();
    std::cerr << "tot node " << theNodes[10]->getTag() << " x " << TotDis11(0) <<" y "<< TotDis11(1) << " z "<< TotDis11(2) << std::endl;
    const Vector &TotDis12 = theNodes[11]->getTrialDisp();
    std::cerr << "tot node " << theNodes[11]->getTag() << " x " << TotDis12(0) <<" y "<< TotDis12(1) << " z "<< TotDis12(2) << std::endl;

    const Vector &TotDis13 = theNodes[12]->getTrialDisp();
    std::cerr << "tot node " << theNodes[12]->getTag() << " x " << TotDis13(0) <<" y "<< TotDis13(1) << " z "<< TotDis13(2) << std::endl;
    const Vector &TotDis14 = theNodes[13]->getTrialDisp();
    std::cerr << "tot node " << theNodes[13]->getTag() << " x " << TotDis14(0) <<" y "<< TotDis14(1) << " z "<< TotDis14(2) << std::endl;
    const Vector &TotDis15 = theNodes[14]->getTrialDisp();
    std::cerr << "tot node " << theNodes[14]->getTag() << " x " << TotDis15(0) <<" y "<< TotDis15(1) << " z "<< TotDis15(2) << std::endl;
    const Vector &TotDis16 = theNodes[15]->getTrialDisp();
    std::cerr << "tot node " << theNodes[15]->getTag() << " x " << TotDis16(0) <<" y "<< TotDis16(1) << " z "<< TotDis16(2) << std::endl;

    const Vector &TotDis17 = theNodes[16]->getTrialDisp();
    std::cerr << "tot node " << theNodes[16]->getTag() << " x " << TotDis17(0) <<" y "<< TotDis17(1) << " z "<< TotDis17(2) << std::endl;
    const Vector &TotDis18 = theNodes[17]->getTrialDisp();
    std::cerr << "tot node " << theNodes[17]->getTag() << " x " << TotDis18(0) <<" y "<< TotDis18(1) << " z "<< TotDis18(2) << std::endl;
    const Vector &TotDis19 = theNodes[18]->getTrialDisp();
    std::cerr << "tot node " << theNodes[18]->getTag() << " x " << TotDis19(0) <<" y "<< TotDis19(1) << " z "<< TotDis19(2) << std::endl;
    const Vector &TotDis20 = theNodes[19]->getTrialDisp();
    std::cerr << "tot node " << theNodes[19]->getTag() << " x " << TotDis20(0) <<" y "<< TotDis20(1) << " z "<< TotDis20(2) << std::endl;




    total_disp(1,1)=TotDis1(0); total_disp(1,2)=TotDis1(1);total_disp(1,3)=TotDis1(2);
    total_disp(2,1)=TotDis2(0); total_disp(2,2)=TotDis2(1);total_disp(2,3)=TotDis2(2);
    total_disp(3,1)=TotDis3(0); total_disp(3,2)=TotDis3(1);total_disp(3,3)=TotDis3(2);
    total_disp(4,1)=TotDis4(0); total_disp(4,2)=TotDis4(1);total_disp(4,3)=TotDis4(2);
    total_disp(5,1)=TotDis5(0); total_disp(5,2)=TotDis5(1);total_disp(5,3)=TotDis5(2);
    total_disp(6,1)=TotDis6(0); total_disp(6,2)=TotDis6(1);total_disp(6,3)=TotDis6(2);
    total_disp(7,1)=TotDis7(0); total_disp(7,2)=TotDis7(1);total_disp(7,3)=TotDis7(2);
    total_disp(8,1)=TotDis8(0); total_disp(8,2)=TotDis8(1);total_disp(8,3)=TotDis8(2);

    total_disp(9,1)=TotDis9(0); total_disp(9,2)=TotDis9(1);total_disp(9,3)=TotDis9(2);
    total_disp(10,1)=TotDis10(0); total_disp(10,2)=TotDis10(1);total_disp(10,3)=TotDis10(2);
    total_disp(11,1)=TotDis11(0); total_disp(11,2)=TotDis11(1);total_disp(11,3)=TotDis11(2);
    total_disp(12,1)=TotDis12(0); total_disp(12,2)=TotDis12(1);total_disp(12,3)=TotDis12(2);

    total_disp(13,1)=TotDis13(0); total_disp(13,2)=TotDis13(1);total_disp(13,3)=TotDis13(2);
    total_disp(14,1)=TotDis14(0); total_disp(14,2)=TotDis14(1);total_disp(14,3)=TotDis14(2);
    total_disp(15,1)=TotDis15(0); total_disp(15,2)=TotDis15(1);total_disp(15,3)=TotDis15(2);
    total_disp(16,1)=TotDis16(0); total_disp(16,2)=TotDis16(1);total_disp(16,3)=TotDis16(2);

    total_disp(17,1)=TotDis17(0); total_disp(17,2)=TotDis17(1);total_disp(17,3)=TotDis17(2);
    total_disp(18,1)=TotDis18(0); total_disp(18,2)=TotDis18(1);total_disp(18,3)=TotDis18(2);
    total_disp(19,1)=TotDis19(0); total_disp(19,2)=TotDis19(1);total_disp(19,3)=TotDis19(2);
    total_disp(20,1)=TotDis20(0); total_disp(20,2)=TotDis20(1);total_disp(20,3)=TotDis20(2);

    return total_disp;
  }


////#############################################################################
 XC::BJtensor XC::TwentyNodeBrick::total_disp(FILE *fp, double * u)
  {
    const std::vector<int> dimensions({20,3});
    BJtensor total_disp(dimensions, 0.0);
    //    double totalx, totaly, totalz;
    //    totalx=0;
    //    totaly=0;
    //    totalz=0;

    //for( int i=0 ; i<20 ; i++ )
    //
    //  {
    //    // total_disp(i+1,1) = nodes[ G_N_numbs[i] ].total_translation_x(u);
    //    // total_disp(i+1,2) = nodes[ G_N_numbs[i] ].total_translation_y(u);
    //    // total_disp(i+1,3) = nodes[ G_N_numbs[i] ].total_translation_z(u);
    //    Vector TotalTranDis = nodes[ G_N_numbs[i] ].getDisp();
    //
    //    total_disp(i+1,1) = TotalTranDis(0);
    // total_disp(i+1,2) = TotalTranDis(1);
    //    total_disp(i+1,3) = TotalTranDis(2);
    //
    //  }

    // Need more work

    //Zhaohui using node pointers, which come from the XC::Domain
    const Vector &TotDis1 = theNodes[0]->getTrialDisp();
    const Vector &TotDis2 = theNodes[1]->getTrialDisp();
    const Vector &TotDis3 = theNodes[2]->getTrialDisp();
    const Vector &TotDis4 = theNodes[3]->getTrialDisp();
    const Vector &TotDis5 = theNodes[4]->getTrialDisp();
    const Vector &TotDis6 = theNodes[5]->getTrialDisp();
    const Vector &TotDis7 = theNodes[6]->getTrialDisp();
    const Vector &TotDis8 = theNodes[7]->getTrialDisp();

    total_disp(1,1)=TotDis1(0); total_disp(1,2)=TotDis1(1);total_disp(1,3)=TotDis1(2);
    total_disp(2,1)=TotDis2(0); total_disp(2,2)=TotDis2(1);total_disp(2,3)=TotDis2(2);
    total_disp(3,1)=TotDis3(0); total_disp(3,2)=TotDis3(1);total_disp(3,3)=TotDis3(2);
    total_disp(4,1)=TotDis4(0); total_disp(4,2)=TotDis4(1);total_disp(4,3)=TotDis4(2);
    total_disp(5,1)=TotDis5(0); total_disp(5,2)=TotDis5(1);total_disp(5,3)=TotDis5(2);
    total_disp(6,1)=TotDis6(0); total_disp(6,2)=TotDis6(1);total_disp(6,3)=TotDis6(2);
    total_disp(7,1)=TotDis7(0); total_disp(7,2)=TotDis7(1);total_disp(7,3)=TotDis7(2);
    total_disp(8,1)=TotDis8(0); total_disp(8,2)=TotDis8(1);total_disp(8,3)=TotDis8(2);

    return total_disp;
  }


////#############################################################################
int XC::TwentyNodeBrick::get_global_number_of_node(int local_node_number)
{
  //return G_N_numbs[local_node_number];
  return theNodes.getTagNode(local_node_number);
}

////#############################################################################
int  XC::TwentyNodeBrick::get_Brick_Number(void)
{
  //return elem_numb;
  return this->getTag();
}

//////#############################################################################
//int * XC::TwentyNodeBrick::get_LM(void)
//  {
//    return 0;
//  }

//Commented out Zhaohui 09-27-2000

//////#############################################################################
//void XC::TwentyNodeBrick::set_LM(Node * node)
//  {
////    unsigned int BrickNumber = this->get_Brick_Number();
////    this->reportshort("");
//// for element numbered BrickNumber create LM array (see Bathe pp984
////    for(int LocalNodeNumber = 1 ; LocalNodeNumber<=20 ; LocalNodeNumber++ )
//    for(int LocalNodeNumber = 1 ; LocalNodeNumber<=8 ; LocalNodeNumber++ )// for 8noded brick
//      {
////        unsigned int global_node_number = b3d[BrickNumber-1].get_global_number_of_node(LocalNodeNumber-1);
//        unsigned int global_node_number = this->get_global_number_of_node(LocalNodeNumber-1);
//        LM[3*LocalNodeNumber-3] = node[global_node_number].eqn_tx();
//        LM[3*LocalNodeNumber-2] = node[global_node_number].eqn_ty();
//        LM[3*LocalNodeNumber-1] = node[global_node_number].eqn_tz();
//      }
//
//      // ::printf("\n\n");
//
////===   this->reportLM("LM");
////   for(int count01=1;count01<=8;count01++)
////     {
////       ::printf("element %4d localNode %4d Globalnode %4d  LM   %4d   %4d   %4d\n", BrickNumber, count01,this->get_global_number_of_node(count01-1),  LM[count01*3-3], LM[count01*3-2], LM[count01*3-1] );
////     }
//
//  }


////#############################################################################
// returns nodal forces for given stress field in an element
XC::BJtensor XC::TwentyNodeBrick::nodal_forces(void) const
  {
    std::vector<int> force_dim({20,3});  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor nodal_forces(force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    std::vector<int> dh_dim({20,3});  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(dh_dim, 0.0);

    stresstensor stress_at_GP(0.0);

    double det_of_Jacobian = 0.0;

    straintensor incremental_strain;

    static std::vector<int> disp_dim({20,3});   // Xiaoyan changed from {20,3} to {8,3}
    BJtensor incremental_displacements(disp_dim,0.0); // \Delta u

    incremental_displacements= incr_disp();

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal = dh("ij") * JacobianINV("jk");// Zhaohui 09-02-2001
                dhGlobal = dh("ij") * JacobianINV("kj");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;
                //..::printf("\n\nIN THE nodal forces ----**************** where = %d \n", where);
                //..::printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //..                           GP_c_r,GP_c_s,GP_c_t);
                //..::printf("WEIGHT = %f", weight);
                //..::printf("determinant of Jacobian = %f", det_of_Jacobian);
                //..matpoint[where].report("Gauss Point\n");

                //..    just determine this BJtensor E yet and for each gauss point different !!!!!!!!!!!!
                //..    here should be based on incremental shifts somewhere
                //..    find incremental deformation (strain_increment) and then with it:
                //..
                //// BJtensor of incremental displacements taken from node objects
                //                incremental_displacements = incr_disp();
                //
                //// incremental strains at this Gauss point
                //                incremental_strain =
                //                  (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                //
                //                incremental_strain.null_indices();
                ////incremental_strain.reportshort("\n incremental_strain XC::BJtensor at GAUSS point\n");
                //
                ////                integr_type = (matpoint)->operator[](where).integration_type();
                ////                if( !strcmp(integr_type,"BakcwardEuler")

                //.. so send stress_increment here because old stress is stored within each
                //.. Gauss points and only send strain_increment to be transmitted
                //.. into the integration routine so that it will return the final voltage and then
                //.. it can that is, the ConstitutiveStiffnessTensor.
                //..    
                //.. Actually send everything you have (incremental_strain, start_stress,
                //.. number_of_subincrements. . . into this Constitutive_tensor function
                //.. so let it go, depending on the model used and let it go
                //.. then there in each special model function return what it needs
                //.. (say Elastic returns Eelastic immediately and let's say MRS_Lade first
                //.. asks what method of integration to use and then depending on it
                //.. call function which integrals for that algorithm (ForwardEuler, BakcwardEuler,
                //.. SemiBackwardEuler ,. . . ) and then when the function returns voltage then
                //.. one wonders again what type of integration was at stake so the right one
                //.. ConstitutiveTensor and brings it back!

                //                   stress_at_GP = (GPstress)->operator[](where);
                //stress_at_GP = GPstress[where];

         //EPState *tmp_eps = (matpoint[where]->matmodel)->getEPS();
  //stress_at_GP = tmp_eps->getStress();
  //std::cerr << "tmp_eps" << (*tmp_eps);

         //NDMaterial *tmp_ndm = (matpoint[where]).getNDMat();

  //if( tmp_eps ) {     //Elasto-plastic case

  //stress_at_GP = (matpoint[where].matmodel->getEPS())->getStress();

  //   EPState *tmp_eps = (matpoint[where]->matmodel)->getEPS();
  //   stress_at_GP = tmp_eps->getStress();



//out May 2004, Guanzhou  incremental_strain =
//out May 2004, Guanzhou                     (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
//out May 2004, Guanzhou//  if(where == 0)
//out May 2004, Guanzhou//     //std::cerr << " In nodal_force delta_incremental_strain tag "<< getTag() <<"  " <<incremental_strain << std::endl;
//out May 2004, Guanzhou////  std::cerr << " el tag = "<< getTag();
//out May 2004, Guanzhou//
//out May 2004, Guanzhou  int err = ( matpoint[where]->matmodel )->setTrialStrainIncr( incremental_strain);
//out May 2004, Guanzhou  if( err) {
//out May 2004, Guanzhou                   std::cerr << "XC::TwentyNodeBrick::nodal_forces (tag: " << this->getTag() << ", not converged\n";
//out May 2004, Guanzhou     exit(-1);
//out May 2004, Guanzhou  }

  //char *test = matpoint[where]->matmodel->getType();
  // fmk - changing if so if into else block must be Template3Dep
//  if(strcmp(matpoint[where]->matmodel->getType(),"Template3Dep") != 0)
     stress_at_GP = matpoint[where]->getStressTensor();

//     stress_at_GP.report("PROBLEM");
//     getchar();

//  else
//  {
//            //Some thing funny happened when getting stress directly from matpoint[where], i have to do it this way!
//     EPState *tmp_eps = ((Template3Dep *)(matpoint[where]->matmodel))->getEPS();
//     stress_at_GP = tmp_eps->getStress();
//     //delete tmp_eps;
//         }

            //double  p = stress_at_GP.p_hydrostatic();
                //if( p < 0.0 )
         //{
         //  std::cerr << getTag();
         //  std::cerr << " ***p  =    " << p << std::endl;
         //}

  //std::cerr << " nodal_force :: stress_at_GP " << stress_at_GP << std::endl;

  //}
  //else if( tmp_ndm ) { //Elastic case
              //    stress_at_GP = (matpoint[where].getNDMat())->getStressTensor();
  //}
  //else {
                //   std::cerr << "XC::TwentyNodeBrick::nodal_forces (tag: %d), could not getStress", this->getTag();
  //   exit(1);
  //}

                //stress_at_GP.report("\n stress_at_GPtensor at GAUSS point for nodal forces \n");

                // nodal forces See Zienkievicz part 1 XC::pp 108
                nodal_forces =
                     nodal_forces + dhGlobal("ib")*stress_at_GP("ab")*weight;
                //nodal_forces.print("nf","\n\n Nodal Forces \n");

              }
          }
      }

    //std::cerr << "\n element no. " << getTag() << std::endl;
    //nodal_forces.print("nf","\n Nodal Forces \n");
    return nodal_forces;

  }

////#############################################################################
// returns nodal forces for given ITERATIVE stress field in an element
XC::BJtensor XC::TwentyNodeBrick::iterative_nodal_forces(void) const
  {
    std::vector<int> force_dim({20,3}); // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor nodal_forces(force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    std::vector<int> dh_dim({20,3});   // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(dh_dim, 0.0);

    stresstensor stress_at_GP(0.0);

    double det_of_Jacobian = 0.0;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                //.....
                //.....::printf("XC::TwentyNodeBrick::iterative_nodal_forces(void)  ----**************** where = %d \n", where);
                //.....::printf("UPDATE ");
                //.....::printf("   GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //.....                           GP_c_r,GP_c_s,GP_c_t);
                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal = dh("ij") * JacobianINV("jk");// Zhaohui 09-02-2001
                dhGlobal = dh("ij") * JacobianINV("kj");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;

                //                   stress_at_GP = (GPstress)->operator[](where);
                //stress_at_GP = GPiterative_stress[where];

  //stress_at_GP = ( matpoint[where].getTrialEPS() )->getStress();
                stress_at_GP = matpoint[where]->getStressTensor();
                stress_at_GP.reportshortpqtheta("\n iterative_stress at GAUSS point in iterative_nodal_force\n");

                // nodal forces See Zienkievicz part 1 XC::pp 108
                nodal_forces =
                  nodal_forces + dhGlobal("ib")*stress_at_GP("ab")*weight;
                //nodal_forces.print("nf","\n XC::TwentyNodeBrick::iterative_nodal_forces Nodal Forces ~~~~\n");

              }
          }
      }


    return nodal_forces;

  }

////#############################################################################
// returns nodal forces for given constant stress field in the element
 XC::BJtensor XC::TwentyNodeBrick::nodal_forces_from_stress(stresstensor & stress) const
  {
    std::vector<int> force_dim({20,3});  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor nodal_forces(force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    double weight = 0.0;

    std::vector<int> dh_dim({20,3}); // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(dh_dim, 0.0);

    double det_of_Jacobian = 0.0;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                //--                where =
                //--                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                //.....
                //.....::printf("XC::TwentyNodeBrick::iterative_nodal_forces(void)  ----**************** where = %d \n", where);
                //.....::printf("UPDATE ");
                //.....::printf("   GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //.....                           GP_c_r,GP_c_s,GP_c_t);
                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal = dh("ij") * JacobianINV("jk");// Zhaohui 09-02-2001
                dhGlobal = dh("ij") * JacobianINV("kj");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;

                //                   stress_at_GP = (GPstress)->operator[](where);
                //                stress_at_GP = GPiterative_stress[where];
                //GPiterative_stress[where].reportshortpqtheta("\n iterative_stress at GAUSS point in iterative_nodal_force\n");

                // nodal forces See Zienkievicz part 1 XC::pp 108
                nodal_forces =
                  nodal_forces + dhGlobal("ib")*stress("ab")*weight;
                //nodal_forces.print("nf","\n XC::TwentyNodeBrick::iterative_nodal_forces Nodal Forces ~~~~\n");

              }
          }
      }

    return nodal_forces;

  }


////#############################################################################
// returns nodal forces for given incremental strain field in an element
// by using the linearized constitutive XC::BJtensor from the beginning of the step !
XC::BJtensor XC::TwentyNodeBrick::linearized_nodal_forces(void) const
  {
    std::vector<int> force_dim({20,3});  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor linearized_nodal_forces(force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    std::vector<int> dh_dim({20,3});  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(dh_dim, 0.0);

    BJtensor Constitutive( def_dim_4, 0.0);

    double det_of_Jacobian = 0.0;

    static std::vector<int> disp_dim({20,3});  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor incremental_displacements(disp_dim,0.0);

    straintensor incremental_strain;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    stresstensor final_linearized_stress;
    //    stresstensor incremental_stress;
    // BJtensor of incremental displacements taken from node objects for this element !
    incremental_displacements = incr_disp();
    //incremental_displacements.print("disp","\n incremental_displacements XC::BJtensor at GAUSS point in iterative_Update\n");

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal = dh("ij") * JacobianINV("jk");// Zhaohui 09-02-2001
         dhGlobal = dh("ij") * JacobianINV("kj");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;
                //..::printf("\n\nIN THE nodal forces ----**************** where = %d \n", where);
                //..::printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //..                           GP_c_r,GP_c_s,GP_c_t);
                //..::printf("WEIGHT = %f", weight);
                //..::printf("determinant of Jacobian = %f", det_of_Jacobian);
                // incremental strains at this Gauss point
                // now in Update we know the incremental displacements so let's find
                // the incremental strain
                incremental_strain =
                 (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                incremental_strain.null_indices();
                //incremental_strain.reportshort("\n incremental_strain XC::BJtensor at GAUSS point in iterative_Update\n");

                //Constitutive = GPtangent_E[where];

         //EPState *tmp_eps = (matpoint[where]).getEPS();
         //NDMaterial *tmp_ndm = (matpoint[where]).getNDMat();

  //if( tmp_eps ) {     //Elasto-plastic case
  //    mmodel->setEPS( *tmp_eps );
  if( ! (matpoint[where]->matmodel)->setTrialStrainIncr( incremental_strain)  )
    std::cerr << "XC::TwentyNodeBrick::linearized_nodal_forces (tag: " << this->getTag() << "), not converged\n";

  Constitutive = (matpoint[where]->matmodel)->getTangentTensor();
        //    matpoint[where].setEPS( mmodel->getEPS() ); //Set the new EPState back
  //}
  //else if( tmp_ndm ) { //Elastic case
  //    (matpoint[where].p_matmodel)->setTrialStrainIncr( incremental_strain );
  //    Constitutive = (matpoint[where].p_matmodel)->getTangentTensor();
  //}
  //else {
                //   std::cerr << "XC::TwentyNodeBrick::incremental_Update (tag: %d), could not getTangentTensor", this->getTag();
  //   exit(1);
  //}

  //Constitutive = ( matpoint[where].getEPS() )->getEep();
                //..//GPtangent_E[where].print("\n tangent E at GAUSS point \n");

                final_linearized_stress =
                  Constitutive("ijkl") * incremental_strain("kl");

                // nodal forces See Zienkievicz part 1 XC::pp 108
                linearized_nodal_forces = linearized_nodal_forces +
                          dhGlobal("ib")*final_linearized_stress("ab")*weight;
                //::::                   nodal_forces.print("nf","\n\n Nodal Forces \n");

              }
          }
      }


    return linearized_nodal_forces;

  }

//....////#############################################################################
//....// updates Gauss point stresses and strains from given displacements
//....void XC::TwentyNodeBrick::update_stress_strain(BJtensor & displacementsT)
//....  {
//....//    std::vector<int> force_dim({20,3});
//....//    BJtensor nodal_forces(force_dim,0.0);
//....
//....    double r  = 0.0;
//....    double rw = 0.0;
//....    double s  = 0.0;
//....    double sw = 0.0;
//....    double t  = 0.0;
//....    double tw = 0.0;
//....
//....    short where = 0;
//....    double weight = 0.0;
//....
//....    std::vector<int> dh_dim({20,3});
//....    BJtensor dh(dh_dim, 0.0);
//....
//....    stresstensor stress_at_GP(0.0);
//....    straintensor strain_at_GP(0.0);
//....
//....    double det_of_Jacobian = 0.0;
//....
//....    BJtensor Jacobian;
//....    BJtensor JacobianINV;
//....    BJtensor dhGlobal;
//....
//....    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
//....      {
//....        r = get_Gauss_p_c( r_integration_order, GP_c_r );
//....        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
//....
//....        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
//....          {
//....            s = get_Gauss_p_c( s_integration_order, GP_c_s );
//....            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
//....
//....            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
//....              {
//....                t = get_Gauss_p_c( t_integration_order, GP_c_t );
//....                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
//....
//....// this short routine is supposed to calculate position of
//....// Gauss point from 3D array of short's
//....                where =
//....                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
//....
//....//........................................................
//....//........................................................
//....// interpolation functions
//....                BJtensor h = b3darray[0].interp_poli_at(r,s,t);
//....                ::printf("\n\n r = %f, s = %f, t = %f\n", r, s, t);
//....//  h.print("h");
//....
//....// displacements
//....//....   BJtensor disp_at_rst = h("i")*displacementsT("ia");
//....//....   disp_at_rst.print("disp");
//....
//....// derivatives of interpolation functions
//....                dh = dh_drst_at(r,s,t);
//....//                ::printf("\n\n r = %f, s = %f, t = %f\n", r, s, t);
//....//  dh.print("dh");
//....
//....                Jacobian = b3darray[0].Jacobian_3D(dh);
//....//                Jacobian.print("J");
//....
//....                JacobianINV = b3darray[0].Jacobian_3Dinv(dh);
//....//                JacobianINV.print("JINV");
//....
//....//                det_of_Jacobian = Jacobian.determinant();
//....//                ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );
//....
//....// Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
//....                dhGlobal = dh("ij") * JacobianINV("jk");
//....// strains
//....//  strain = (dh("ib")*displacements("ia")).symmetrize11();
//....                strain = (dhGlobal("ib")*displacementsT("ia")).symmetrize11();
//....//  straintensor strain = dh("ib")*displacements("ia");
//....                strain.reportshort("\n strain XC::BJtensor\n");
//....                strain.null_indices();
//....
//....//                BJtensor E = mmElastic.ElasticStiffness();
//....
//....//stresses
//....                stress = E("ijkl") * strain("kl");
//....                stress.reportshort("\n\n stress XC::BJtensor \n");
//....//...
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....
//....
//....              }
//....          }
//....      }
//....
//....  }

////#############################################################################
////#############################################################################
//double XC::TwentyNodeBrick::get_first_q_ast(void)
//  {
//    double ret = matpoint[0].kappa_cone_get();
//
//    return ret;
//
//  }
////#############################################################################
//double XC::TwentyNodeBrick::get_first_etacone(void)
//  {
//    double ret = matpoint[0].etacone();
//
//    return ret;
//
//  }
//

//#############################################################################
void XC::TwentyNodeBrick::report(const std::string & msg)
  {
    std::cerr << "** " << msg;
    ::printf("\n XC::Element Number = %d\n", this->getTag() );
    ::printf("\n Number of nodes in a TwentyNodebrick = %d\n",getNumExternalNodes());
    ::printf("\n Determinant of Jacobian (! ==0 before comp.) = %f\n",
                                                  determinant_of_Jacobian);

    ::printf("Node numbers \n");
    ::printf(".....1.....2.....3.....4.....5.....6.....7.....8.....9.....0.....1.....2\n");
           for( int i=0 ; i<20 ; i++ )
     //::printf("%6d",G_N_numbs[i]);
     ::printf("%6d",theNodes.getTagNode(i));
    ::printf("\n");
    //           for( int j=8 ; j<20 ; j++ )
    //             ::printf("%6d",G_N_numbs[j]);    // Commented by Xiaoyan
    ::printf("\n\n");

    //    ::printf("Node existence array \n");
    //           for( int k=0 ; k<15 ; k++ )
    //             ::printf("%6d",node_existance[k]);
    //           ::printf("\n\n");       // Commented by Xiaoyan


    int total_number_of_Gauss_points = r_integration_order*
                                       s_integration_order*
                                       t_integration_order;
    if( total_number_of_Gauss_points != 0 )
      {
           // report from XC::Node class
           //for( int in=0 ; in<8 ; in++ )
           //             (nodes[G_N_numbs[in]]).report("nodes from within element (first 8)\n");
           //Xiaoyan changed .report to . Print in above line 09/27/00
    //  (nodes[G_N_numbs[in]]).Print(std::cerr);

    theNodes[0]->Print(std::cerr);
    theNodes[1]->Print(std::cerr);
    theNodes[2]->Print(std::cerr);
    theNodes[3]->Print(std::cerr);
    theNodes[4]->Print(std::cerr);
    theNodes[5]->Print(std::cerr);
           theNodes[6]->Print(std::cerr);
    theNodes[7]->Print(std::cerr);
    theNodes[8]->Print(std::cerr);
    theNodes[9]->Print(std::cerr);
    theNodes[10]->Print(std::cerr);
     theNodes[11]->Print(std::cerr);
     theNodes[12]->Print(std::cerr);
    theNodes[13]->Print(std::cerr);
    theNodes[14]->Print(std::cerr);
    theNodes[15]->Print(std::cerr);
    theNodes[16]->Print(std::cerr);
    theNodes[17]->Print(std::cerr);
    theNodes[18]->Print(std::cerr);
    theNodes[19]->Print(std::cerr);

    //           for( int jn=8 ; jn<20 ; jn++ )
           //             (nodes[G_N_numbs[jn]]).report("nodes from within element (last 15)\n");
           // Commented by Xiaoyan
      }

    ::printf("\n\nGauss-Legendre integration order\n");
    ::printf("Integration order in r direction = %d\n",r_integration_order);
    ::printf("Integration order in s direction = %d\n",s_integration_order);
    ::printf("Integration order in t direction = %d\n\n",t_integration_order);



    for( int GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        for( int GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            for( int GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                short where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                ::printf("\n\n----------------**************** where = %d \n", where);
                ::printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                            GP_c_r,GP_c_s,GP_c_t);
                matpoint[where]->report("Material Point\n");
                //GPstress[where].reportshort("stress at Gauss Point");
                //GPstrain[where].reportshort("strain at Gauss Point");
                //matpoint[where].report("Material model  at Gauss Point");
              }
          }
      }

  }


//#############################################################################
void XC::TwentyNodeBrick::reportshort(const std::string & msg)
  {
    std::cerr << "** " << msg;
    ::printf("\n XC::Element Number = %d\n", this->getTag() );
    ::printf("\n Number of nodes in a XC::TwentyNodeBrick = %d\n",getNumExternalNodes());
    ::printf("\n Determinant of Jacobian (! ==0 before comp.) = %f\n",
                                                  determinant_of_Jacobian);

    ::printf("Node numbers \n");
    ::printf(".....1.....2.....3.....4.....5.....6.....7.....8.....9.....0.....1.....2\n");
    for( int i=0 ; i<getNumExternalNodes(); i++ )
             //::printf("%6d",G_N_numbs[i]);
             ::printf( "%6d",theNodes.getTagNode(i) );

    ::printf("\n");
           //           for( int j=8 ; j<20 ; j++ )
           //             ::printf("%6d",G_N_numbs[j]);   //// Commented by Xiaoyan
           ::printf("\n\n");

           //    ::printf("Node existence array \n");
           //           for( int k=0 ; k<15 ; k++ )
           //             ::printf("%6d",node_existance[k]);    // Commented by Xiaoyan
           ::printf("\n\n");

  }




//#############################################################################
void XC::TwentyNodeBrick::reportPAK(const std::string & msg)
  {
    std::cerr << "** " << msg;
    ::printf("%10d   ",  this->getTag());
    static const int nodes_in_brick= getNumExternalNodes();
    for( int i=0 ; i<nodes_in_brick ; i++ )
       ::printf( "%6d",theNodes.getTagNode(i) );
       //::printf("%6d",G_N_numbs[i]);

    printf("\n");
  }


//#############################################################################
void XC::TwentyNodeBrick::reportpqtheta(int GP_numb)
  {
    short where = GP_numb-1;
    matpoint[where]->reportpqtheta("");
  }

//#############################################################################
//void XC::TwentyNodeBrick::reportLM(const std::string & msg)
//  {
//    if( msg ) ::printf("%s",msg);
//    ::printf("Element # %d, LM->", this->get_Brick_Number());
//    for(int count = 0 ; count < 24 ; count++)
//      {
//        ::printf(" %d", LM[count]);
//      }
//    ::printf("\n");
//
//  }

//#############################################################################
//Compute Gauss Point coordinates and store it in global Gsc
void XC::TwentyNodeBrick::computeGaussPoint(void)
{
    //    if( msg ) ::printf("** %s\n",msg);

    // special case for 8 nodes only
    // special case for 8 nodes only
    int count;
    count = FixedOrder*FixedOrder*FixedOrder;
    //Vector Gsc(count*3+1); //+1: number of Gauss point in element
    Gsc(0) = count;

    double r  = 0.0;
    double s  = 0.0;
    double t  = 0.0;

    short where = 0;

    // special case for 8 nodes only
    static const std::vector<int> dim({3, 20}); // static-> see ARM pp289-290
    static const std::vector<int> dim27({3, count}); // static-> see ARM pp289-290 Joey found a bug: dimension count is integr. points dependent
    BJtensor NodalCoord(dim, 0.0);
    BJtensor matpointCoord(dim27, 0.0);
    std::vector<int> h_dim({60,3});
    BJtensor H(h_dim, 0.0);

    //for(int ncount = 1 ; ncount <= 8 ; ncount++ )
    ////  for(int ncount = 0 ; ncount <= 7 ; ncount++ )
    //  {
    // //int global_node_number = get_global_number_of_node(ncount-1);
    // // printf("global node num %d",global_node_number);
    //
    //    //   NodalCoord(1,ncount) = nodes[global_node_number].x_coordinate();
    //    //   NodalCoord(2,ncount) = nodes[global_node_number].y_coordinate();
    //    //   NodalCoord(3,ncount) = nodes[global_node_number].z_coordinate();
    //    // Xiaoyan changed to the following:  09/27/00
    // Vector Coordinates = nodes[global_node_number].getCrds();
    //
    //    NodalCoord(1,ncount) = Coordinates(0);
    //    NodalCoord(2,ncount) = Coordinates(1);
    //    NodalCoord(3,ncount) = Coordinates(2);
    //printf("global point %d     x=%+.6e   y=%+.6e   z=%+.6e \n ", global_node_number,
    //                                                      NodalCoord(1,ncount),
    //            NodalCoord(2,ncount),
    //            NodalCoord(3,ncount));
    //}

    //Zhaohui using node pointers, which come from the XC::Domain
    const Vector &nd1Crds = theNodes[0]->getCrds();
    const Vector &nd2Crds = theNodes[1]->getCrds();
    const Vector &nd3Crds = theNodes[2]->getCrds();
    const Vector &nd4Crds = theNodes[3]->getCrds();

    const Vector &nd5Crds = theNodes[4]->getCrds();
    const Vector &nd6Crds = theNodes[5]->getCrds();
    const Vector &nd7Crds = theNodes[6]->getCrds();
    const Vector &nd8Crds = theNodes[7]->getCrds();

    const Vector &nd9Crds  = theNodes[8]->getCrds();
    const Vector &nd10Crds = theNodes[9]->getCrds();
    const Vector &nd11Crds = theNodes[10]->getCrds();
    const Vector &nd12Crds = theNodes[11]->getCrds();

    const Vector &nd13Crds = theNodes[12]->getCrds();
    const Vector &nd14Crds = theNodes[13]->getCrds();
    const Vector &nd15Crds = theNodes[14]->getCrds();
    const Vector &nd16Crds = theNodes[15]->getCrds();

    const Vector &nd17Crds = theNodes[16]->getCrds();
    const Vector &nd18Crds = theNodes[17]->getCrds();
    const Vector &nd19Crds = theNodes[18]->getCrds();
    const Vector &nd20Crds = theNodes[19]->getCrds();

    NodalCoord(1, 1)=nd1Crds( 0); NodalCoord(2, 1)=nd1Crds( 1); NodalCoord(3, 1)=nd1Crds( 2);
    NodalCoord(1, 2)=nd2Crds( 0); NodalCoord(2, 2)=nd2Crds( 1); NodalCoord(3, 2)=nd2Crds( 2);
    NodalCoord(1, 3)=nd3Crds( 0); NodalCoord(2, 3)=nd3Crds( 1); NodalCoord(3, 3)=nd3Crds( 2);
    NodalCoord(1, 4)=nd4Crds( 0); NodalCoord(2, 4)=nd4Crds( 1); NodalCoord(3, 4)=nd4Crds( 2);
    NodalCoord(1, 5)=nd5Crds( 0); NodalCoord(2, 5)=nd5Crds( 1); NodalCoord(3, 5)=nd5Crds( 2);
    NodalCoord(1, 6)=nd6Crds( 0); NodalCoord(2, 6)=nd6Crds( 1); NodalCoord(3, 6)=nd6Crds( 2);
    NodalCoord(1, 7)=nd7Crds( 0); NodalCoord(2, 7)=nd7Crds( 1); NodalCoord(3, 7)=nd7Crds( 2);
    NodalCoord(1, 8)=nd8Crds( 0); NodalCoord(2, 8)=nd8Crds( 1); NodalCoord(3, 8)=nd8Crds( 2);
    NodalCoord(1, 9)=nd9Crds( 0); NodalCoord(2, 9)=nd9Crds( 1); NodalCoord(3, 9)=nd9Crds( 2);
    NodalCoord(1,10)=nd10Crds(0); NodalCoord(2,10)=nd10Crds(1); NodalCoord(3,10)=nd10Crds(2);
    NodalCoord(1,11)=nd11Crds(0); NodalCoord(2,11)=nd11Crds(1); NodalCoord(3,11)=nd11Crds(2);
    NodalCoord(1,12)=nd12Crds(0); NodalCoord(2,12)=nd12Crds(1); NodalCoord(3,12)=nd12Crds(2);
    NodalCoord(1,13)=nd13Crds(0); NodalCoord(2,13)=nd13Crds(1); NodalCoord(3,13)=nd13Crds(2);
    NodalCoord(1,14)=nd14Crds(0); NodalCoord(2,14)=nd14Crds(1); NodalCoord(3,14)=nd14Crds(2);
    NodalCoord(1,15)=nd15Crds(0); NodalCoord(2,15)=nd15Crds(1); NodalCoord(3,15)=nd15Crds(2);
    NodalCoord(1,16)=nd16Crds(0); NodalCoord(2,16)=nd16Crds(1); NodalCoord(3,16)=nd16Crds(2);
    NodalCoord(1,17)=nd17Crds(0); NodalCoord(2,17)=nd17Crds(1); NodalCoord(3,17)=nd17Crds(2);
    NodalCoord(1,18)=nd18Crds(0); NodalCoord(2,18)=nd18Crds(1); NodalCoord(3,18)=nd18Crds(2);
    NodalCoord(1,19)=nd19Crds(0); NodalCoord(2,19)=nd19Crds(1); NodalCoord(3,19)=nd19Crds(2);
    NodalCoord(1,20)=nd20Crds(0); NodalCoord(2,20)=nd20Crds(1); NodalCoord(3,20)=nd20Crds(2);

    //NodalCoord.print("NC");

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates

               H = H_3D(r,s,t);
        //H.print("H");
         static const int nodes_in_brick= getNumExternalNodes();
         for(int encount=1 ; encount <= nodes_in_brick; encount++ )
               //        for(int encount=0 ; encount <= 7 ; encount++ )
        {
                  //  matpointCoord(1,where+1) =+NodalCoord(1,where+1) * H(encount*3-2,1);
                  //  matpointCoord(2,where+1) =+NodalCoord(2,where+1) * H(encount*3-1,2);
                  //  matpointCoord(3,where+1) =+NodalCoord(3,where+1) * H(encount*3-0,3);
                  matpointCoord(1,where+1) +=NodalCoord(1,encount) * H(encount*3-2,1);
                  //::printf("-- NO nodal, H_val :%d %+.2e %+.2e %+.5e\n", encount,NodalCoord(1,encount),H(encount*3-2,1),matpointCoord(1,where+1) );
                  matpointCoord(2,where+1) +=NodalCoord(2,encount) * H(encount*3-1,2);
                  matpointCoord(3,where+1) +=NodalCoord(3,encount) * H(encount*3-0,3);
        }

               //::printf("gauss point# %d   %+.6e %+.6e %+.6e \n", where+1,
               //                               matpointCoord(1,where+1),
               //                               matpointCoord(2,where+1),
               //                               matpointCoord(3,where+1));

        Gsc(where*3+1) = matpointCoord(1,where+1);
        Gsc(where*3+2) = matpointCoord(2,where+1);
        Gsc(where*3+3) = matpointCoord(3,where+1);


              }
          }
      }
      //return Gsc;

 }


////#############################################################################

//#############################################################################
//void XC::TwentyNodeBrick::reportTensor(const std::string & msg)
// ZHaohui added to print gauss point coord. to file fp

void XC::TwentyNodeBrick::reportTensorF(FILE * fp)
  {
    //if( msg ) ::printf("** %s\n",msg);

    // special case for 8 nodes only
    // special case for 8 nodes only
    double r  = 0.0;
    double s  = 0.0;
    double t  = 0.0;

    short where = 0;

    // special case for 8 nodes only
    static const std::vector<int> dim({3, 20}); // static-> see ARM pp289-290
    BJtensor NodalCoord(dim, 0.0);
    BJtensor matpointCoord(dim, 0.0);
    std::vector<int> h_dim({60,3});  // Xiaoyan changed from {60,3} to {24,3} for 8 nodes

    BJtensor H(h_dim, 0.0);

    //for(int ncount = 1 ; ncount <= 8 ; ncount++ )
    //  // for(int ncount = 0 ; ncount <= 7 ; ncount++ )
    //  {
    // int global_node_number = get_global_number_of_node(ncount-1);
    // // printf("global node num %d",global_node_number);
    //
    //    //        NodalCoord(1,ncount) = nodes[global_node_number].x_coordinate();
    //    //        NodalCoord(2,ncount) = nodes[global_node_number].y_coordinate();
    //    //        NodalCoord(3,ncount) = nodes[global_node_number].z_coordinate();
    //    // Xiaoyan changed to the following:  09/27/00
    // Vector Coordinates = nodes[global_node_number].getCrds();
    //    NodalCoord(1,ncount) = Coordinates(0);
    //    NodalCoord(2,ncount) = Coordinates(1);
    //    NodalCoord(3,ncount) = Coordinates(2);
    //printf("global point %d     x=%+.6e   y=%+.6e   z=%+.6e \n ", global_node_number,
    //                                                      NodalCoord(1,ncount),
    //            NodalCoord(2,ncount),
    //            NodalCoord(3,ncount));
    //  }

    //Zhaohui using node pointers, which come from the XC::Domain
    const Vector &nd1Crds = theNodes[0]->getCrds();
    const Vector &nd2Crds = theNodes[1]->getCrds();
    const Vector &nd3Crds = theNodes[2]->getCrds();
    const Vector &nd4Crds = theNodes[3]->getCrds();
    const Vector &nd5Crds = theNodes[4]->getCrds();
    const Vector &nd6Crds = theNodes[5]->getCrds();
    const Vector &nd7Crds = theNodes[6]->getCrds();
    const Vector &nd8Crds = theNodes[7]->getCrds();
    const Vector &nd9Crds  =  theNodes[8]->getCrds();
    const Vector &nd10Crds = theNodes[9]->getCrds();
    const Vector &nd11Crds = theNodes[10]->getCrds();
    const Vector &nd12Crds = theNodes[11]->getCrds();
    const Vector &nd13Crds = theNodes[12]->getCrds();
    const Vector &nd14Crds = theNodes[13]->getCrds();
    const Vector &nd15Crds = theNodes[14]->getCrds();
    const Vector &nd16Crds = theNodes[15]->getCrds();
    const Vector &nd17Crds = theNodes[16]->getCrds();
    const Vector &nd18Crds = theNodes[17]->getCrds();
    const Vector &nd19Crds = theNodes[18]->getCrds();
    const Vector &nd20Crds = theNodes[19]->getCrds();

    NodalCoord(1,1)=nd1Crds(0); NodalCoord(2,1)=nd1Crds(1); NodalCoord(3,1)=nd1Crds(2);
    NodalCoord(1,2)=nd2Crds(0); NodalCoord(2,2)=nd2Crds(1); NodalCoord(3,2)=nd2Crds(2);
    NodalCoord(1,3)=nd3Crds(0); NodalCoord(2,3)=nd3Crds(1); NodalCoord(3,3)=nd3Crds(2);
    NodalCoord(1,4)=nd4Crds(0); NodalCoord(2,4)=nd4Crds(1); NodalCoord(3,4)=nd4Crds(2);
    NodalCoord(1,5)=nd5Crds(0); NodalCoord(2,5)=nd5Crds(1); NodalCoord(3,5)=nd5Crds(2);
    NodalCoord(1,6)=nd6Crds(0); NodalCoord(2,6)=nd6Crds(1); NodalCoord(3,6)=nd6Crds(2);
    NodalCoord(1,7)=nd7Crds(0); NodalCoord(2,7)=nd7Crds(1); NodalCoord(3,7)=nd7Crds(2);
    NodalCoord(1,8)=nd8Crds(0); NodalCoord(2,8)=nd8Crds(1); NodalCoord(3,8)=nd8Crds(2);
    NodalCoord(1,9)=nd9Crds(0); NodalCoord(2,9)=nd9Crds(1); NodalCoord(3,9)=nd9Crds(2);
    NodalCoord(1,10)=nd10Crds(0); NodalCoord(2,10)=nd10Crds(1); NodalCoord(3,10)=nd10Crds(2);
    NodalCoord(1,11)=nd11Crds(0); NodalCoord(2,11)=nd11Crds(1); NodalCoord(3,11)=nd11Crds(2);
    NodalCoord(1,12)=nd12Crds(0); NodalCoord(2,12)=nd12Crds(1); NodalCoord(3,12)=nd12Crds(2);
    NodalCoord(1,13)=nd13Crds(0); NodalCoord(2,13)=nd13Crds(1); NodalCoord(3,13)=nd13Crds(2);
    NodalCoord(1,14)=nd14Crds(0); NodalCoord(2,14)=nd14Crds(1); NodalCoord(3,14)=nd14Crds(2);
    NodalCoord(1,15)=nd15Crds(0); NodalCoord(2,15)=nd15Crds(1); NodalCoord(3,15)=nd15Crds(2);
    NodalCoord(1,16)=nd16Crds(0); NodalCoord(2,16)=nd16Crds(1); NodalCoord(3,16)=nd16Crds(2);
    NodalCoord(1,17)=nd17Crds(0); NodalCoord(2,17)=nd17Crds(1); NodalCoord(3,17)=nd17Crds(2);
    NodalCoord(1,18)=nd18Crds(0); NodalCoord(2,18)=nd18Crds(1); NodalCoord(3,18)=nd18Crds(2);
    NodalCoord(1,19)=nd19Crds(0); NodalCoord(2,19)=nd19Crds(1); NodalCoord(3,19)=nd19Crds(2);
    NodalCoord(1,20)=nd20Crds(0); NodalCoord(2,20)=nd20Crds(1); NodalCoord(3,20)=nd20Crds(2);

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates

               H = H_3D(r,s,t);
         static const int nodes_in_brick= getNumExternalNodes();
         for(int encount=1 ; encount <= nodes_in_brick ; encount++ )
                //        for(int encount=0 ; encount <= 7 ; encount++ )
        {
                  //  matpointCoord(1,where+1) =+NodalCoord(1,where+1) * H(encount*3-2,1);
                  //  matpointCoord(2,where+1) =+NodalCoord(2,where+1) * H(encount*3-1,2);
                  //  matpointCoord(3,where+1) =+NodalCoord(3,where+1) * H(encount*3-0,3);
                  matpointCoord(1,where+1) +=NodalCoord(1,encount) * H(encount*3-2,1);
                  //::printf("-- NO nodal, H_val :%d %+.2e %+.2e %+.5e\n", encount,NodalCoord(1,encount),H(encount*3-2,1),matpointCoord(1,where+1) );
                  matpointCoord(2,where+1) +=NodalCoord(2,encount) * H(encount*3-1,2);
                  matpointCoord(3,where+1) +=NodalCoord(3,encount) * H(encount*3-0,3);

        }

    fprintf(fp, "gauss point# %d   %+.6e %+.6e %+.6e \n", where+1,
                                                          matpointCoord(1,where+1),
                                                          matpointCoord(2,where+1),
                                                          matpointCoord(3,where+1));

    //matpoint[where].reportTensor("");


              }
          }
      }

 }

//=============================================================================
//  The following are come from XC::FourNodeQuad.cc  Xiaoyan 07/06/00
//  The following are come from XC::FourNodeQuad.cc  Xiaoyan 07/06/00
//  The following are come from XC::FourNodeQuad.cc  Xiaoyan 07/06/00
//=============================================================================


//=============================================================================
int XC::TwentyNodeBrick::getNumDOF(void) const
  {
    return 3*getNumExternalNodes();  //Changed from 2*4=8 to 3*8=24 Xiaoyan 07/06/00 LCPT 04/06/2006
  }

//=============================================================================
void XC::TwentyNodeBrick::setDomain(Domain *theDomain)
  {
    ElementBase<20>::setDomain(theDomain);
    theNodes.checkNumDOF(3,getTag());
  }

//=============================================================================
int XC::TwentyNodeBrick::commitState(void)
{
  int retVal = 0;

  // call element commitState to do any base class stuff
  if((retVal = this->XC::Element::commitState()) != 0) {
    std::cerr << "XC::TwentyNodeBrick::commitState () - failed in base class";
  }

    // int order = theQuadRule->getOrder();     // Commented by Xiaoyan

    int i;
    //int j, k;      // Xiaoyan added k for three dimension

    // Loop over the integration points and commit the material states
    int count  = r_integration_order* s_integration_order * t_integration_order;
    //for(i = 0; i < r_integration_order; i++)      // Xiaoyan changed order to
    //  for(j = 0; j < s_integration_order; j++)     // r_integration_order,
    //           // s_integration_order, and
    //     for(k = 0; k < t_integration_order; k++)     // added t_integration_order,
    //         retVal += (GaussPtheMaterial[i][j][k]).commitState();

    XC::Vector pp = getResistingForce();

    //if( this->getTag() == 1 || this->getTag() == 700)
    //{
      for(i = 0; i < count; i++)
      //for(i = 0; i < 27; i++)
      {
         retVal += matpoint[i]->commitState();
         //if(i == 4 && strcmp(matpoint[i]->matmodel->getType(),"Template3Dep") == 0)
         stresstensor st;
  stresstensor prin;
         straintensor stn;
         straintensor stnprin;

         st = matpoint[i]->getStressTensor();
         prin = st.principal();
         stn = matpoint[i]->getStrainTensor();
         stnprin = stn.principal();
         /*
  std::cerr << "\nGauss Point: " << i << std::endl;
  std::cerr << "sigma11: "<< st(1, 1) << " "<< st(1, 2) << " " << st(1, 3) << std::endl;
  std::cerr << "sigma21: "<< st(2, 1) << " "<< st(2, 2) << " " << st(2, 3) << std::endl;
   std::cerr << "sigma31: "<< st(3, 1) << " "<< st(3, 2) << " " << st(3, 3) << std::endl << std::endl;
  */
  //std::cerr << "strain11: "<< stn(1, 1) << " "<< stn(1, 2) << " " << stn(1, 3) << std::endl;
  //std::cerr << "strain21: "<< stn(2, 1) << " "<< stn(2, 2) << " " << stn(2, 3) << std::endl;
   //std::cerr << "strain31: "<< stn(3, 1) << " "<< stn(3, 2) << " " << stn(3, 3) << std::endl;

         //const double  p = -1*( prin(1, 1)+ prin(2, 2) +prin(3, 3) )/3.0;
         //const double  ev = -1*( stnprin(1, 1)+ stnprin(2, 2) + stnprin(3, 3) )/3.0;
  //std::cerr << "   " << p;

  //if(p < 0)
  //  std::cerr  << "gs pnt:" << i << "  p="<< p;


  double q;
  //if( fabs(prin(1, 1) - prin(2, 2) ) <=  0.0001 )
        if( fabs(prin(1, 1) - prin(2, 2) ) <=  0.001 )
        {
            q = prin(1, 1) - prin(3, 3);
            //std::cerr << "1 = 2";
        }
        else
            q = prin(3, 3) - prin(1, 1);

  //Triaxial compr.  fabs
        //std::cerr << "     " << st(2, 3); //tau_yz
  //std::cerr << "     " << q;
  ////----std::cerr << "     " << fabs(q);

        //std::cerr << "     " << ev << std::endl;

//out22Jan2001  if(strcmp(matpoint[i]->matmodel->getType(),"Template3Dep") == 0)
//out22Jan2001          {
//out22Jan2001          st = ( ((Template3Dep *)(matpoint[i]->matmodel))->getEPS())->getStress();
//out22Jan2001          prin = st.principal();
//out22Jan2001   }
//out22Jan2001   else
//out22Jan2001   {
//out22Jan2001           st = matpoint[i]->getStressTensor();
//out22Jan2001          prin = st.principal();
//out22Jan2001
//out22Jan2001   }

   //double  p = st.p_hydrostatic();
   //double  p = -1*( prin(1, 1)+ prin(2, 2) +prin(3, 3) )/3.0;
         //std::cerr << "\n " << prin(1, 1) << "   " << prin(2, 2) << "  " <<  prin(3, 3) << std::endl;
          //if( getTag() == 960)
          //std::cerr << " El= " << getTag() << " , p    " << p << std::endl;

   //printf(stderr, " Gauss Point i = %d ", (i+1));
   //printf(stderr, " Gauss Point i = %d ", (i+1));


          //if( p < 0 )
   //{
   //  std::cerr << getTag();
   //  std::cerr << " ***p  =    " << p << std::endl;
   //}
         //J2D
         //std::cerr << "        " << st.q_deviatoric();

         //double q;
         //if( fabs(prin(1, 1) - prin(2, 2) ) <=  0.0001 )
         //{
         //    q = prin(1, 1) - prin(3, 3);
         //    //std::cerr << "1 = 2";
         //}
         //else
         //    q = prin(3, 3) - prin(1, 1);

         //Triaxial compr.
         //std::cerr << "        " << q;
         //}
      }

      //std::cerr << " at elements " << this->getTag() << std::endl;


      //output nodal force
      //std::cerr << "    " << pp(2) << std::endl;
    //}

    return retVal;
}

//=============================================================================
int XC::TwentyNodeBrick::revertToLastCommit(void)
  {
    //  int order = theQuadRule->getOrder(); // Commented by Xiaoyan
    
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    // int retVal= ElementBase<20>::revertToLastCommit();
    int retVal= 0;

    // Loop over the integration points and revert to last committed material states
    int count  = r_integration_order* s_integration_order * t_integration_order;
    //for(i = 0; i < r_integration_order; i++)     // Xiaoyan changed order to
    // for(j = 0; j < s_integration_order; j++)    // r_integration_order,
    //     for(k = 0; k < t_integration_order; k++)    // s_integration_order, and
                // added t_integration_order,
     //retVal += (theMaterial[i][j][k]).revertToLastCommit();

    for(int i = 0; i < count; i++)
       retVal += matpoint[i]->revertToLastCommit();


    return retVal;
}

//=============================================================================
int XC::TwentyNodeBrick::revertToStart()
  {
    int retVal= ElementBase<20>::revertToStart();
    int i;     // Xiaoyan added k for three dimension

    // Loop over the integration points and revert to last committed material states
    //for(i = 0; i < r_integration_order; i++)     // Xiaoyan changed order to
    // for(j = 0; j < s_integration_order; j++)    // r_integration_order,
    //     for(k = 0; k < t_integration_order; k++)    // s_integration_order, and
          // added t_integration_order,
    //     retVal += (theMaterial[i][j][k]).revertToStart();

    int count  = r_integration_order* s_integration_order * t_integration_order;

    for(i = 0; i < count; i++)
       retVal+= matpoint[i]->revertToStart();


    return retVal;

    // Loop over the integration points and revert to initial material states
  }


//=============================================================================
const XC::Matrix &XC::TwentyNodeBrick::getTangentStiff(void) const
  {
     BJtensor stifftensor = getStiffnessTensor();
     int Ki=0;
     int Kj=0;
     static const int nodes_in_brick= getNumExternalNodes();
     for( int i=1 ; i<=nodes_in_brick ; i++ )
     {
        for( int j=1 ; j<=nodes_in_brick ; j++ )
        {
           for( int k=1 ; k<=3 ; k++ )
           {
              for( int l=1 ; l<=3 ; l++ )
              {
                 Ki = k+3*(i-1);
                 Kj = l+3*(j-1);
                 K( Ki-1 , Kj-1 ) = stifftensor(i,k,l,j);
              }
           }
        }
     }

     //std::cerr << " K " << K << std::endl;
     //K.Output(std::cerr);
     if(isDead())
      K*=dead_srf;
     return K;
  }

//=============================================================================
const XC::Matrix &XC::TwentyNodeBrick::getInitialStiff(void) const
{
  //std::cerr << "WARNING - XC::TwentyNodeBrick::getInitialStiff() - not yet implemented\n";
  return this->getTangentStiff();
}

//=============================================================================
//Get lumped mass
//const Matrix &XC::TwentyNodeBrick::getMass ()
const XC::Matrix &XC::TwentyNodeBrick::getConsMass(void) const
  {
     BJtensor masstensor = getMassTensor();
     //int Ki=0;
     //int Kj=0;

     //double tot_mass = 0.0;
     //double diag_mass = 0.0;
     double column_mass;
     static const int nodes_in_brick= getNumExternalNodes();
     for( int i=1 ; i<=nodes_in_brick*3 ; i++ )
     {
        column_mass = 0.0;
 for( int j=1 ; j<=nodes_in_brick*3 ; j++ )
        {

    //M( i-1 , j-1 ) = masstensor(i,j);

    column_mass += masstensor(i,j);
    M( i-1 , j-1 ) = 0;
    //tot_mass += M( i-1 , j-1 );
    //if(i == j)
    //   diag_mass += M( i-1 , j-1 );
        }
 M( i-1 , i-1 ) = column_mass;

     }

     //std::cerr << " tot_mass= "<< tot_mass << " column_mass =" << column_mass << " diag_mass= " <<  diag_mass << std::endl;
     //std::cerr << "" << M.Output(std::cerr);
     //std::cerr << " M " << M;

     return M;
  }

//=============================================================================
//Get consistent mass
//const Matrix &XC::TwentyNodeBrick::getConsMass ()
const XC::Matrix &XC::TwentyNodeBrick::getMass(void) const
  {
     BJtensor masstensor = getMassTensor();
     //int Ki=0;
     //int Kj=0;

     //double tot_mass = 0.0;
     //double diag_mass = 0.0;
     //double column_mass;

     static const int nodes_in_brick= getNumExternalNodes();
     for( int i=1 ; i<=nodes_in_brick*3 ; i++ )
     {
        //column_mass = 0.0;
 for( int j=1 ; j<=nodes_in_brick*3 ; j++ )
        {
    M( i-1 , j-1 ) = masstensor(i,j);

    //column_mass += masstensor(i,j);
    //M( i-1 , j-1 ) = 0;
    //tot_mass += M( i-1 , j-1 );
    //if(i == j)
    //   diag_mass += M( i-1 , j-1 );
        }
 //M( i-1 , i-1 ) = column_mass;

     }

     //std::cerr << " tot_mass= "<< tot_mass << " column_mass =" << column_mass << " diag_mass= " <<  diag_mass << std::endl;
     //std::cerr << "" << M.Output(std::cerr);
     //std::cerr << " M " << M;

     if(isDead())
       M*=dead_srf;
     return M;
  }

//! @brief Reactivates the element.
void XC::TwentyNodeBrick::alive(void)
  {
    if(isDead())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
  		  << "; not implemented yet."
                  << std::endl;
        ElementBase<20>::alive();
      }
  }

//=============================================================================
int XC::TwentyNodeBrick::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    //std::clog << "XC::TwentyNodeBrick::addLoad - load type unknown for ele with tag: " << this->getTag() << std::endl;

    if(isDead())
      std::cerr << getClassName() 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
        BrickSelfWeight *brkLoad= dynamic_cast<BrickSelfWeight *>(theLoad);
        if(brkLoad)
          {
            Vector bforce(60);
            // Check for a quick return
            //std::cerr << "rho " << rho << std::endl;
            if(rho == 0.0)
              return 0;

            Vector baNodes(60), bfx(3);
            bfx(0)= bf(0) * loadFactor;
            bfx(1)= bf(1) * loadFactor;
            bfx(2)= bf(2) * loadFactor;

            baNodes( 0) = bfx(0);
            baNodes( 1) = bfx(1);
            baNodes( 2) = bfx(2);
            baNodes( 3) = bfx(0);
            baNodes( 4) = bfx(1);
            baNodes( 5) = bfx(2);
            baNodes( 6) = bfx(0);
            baNodes( 7) = bfx(1);
            baNodes( 8) = bfx(2);
            baNodes( 9) = bfx(0);
            baNodes(10) = bfx(1);
            baNodes(11) = bfx(2);
            baNodes(12) = bfx(0);
            baNodes(13) = bfx(1);
            baNodes(14) = bfx(2);
            baNodes(15) = bfx(0);
            baNodes(16) = bfx(1);
            baNodes(17) = bfx(2);
            baNodes(18) = bfx(0);
            baNodes(19) = bfx(1);
            baNodes(20) = bfx(2);
            baNodes(21) = bfx(0);
            baNodes(22) = bfx(1);
            baNodes(23) = bfx(2);
            baNodes(24) = bfx(0);
            baNodes(25) = bfx(1);
            baNodes(26) = bfx(2);
            baNodes(27) = bfx(0);
            baNodes(28) = bfx(1);
            baNodes(29) = bfx(2);
            baNodes(30) = bfx(0);
            baNodes(31) = bfx(1);
            baNodes(32) = bfx(2);
            baNodes(33) = bfx(0);
            baNodes(34) = bfx(1);
            baNodes(35) = bfx(2);
            baNodes(36) = bfx(0);
            baNodes(37) = bfx(1);
            baNodes(38) = bfx(2);
            baNodes(39) = bfx(0);
            baNodes(40) = bfx(1);
            baNodes(41) = bfx(2);
            baNodes(42) = bfx(0);
            baNodes(43) = bfx(1);
            baNodes(44) = bfx(2);
            baNodes(45) = bfx(0);
            baNodes(46) = bfx(1);
            baNodes(47) = bfx(2);
            baNodes(48) = bfx(0);
            baNodes(49) = bfx(1);
            baNodes(50) = bfx(2);
            baNodes(51) = bfx(0);
            baNodes(52) = bfx(1);
            baNodes(53) = bfx(2);
            baNodes(54) = bfx(0);
            baNodes(55) = bfx(1);
            baNodes(56) = bfx(2);
            baNodes(57) = bfx(0);
            baNodes(58) = bfx(1);
            baNodes(59) = bfx(2);

            //Form equivalent body force
            this->getMass();
            bforce.addMatrixVector(0.0, M, baNodes, 1.0);
            load.addVector(1.0, bforce, 1.0);
          }
        else
          {
            std::cerr << "XC::TwentyNodeBrick::addLoad() - 20NodeBrick " << this->getTag() <<
 ",load type unknown\n";
            return -1;
          }
      }
    return 0;
  }


////=============================================================================
//int  XC::TwentyNodeBrick::addLoad(const Vector &addLoad)
//{
//     if(addLoad.Size() != 60) {
//      std::cerr << "XC::TwentyNodeBrick::addLoad %s\n",
//        "Vector not of correct size");
//      return -1;
//     }
//
//     // Add to the external nodal loads
//     load += addLoad;
//
//     return 0;
//}

//=============================================================================
int XC::TwentyNodeBrick::addInertiaLoadToUnbalance(const Vector &accel)
{
 // Check for a quick return
 if(rho == 0.0)
  return 0;

 // Get R * accel from the nodes
 const Vector &Raccel1  = theNodes[0]->getRV(accel);
 const Vector &Raccel2  = theNodes[1]->getRV(accel);
 const Vector &Raccel3  = theNodes[2]->getRV(accel);
 const Vector &Raccel4  = theNodes[3]->getRV(accel);
 const Vector &Raccel5  = theNodes[4]->getRV(accel);
 const Vector &Raccel6  = theNodes[5]->getRV(accel);
 const Vector &Raccel7  = theNodes[6]->getRV(accel);
 const Vector &Raccel8  = theNodes[7]->getRV(accel);
 const Vector &Raccel9  = theNodes[8]->getRV(accel);
 const Vector &Raccel10 = theNodes[9]->getRV(accel);
 const Vector &Raccel11 = theNodes[10]->getRV(accel);
 const Vector &Raccel12 = theNodes[11]->getRV(accel);
 const Vector &Raccel13 = theNodes[12]->getRV(accel);
 const Vector &Raccel14 = theNodes[13]->getRV(accel);
 const Vector &Raccel15 = theNodes[14]->getRV(accel);
 const Vector &Raccel16 = theNodes[15]->getRV(accel);
 const Vector &Raccel17 = theNodes[16]->getRV(accel);
 const Vector &Raccel18 = theNodes[17]->getRV(accel);
 const Vector &Raccel19 = theNodes[18]->getRV(accel);
 const Vector &Raccel20 = theNodes[19]->getRV(accel);

    if(3 != Raccel1.Size()  || 3 != Raccel2.Size()  || 3 != Raccel3.Size()  || 3 != Raccel4.Size() ||
        3 != Raccel5.Size()  || 3 != Raccel6.Size()  || 3 != Raccel7.Size()  || 3 != Raccel8.Size() ||
        3 != Raccel9.Size()  || 3 != Raccel10.Size() || 3 != Raccel11.Size() || 3 != Raccel12.Size()||
        3 != Raccel13.Size() || 3 != Raccel14.Size() || 3 != Raccel15.Size() || 3 != Raccel16.Size()||
        3 != Raccel17.Size() || 3 != Raccel18.Size() || 3 != Raccel19.Size() || 3 != Raccel20.Size()   ){
 // Xiaoyan changed 2 to 3 and added Racce15-18  09/27/00
std::cerr << "XC::TwentyNodeBrick::addInertiaLoadToUnbalance " <<
  "matrix and vector sizes are incompatible\n";
  return -1;
    }

 static XC::Vector ra(60);  // Changed form 8 to 24(3*8)  Xiaoyan 09/27/00

 ra( 0) = Raccel1(0);
 ra( 1) = Raccel1(1);
 ra( 2) = Raccel1(2);
 ra( 3) = Raccel2(0);
 ra( 4) = Raccel2(1);
 ra( 5) = Raccel2(2);
 ra( 6) = Raccel3(0);
 ra( 7) = Raccel3(1);
 ra( 8) = Raccel3(2);
 ra( 9) = Raccel4(0);
 ra(10) = Raccel4(1);
 ra(11) = Raccel4(2);
     ra(12) = Raccel5(0);
 ra(13) = Raccel5(1);
 ra(14) = Raccel5(2);
 ra(15) = Raccel6(0);
 ra(16) = Raccel6(1);
 ra(17) = Raccel6(2);
 ra(18) = Raccel7(0);
 ra(19) = Raccel7(1);
 ra(20) = Raccel7(2);
 ra(21) = Raccel8(0);
 ra(22) = Raccel8(1);
 ra(23) = Raccel8(2);
 ra(24) = Raccel9(0);
  ra(25) = Raccel9(1);
 ra(26) = Raccel9(2);
 ra(27) = Raccel10(0);
 ra(28) = Raccel10(1);
 ra(29) = Raccel10(2);
 ra(30) = Raccel11(0);
 ra(31) = Raccel11(1);
 ra(32) = Raccel11(2);
 ra(33) = Raccel12(0);
 ra(34) = Raccel12(1);
 ra(35) = Raccel12(2);
 ra(36) = Raccel13(0);
 ra(37) = Raccel13(1);
 ra(38) = Raccel13(2);
 ra(39) = Raccel14(0);
 ra(40) = Raccel14(1);
 ra(41) = Raccel14(2);
 ra(42) = Raccel15(0);
 ra(43) = Raccel15(1);
 ra(44) = Raccel15(2);
 ra(45) = Raccel16(0);
 ra(46) = Raccel16(1);
 ra(47) = Raccel16(2);
 ra(48) = Raccel17(0);
 ra(49) = Raccel17(1);
 ra(50) = Raccel17(2);
 ra(51) = Raccel18(0);
 ra(52) = Raccel18(1);
 ra(53) = Raccel18(2);
 ra(54) = Raccel19(0);
 ra(55) = Raccel19(1);
 ra(56) = Raccel19(2);
 ra(57) = Raccel20(0);
 ra(58) = Raccel20(1);
 ra(59) = Raccel20(2);


    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    // Mass matrix is computed in setDomain()

    //double column_mass = 0;
    //for(int i = 0; i < 24; i++)
    //   column_mass += M(1,i);
    //column_mass = column_mass/3.0;

    //std::cerr << " addInerti... column_mass " << column_mass << std::endl;

    //for(int i = 0; i < nodes_in_brick*3; i++)
    //  load(i) += -M(i,i)*ra(i);
    load.addMatrixVector(1.0, M, ra, -1.0);

    return 0;
}

//=============================================================================
const XC::Vector XC::TwentyNodeBrick::FormEquiBodyForce(void)
  {
    XC::Vector bforce(60);

    // Check for a quick return
    //std::cerr << "rho " << rho << std::endl;
    if(rho == 0.0)
      return bforce;

    Vector baNodes(60);

    baNodes( 0) = bf(0);
    baNodes( 1) = bf(1);
    baNodes( 2) = bf(2);
    baNodes( 3) = bf(0);
    baNodes( 4) = bf(1);
    baNodes( 5) = bf(2);
    baNodes( 6) = bf(0);
    baNodes( 7) = bf(1);
    baNodes( 8) = bf(2);
    baNodes( 9) = bf(0);
    baNodes(10) = bf(1);
    baNodes(11) = bf(2);
    baNodes(12) = bf(0);
    baNodes(13) = bf(1);
    baNodes(14) = bf(2);
    baNodes(15) = bf(0);
    baNodes(16) = bf(1);
    baNodes(17) = bf(2);
    baNodes(18) = bf(0);
    baNodes(19) = bf(1);
    baNodes(20) = bf(2);
    baNodes(21) = bf(0);
    baNodes(22) = bf(1);
    baNodes(23) = bf(2);
    baNodes(24) = bf(0);
    baNodes(25) = bf(1);
    baNodes(26) = bf(2);
    baNodes(27) = bf(0);
    baNodes(28) = bf(1);
    baNodes(29) = bf(2);
    baNodes(30) = bf(0);
    baNodes(31) = bf(1);
    baNodes(32) = bf(2);
    baNodes(33) = bf(0);
    baNodes(34) = bf(1);
    baNodes(35) = bf(2);
    baNodes(36) = bf(0);
    baNodes(37) = bf(1);
    baNodes(38) = bf(2);
    baNodes(39) = bf(0);
    baNodes(40) = bf(1);
    baNodes(41) = bf(2);
    baNodes(42) = bf(0);
    baNodes(43) = bf(1);
    baNodes(44) = bf(2);
    baNodes(45) = bf(0);
    baNodes(46) = bf(1);
    baNodes(47) = bf(2);
    baNodes(48) = bf(0);
    baNodes(49) = bf(1);
    baNodes(50) = bf(2);
    baNodes(51) = bf(0);
    baNodes(52) = bf(1);
    baNodes(53) = bf(2);
    baNodes(54) = bf(0);
    baNodes(55) = bf(1);
    baNodes(56) = bf(2);
    baNodes(57) = bf(0);
    baNodes(58) = bf(1);
    baNodes(59) = bf(2);

    //Form equivalent body force
    bforce.addMatrixVector(0.0, M, baNodes, 1.0);
    //std::cerr << " baNodes " << baNodes;
    //std::cerr << " M " << M;
    //if(getTag() == 886)
    //std::cerr << " @@@@@ FormEquiBodyForce  " << bforce;

    return bforce;
  }

//=============================================================================
// Setting initial E according to the initial pressure p
//void XC::TwentyNodeBrick::setInitE(void)
//{
//    //Get the coors of each node
//
//    const Vector &nd1Crds = theNodes[0]->getCrds();
//    const Vector &nd2Crds = theNodes[1]->getCrds();
//    const Vector &nd3Crds = theNodes[2]->getCrds();
//    const Vector &nd4Crds = theNodes[3]->getCrds();
//    const Vector &nd5Crds = theNodes[4]->getCrds();
//    const Vector &nd6Crds = theNodes[5]->getCrds();
//    const Vector &nd7Crds = theNodes[6]->getCrds();
//    const Vector &nd8Crds = theNodes[7]->getCrds();
//
//    //dir is the XC::ID for vertical direction, e.g. 1 means x-dir is vertical...
//    double Zavg = nd1Crds( dir-1)+
//         nd2Crds( dir-1)+
//         nd3Crds( dir-1)+
//         nd4Crds( dir-1)+
//         nd5Crds( dir-1)+
//         nd6Crds( dir-1)+
//         nd7Crds( dir-1)+
//         nd8Crds( dir-1);
//    Zavg = Zavg / 8;
//
//    //Estimate the pressure at that depth
//    double sigma_v = (Zavg - surflevel) * rho * 9.81; //units in SI system
//    double ko = 0.5;
//    double p_est = sigma_v*( 2.0*ko+1.0)/3.0;
//    //std::cerr << " Initial P " << p_est << std::endl;
//
//    int i;
//
//    // Loop over the integration points and set the initial material state
//    int count  = r_integration_order* s_integration_order * t_integration_order;
//
//    //For elastic-isotropic material
//    if(strcmp(matpoint[i]->matmodel->getType(),strTypeElasticIsotropic3D) == 0)
//    {
//       for(i = 0; i < count; i++)
//           (matpoint[i]->matmodel)->setElasticStiffness( p_est );
//    }
//
//    //return ;
//}


//=============================================================================
const XC::Vector &XC::TwentyNodeBrick::getResistingForce(void) const
  {
    std::vector<int> force_dim({20,3});
    BJtensor nodalforces(force_dim,0.0);

    nodalforces = nodal_forces();

    //converting nodalforce XC::BJtensor to vector
    static const int nodes_in_brick= getNumExternalNodes();
    for(int i = 0; i< nodes_in_brick; i++)
      for(int j = 0; j < 3; j++)
        P(i *3 + j) = nodalforces(i+1, j+1);

    //std::cerr << "P" << P;
    //std::cerr << "load" << Q;
    P = P - load;

    //std::cerr << "P-load" << P;
    if(isDead())
      P*=dead_srf;
    return P;
  }

//=============================================================================
const XC::Vector &XC::TwentyNodeBrick::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    //
    // now add dynamic terms
    // P += M * a + C * v
    //

    if(rho != 0.0)
      {
        // form the mass matrix
        this->getMass();

        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();
        const Vector &accel3 = theNodes[2]->getTrialAccel();
        const Vector &accel4 = theNodes[3]->getTrialAccel();
        const Vector &accel5 = theNodes[4]->getTrialAccel();
        const Vector &accel6 = theNodes[5]->getTrialAccel();
        const Vector &accel7 = theNodes[6]->getTrialAccel();
        const Vector &accel8 = theNodes[7]->getTrialAccel();
        const Vector &accel9 = theNodes[8]->getTrialAccel();
        const Vector &accel10 = theNodes[9]->getTrialAccel();
        const Vector &accel11 = theNodes[10]->getTrialAccel();
        const Vector &accel12 = theNodes[11]->getTrialAccel();
        const Vector &accel13 = theNodes[12]->getTrialAccel();
        const Vector &accel14 = theNodes[13]->getTrialAccel();
        const Vector &accel15 = theNodes[14]->getTrialAccel();
        const Vector &accel16 = theNodes[15]->getTrialAccel();
        const Vector &accel17 = theNodes[16]->getTrialAccel();
        const Vector &accel18 = theNodes[17]->getTrialAccel();
        const Vector &accel19 = theNodes[18]->getTrialAccel();
        const Vector &accel20 = theNodes[19]->getTrialAccel();

        static Vector a(60);  // originally 8

        a( 0) = accel1(0);
        a( 1) = accel1(1);
        a( 2) = accel1(2);
        a( 3) = accel2(0);
        a( 4) = accel2(1);
        a( 5) = accel2(2);
        a( 6) = accel3(0);
        a( 7) = accel3(1);
        a( 8) = accel3(2);
        a( 9) = accel4(0);
        a(10) = accel4(1);
        a(11) = accel4(2);
        a(12) = accel5(0);
        a(13) = accel5(1);
        a(14) = accel5(2);
        a(15) = accel6(0);
        a(16) = accel6(1);
        a(17) = accel6(2);
        a(18) = accel7(0);
        a(19) = accel7(1);
        a(20) = accel7(2);
        a(21) = accel8(0);
        a(22) = accel8(1);
        a(23) = accel8(2);
        a(24) = accel9(0);
        a(25) = accel9(1);
        a(26) = accel9(2);
        a(27) = accel10(0);
        a(28) = accel10(1);
        a(29) = accel10(2);
        a(30) = accel11(0);
        a(31) = accel11(1);
        a(32) = accel11(2);
        a(33) = accel12(0);
        a(34) = accel12(1);
        a(35) = accel12(2);
        a(36) = accel13(0);
        a(37) = accel13(1);
        a(38) = accel13(2);
        a(39) = accel14(0);
        a(40) = accel14(1);
        a(41) = accel14(2);
        a(42) = accel15(0);
        a(43) = accel15(1);
        a(44) = accel15(2);
        a(45) = accel16(0);
        a(46) = accel16(1);
        a(47) = accel16(2);
        a(48) = accel17(0);
        a(49) = accel17(1);
        a(50) = accel17(2);
        a(51) = accel18(0);
        a(52) = accel18(1);
        a(53) = accel18(2);
        a(54) = accel19(0);
        a(55) = accel19(1);
        a(56) = accel19(2);
        a(57) = accel20(0);
        a(58) = accel20(1);
        a(59) = accel20(2);

        // P += M * a
        P.addMatrixVector(1.0, M, a, 1.0);

        // add the damping forces if rayleigh damping
        if(!rayFactors.nullValues())
          P+= this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.nullKValues())
          P+= this->getRayleighDampingForces();
      }
    if(isDead())
      P*=dead_srf;
    return P;
  }

//=============================================================================
int XC::TwentyNodeBrick::sendSelf(Communicator &comm)
  {
     // Not implemtented yet
     return 0;
  }

//=============================================================================
int XC::TwentyNodeBrick::recvSelf(const Communicator &comm)
  {
     // Not implemtented yet
     return 0;
  }


//=============================================================================
void XC::TwentyNodeBrick::Print(std::ostream &s, int flag) const
{
    //report(" TwentyNodeBrick ");
    s << "TwentyNodeBrick, element id:  " << this->getTag() << std::endl;
    s << "Connected external nodes:  " << theNodes;

    int total_number_of_Gauss_points = r_integration_order*
                                       s_integration_order*
                                       t_integration_order;
    if( total_number_of_Gauss_points != 0 )
      {
    theNodes[0]->Print(std::cerr);
    theNodes[1]->Print(std::cerr);
    theNodes[2]->Print(std::cerr);
    theNodes[3]->Print(std::cerr);
    theNodes[4]->Print(std::cerr);
    theNodes[5]->Print(std::cerr);
           theNodes[6]->Print(std::cerr);
    theNodes[7]->Print(std::cerr);
    theNodes[8]->Print(std::cerr);
    theNodes[9]->Print(std::cerr);
    theNodes[10]->Print(std::cerr);
    theNodes[11]->Print(std::cerr);
    theNodes[12]->Print(std::cerr);
    theNodes[13]->Print(std::cerr);
    theNodes[14]->Print(std::cerr);
    theNodes[15]->Print(std::cerr);
    theNodes[16]->Print(std::cerr);
    theNodes[17]->Print(std::cerr);
    theNodes[18]->Print(std::cerr);
    theNodes[19]->Print(std::cerr);

    }
    s << "Element mass density:  " << rho << std::endl << std::endl;
    s << "Material model:  " << std::endl;

    for( int GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
    {
      for( int GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
      {
        for( int GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
        {
           // this short routine is supposed to calculate position of
           // Gauss point from 3D array of short's
           short where =
           ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

           s << "\n where = " << where << std::endl;
           s << " GP_c_r= " << GP_c_r << "GP_c_s = " << GP_c_s << " GP_c_t = " << GP_c_t << std::endl;
           matpoint[where]->report("Material Point\n");
           //GPstress[where].reportshort("stress at Gauss Point");
           //GPstrain[where].reportshort("strain at Gauss Point");
           //matpoint[where].report("Material model  at Gauss Point");
        }
      }
    }

}

//=============================================================================
 XC::Response * XC::TwentyNodeBrick::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
{
    //========================================================
    if(argv[0] == "force" || argv[0] == "forces")
  return new ElementResponse(this, 1, P);

    //========================================================
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 5, K);

    //========================================================
    else if(argv[0] == "plastic" || argv[0] == "plastified")
    {
       ////checking if element plastified
       //int count  = r_integration_order* s_integration_order * t_integration_order;
       //straintensor pl_stn;
       //int plastify = 0;
       //
       //for(int i = 0; i < count; i++) {
       //  pl_stn = matpoint[i]->getPlasticStrainTensor();
       //  double  p_plastc = pl_stn.p_hydrostatic();
       //
       //  if(  fabs(p_plastc) > 0 ) {
       //     plastify = 1;
       //     break;
       //  }
       //}

       return new ElementResponse(this, 2, InfoPt);
    }
    //========================================================
    //Specially designed for moment computation of solid pile elements Zhaohui Yang UCDavis August 1, 2001
    else if(argv[0] == "PileM" || argv[0] == "PileM")
    {
       return new ElementResponse(this, 3, InfoSt);
    }
    //========================================================
    else if(argv[0] == "stress" || argv[0] == "stresses")
    {
       return new ElementResponse(this, 4, InfoSt);
    }

    //========================================================
    else if(argv[0] == "pq" || argv[0] == "PQ")
    {
       return new ElementResponse(this, 41, InfoSpq2);
    }

    //========================================================
    else if(argv[0] == "GaussPoint" || argv[0] == "gausspoint")
    {
       return new ElementResponse(this, 6, Gsc);
    }
    /*else if(argv[0] == "material" || argv[0] == "integrPoint") {
        int pointNum = atoi(argv[1]);
 if(pointNum > 0 && pointNum <= 4)
  return setMaterialResponse(theMaterial[pointNum-1],argv,2,eleInfo);
        else
         return 0;
    }*/

    // otherwise response quantity is unknown for the quad class
    else
  return 0;
}

//=============================================================================
int XC::TwentyNodeBrick::getResponse (int responseID, Information &eleInfo)
{
       switch (responseID) {

    case 1:
     return eleInfo.setVector( this->getResistingForce() );

    case 2:
              {
  //checking if element plastified
                int count  = r_integration_order* s_integration_order * t_integration_order;

                //Vector Gsc(81+1);  // 27*3 + count
  //Gsc = this->reportGaussPoint();
  this->computeGaussPoint();

  //Vector Info(109); // count * 4 +1
  InfoPt(0) = Gsc(0);

                straintensor pl_stn;

  int plastify;
                for(int i = 0; i < count; i++) {
                  plastify = 0;
       InfoPt(i*4+1) = Gsc(i*3+1); //x
       InfoPt(i*4+2) = Gsc(i*3+2); //y
       InfoPt(i*4+3) = Gsc(i*3+3); //z
                  pl_stn = matpoint[i]->getPlasticStrainTensor();
                  //double  p_plastc = pl_stn.p_hydrostatic();
                  double  q_plastc = pl_stn.q_deviatoric();

    //if(  fabs(p_plastc) > 0 ) {
                  //   plastify = 1;
                  //}
    //else
    //   plastify = 0;

       InfoPt(i*4+4) = q_plastc; //plastify; //Plastified?

                }
     return eleInfo.setVector( InfoPt );
  //return plastify;

       }
    case 3:
       {
         //int count = r_integration_order* s_integration_order * t_integration_order;
         stresstensor sts;
         //Vector Gsc(81+1);  // 27*3 + count
  this->computeGaussPoint();
  XC::Vector wt(9);
  int i, rs;

  //Vector Info(109 + 3 ); //Z values, x-disp. and corresponding avg. moment
  Info(0) = Gsc(0);
  XC::Vector Mt(3), Q(3);

  //Vector Zcoor(3);
  Info(109+0) = Gsc(6); //Zcoor of middle layer

                //Computing Height of element
  //const Vector &coor= theNodes[17]->getCrds();
  const Vector &TotDis = theNodes[17]->getTrialDisp();
         //checking Z-coor. of moddile layer gauss point
  //if( (coor(2) - Gsc(6)) > 0.0001 )
  //  std::cerr << " Warning: Middle layer Gauss Point Z-coor. wrong...\n";

  //Info(109+0) = Gsc(6);
  Info(109+1) = TotDis(0); //x-displacement ...Lateral displacement
  //Info(109+3) = Gsc(6);
  //Info(109+6) = Gsc(9);
  //std::cerr << " Zz " << Gsc(3) << " " << Gsc(6) << " "<< Gsc(9) << std::endl;

  const std::string &tp = matpoint[1]->getType();
                int tag = matpoint[1]->getTag();
  //std::cerr << "Material Tag:" << tag << std::endl;
  //tp = strTypeElasticIsotropic3D;
  float height = 1;
            //std::cerr << "height" << height;
  double offset[30];
  //single pile group My ---- change multiplier to y
  offset[1] = -0.000;/*pile no. 1 */  offset[4] = -0.000;/*pile no. 4 3X3*/
  offset[2] =  0.000;/*pile no. 2 */  offset[5] =  0.000;/*pile no. 5    */
  offset[3] =  0.000;/*pile no. 3 */  offset[6] =  0.000;/*pile no. 6    */

  //3X3 pile group My ---- change multiplier to y
  //offset[1] = -1.287;/*pile no. 1 */  offset[4] = -1.287;/*pile no. 4 3X3 or 2X2*/
  //offset[2] =  0.000;/*pile no. 2 */  offset[5] =  0.000;/*pile no. 5    */
  //offset[3] =  1.287;/*pile no. 3 */  offset[6] =  1.287;/*pile no. 6    */

  //3X3 pile group Mx ---- change multiplier to y
  //offset[1] = 1.287;/*pile no. 1 */  offset[4] = 0.0000;/*pile no. 4 3X3*/
  //offset[2] = 1.287;/*pile no. 2 */  offset[5] = 0.0000;/*pile no. 5    */
  //offset[3] = 1.287;/*pile no. 3 */  offset[6] = 0.0000;/*pile no. 6    */

  //4X3 pile group My  ---- change multiplier to x
  //offset[1] = -1.9305;/*pile no. 1*/  offset[5] = -1.9305;/*pile no. 4 4X3*/
  //offset[2] = -0.6435;/*pile no. 2*/  offset[6] = -0.6435;/*pile no. 5    */
  //offset[3] =  0.6435;/*pile no. 3*/  offset[7] =  0.6435;/*pile no. 6    */
  //offset[4] =  1.9305;/*pile no. 3*/  offset[8] =  1.9305;/*pile no. 6    */

  //4X3 pile group Mx  ---- change multiplier to y
  //offset[1] = 1.287;/*pile no. 1*/  offset[5] = 0.0000;/*pile no. 4 4X3*/
  //offset[2] = 1.287;/*pile no. 2*/  offset[6] = 0.0000;/*pile no. 5    */
  //offset[3] = 1.287;/*pile no. 3*/  offset[7] = 0.0000;/*pile no. 6    */
  //offset[4] = 1.287;/*pile no. 3*/  offset[8] = 0.0000;/*pile no. 6    */


  if(tp==strTypeElasticIsotropic3D)
    {
     wt = getWeightofGP();
            const Vector &end1Crd = theNodes[0]->getCrds();
                   const Vector &end5Crd = theNodes[4]->getCrds();
     height = end1Crd(2) - end5Crd(2);
     //if(getTag() == 432) {
     //   std::cerr << getTag() << " height " << height << std::endl;
     //   std::cerr << " Weight " << wt << std::endl;
     //}
    }


  Mt(0) = 0; Mt(1) = 0; Mt(2) = 0;
  //Q(0) = 0; Q(1) = 0; Q(2) = 0;


                for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
                {
                    //r = get_Gauss_p_c( r_integration_order, GP_c_r );
                    for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
                    {
                        //s = get_Gauss_p_c( s_integration_order, GP_c_s );
                        rs = (GP_c_r-1)*s_integration_order+GP_c_s-1;

   for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
                        {
            //for(int i = 0; i < count; i++)
                          i =
                             ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                          sts = matpoint[i]->getStressTensor();
        Info(i*4+1) = Gsc(i*3+1); //x
        Info(i*4+2) = Gsc(i*3+2); //y
        Info(i*4+3) = Gsc(i*3+3); //z
        Info(i*4+4) = sts(3,3);//Assign sigma_zz
            //if(getTag() == 432) {
     //    sts.print();
     //    std::cerr << " rs " << rs << "\n "<< Info(i*4+1) << " " ;
     //    std::cerr << Info(i*4+2) << " "<< Info(i*4+3)<< " sts "<< Info(i*4+4) << "\n ";
            //}
     if(tp==strTypeElasticIsotropic3D)
       {
        Mt(GP_c_t-1) += wt(rs)*sts(3,3)*( Info(i*4+1)-offset[tag] )/height;//x--Calculating Moment_y wt(ts) / height = Area corresponding to the gauss point stress
        //Mt(GP_c_t-1) += wt(rs)*sts(3,3)*( Info(i*4+2)-offset[tag] )/height; //y--Calculating Moment_x wt(ts) / height = Area corresponding to the gauss point stress
                    //Q(GP_c_t-1) += wt(rs)*sts(3,1)/ height;   //Calculating Q
       }

            //if(getTag() == 432) {
     //   std::cerr << (GP_c_t-1) << " " << Mt(GP_c_t-1) << std::endl ;
     //   std::cerr << (GP_c_t-1) << " " << Q(GP_c_t-1) << std::endl ;
                   //}
          }
      }
  }
  //Storing avg. M and Q to Info
  Info(109+2) = ( Mt(0)+Mt(1)+Mt(2) )*0.3333;
  //Info(109+3) = (  Q(0)+ Q(1)+ Q(2) )*0.3333;
  //Info(109+4) = Mt(1);
  //Info(109+5) = Q(1);
  //Info(109+7) = Mt(2);
  //Info(109+8) = Q(2);

  //std::cerr << " Mt " << Mt(0) << " " << Mt(1) << " "<< Mt(2) << std::endl;
      return eleInfo.setVector( Info );
       }
    case 4:
       {
                int count = r_integration_order* s_integration_order * t_integration_order;
  int i;
                stresstensor sts;
                //Vector Gsc(81+1);  // 8*3 + count
  //Gsc = this->reportTensor("Gauss Point Coor.");

  //Vector Info(109 + 3 ); //Z values, x-disp. and corresponding avg. moment
  InfoSt(0) = count;

                for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
                {
                    //r = get_Gauss_p_c( r_integration_order, GP_c_r );
                    for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
                    {
                        //s = get_Gauss_p_c( s_integration_order, GP_c_s );
                        //rs = (GP_c_r-1)*s_integration_order+GP_c_s-1;

   for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
                        {
            //for(int i = 0; i < count; i++)
                          i =
                             ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                          sts = matpoint[i]->getStressTensor();
        InfoSt(i*6+1) = sts(1,1); //sigma_xx
        InfoSt(i*6+2) = sts(2,2); //sigma_yy
        InfoSt(i*6+3) = sts(3,3); //sigma_zz
        InfoSt(i*6+4) = sts(1,2); //Assign sigma_xy
        InfoSt(i*6+5) = sts(1,3); //Assign sigma_xz
        InfoSt(i*6+6) = sts(2,3); //Assign sigma_yz
          }
      }
  }
      return eleInfo.setVector( InfoSt );
       }
    //Added Joey 03-12-03
    case 41:
       {
                int count = r_integration_order* s_integration_order * t_integration_order;
  count = count / 2;
                stresstensor sts;
                sts = matpoint[count]->getStressTensor();
  InfoSpq2(0) =sts.p_hydrostatic();
  InfoSpq2(1) =sts.q_deviatoric();
      return eleInfo.setVector( InfoSpq2 );
       }
    case 5:
     return eleInfo.setMatrix(this->getTangentStiff());

    case 6:
    {
  this->computeGaussPoint();
     return eleInfo.setVector( Gsc );
    }

    default:
     return -1;
 }
     //return 0;
}



////#############################################################################
 XC::Vector XC::TwentyNodeBrick::getWeightofGP(void)
  {
    //std::vector<int> M_dim({8,3,3,8});
    //std::vector<int> M_dim({60,60});
    //BJtensor Mm(M_dim,0.0);

    XC::Vector Weight( FixedOrder * FixedOrder );

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    //short where = 0;
    short rs = 0;
    double tmp = 0;

    double weight = 0.0;

    std::vector<int> dh_dim({20,3});

    BJtensor dh(dh_dim, 0.0);

    double det_of_Jacobian = 0.0;

    BJtensor Jacobian;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            rs = (GP_c_r-1)*s_integration_order+GP_c_s-1;
     Weight(rs) = 0;

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                //where =
                //((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh = dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                det_of_Jacobian  = Jacobian.determinant();

  //H = H_3D(r,s,t);

                weight = rw * sw * tw * det_of_Jacobian;
  Weight(rs) += weight;//Volume calculation
               //std::cerr << " where " << where << " r  " << r << " s  " << s << " t " << t << std::endl;

  //Mm = Mm + H("ib")*H("kb")*weight;
         // printf("\n +++++++++++++++++++++++++ \n\n");
        //Mm.printshort("M");
              }
       //std::cerr << " rs " << rs << " " << Weight(rs) << std::endl;
       tmp += Weight(rs);
          }
      }
    //M = Mm;
    //Mm.printshort("M");
    //std::cerr << " total: " << tmp << std::endl;

    return Weight;
  }


////#############################################################################




//=============================================================================

//const Matrix&
//XC::TwentyNodeBrick::getTangentStiff ()
//{
// int order = theQuadRule->getOrder();
// const Vector &intPt = theQuadRule->getIntegrPointCoords();
// const Vector &intWt = theQuadRule->getIntegrPointWeights();
//
// const Vector &disp1 = theNodes[0]->getTrialDisp();
//        const Vector &disp2 = theNodes[1]->getTrialDisp();
// const Vector &disp3 = theNodes[2]->getTrialDisp();
//        const Vector &disp4 = theNodes[3]->getTrialDisp();
//       // Xiaoyan added 5-8 07/06/00
//        const Vector &disp5 = theNodes[4]->getTrialDisp();
//        const Vector &disp6 = theNodes[5]->getTrialDisp();
// const Vector &disp7 = theNodes[6]->getTrialDisp();
//        const Vector &disp8 = theNodes[7]->getTrialDisp();
//
// static XC::Vector u(24);     //Changed from u(8) to u(24) Xiaoyn 07/06/00
//
// u(0) = disp1(0);
// u(1) = disp1(1);
//        u(2) = disp1(2);
// u(3) = disp2(0);
// u(4) = disp2(1);
// u(5) = disp2(2);
//        u(6) = disp3(0);
// u(7) = disp3(1);
// u(8) = disp3(2);
// u(9) = disp4(0);
// u(10) = disp4(1);
// u(11) = disp4(2);
// u(12) = disp5(0);
// u(13) = disp5(1);
// u(14) = disp5(2);
// u(15) = disp6(0);
// u(16) = disp6(1);
// u(17) = disp6(2);
// u(18) = disp7(0);
// u(19) = disp7(1);
// u(20) = disp7(2);
// u(21) = disp8(0);
// u(22) = disp8(1);
// u(23) = disp8(2);


// static XC::Vector eps (6);    // Changed eps(3) to eps(6) Xiaoyan 07/06/00

// K.Zero();

// // Loop over the integration points
// for(int i = 0; i < order; i++)
// {
//  for(int j = 0; j < order; j++)
//  {
//
//   // Determine Jacobian for this integration point
//   this->setJacobian (intPt(i), intPt(j));
//
//   // Interpolate strains
//   this->formBMatrix (intPt(i), intPt(j));
//   eps = B*u;
//
//   // Set the material strain
//   (theMaterial[i][j])->setTrialStrain (eps);
//
//   // Get the material tangent
//   const Matrix &D = (theMaterial[i][j])->getTangent();
//
//   // Form the Jacobian of the coordinate transformation
//   double detJ = this->formDetJ (intPt(i), intPt(j));
//
//   // Perform numerical integration
//   K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
//  }
// }
//
// K = K * thickness;
//
// return K;
//}

//const Matrix&
//XC::TwentyNodeBrick::getSecantStiff ()
//{
// return K;
//}

//Commented by Xiaoyan     Use the form like Brick3d
//const Matrix & XC::TwentyNodeBrick::getDamp ()
//{
// return C;
//}
// Commented by Xiaoyan 08/04/00

//const Matrix&
//XC::TwentyNodeBrick::getMass ()
//{
// int order = theQuadRule->getOrder();
// const Vector &intPt = theQuadRule->getIntegrPointCoords();
// const Vector &intWt = theQuadRule->getIntegrPointWeights();
//
// M.Zero();
//
// int i, j;
//
// // Loop over the integration points
// for(i = 0; i < order; i++)
// {
//  for(j = 0; j < order; j++)
//  {
//   // Determine Jacobian for this integration point
//   this->setJacobian (intPt(i), intPt(j));
//
//   // Interpolate strains
//   this->formNMatrix (intPt(i), intPt(j));
//
//   // Form the Jacobian of the coordinate transformation
//   double detJ = this->formDetJ (intPt(i), intPt(j));
//
//   // Perform numerical integration
//   M = M + (N^ N) * intWt(i)*intWt(j) * detJ;
//  }
// }
//
// M = M * thickness * rho;
//
// // Lumped mass ... can be optional
// for(i = 0; i < 24; i++)      // Changed 8 to 24  Xiaoyan 07/06/00
// {
//  double sum = 0.0;
//  for(j = 0; j < 24; j++)    // Changed 8 to 24  Xiaoyan 07/06/00
//  {
//   sum += M(i,j);
//   M(i,j) = 0.0;
//  }
//  M(i,i) = sum;
// }
//
// return M;
//}
//
//const Vector&
//XC::TwentyNodeBrick::getResistingForce ()
//{
// int order = theQuadRule->getOrder();
// const Vector &intPt = theQuadRule->getIntegrPointCoords();
// const Vector &intWt = theQuadRule->getIntegrPointWeights();
//
// const Vector &disp1 = theNodes[0]->getTrialDisp();
//        const Vector &disp2 = theNodes[1]->getTrialDisp();
// const Vector &disp3 = theNodes[2]->getTrialDisp();
//        const Vector &disp4 = theNodes[3]->getTrialDisp();
// //6-8 added by Xiaoyan 07/06/00
// const Vector &disp5 = theNodes[4]->getTrialDisp();
//        const Vector &disp6 = theNodes[5]->getTrialDisp();
// const Vector &disp7 = theNodes[6]->getTrialDisp();
//        const Vector &disp8 = theNodes[7]->getTrialDisp();
//
//
// static XC::Vector u(24);     //Changed from u(8) to u(24) Xiaoyn 07/06/00
//
// u(0) = disp1(0);
// u(1) = disp1(1);
//        u(2) = disp1(2);
// u(3) = disp2(0);
// u(4) = disp2(1);
// u(5) = disp2(2);
//        u(6) = disp3(0);
// u(7) = disp3(1);
// u(8) = disp3(2);
// u(9) = disp4(0);
// u(10) = disp4(1);
// u(11) = disp4(2);
// u(12) = disp5(0);
// u(13) = disp5(1);
// u(14) = disp5(2);
// u(15) = disp6(0);
// u(16) = disp6(1);
// u(17) = disp6(2);
// u(18) = disp7(0);
// u(19) = disp7(1);
// u(20) = disp7(2);
// u(21) = disp8(0);
// u(22) = disp8(1);
// u(23) = disp8(2);
//
// eps (6);      //Changed eps(3) to eps(6) Xiaoyan 07/06/00
//
// P.Zero();
//
// // Loop over the integration points
// for(int i = 0; i < order; i++)
// {
//  for(int j = 0; j < order; j++)
//  {
//   // Determine Jacobian for this integration point
//   this->setJacobian (intPt(i), intPt(j));
//
//   // Interpolate strains
//   this->formBMatrix (intPt(i), intPt(j));
//   eps = B*u;
//
//   // Set the material strain
//   (theMaterial[i][j])->setTrialStrain (eps);
//
//   // Get material stress response
//   const Vector &sigma = (theMaterial[i][j])->getStress();
//
//   // Form the Jacobian of the coordinate transformation
//   double detJ = this->formDetJ (intPt(i), intPt(j));
//
//   // Perform numerical integration
//   P = P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
//  }
// }
//
// P = P * thickness * -1;
//
// return P;
//}
//
//const Vector&
//XC::TwentyNodeBrick::getResistingForceIncInertia ()
//{
// // Yet to implement
// return P;
//}
//
//
//
//void
//XC::TwentyNodeBrick::Print(std::ostream &s, int flag) const
//{
// s << "TwentyNodeBrick, element id:  " << this->getTag() << std::endl;
// s << "Connected external nodes:  " << theNodes;
// s << "Material model:  " << theMaterial[0][0]->getType() << std::endl;
// s << "Element thickness:  " << thickness << std::endl;
// s << "Element mass density:  " << rho << std::endl << std::endl;
//}
//
//
//int
//XC::TwentyNodeBrick::displaySelf (Renderer &theViewer, int displayMode, float fact)
//{
    // first determine the end points of the quad based on
    // the display factor (a measure of the distorted image)
    // store this information in 2 3d vectors v1 and v2
//        const Vector &end1Crd = theNodes[0]->getCrds();
//        const Vector &end2Crd = theNodes[1]->getCrds();
// const Vector &end3Crd = theNodes[2]->getCrds();
// const Vector &end4Crd = theNodes[3]->getCrds();
// // 5-8 were added by Xiaoyan
//        const Vector &end5Crd = theNodes[4]->getCrds();
//        const Vector &end6Crd = theNodes[5]->getCrds();
// const Vector &end7Crd = theNodes[6]->getCrds();
// const Vector &end8Crd = theNodes[7]->getCrds();
////---------------------------------------------------------------
//     const Vector &end1Disp = theNodes[0]->getDisp();
// const Vector &end2Disp = theNodes[1]->getDisp();
// const Vector &end3Disp = theNodes[2]->getDisp();
// const Vector &end4Disp = theNodes[3]->getDisp();
//
 // 5-8 were added by Xiaoyan
//        const Vector &end5Disp = theNodes[4]->getDisp();
// const Vector &end6Disp = theNodes[5]->getDisp();
// const Vector &end7Disp = theNodes[6]->getDisp();
// const Vector &end8Disp = theNodes[7]->getDisp();
//
// Vector v1(3);
// Vector v2(3);
// Vector v3(3);
// Vector v4(3);
// //5-8 added by Xiaoyan 07/06/00
// Vector v5(3);
// Vector v6(3);
// Vector v7(3);
// Vector v8(3);
//
// for(int i = 0; i < 3; i++)     //Changed from i<2 to i<3, Xiaonyan 07/06/00
// {
//  v1(i) = end1Crd(i) + end1Disp(i)*fact;
//  v2(i) = end2Crd(i) + end2Disp(i)*fact;
//  v3(i) = end3Crd(i) + end3Disp(i)*fact;
//  v4(i) = end4Crd(i) + end4Disp(i)*fact;
//
//  //5-8 added by Xiaoyan 07/06/00
//     v5(i) = end5Crd(i) + end1Disp(i)*fact;
//  v6(i) = end6Crd(i) + end2Disp(i)*fact;
//  v7(i) = end7Crd(i) + end3Disp(i)*fact;
//  v8(i) = end8Crd(i) + end4Disp(i)*fact;
// }
// int error = 0;
//
// error += theViewer.drawLine (v1, v2, 1.0, 1.0);
// error += theViewer.drawLine (v2, v3, 1.0, 1.0);
// error += theViewer.drawLine (v3, v4, 1.0, 1.0);
// error += theViewer.drawLine (v4, v5, 1.0, 1.0);   // 5-8 added by Xiaoyan 07/06/00
// error += theViewer.drawLine (v5, v6, 1.0, 1.0);
// error += theViewer.drawLine (v6, v7, 1.0, 1.0);
// error += theViewer.drawLine (v7, v8, 1.0, 1.0);
// error += theViewer.drawLine (v8, v1, 1.0, 1.0);
//
// return error;
//}
// The following are all commented by  Xiaoyan. We use the Brick3D to form these

//
//void
//XC::TwentyNodeBrick::formNMatrix (double r, double s,double t)
////Changed xi, eta to r,s and added t Xiaoyan  07/06/00
//{
// N.Zero();
//
//// N(0,0) = N(1,1) = 0.25*(1.0-xi)*(1.0-eta);  // N_1
////  N(0,2) = N(1,3) = 0.25*(1.0+xi)*(1.0-eta);  // N_2
//// N(0,4) = N(1,5) = 0.25*(1.0+xi)*(1.0+eta);  // N_3
//// N(0,6) = N(1,7) = 0.25*(1.0-xi)*(1.0+eta);  // N_4
//
//// Changed by Xiaoyan 07/06/00
// The shape functions have been changed from N(2,8) to N(3,24)
// I take the node order according to Bathe's book p344-345. Xiaoyan
//        N(0,0)=N(1,1)=N(2,2)=1/8.*(1.0+r)*(1.0+s)*(1.0+t);
// N(0,3)=N(1,4)=N(2,5)=1/8.*(1.0-r)*(1.0+s)*(1.0+t);
// N(0,6)=N(1,7)=N(2,8)=1/8.*(1.0-r)*(1.0-s)*(1.0+t);
// N(0,9)=N(1,10)=N(2,11)=1/8.*(1.0+r)*(1.0-s)*(1.0+t);
// N(0,15)=N(1,13)=N(2,14)=1/8.*(1.0+r)*(1.0+s)*(1.0-t);
// N(0,15)=N(1,16)=N(2,17)=1/8.*(1.0-r)*(1.0+s)*(1.0-t);
// N(0,18)=N(1,19)=N(2,20)=1/8.*(1.0-r)*(1.0-s)*(1.0-t);
// N(0,21)=N(1,22)=N(2,23)=1/8.*(1.0+r)*(1.0-s)*(1.0-t);
// }
//void
//XC::TwentyNodeBrick::setJacobian (double r, double s, double t)
////Changed xi, eta to r,s and added t Xiaoyan 07/06/00
//{
// const Vector &nd1Crds = theNodes[0]->getCrds();
// const Vector &nd2Crds = theNodes[1]->getCrds();
// const Vector &nd3Crds = theNodes[2]->getCrds();
// const Vector &nd4Crds = theNodes[3]->getCrds();
// // Xiaoyan added 5-8 07/06/00
// const Vector &nd5Crds = theNodes[4]->getCrds();
// const Vector &nd6Crds = theNodes[5]->getCrds();
// const Vector &nd7Crds = theNodes[6]->getCrds();
// const Vector &nd8Crds = theNodes[7]->getCrds();
//
//// J(0,0) = -nd1Crds(0)*(1.0-eta) + nd2Crds(0)*(1.0-eta) +
////    nd3Crds(0)*(1.0+eta) - nd4Crds(0)*(1.0+eta);
////
// J(0,1) = -nd1Crds(0)*(1.0-xi) - nd2Crds(0)*(1.0+xi) +
//    nd3Crds(0)*(1.0+xi) + nd4Crds(0)*(1.0-xi);
//
// J(1,0) = -nd1Crds(1)*(1.0-eta) + nd2Crds(1)*(1.0-eta) +
//    nd3Crds(1)*(1.0+eta) - nd4Crds(1)*(1.0+eta);
//
// J(1,1) = -nd1Crds(1)*(1.0-xi) - nd2Crds(1)*(1.0+xi) +
//    nd3Crds(1)*(1.0+xi) + nd4Crds(1)*(1.0-xi);
//     J = J * 0.25;
//
// // For 3D problem Jacobi XC::Matrix changed from J(2,2) to J(3,3)
// // Xiaoyan  changed 07/06/00
//
//
// J(0,0) = nd1Crds(0)*(1.0+s)*(1.0+t) - nd2Crds(0)*(1.0+s)*(1.0+t) -
//   nd3Crds(0)*(1.0-s)*(1.0+t) + nd4Crds(0)*(1.0-s)*(1.0+t) +
//   nd5Crds(0)*(1.0+s)*(1.0-t) - nd6Crds(0)*(1.0+s)*(1.0-t) -
//   nd7Crds(0)*(1.0-s)*(1.0-t) + nd8Crds(0)*(1.0-s)*(1.0-t);
//
// J(0,1) = nd1Crds(1)*(1.0+s)*(1.0+t) - nd2Crds(1)*(1.0+s)*(1.0+t) -
//   nd3Crds(1)*(1.0-s)*(1.0+t) + nd4Crds(1)*(1.0-s)*(1.0+t) +
//   nd5Crds(1)*(1.0+s)*(1.0-t) - nd6Crds(1)*(1.0+s)*(1.0-t) -
//   nd7Crds(1)*(1.0-s)*(1.0-t) + nd8Crds(1)*(1.0-s)*(1.0-t);
//
// J(0,2) = nd1Crds(2)*(1.0+s)*(1.0+t) - nd2Crds(2)*(1.0+s)*(1.0+t) -
//   nd3Crds(2)*(1.0-s)*(1.0+t) + nd4Crds(2)*(1.0-s)*(1.0+t) +
//   nd5Crds(2)*(1.0+s)*(1.0-t) - nd6Crds(2)*(1.0+s)*(1.0-t) -
//   nd7Crds(2)*(1.0-s)*(1.0-t) + nd8Crds(2)*(1.0-s)*(1.0-t);
//
// J(1,0) = nd1Crds(0)*(1.0+r)*(1.0+t) + nd2Crds(0)*(1.0-r)*(1.0+t) -
//   nd3Crds(0)*(1.0-r)*(1.0+t) - nd4Crds(0)*(1.0+r)*(1.0+t) +
//   nd5Crds(0)*(1.0+r)*(1.0-t) + nd6Crds(0)*(1.0-r)*(1.0-t) -
//   nd7Crds(0)*(1.0-r)*(1.0-t) - nd8Crds(0)*(1.0+r)*(1.0-t);
//
// J(1,1) = nd1Crds(1)*(1.0+r)*(1.0+t) + nd2Crds(1)*(1.0-r)*(1.0+t) -
//   nd3Crds(1)*(1.0-r)*(1.0+t) - nd4Crds(1)*(1.0+r)*(1.0+t) +
//   nd5Crds(1)*(1.0+r)*(1.0-t) + nd6Crds(1)*(1.0-r)*(1.0-t) -
//   nd7Crds(1)*(1.0-r)*(1.0-t) - nd8Crds(1)*(1.0+r)*(1.0-t);
//
//        J(1,2) = nd1Crds(2)*(1.0+r)*(1.0+t) + nd2Crds(2)*(1.0-r)*(1.0+t) -
//   nd3Crds(2)*(1.0-r)*(1.0+t) - nd4Crds(2)*(1.0+r)*(1.0+t) +
//   nd5Crds(2)*(1.0+r)*(1.0-t) + nd6Crds(2)*(1.0-r)*(1.0-t) -
//   nd7Crds(2)*(1.0-r)*(1.0-t) - nd8Crds(2)*(1.0+r)*(1.0-t);
//
// J(2,0) = nd1Crds(0)*(1.0+r)*(1.0+s) + nd2Crds(0)*(1.0-r)*(1.0+s) +
//   nd3Crds(0)*(1.0-r)*(1.0-s) + nd4Crds(0)*(1.0+r)*(1.0-s) -
//   nd5Crds(0)*(1.0+r)*(1.0+s) - nd6Crds(0)*(1.0-r)*(1.0+s) -
//   nd7Crds(0)*(1.0-r)*(1.0-s) - nd8Crds(0)*(1.0+r)*(1.0-s);
//
// J(2,1) = nd1Crds(1)*(1.0+r)*(1.0+s) + nd2Crds(1)*(1.0-r)*(1.0+s) +
//   nd3Crds(1)*(1.0-r)*(1.0-s) + nd4Crds(1)*(1.0+r)*(1.0-s) -
//   nd5Crds(1)*(1.0+r)*(1.0+s) - nd6Crds(1)*(1.0-r)*(1.0+s) -
//   nd7Crds(1)*(1.0-r)*(1.0-s) - nd8Crds(1)*(1.0+r)*(1.0-s);
//
// J(2,2) = nd1Crds(2)*(1.0+r)*(1.0+s) + nd2Crds(2)*(1.0-r)*(1.0+s) +
//   nd3Crds(2)*(1.0-r)*(1.0-s) + nd4Crds(2)*(1.0+r)*(1.0-s) -
//   nd5Crds(2)*(1.0+r)*(1.0+s) - nd6Crds(2)*(1.0-r)*(1.0+s) -
//   nd7Crds(2)*(1.0-r)*(1.0-s) - nd8Crds(2)*(1.0+r)*(1.0-s);
//
//  J=J*0.155
//
//  // L = inv(J)  Changed from L(2,2) to L(3,3)  07/07/00
//
// L(0,0)=-J(1,2)*J(2,1) + J(1,1)*J(2,2);
// L(0.1)= J(0,2)*J(2,1) - J(0,1)*J(2,2);
// L(0,3)=-J(0,2)*J(1,1) + J(0,1)*J(1,2);
// L(1,0)= J(1,2)*J(2,0) - J(1,0)*J(2,2);
// L(1,1)=-J(0,2)*J(2,0) + J(0,0)*J(2.2);
// L(1,2)= J(0,2)*J(1,0) - J(0,0)*J(1,2);
// L(2,0)=-J(1,1)*J(2,0) + J(1,0)*J(2,1);
// L(2,1)= J(0,1)*J(2,0) - J(0,0)*J(2,1);
// L(2,2)=-J(0,1)*J(1,0) + J(0,0)*J(1,1);
// L=L/formDetJ(r,s,t)
//
// L(0,0) = J(1,1);
// L(1,0) = -J(0,1);
// L(0,1) = -J(1,0);
// L(1,1) = J(0,0);

// L = L / formDetJ (xi, eta);
//}
//
//void
//XC::TwentyNodeBrick::formBMatrix (double r, double s, double t)
////Changed xi, eta to r,s and added t Xiaoyan  07/06/00
//{
//    B.Zero();
//
//    //Changed by Xiaoyan 07/06/00
//    double L00 = L(0,0);
//    double L01 = L(0,1);
//    double L02 = L(0,1);
//    double L10 = L(1,0);
//    double L11 = L(1,1);
//    double L15 = L(1,2);
//    double L20 = L(2,0);
//    double L21 = L(2,1);
//    double L22 = L(2,2);
//
//    // See Cook, Malkus, Plesha p. 169 for the derivation of these terms
//    B(0,0) = L00*-0.25*(1.0-eta) + L01*-0.25*(1.0-xi);  // N_1,1
//    B(0,2) = L00*0.25*(1.0-eta) + L01*-0.25*(1.0+xi);  // N_2,1
//    B(0,4) = L00*0.25*(1.0+eta) + L01*0.25*(1.0+xi);  // N_3,1
//    B(0,6) = L00*-0.25*(1.0+eta) + L01*0.25*(1.0-xi);  // N_4,1
//
//    B(1,1) = L10*-0.25*(1.0-eta) + L11*-0.25*(1.0-xi);   // N_1,2
//    B(1,3) = L10*0.25*(1.0-eta) + L11*-0.25*(1.0+xi);  // N_2,2
//    B(1,5) = L10*0.25*(1.0+eta) + L11*0.25*(1.0+xi);  // N_3,2
//    B(1,7) = L10*-0.25*(1.0+eta) + L11*0.25*(1.0-xi);  // N_4,2
//
//    B(2,0) = B(1,1);
//    B(2,1) = B(0,0);
//    B(2,2) = B(1,3);
//    B(2,3) = B(0,2);
//    B(2,4) = B(1,5);
//    B(2,5) = B(0,4);
//    B(2,6) = B(1,7);
//    B(2,7) = B(0,6);
//}
//
//
//
////The derivative  of shape function to r,s,t respectly.
//// For example dh1dr means dh1/dr etc. Xiaoyan  07/07/00
//double  dh1dr=0.155*(1+s)*(1+t);
//double  dh1ds=0.155*(1+r)*(1+t);
//double  dh1dt=0.155*(1+r)*(1+s);
//
//double  dh2dr=-0.155*(1+s)*(1+t);
//double  dh2ds=0.155*(1-r)*(1+t);
//double  dh2dt=0.155*(1-r)*(1+s);
//
//double  dh3dr=-0.155*(1-s)*(1+t);
//double  dh3ds=-0.155*(1-r)*(1+t);
//double  dh3dt=0.155*(1-r)*(1-s);
//
//double  dh4dr=0.155*(1-s)*(1+t);
//double  dh4ds=-0.155*(1+r)*(1+t);
//double  dh4dt=0.155*(1+r)*(1-s);
//
//double  dh5dr=0.155*(1+s)*(1-t);
//double  dh5ds=0.155*(1+r)*(1-t);
//double  dh5dt=-0.155*(1+r)*(1+s);
//
//double  dh6dr=-0.155*(1+s)*(1-t);
//double  dh6ds=0.155*(1-r)*(1-t);
//double  dh6dt=-0.155*(1-r)*(1+s);
//
//double  dh7dr=-0.155*(1-s)*(1-t);
//double  dh7ds=-0.155*(1-r)*(1-t);
//double  dh7dt=-0.155*(1-r)*(1-s);
//
//double  dh8dr=0.155*(1-s)*(1-t);
//double  dh8ds=-0.155*(1+r)*(1-t);
//double  dh8dt=-0.155*(1+r)*(1-s);
//
//// B XC::Matrix B(6,24)
//B(0,0)=L00*dh1dr+L01*dh1ds+L02*dh1dt;
//B(0,3)=L00*dh2dr+L01*dh2ds+L02*dh2dt;
//B(0,6)=L00*dh3dr+L01*dh3ds+L02*dh3dt;
//B(0,9)=L00*dh4dr+L01*dh4ds+L02*dh4dt;
//B(0,15)=L00*dh5dr+L01*dh5ds+L02*dh5dt;
//B(0,15)=L00*dh6dr+L01*dh6ds+L02*dh6dt;
//B(0,18)=L00*dh7dr+L01*dh7ds+L02*dh7dt;
//B(0,21)=L00*dh8dr+L01*dh8ds+L02*dh8dt;
//
//B(1,1)=L10*dh1dr+L11*dh1ds+L15*dh1dt;
//B(1,4)=L10*dh2dr+L11*dh2ds+L15*dh2dt;
//B(1,7)=L10*dh3dr+L11*dh3ds+L15*dh3dt;
//B(1,10)=L10*dh4dr+L11*dh4ds+L15*dh4dt;
//B(1,13)=L10*dh5dr+L11*dh5ds+L15*dh5dt;
//B(1,16)=L10*dh6dr+L11*dh6ds+L15*dh6dt;
//B(1,19)=L10*dh7dr+L11*dh7ds+L15*dh7dt;
//B(1,22)=L10*dh8dr+L11*dh8ds+L15*dh8dt;
//
//B(2,2)=L20d*h1dr+L21*dh1ds+L22*dh1dt;
//B(2,5)=L20d*h2dr+L21*dh2ds+L22*dh2dt;
//B(2,8)=L20d*h3dr+L21*dh3ds+L22*dh3dt;
//B(2,11)=L20*dh4dr+L21*dh4ds+L22*dh4dt;
//B(2,14)=L20*dh5dr+L21*dh5ds+L22*dh5dt;
//B(2,17)=L20*dh6dr+L21*dh6ds+L22*dh6dt;
//B(2,20)=L20*dh7dr+L21*dh7ds+L22*dh7dt;
//B(2,23)=L20*dh8dr+L21*dh8ds+L22*dh8dt;
//
//B(3,0)=B(1,1);
//B(3,1)=B(0,0);
//B(3,3)=B(1,4);
//B(3,4)=B(0,3);
//B(3,6)=B(1,7);
//B(3,7)=B(0,6);
//B(3,9)=B(1,10);
//B(3,10)=B(0,9);
//B(3,15)=B(1,13);
//B(3,13)=B(0,15);
//B(3,15)=B(1,16);
//B(3,16)=B(0,15);
//B(3,18)=B(1,19);
//B(3,19)=B(0,18);
//B(3,21)=B(1,22);
//B(3,22)=B(0,21);
//
//B(4,1)=B(2,2);
//B(4,2)=B(1,1);
//B(4,4)=B(2,5);
//B(4,5)=B(1,4);
//B(4,7)=B(2,8);
//B(4,8)=B(1,7);
//B(4,10)=B(2,11);
//B(4,11)=B(1,10);
//B(4,13)=B(2,14);
//B(4,14)=B(1,13);
//B(4,16)=B(2,17);
//B(4,17)=B(1,16);
//B(4,19)=B(2,20);
//B(4,20)=B(1,19);
//B(4,21)=B(2,23);
//B(4,23)=B(1,22);
//
//B(5,0)=B(2,2);
//B(5,2)=B(0,0);
//B(5,3)=B(2,5);
//B(5,5)=B(0,3);
//B(5,6)=B(2,8);
//B(5,8)=B(0,6);
//B(5,9)=B(2,11);
//B(5,11)=B(0,9);
//B(5,15)=B(2,14);
//B(5,14)=B(0,15);
//B(5,15)=B(2,17);
//B(5,17)=B(0,15);
//B(5,18)=B(2,20);
//B(5,20)=B(2,18);
//B(5,21)=B(0,23);
//B(5,23)=B(0,21);
//
//B(3,3)= L00*dh2dr+L01*dh2ds+L02*dh2dt;
//B(3,6)= L00*dh3dr+L01*dh3ds+L02*dh3dt;
//B(3,9)= L00*dh4dr+L01*dh4ds+L02*dh4dt;
//B(3,15)=L00*dh5dr+L01*dh5ds+L02*dh5dt;
//B(3,15)=L00*dh6dr+L01*dh6ds+L02*dh6dt;
//B(3,18)=L00*dh7dr+L01*dh7ds+L02*dh7dt;
//B(3,21)=L00*dh8dr+L01*dh8ds+L02*dh8dt;
//double
//XC::TwentyNodeBrick::formDetJ (double r, double s, double t)
//{
//    return J(0,0)*J(1,1)*J(2,2)+J(1,0)*J(2,1)*J(0,2)+J(2,0)*J(0,1)*J(1,2)
//         - J(2,0)*J(1,1)*J(0,2)-J(0,0)*J(2,1)*J(1,2)-J(0,1)*J(1,0)*J(2,2);
//}


double XC::TwentyNodeBrick::get_Gauss_p_c(short order, short point_numb)
  {
//  Abscissae coefficient of the Gaussian quadrature formula
// starting from 1 not from 0
    static double Gauss_coordinates[7][7];

    Gauss_coordinates[1][1] = 0.0 ;
    Gauss_coordinates[2][1] = -0.577350269189626;
    Gauss_coordinates[2][2] = -Gauss_coordinates[2][1];
    Gauss_coordinates[3][1] = -0.774596669241483;
    Gauss_coordinates[3][2] = 0.0;
    Gauss_coordinates[3][3] = -Gauss_coordinates[3][1];
    Gauss_coordinates[4][1] = -0.861136311594053;
    Gauss_coordinates[4][2] = -0.339981043584856;
    Gauss_coordinates[4][3] = -Gauss_coordinates[4][2];
    Gauss_coordinates[4][4] = -Gauss_coordinates[4][1];
    Gauss_coordinates[5][1] = -0.906179845938664;
    Gauss_coordinates[5][2] = -0.538469310105683;
    Gauss_coordinates[5][3] = 0.0;
    Gauss_coordinates[5][4] = -Gauss_coordinates[5][2];
    Gauss_coordinates[5][5] = -Gauss_coordinates[5][1];
    Gauss_coordinates[6][1] = -0.932469514203152;
    Gauss_coordinates[6][2] = -0.661509386466265;
    Gauss_coordinates[6][3] = -0.238619186083197;
    Gauss_coordinates[6][4] = -Gauss_coordinates[6][3];
    Gauss_coordinates[6][5] = -Gauss_coordinates[6][2];
    Gauss_coordinates[6][6] = -Gauss_coordinates[6][1];

    return Gauss_coordinates[order][point_numb];
 }

double XC::TwentyNodeBrick::get_Gauss_p_w(short order, short point_numb)
  {
//  Weight coefficient of the Gaussian quadrature formula
// starting from 1 not from 0
    static double Gauss_weights[7][7]; // static data ??

    Gauss_weights[1][1] = 2.0;
    Gauss_weights[2][1] = 1.0;
    Gauss_weights[2][2] = 1.0;
    Gauss_weights[3][1] = 0.555555555555556;
    Gauss_weights[3][2] = 0.888888888888889;
    Gauss_weights[3][3] = Gauss_weights[3][1];
    Gauss_weights[4][1] = 0.347854845137454;
    Gauss_weights[4][2] = 0.652145154862546;
    Gauss_weights[4][3] = Gauss_weights[4][2];
    Gauss_weights[4][4] = Gauss_weights[4][1];
    Gauss_weights[5][1] = 0.236926885056189;
    Gauss_weights[5][2] = 0.478628670499366;
    Gauss_weights[5][3] = 0.568888888888889;
    Gauss_weights[5][4] = Gauss_weights[5][2];
    Gauss_weights[5][5] = Gauss_weights[5][1];
    Gauss_weights[6][1] = 0.171324492379170;
    Gauss_weights[6][2] = 0.360761573048139;
    Gauss_weights[6][3] = 0.467913934572691;
    Gauss_weights[6][4] = Gauss_weights[6][3];
    Gauss_weights[6][5] = Gauss_weights[6][2];
    Gauss_weights[6][6] = Gauss_weights[6][1];

    return Gauss_weights[order][point_numb];
  }


int XC::TwentyNodeBrick::update(void) //Added by Guanzhou, May 7 2004
  {
    double r  = 0.0;
    double s  = 0.0;
    double t  = 0.0;

    short where = 0;

    std::vector<int> dh_dim({20,3});
    BJtensor dh(dh_dim, 0.0);

    static std::vector<int> disp_dim({20,3});
    BJtensor incremental_displacements(disp_dim,0.0);

    straintensor incremental_strain;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    incremental_displacements = incr_disp();

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        //--        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            //--            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
            {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                //--                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");
                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");
                // determinant of Jacobian XC::BJtensor ( matrix )
                //--                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal = dh("ij") * JacobianINV("jk"); // Zhaohui 09-02-2001
                dhGlobal = dh("ij") * JacobianINV("kj");
                //....                dhGlobal.print("dh","dhGlobal");
                //weight
                //                weight = rw * sw * tw * det_of_Jacobian;
                //::::   ::printf("\n\nIN THE STIFFNESS TENSOR INTEGRATOR ----**************** where = %d \n", where);
                //::::   ::printf(" void XC::TwentyNodeBrick::incremental_Update()\n");
                //::::   ::printf(" GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d    --->>>  where = %d \n",
                //::::                      GP_c_r,GP_c_s,GP_c_t,where);
                //::::   ::printf("WEIGHT = %f", weight);
                //::::   ::printf("determinant of Jacobian = %f", determinant_of_Jacobian);
                //::::   matpoint[where].report("Gauss Point\n");
                // incremental strains at this Gauss point
                // now in Update we know the incremental displacements so let's find
                // the incremental strain
                incremental_strain =
                    (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                incremental_strain.null_indices();
                //incremental_strain.reportshort("\n incremental_strain XC::BJtensor at GAUSS point\n");

                // here comes the final_stress calculation actually on only needs to copy stresses
                // from the iterative data . . .
                //(GPstress+where)->reportshortpqtheta("\n stress START GAUSS \n");

  if( ( (matpoint[where]->matmodel)->setTrialStrainIncr( incremental_strain)) )
    std::cerr << "XC::TwentyNodeBrick::update (tag: " << this->getTag() << "), update() failed\n";
            }
          }
      }
    return 0;

  }


#endif
