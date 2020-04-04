//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
// COPYLEFT (C):     :-))
//``This  source code is Copyrighted in U.S., by the The Regents of the University
//of California, for an indefinite period, and anybody caught using it without our
//permission,  will  be  mighty  good friends of ourn, cause we don't give a darn.
//Hack  it.  Compile it. Debug it. Run it. Yodel it. Enjoy it. We wrote it, that's
//all we wanted to do.'' bj
// PROJECT:           Object Oriented Finite XC::Element Program
// FILE:              EightNodeBrick.cpp
// CLASS:             EightNodeBrick
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
// DATE:              Aug. 2000
// UPDATE HISTORY:    Modified from Brick3D and XC::FourNodeQuad.hh  07/06/00
//                    Sept. - Oct 2000 connected to OpenSees by Zhaohui
//                    Sept 2001 optimized to some extent (static tensors...)
//                    May 2004 Guanzhou added update()
//                    Mar2005 Guanzhou fixes XC::EightNodeBrick::update(void)
//
//
///////////////////////////////////////////////////////////////////////////////
//

#ifndef EIGHTNODEBRICK_CPP
#define EIGHTNODEBRICK_CPP

#include <domain/mesh/element/volumetric/8nbrick/EightNodeBrick.h>
#include "domain/mesh/node/Node.h"

#include <material/nD/Template3Dep/MatPoint3D.h>

#include <material/nD/NDMaterial.h>
#include <material/nD/Template3Dep/Template3Dep.h>

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <domain/load/ElementalLoad.h>
#include "domain/load/volumetric/BrickSelfWeight.h"
#include <utility/matrix/nDarray/straint.h>
#include <utility/matrix/nDarray/stresst.h>

#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include "utility/matrix/nDarray/BJmatrix.h"


#define FixedOrder 2

 XC::Matrix XC::EightNodeBrick::K(24, 24);
 XC::Matrix XC::EightNodeBrick::C(24, 24);
 XC::Matrix XC::EightNodeBrick::M(24, 24);
 XC::Vector XC::EightNodeBrick::P(24);
 XC::Vector InfoP(FixedOrder*FixedOrder*FixedOrder*4+1); //Plastic info(coor+pls) 32+1 2X2X2
 XC::Vector InfoP1(FixedOrder*FixedOrder*FixedOrder+1); //Plastic info, no Gauss point coordinates
 XC::Vector InfoS(FixedOrder*FixedOrder*FixedOrder*6+1); //Stress 8*6+1  2X2X2
 XC::Vector InfoSpq(2); //p and q of count/2
 XC::Vector InfoSpq_all(2*FixedOrder*FixedOrder*FixedOrder+4); //p and q of all GS points + Sig11-33 +Strain_plastic_vv +psi
 XC::Vector Gsc8(FixedOrder*FixedOrder*FixedOrder*3+1); //Gauss point coordinates

//====================================================================
//Reorganized constructor ____ Zhaohui 02-10-2000
//====================================================================

XC::EightNodeBrick::EightNodeBrick(int element_number,
                               int node_numb_1, int node_numb_2, int node_numb_3, int node_numb_4,
                               int node_numb_5, int node_numb_6, int node_numb_7, int node_numb_8,
                               NDMaterial * Globalmmodel, const BodyForces3D &bForces,
             double r, double p)

  :ElementBase<8>(element_number, ELE_TAG_EightNodeBrick ), Ki(0), bf(bForces),
  rho(r), pressure(p)
  {
    load.reset(24);
    //BJ//BJ
    //BJ    std::cerr << "\n\n\n\n Print in XC::EightNodeBrick::EightNodeBrick" <<std::endl; this->Print(std::cerr);
    //BJ//BJ
    //elem_numb= element_number;

    determinant_of_Jacobian= 0.0;

    //r_integration_order= r_int_order;
    //s_integration_order= s_int_order;
    //t_integration_order= t_int_order;
    r_integration_order= FixedOrder; // Gauss-Legendre integration order in r direction
    s_integration_order= FixedOrder; // Gauss-Legendre integration order in s direction
    t_integration_order= FixedOrder; // Gauss-Legendre integration order in t direction
    //Not needed. Right now we have one XC::NDMaterial for each material point
    //mmodel= Globalmmodel->getCopy( type ); // One global mat model

    int total_number_of_Gauss_points= r_integration_order*s_integration_order*t_integration_order;


    if(total_number_of_Gauss_points != 0)
      { matpoint= std::vector<MatPoint3D>(total_number_of_Gauss_points); }

    ////////////////////////////////////////////////////////////////////
    //dakle posto:
    //// according to ARM pp.61 default constructor will be called!!
    //onda oni vec postoje u memoriji i samo im treba dodeliti prave
    //vrednosti iz onog modela koji je prenesen unutra. Znaci onInitializxe funkcija
    //sa modelom kao argumentom Initialize(taj_Model).
    //za stresstensor i straintensor isto to napravi
    //tu Initialize funkciju pa da uzima argument i da samo
    //koristi njegove vrednosti pa stavi u vec postojece mesto
    //u memoriji ove vrednsoti. Takodje unutar brick3d napravi ih
    //( te stresstensor , straintensor i mmodel ) static da ostanu tu
    //da se ne pozove destructor . . .

    short where= 0;

    for(short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        double r= get_Gauss_p_c( r_integration_order, GP_c_r );
        double rw= get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            double s= get_Gauss_p_c( s_integration_order, GP_c_s );
            double sw= get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                double t= get_Gauss_p_c( t_integration_order, GP_c_t );
                double tw= get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where= ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                //XC::DB::printf("\n\nBefore Initialization **************** where= %d \n",where);
                //XC::DB::printf("GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
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

                // Zhaohui  09-30-2000
                //GPtangent_E[where].Initialize_all(*IN_tangent_E);
                //GPstress[where].Initialize(*INstress);
                //GPiterative_stress[where].Initialize(*INiterative_stress);
                //GPq_ast_iterative[where]= IN_q_ast_iterative[where];
                //GPstrain[where].Initialize(*INstrain);

                // Already assigned the copy of Globalmmodel to each element of matpoint
                //(matpoint)->operator[](where).Initialize(*Globalmmodel);

                //NDMaterial *NMD= Globalmmodel->getCopy();
                //if(strcmp( Globalmmodel->getType(), "Template3Dep" ) == 0)
                //   NMD= 0;

                //std::cerr << "where= " << where << std::std::endl;
                matpoint[where]= MatPoint3D(GP_c_r,GP_c_s,GP_c_t,r, s, t,rw, sw, tw,
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
      theNodes.set_id_nodes(node_numb_1,node_numb_2,node_numb_3,node_numb_4,node_numb_5,node_numb_6,node_numb_7,node_numb_8);
  }

//====================================================================
XC::EightNodeBrick::EightNodeBrick(void)
  :ElementBase<8>(0, ELE_TAG_EightNodeBrick), Ki(0), bf(3), rho(0.0), pressure(0.0), mmodel(0)
  {load.reset(24);}


// Initialize
// default constructor
// this one takes only one material model and forwards it to all
// Gauss points.
//#############################################################################
//
//int XC::EightNodeBrick::Initialize(int element_number,
//                                int node_numb_1, int node_numb_2, int node_numb_3, int node_numb_4,
//                                int node_numb_5, int node_numb_6, int node_numb_7, int node_numb_8,
//                                NDMaterial * Globalmmodel,  double b1, double b2, double b3,
//                                double p, double r
//        //, EPState * InitEPS  const char * type,
//              //BJtensor       * IN_tangent_E,//stresstensor * INstress, //stresstensor * INiterative_stress, //double  * IN_q_ast_iterative, //straintensor * INstrain
//              )
//  {
//    //elem_numb= element_number;
//    bf(0)= b1;
//    bf(1)= b2;
//    bf(2)= b3;
//    rho= r;
//    pressure= p;
//    // r_integration_order= r_int_order;
//    // s_integration_order= s_int_order;
//    // t_integration_order= t_int_order;
//    r_integration_order= FixedOrder; // Gauss-Legendre integration order in r direction
//    s_integration_order= FixedOrder; // Gauss-Legendre integration order in s direction
//    t_integration_order= FixedOrder; // Gauss-Legendre integration order in t direction
//
//    //mmodel= Globalmmodel->getCopy(); // One global mat model
//
//    int total_number_of_Gauss_points= r_integration_order*
//                                       s_integration_order*
//                                       t_integration_order;
//
//    //matpoint= Globalmmodel->new_mm(total_number_of_Gauss_points);
//    //matpoint= Globalmmodel->getCopy(total_number_of_Gauss_points);
//
//    //GPstress= new stresstensor[total_number_of_Gauss_points];
//    //GPiterative_stress= new stresstensor[total_number_of_Gauss_points];
//    //GPq_ast_iterative= new double[total_number_of_Gauss_points];
//    //GPstrain= new straintensor[total_number_of_Gauss_points];
//    //GPtangent_E= new BJtensor[total_number_of_Gauss_points];
//    //matpoint= new MatPoint3D[total_number_of_Gauss_points];
//
//    matpoint= std::vector<MatPoint3D>(total_number_of_Gauss_points);
//
//    ////////////////////////////////////////////////////////////////////
//    //dakle posto:
//    //// according to ARM pp.61 default constructor will be called!!
//    //onda oni vec postoje u memoriji i samo im treba dodeliti prave
//    //vrednosti iz onog modela koji je prenesen unutra. Znaci onInitializxe funkcija
//    //sa modelom kao argumentom Initialize(taj_Model).
//    //za stresstensor i straintensor isto to napravi
//    //tu Initialize funkciju pa da uzima argument i da samo
//    //koristi njegove vrednosti pa stavi u vec postojece mesto
//    //u memoriji ove vrednsoti. Takodje unutar brick3d napravi ih
//    //( te stresstensor , straintensor i mmodel ) static da ostanu tu
//    //da se ne pozove destructor . . .
//
//    short where= 0;
//
//    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
//      {
//        double r= get_Gauss_p_c( r_integration_order, GP_c_r );
//        double rw= get_Gauss_p_w( r_integration_order, GP_c_r );
//
//        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
//          {
//            double s= get_Gauss_p_c( s_integration_order, GP_c_s );
//            double sw= get_Gauss_p_w( s_integration_order, GP_c_s );
//
//            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
//              {
//                double t= get_Gauss_p_c( t_integration_order, GP_c_t );
//                double tw= get_Gauss_p_w( t_integration_order, GP_c_t );
//
//                // this short routine is supposed to calculate position of
//                // Gauss point from 3D array of short's
//                where =
//                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
//
//                //XC::DB::printf("\n\nBefore Initialization **************** where= %d \n",where);
//                //XC::DB::printf("GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
//                //DB            GP_c_r,GP_c_s,GP_c_t);
//                //DB
//                //DBGPstress[where].reportshort("stress within before Initialization");
//                //DBGPstrain[where].reportshort("strain within before Initialization");
//                //DB
//                //DB// I suspect that [] should be overloaded so that compiler knows which
//                //DB// material model is returning a pointer and for the purpose
//                //DB//matpoint[where].report("mmodel within before Initialization");
//                //DB//matpoint[where].report("mmodel within before Initialization"); // operator[] overloaded
//                //DB(matpoint)->operator[](where).report("mmodel within before Initialization"); // operator[] overloaded
//                //DB                                                               // for XC::NDMaterial and
//                //DB                                                               // derived types!
//
//                //.matpoint[where].Initialize(GP_c_r,
//                //.                         GP_c_s,
//                //.                         GP_c_t,
//                //.                         r, s, t,
//                //.                         rw, sw, tw,
//                //.                         &( GPstress[where] ),
//                //.                         &( GPstrain[where] ),
//                //.                         &( (matpoint)->operator[](where) )
//                //.                        );      // ugly syntax but it works!
//                //.                                // still don't know what's wrong
//                //.                                // with the old style matpoint[where]
//                // Initialize it with the elastic stiffness XC::BJtensor at the beginning
//                //````double Ey= (matpoint)->operator[](where).E();
//                //````double nu= (matpoint)->operator[](where).nu();
//                //````BJtensor Constitutive= (matpoint)->operator[](where).StiffnessTensorE(Ey,nu);
//                //````
//                //````GPtangent_E[where].Initialize_all(Constitutive);
//
//    //GPtangent_E[where].Initialize_all(*IN_tangent_E);
//                //GPstress[where].Initialize(*INstress);
//                //GPiterative_stress[where].Initialize(*INiterative_stress);
//                //GPq_ast_iterative[where]= IN_q_ast_iterative[where];
//                //GPstrain[where].Initialize(*INstrain);
//
//    //(matpoint)->operator[](where).Initialize(*Globalmmodel);
//    // Already initialized using Globalmmodel in getCopy() --Zhaohui 09-29-2000
//
//                //NDMaterial *NMD= Globalmmodel->getCopy();
//    //if(strcmp( Globalmmodel->getType(), "Template3Dep" ) == 0)
//     //   NMD= 0;
//
//                //matpoint[where].Initialize(GP_c_r,
//                matpoint[where]= MatPoint3D(GP_c_r,
//                                           GP_c_s,
//                                           GP_c_t,
//                                           r, s, t,
//                                           rw, sw, tw,
//                                           //InitEPS,
//             Globalmmodel);
//             //NMD );
//                                           // &( GPstress[where] ), //&( GPiterative_stress[where] ), // IN_q_ast_iterative[where],
//                                           // &( GPstrain[where] ),  //&( GPtangent_E[where] ),  // ZHaohui 09-29-2000
//                                           // &( (matpoint)->operator[](where) )
//                                           // ugly syntax but it works! // still don't know what's wrong   // with the old style matpoint[where]
//              }
//          }
//      }
//
//      // Set connected external node IDs
//      theNodes.getTagNode(0)= node_numb_1;
//      theNodes.getTagNode(1)= node_numb_2;
//      theNodes.getTagNode(2)= node_numb_3;
//      theNodes.getTagNode(3)= node_numb_4;
//      theNodes.getTagNode(4)= node_numb_5;
//      theNodes.getTagNode(5)= node_numb_6;
//      theNodes.getTagNode(6)= node_numb_7;
//      theNodes.getTagNode(7)= node_numb_8;
//
//      //G_N_numbs[0]= node_numb_1;
//      //G_N_numbs[1]= node_numb_2;
//      //G_N_numbs[2]= node_numb_3;
//      //G_N_numbs[3]= node_numb_4;
//      //G_N_numbs[4]= node_numb_5;
//      //G_N_numbs[5]= node_numb_6;
//      //G_N_numbs[6]= node_numb_7;
//      //G_N_numbs[7]= node_numb_8;
//
//      //nodes= GlobalNodes;
//
//      // loop nodes 9-20 and :
//      //  1) put the right coordinates on place,
//      //  2) calculate the total number of nodes
//      // loop nodes 9-20 and :
//      //  1) put the right coordinates on place,
//      //  2) calculate the total number of nodes
//      nodes_in_brick= 8;
//      //    for( int i=8 ; i<20 ; i++ )
//      //      {
//      //        if(G_N_numbs[i] == 0 )
//      //          {
//      //                   node_existance[i-8]= 0;
//      //          }
//      //        else
//      //          {
//      //            nodes_in_brick++;
//      //            node_existance[i-8]= 1;
//      //          }
//      //      }
//      //    Node * N= new Node[nodes_in_brick];
//      //  // Xiaoyan comment for only 8 nodes  07/11/00
//      return 0;
//  }

//#############################################################################

//! @brief Virtual constructor.
XC::Element* XC::EightNodeBrick::getCopy(void) const
  { return new EightNodeBrick(*this); }


XC::EightNodeBrick::~EightNodeBrick(void)
  {
    if(Ki)
      delete Ki;
    Ki= nullptr;
    //if(mmodel)
    //  delete [] mmodel;
  }


void XC::EightNodeBrick::incremental_Update()
  {

//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in XC::EightNodeBrick::incremental_Update()" <<std::endl; this->Print(std::cerr);
//BJ//BJ

    double r= 0.0;
    // double rw= 0.0;
    double s= 0.0;
    // double sw= 0.0;
    double t= 0.0;
    // double tw= 0.0;

    short where= 0;
    //,,,,,    double weight= 0.0;

    //double this_one_PP= (matpoint)->operator[](where).IS_Perfect_Plastic();

    int dh_dim[]= {8,3};   //Xiaoyan changed from {20,3} to {8,3}  07/12/00
    BJtensor dh(2, dh_dim, 0.0);

//    BJtensor Constitutive( 4, def_dim_4, 0.0);

    //    double det_of_Jacobian= 0.0;

    static int disp_dim[]= {8,3}; //Xiaoyan changed from {20,3} to {8,3}  07/12/00
    BJtensor incremental_displacements(2,disp_dim,0.0);

    straintensor incremental_strain;
//    straintensor total_strain_at_GP;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    //....    int number_of_subincrements= 1;
    //....    double this_one_PP= 1.0; // if set to 0.0 -> perfectly plastic
    //....                              // if set to 1.0 -> elasto plastic

//    stresstensor final_stress_after_integration;

    ///    stresstensor incremental_stress;
    // BJtensor of incremental displacements taken from node objects
    incremental_displacements= incr_disp();

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        //--        rw= get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            //--            sw= get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
            {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                //--                tw= get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordiantes with respect to local coordiantes
                dh= dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //....                Jacobian.print("J");
                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV= Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");
                // determinant of Jacobian XC::BJtensor ( matrix )
                //--                det_of_Jacobian= Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //dhGlobal= dh("ij") * JacobianINV("jk"); //Zhaohui Yang 09-02-2001
                dhGlobal= dh("ij") * JacobianINV("kj");
                //....                dhGlobal.print("dh","dhGlobal");
                //weight
                //                weight= rw * sw * tw * det_of_Jacobian;
                //::::   ::printf("\n\nIN THE STIFFNESS TENSOR INTEGRATOR ----**************** where= %d \n", where);
                //::::   ::printf(" void XC::EightNodeBrick::incremental_Update()\n");
                //::::   ::printf(" GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d    --->>>  where= %d \n",
                //::::                      GP_c_r,GP_c_s,GP_c_t,where);
                //::::   ::printf("WEIGHT= %f", weight);
                //::::   ::printf("determinant of Jacobian= %f", determinant_of_Jacobian);
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

    // Getting final_stress_after_integration is  Done inside CDriver on EPState____ZHaohui
    //final_stress_after_integration= GPiterative_stress[where];
    //(matpoint)->operator[](where).kappa_set(final_stress_after_integration,
                //                                 GPq_ast_iterative[where]);

    //....         final_stress_after_integration =
                //....           (matpoint)->operator[](where).FinalStress(*(GPstress+where),
                //....                                                     incremental_strain,
                //....                                                     (matpoint)->operator[](where),
                //....                                                     number_of_subincrements,
                //....                                                     this_one_PP);
                //....//final_stress_after_integration.reportshortpqtheta("\n final_stress_after_integration GAUSS \n");
                // calculate the constitutive XC::BJtensor

                // We do not need: final_stress_after_integration


          //Constitutive =
                //  (matpoint)->operator[](where).ConstitutiveTensor(final_stress_after_integration,
                //                                                   *(GPstress+where),
                //                                                   incremental_strain,
                //                                                   (matpoint)->operator[](where),
                //                                                   this_one_PP);

    // ZHaohui modified __09-29-2000

          // Now no XC::EPState  but XC::NDMaterial for each MatPoint
    //EPState *tmp_eps= (matpoint[where]).getEPS();
          //NDMaterial *tmp_ndm= (matpoint[where]).getNDMat();

    //if( tmp_eps ) { //if there is an XC::EPState for the XC::MatPoint3D
    //  mmodel->setEPS( *tmp_eps );

    if( ( (matpoint[where].matmodel)->setTrialStrainIncr( incremental_strain)) )
      std::cerr << "XC::EightNodeBrick::incremental_Update (tag: " << this->getTag() << "), not converged\n";

    //matpoint[where].setEPS( mmodel->getEPS() );
    //}

    //else if( tmp_ndm )
    //  (matpoint[where].p_matmodel)->setTrialStrainIncr( incremental_strain );
    //else {
    //   std::cerr << "XC::EightNodeBrick::incremental_Update (tag: %d), no strain or stress state vars" << this->getTag() << std::endl;
    //   exit(1);
    //}

    //Constitutive= trialEPS.getEep();

    //::::                   Constitutive.print("C","\n\n C XC::BJtensor \n");
          // this is update of constitutive XC::BJtensor at this Gauss point

    // All done in XC::EPState when calling setTrialStrainIncr and converged
    //GPtangent_E[where].Initialize(Constitutive);
          //GPtangent_E[where].print("\n tangent E at GAUSS point \n");

                //total_strain_at_GP.Initialize(*(GPstrain+where));
                //total_strain_at_GP.reportshort("\n total_strain XC::BJtensor at GAUSS point \n");
                //total_strain_at_GP= total_strain_at_GP + incremental_strain;
                //total_strain_at_GP.reportshort("\n total_strain XC::BJtensor at GAUSS point AFTER\n");
                //GPstress[where].Initialize(final_stress_after_integration);
                //GPstress[where].reportshortpqtheta("\n stress at GAUSS point \n");

    //GPstrain[where].Initialize(total_strain_at_GP);

    //GPstrain[where].reportshort("\n strain at GAUSS point \n");
            }
          }
      }
  }

//#############################################################################
//#############################################################################
//***************************************************************
 XC::BJtensor XC::EightNodeBrick::H_3D(double r1, double r2, double r3) const
  {

    int dimension[]= {24,3}; // Xiaoyan changed from {60,3} to {24,3} for 8 nodes
                              // 3*8=24  07/12/00
    BJtensor H(2, dimension, 0.0);

    // influence of the node number 8
    H.val(22,1)=(1.0+r1)*(1.0-r2)*(1.0-r3)*0.125;// - (H.val(43,1)+H.val(48,3)+H.val(60,3))/2.0;
    H.val(23,2)= H.val(22,1); //(1.0+r1)*(1.0-r2)*(1.0-r3)*0.125;// - (H.val(43,1)+H.val(48,3)+H.val(60,3))/2.0;
    H.val(24,3)= H.val(22,1); //(1.0+r1)*(1.0-r2)*(1.0-r3)*0.125;// - (H.val(43,1)+H.val(48,3)+H.val(60,3))/2.0;
    // influence of the node number 7
    H.val(19,1)=(1.0-r1)*(1.0-r2)*(1.0-r3)*0.125;// - (H.val(42,3)+H.val(43,1)+H.val(57,3))/2.0;
    H.val(20,2)=H.val(19,1); //(1.0-r1)*(1.0-r2)*(1.0-r3)*0.125;// - (H.val(42,3)+H.val(43,1)+H.val(57,3))/2.0;
    H.val(21,3)=H.val(19,1); //(1.0-r1)*(1.0-r2)*(1.0-r3)*0.125;// - (H.val(42,3)+H.val(43,1)+H.val(57,3))/2.0;
    // influence of the node number 6
    H.val(16,1)=(1.0-r1)*(1.0+r2)*(1.0-r3)*0.125;//- (H.val(39,3)+H.val(42,3)+H.val(54,3))/2.0;
    H.val(17,2)=H.val(16,1); //(1.0-r1)*(1.0+r2)*(1.0-r3)*0.125;//- (H.val(39,3)+H.val(42,3)+H.val(54,3))/2.0;
    H.val(18,3)=H.val(16,1); //(1.0-r1)*(1.0+r2)*(1.0-r3)*0.125;//- (H.val(39,3)+H.val(42,3)+H.val(54,3))/2.0;
    // influence of the node number 5
    H.val(13,1)=(1.0+r1)*(1.0+r2)*(1.0-r3)*0.125;//- (H.val(39,3)+H.val(48,3)+H.val(51,3))/2.0;
    H.val(14,2)=H.val(13,1); //(1.0+r1)*(1.0+r2)*(1.0-r3)*0.125;//- (H.val(39,3)+H.val(48,3)+H.val(51,3))/2.0;
    H.val(15,3)=H.val(13,1); //(1.0+r1)*(1.0+r2)*(1.0-r3)*0.125;//- (H.val(39,3)+H.val(48,3)+H.val(51,3))/2.0;

    // influence of the node number 4
    H.val(10,1)=(1.0+r1)*(1.0-r2)*(1.0+r3)*0.125;//- (H.val(33,3)+H.val(36,3)+H.val(60,3))/2.0;
    H.val(11,2)=H.val(10,1); //(1.0+r1)*(1.0-r2)*(1.0+r3)*0.125;//- (H.val(33,3)+H.val(36,3)+H.val(60,3))/2.0;
    H.val(12,3)=H.val(10,1); //(1.0+r1)*(1.0-r2)*(1.0+r3)*0.125;//- (H.val(33,3)+H.val(36,3)+H.val(60,3))/2.0;
    // influence of the node number 3
    H.val(7,1)=(1.0-r1)*(1.0-r2)*(1.0+r3)*0.125;//- (H.val(30,3)+H.val(33,3)+H.val(57,3))/2.0;
    H.val(8,2)=H.val(7,1); //(1.0-r1)*(1.0-r2)*(1.0+r3)*0.125;//- (H.val(30,3)+H.val(33,3)+H.val(57,3))/2.0;
    H.val(9,3)=H.val(7,1); //(1.0-r1)*(1.0-r2)*(1.0+r3)*0.125;//- (H.val(30,3)+H.val(33,3)+H.val(57,3))/2.0;
    // influence of the node number 2
    H.val(4,1)=(1.0-r1)*(1.0+r2)*(1.0+r3)*0.125;//- (H.val(30,3)+H.val(54,3)+H.val(27,3))/2.0;
    H.val(5,2)=H.val(4,1); //(1.0-r1)*(1.0+r2)*(1.0+r3)*0.125;//- (H.val(30,3)+H.val(54,3)+H.val(27,3))/2.0;
    H.val(6,3)=H.val(4,1); //(1.0-r1)*(1.0+r2)*(1.0+r3)*0.125;//- (H.val(30,3)+H.val(54,3)+H.val(27,3))/2.0;
    // influence of the node number 1
    H.val(1,1)=(1.0+r1)*(1.0+r2)*(1.0+r3)*0.125;//- (H.val(36,3)+H.val(51,3)+H.val(27,3))/2.0;
    H.val(2,2)=H.val(1,1); //(1.0+r1)*(1.0+r2)*(1.0+r3)*0.125;//- (H.val(36,3)+H.val(51,3)+H.val(27,3))/2.0;
    H.val(3,3)=H.val(1,1); //(1.0+r1)*(1.0+r2)*(1.0+r3)*0.125;//- (H.val(36,3)+H.val(51,3)+H.val(27,3))/2.0;

                 // The second part were commented by Xiaoyan
    //         double sum= 0;
    //
    //   for(int i=1; i<=60 ; i++)
    //           {
    // //        sum+=H.cval(i,1);
    //       for(int j=1; j<= 1; j++)
    //          {
    //                    sum+=H.cval(i,1);
    //             ::printf( "  %+9.2e", H.cval(i,j) );
    //           }
    //            // ::printf( "  %d \n", i);
    //      }
    //       ::printf( " \n sum= %+6.2e\n", sum );


    //    printf("r1= %lf, r2= %lf, r3= %lf\n", r1, r2, r3);
    //    H.print("h");

    return H;
  }

//#############################################################################
//***************************************************************
 XC::BJtensor XC::EightNodeBrick::interp_poli_at(double r1, double r2, double r3)
  {

    int dimension[]= {8};  // Xiaoyan changed from {20} to {8} for 8 nodes 07/12
    BJtensor h(1, dimension, 0.0);


    // Commented by Xiaoyan

    // influence of the node number 8
    h.val(8)=(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0;// - (h.val(15)+h.val(16)+h.val(20))/2.0;
    // influence of the node number 7
    h.val(7)=(1.0-r1)*(1.0-r2)*(1.0-r3)/8.0;// - (h.val(14)+h.val(15)+h.val(19))/2.0;
    // influence of the node number 6
    h.val(6)=(1.0-r1)*(1.0+r2)*(1.0-r3)/8.0;// - (h.val(13)+h.val(14)+h.val(18))/2.0;
    // influence of the node number 5
    h.val(5)=(1.0+r1)*(1.0+r2)*(1.0-r3)/8.0;// - (h.val(13)+h.val(16)+h.val(17))/2.0;

    // influence of the node number 4
    h.val(4)=(1.0+r1)*(1.0-r2)*(1.0+r3)/8.0;// - (h.val(11)+h.val(12)+h.val(20))/2.0;
    // influence of the node number 3
    h.val(3)=(1.0-r1)*(1.0-r2)*(1.0+r3)/8.0;// - (h.val(10)+h.val(11)+h.val(19))/2.0;
    // influence of the node number 2
    h.val(2)=(1.0-r1)*(1.0+r2)*(1.0+r3)/8.0;// - (h.val(10)+h.val(18)+h.val(9))/2.0;
    // influence of the node number 1
    h.val(1)=(1.0+r1)*(1.0+r2)*(1.0+r3)/8.0;// - (h.val(12)+h.val(17)+h.val(9))/2.0;

    return h;
  }



 XC::BJtensor XC::EightNodeBrick::dh_drst_at(double r1, double r2, double r3) const
  {

    int dimensions[]= {8,3};  // Changed from{20,3} to {8,3} Xiaoyan 07/12
    BJtensor dh(2, dimensions, 0.0);


    // influence of the node number 8
    dh.val(8,1)= (1.0-r2)*(1.0-r3)*0.125; ///8.0;// - (dh.val(15,1)+dh.val(16,1)+dh.val(20,1))/2.0;
    dh.val(8,2)=-(1.0+r1)*(1.0-r3)*0.125; ///8.0;// - (dh.val(15,2)+dh.val(16,2)+dh.val(20,2))/2.0;
    dh.val(8,3)=-(1.0+r1)*(1.0-r2)*0.125; ///8.0;// - (dh.val(15,3)+dh.val(16,3)+dh.val(20,3))/2.0;
    // influence of the node number 7
    dh.val(7,1)=-(1.0-r2)*(1.0-r3)*0.125; ///8.0;// - (dh.val(14,1)+dh.val(15,1)+dh.val(19,1))/2.0;
    dh.val(7,2)=-(1.0-r1)*(1.0-r3)*0.125; ///8.0;// - (dh.val(14,2)+dh.val(15,2)+dh.val(19,2))/2.0;
    dh.val(7,3)=-(1.0-r1)*(1.0-r2)*0.125; ///8.0;// - (dh.val(14,3)+dh.val(15,3)+dh.val(19,3))/2.0;
    // influence of the node number 6
    dh.val(6,1)=-(1.0+r2)*(1.0-r3)*0.125; ///8.0;// - (dh.val(13,1)+dh.val(14,1)+dh.val(18,1))/2.0;
    dh.val(6,2)= (1.0-r1)*(1.0-r3)*0.125; ///8.0;// - (dh.val(13,2)+dh.val(14,2)+dh.val(18,2))/2.0;
    dh.val(6,3)=-(1.0-r1)*(1.0+r2)*0.125; ///8.0;//- (dh.val(13,3)+dh.val(14,3)+dh.val(18,3))/2.0;
    // influence of the node number 5
    dh.val(5,1)= (1.0+r2)*(1.0-r3)*0.125; ///8.0;// - (dh.val(13,1)+dh.val(16,1)+dh.val(17,1))/2.0;
    dh.val(5,2)= (1.0+r1)*(1.0-r3)*0.125; ///8.0;// - (dh.val(13,2)+dh.val(16,2)+dh.val(17,2))/2.0;
    dh.val(5,3)=-(1.0+r1)*(1.0+r2)*0.125; ///8.0;// - (dh.val(13,3)+dh.val(16,3)+dh.val(17,3))/2.0;

    // influence of the node number 4
    dh.val(4,1)= (1.0-r2)*(1.0+r3)*0.125; ///8.0;// - (dh.val(11,1)+dh.val(12,1)+dh.val(20,1))/2.0;
    dh.val(4,2)=-(1.0+r1)*(1.0+r3)*0.125; ///8.0;// - (dh.val(11,2)+dh.val(12,2)+dh.val(20,2))/2.0;
    dh.val(4,3)= (1.0+r1)*(1.0-r2)*0.125; ///8.0;// - (dh.val(11,3)+dh.val(12,3)+dh.val(20,3))/2.0;
    // influence of the node number 3
    dh.val(3,1)=-(1.0-r2)*(1.0+r3)*0.125; ///8.0;// - (dh.val(10,1)+dh.val(11,1)+dh.val(19,1))/2.0;
    dh.val(3,2)=-(1.0-r1)*(1.0+r3)*0.125; ///8.0;// - (dh.val(10,2)+dh.val(11,2)+dh.val(19,2))/2.0;
    dh.val(3,3)= (1.0-r1)*(1.0-r2)*0.125; ///8.0;// - (dh.val(10,3)+dh.val(11,3)+dh.val(19,3))/2.0;
    // influence of the node number 2
    dh.val(2,1)=-(1.0+r2)*(1.0+r3)*0.125; ///8.0;// - (dh.val(10,1)+dh.val(18,1)+dh.val(9,1))/2.0;
    dh.val(2,2)= (1.0-r1)*(1.0+r3)*0.125; ///8.0;// - (dh.val(10,2)+dh.val(18,2)+dh.val(9,2))/2.0;
    dh.val(2,3)= (1.0-r1)*(1.0+r2)*0.125; ///8.0;// - (dh.val(10,3)+dh.val(18,3)+dh.val(9,3))/2.0;
    // influence of the node number 1
    dh.val(1,1)= (1.0+r2)*(1.0+r3)*0.125; ///8.0;// - (dh.val(12,1)+dh.val(17,1)+dh.val(9,1))/2.0;
    dh.val(1,2)= (1.0+r1)*(1.0+r3)*0.125; ///8.0;// - (dh.val(12,2)+dh.val(17,2)+dh.val(9,2))/2.0;
    dh.val(1,3)= (1.0+r1)*(1.0+r2)*0.125; ///8.0;//- (dh.val(12,3)+dh.val(17,3)+dh.val(9,3))/2.0;
               // Commented by Xiaoyan
    return dh;
  }

////#############################################################################
 XC::EightNodeBrick & XC::EightNodeBrick::operator[](int subscript)
  {
    return ( *(this+subscript) );
  }

//Finite_Element & XC::EightNodeBrick::operator[](short subscript)
//  {
//    return ( *(this+subscript) );
//  }

//Finite_Element & XC::EightNodeBrick::operator[](unsigned subscript)
//  {
//    return ( *(this+subscript) );
//  }


////#############################################################################
 XC::BJtensor XC::EightNodeBrick::getStiffnessTensor(void) const
  {


//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in XC::BJtensor XC::EightNodeBrick::getStiffnessTensor(void) " <<std::endl; this->Print(std::cerr);
//BJ//BJ

    int K_dim[]= {8,3,3,8};  // Xiaoyan changed from {20,3,3,20} to {8,3,3,8}
            // for 8 nodes      07/12
    BJtensor Kk(4,K_dim,0.0);
    BJtensor Kkt(4,K_dim,0.0);

    //debugging
    matrix Kmat;

    double r= 0.0;
    double rw= 0.0;
    double s= 0.0;
    double sw= 0.0;
    double t= 0.0;
    double tw= 0.0;

    short where= 0;
    double weight= 0.0;

    int dh_dim[]= {8,3};  // Xiaoyan changed from {20,3} to {8,3}
    BJtensor dh(2, dh_dim, 0.0);

    //    BJtensor Constitutive( 4, def_dim_4, 0.0);
    BJtensor Constitutive;

    double det_of_Jacobian= 0.0;

    static int disp_dim[]= {8,3};   // Xiaoyan changed from {20,3} to {8,3}
    BJtensor incremental_displacements(2,disp_dim,0.0); // \Delta u

    straintensor incremental_strain;
//    straintensor total_strain_at_GP;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor JacobianINVtemp;
    BJtensor dhGlobal;

    //double tmp= 0;


    // BJtensor of incremental displacements taken from node objects
    //incremental_displacements= incr_disp();
    //std::cerr << "\n=======" << getTag() << "===========\n";
    //incremental_displacements.print("incr_disp");

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        rw= get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            sw= get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                tw= get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh= dh_drst_at(r,s,t);
    //dh.print("dh");
                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //Jacobian.print("J");

    // Inverse of Jacobian XC::BJtensor ( matrix )
                //JacobianINV= Jacobian_3Dinv(dh);
                JacobianINV= Jacobian.inverse();
    //JacobianINVtemp= JacobianINVtemp - JacobianINV;
    //JacobianINV.print();
       //JacobianINVtemp.print();

                //JacobianINV= Jacobian.inverse(); //Zhaohui 08-31-2001


                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian= Jacobian.determinant();
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal= dh("ij") * JacobianINV("kj");
                //        ::fprintf(stdout," # %d \n\n\n\n\n\n\n\n", El_count);
    //dhGlobal.print("dhGlobal");
                //weight
                weight= rw * sw * tw * det_of_Jacobian;
                //::::
                //printf("\n\nIN THE STIFFNESS TENSOR INTEGRATOR ----**************** where= %d \n", where);
                //printf("  Stifness_Tensor \n");
                //printf("                    GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
                //                            GP_c_r,GP_c_s,GP_c_t);
                //printf("WEIGHT= %f", weight);
                //Jacobian.print("J");
                //JacobianINV.print("JINV");
                //JacobianINVtemp.print("JINVtemp");
                //BJtensor I= JacobianINV("ij")*Jacobian("jk");
                //I.print("I");

                //printf("determinant of Jacobian= %.6e", det_of_Jacobian);
                //matpoint[where].report("Gauss Point\n");

                // incremental strains at this Gauss point
                //GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor1\n");

    //incremental_strain =
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
                //                total_strain_at_GP= total_strain_at_GP + incremental_strain;
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
                //......         Constitutive=  GPtangent_E[where];

    //Constitutive=  (matpoint)->operator[](where).ConstitutiveTensor(final_stress_after_integration,
                //                                         *(GPstress+where),
                //                                          incremental_strain,
                //                                          (matpoint)->operator[](where),
                //                                          this_one_PP);
                //Constitutive.print("C","\n\n C XC::BJtensor \n");

          //EPState *tmp_eps= (matpoint[where]).getEPS();
          //NDMaterial *tmp_ndm= (matpoint[where]).getNDMat();

    //if( tmp_eps ) {     //Elasto-plastic case
    //    mmodel->setEPS( *tmp_eps );

    //int err= ( matpoint[where].matmodel )->setTrialStrainIncr( incremental_strain);
    //if( err)
                 //   std::cerr << "XC::EightNodeBrick::getStiffnessTensor (tag: %d), not converged",
    //         this->getTag());

    Constitutive= (matpoint[where].matmodel)->getTangentTensor();
                //Constitutive.print("C","\n\n C XC::BJtensor \n");

    //    matpoint[where].setEPS( mmodel->getEPS() );
    //}
    //else if( tmp_ndm ) { //Elastic case
    //    (matpoint[where].p_matmodel)->setTrialStrainIncr( incremental_strain );
    //    Constitutive= (matpoint[where].p_matmodel)->getTangentTensor();
    //}
    //else {
                 //   std::cerr << "XC::EightNodeBrick::incremental_Update (tag: %d), could not getTangentTensor", this->getTag();
    //   exit(1);
    //}

    //printf("Constitutive.trace= %12.6e\n", Constitutive.trace());
                //Kmat= this->stiffness_matrix(Constitutive);
                //printf("Constitutive XC::BJtensor max:= %10.3e\n", Kmat.mmax());

                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor5\n");
                // this is update of constitutive XC::BJtensor at this Gauss point
                //GPtangent_E[where].Initialize(Constitutive);
                //....GPtangent_E[where].print(" tangent E at GAUSS point");

                //GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor6\n");

                //K= K + temp2;

    Kkt= dhGlobal("ib")*Constitutive("abcd");
    Kkt= Kkt("aicd")*dhGlobal("jd")*weight;
       Kk= Kk + Kkt;

    //Kk= Kk + dhGlobal("ib")*Constitutive("abcd")*dhGlobal("jd")*weight;
                //....K.print("K","\n\n K XC::BJtensor \n");

    //Kmat= this->stiffness_matrix(Kkt);
                //Kmat.print("Kmat");

    //printf("K XC::BJtensor max= %10.3e\n", Kmat.mmax());

                //convert constitutive and K to matrix and find min and max and print!



              }
          }
      }
    //Kk.print("K","\n\n K XC::BJtensor \n");
    //K= Kk;
    return Kk;
  }


//#############################################################################

void XC::EightNodeBrick::set_strain_stress_tensor(FILE *fp, double * u)
  {

//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in void XC::EightNodeBrick::set_strain_stress_tensor" <<std::endl; this->Print(std::cerr);
//BJ//BJ

    int dh_dim[]= {8,3};   // Xiaoyan changed from {20,3} to {8,3}
    BJtensor dh(2, dh_dim, 0.0);

//    BJtensor Constitutive( 4, def_dim_4, 0.0);
    BJtensor Constitutive;
    double r= 0.0;
    double s= 0.0;
    double t= 0.0;
    int where= 0;

    double det_of_Jacobian;

    straintensor strain;
    stresstensor stress;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;


    static int disp_dim[]= {8,3};    // Xiaoyan changed from {20,3} to {8,3}
    BJtensor total_displacements(2,disp_dim,0.0); //

    total_displacements= total_disp(fp, u);

    std::cerr << "\n  displacement(x-y-z) at GAUSS pt " << where+1 << " \n\n";
    for(int ii=1; ii<=8;ii++)
      {
	std::cerr << "Global# " << theNodes.getTagNode(ii-1)
	          << "Local# " << ii
                  << " " << total_displacements.val(ii,1)
                  << " " << total_displacements.val(ii,2)
                  << " " << total_displacements.val(ii,3) << std::endl;
      }
    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh= dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV= Jacobian_3Dinv(dh);
                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian= Jacobian.determinant();
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal= dh("ij") * JacobianINV("kj");
                //weight
                // strains at this Gauss point from displacement
                strain= (dhGlobal("ib")*total_displacements("ia")).symmetrize11();
                strain.null_indices();
                // here comes the final_stress calculation
                // at this Gauss point.

                //Constitutive=  GPtangent_E[where];
                //Constitutive=  (matpoint->getEPS() )->getEep();
                // if set total displ, then it should be elstic material
                Constitutive=  ( matpoint[where].matmodel)->getTangentTensor();

                stress= Constitutive("ijkl") * strain("kl");   //<<<<<<<<<<<<<<<
                stress.null_indices();

		std::cerr << "\n  strain tensor at GAUSS point " <<  where+1 << std::endl;
                strain.reportshort("");
		std::cerr << "\n  stress tensor at GAUSS point " <<  where+1 << std::endl;
                stress.reportshort("");
              }
          }
      }
  }


//! @brief Returns the tensor de masas.
XC::BJtensor XC::EightNodeBrick::getMassTensor(void) const
  {
    //int M_dim[]= {8,3,3,8};
    int M_dim[]= {24,24};
    BJtensor Mm(2,M_dim,0.0);

    double r= 0.0;
    double rw= 0.0;
    double s= 0.0;
    double sw= 0.0;
    double t= 0.0;
    double tw= 0.0;

    short where= 0;
    double weight= 0.0;

    int dh_dim[]= {8,3};    // Xiaoyan changed from {20,3} to {8,3}

    BJtensor dh(2, dh_dim, 0.0);

    int h_dim[]= {24,3};  // Xiaoyan changed from {60,3} to {24,3}
    //int h_dim[]= {8,3};  // Xiaoyan changed from {60,3} to {24,3}
    //int h_dim[]= {20,3};
    BJtensor H(2, h_dim, 0.0);

    double det_of_Jacobian= 0.0;

    BJtensor Jacobian;

    double RHO;
    RHO= rho;    //global

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        rw= get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            sw= get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                tw= get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh= dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //     Jacobian.print("J","Jacobian");
                // Inverse of Jacobian XC::BJtensor ( matrix )
                //                JacobianINV= Jacobian_3Dinv(dh);
                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian= Jacobian.determinant();
                //     printf("det_of_Jacobian= %6.2e \n",det_of_Jacobian);
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //                dhGlobal= dh("ij") * JacobianINV("kj");
                // derivatives of local coordinates with respect to local coordinates


                // printf("\n\nIN THE MASS TENSOR INTEGRATOR ----**************** where= %d \n", where);
                // printf("  Mass_Tensor \n");
                // printf("                    GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
                //                             GP_c_r,GP_c_s,GP_c_t);
                //
                H= H_3D(r,s,t);

                //  double sum= 0.0;
                //  for(int i=1; i<=60 ; i++)
                //           {
                // //        sum+=H.cval(i,1);
                //       for(int j=1; j<= 3; j++)
                //          {
                //                    sum+=H.cval(i,j);
                //             ::printf( "  %+9.2e", H.cval(i,j) );
                //           }
                //             ::printf( "  %d \n", i);
                //      }
                //       ::printf( " \n sum= %+6.2e\n", sum );




                // matpoint GaPo= XC::MatPoint3D::GP()+where;

    // if it's elasto-plastic, then use the rho at each integration point
    //if( matpoint[where] ) //need to refine possible bug
    //  RHO= matpoint[where].getrho();

    //printf("RHO= %10.3e",RHO);
                //    getchar();

                //weight
                weight= rw * sw * tw * RHO * det_of_Jacobian;
            //  printf("weight= %6.2e \n",weight);

    //M.print("M","BEFORE");

          //  BJtensor temp= H("ib")*H("kb");
    //temp.print("t","temporary XC::BJtensor H(\"ib\")*H(\"kb\") \n\n" );

    Mm= Mm + H("ib")*H("kb")*weight;
         //  printf("\n +++++++++++++++++++++++++ \n\n");
          //Mm.printshort("M");
              }
          }
      }
    //M= Mm;
    //Mm.printshort("M");
    return Mm;
  }



////#############################################################################

 XC::BJtensor XC::EightNodeBrick::stiffness_matrix(const XC::BJtensor & K)
  {
//BJ/BJ
//BJ    std::cerr << "\n\n\n\n Print in XC::BJtensor XC::EightNodeBrick::stiffness_matrix" <<std::endl; this->Print(std::cerr);
//BJ//BJ
//    int K_dim[]= {20,3,3,20};
//    BJtensor K(4,K_dim,0.0);
    matrix Kmatrix(24,24,0.0);   // Xiaoyan changed from (60,60,0,0) to (24,24,0,0)

    int Ki=0;
    int Kj=0;

    for( int i=1 ; i<=8 ; i++ )  // Xiaoyan changed from i<=20 to i<=8 for 8 nodes
      {
        for( int j=1 ; j<=8 ; j++ )  // Xiaoyan changed from i<=20 to i<=8 for 8 nodes
          {
            for( int k=1 ; k<=3 ; k++ )
              {
                for( int l=1 ; l<=3 ; l++ )
                  {
                    Ki= k+3*(i-1);
                    Kj= l+3*(j-1);
                    //::printf("i=%d k=%d  Ki=%d       j=%d l=%d  Kj=%d\n",i,k,Ki,j,l,Kj);

                    Kmatrix.val( Ki , Kj )= K.cval(i,k,l,j);
                  }
              }
          }
      }
    return BJtensor(Kmatrix);
  }

//#############################################################################

////#############################################################################
// Constructing mass matrix from mass XC::BJtensor ___Zhaohui 07-05-99
 XC::BJtensor XC::EightNodeBrick::mass_matrix(const XC::BJtensor & M)
  {
    //    int K_dim[]= {20,3,3,20};
    //    BJtensor K(4,K_dim,0.0);
    matrix Mmatrix(24,24,0.0);  // Xiaoyan changed from (60,60,0,0) to (24,24,0,0) for 8 nodes

    for( int i=1 ; i<=24 ; i++ )   // Xiaoyan changed from i<=60 to i<=24 for 8 nodes
      {
        for( int j=1 ; j<=24 ; j++ ) // Xiaoyan changed from i<=60 to i<=24 for 8 nodes
          {
             Mmatrix.val( i , j )= M.cval(i,j);
             //  ::printf("Mi Mj %d %d %+6.2e ",Mi,Mj,Mmatrix.val( Mi , Mj ) );
          }
      }
    return BJtensor(Mmatrix);
  }
////#############################################################################

////#############################################################################
 XC::BJtensor XC::EightNodeBrick::Jacobian_3D(BJtensor dh) const
  {
     //       dh ( 20*3)  // dh(8*3) Xiaoyan
     BJtensor N_C= Nodal_Coordinates(); // 20*3    // 8*3 Xiaoyan
     BJtensor Jacobian_3D= dh("ij") * N_C("ik");
     return Jacobian_3D;
  }

//#############################################################################
 XC::BJtensor XC::EightNodeBrick::Jacobian_3Dinv(BJtensor dh) const
  {
     //       dh ( 20*3)    // dh(8*3) Xiaoyan
     BJtensor N_C= Nodal_Coordinates(); // 20*3        // 8*3 Xiaoyan
     BJtensor Jacobian_3Dinv= (dh("ij") * N_C("ik")).inverse();
     return Jacobian_3Dinv;
  }


////#############################################################################
 XC::BJtensor XC::EightNodeBrick::Nodal_Coordinates(void) const
  {
    const int dimensions[]= {8,3};  // Xiaoyan changed from {20,3} to {8,3} for 8 nodes
    BJtensor N_coord(2, dimensions, 0.0);

    //for( int i=0 ; i<8 ; i++ )    // Xiaoyan changed from 20 to 8 for 8 nodes
    //  {
    //    //        N_coord.val(i+1,1)= nodes[ G_N_numbs[i] ].x_coordinate();
    //    //        N_coord.val(i+1,2)= nodes[ G_N_numbs[i] ].y_coordinate();
    //    //        N_coord.val(i+1,3)= nodes[ G_N_numbs[i] ].z_coordinate();
    //    // Xiaoyan changed to the following:  09/27/00
    //    /// LOOK WITH DDD
    //  Vector Coordinates= nodes[ G_N_numbs[i] ].getCrds();
    //    N_coord.val(i+1,1)= Coordinates(0);
    //    N_coord.val(i+1,2)= Coordinates(1);
    //    N_coord.val(i+1,3)= Coordinates(2);
    //  }

    //Zhaohui using node pointers, which come from the XC::Domain
    const XC::Vector &nd1Crds= theNodes[0]->getCrds();
    const XC::Vector &nd2Crds= theNodes[1]->getCrds();
    const XC::Vector &nd3Crds= theNodes[2]->getCrds();
    const XC::Vector &nd4Crds= theNodes[3]->getCrds();
    const XC::Vector &nd5Crds= theNodes[4]->getCrds();
    const XC::Vector &nd6Crds= theNodes[5]->getCrds();
    const XC::Vector &nd7Crds= theNodes[6]->getCrds();
    const XC::Vector &nd8Crds= theNodes[7]->getCrds();

    N_coord.val(1,1)=nd1Crds(0); N_coord.val(1,2)=nd1Crds(1); N_coord.val(1,3)=nd1Crds(2);
    N_coord.val(2,1)=nd2Crds(0); N_coord.val(2,2)=nd2Crds(1); N_coord.val(2,3)=nd2Crds(2);
    N_coord.val(3,1)=nd3Crds(0); N_coord.val(3,2)=nd3Crds(1); N_coord.val(3,3)=nd3Crds(2);
    N_coord.val(4,1)=nd4Crds(0); N_coord.val(4,2)=nd4Crds(1); N_coord.val(4,3)=nd4Crds(2);
    N_coord.val(5,1)=nd5Crds(0); N_coord.val(5,2)=nd5Crds(1); N_coord.val(5,3)=nd5Crds(2);
    N_coord.val(6,1)=nd6Crds(0); N_coord.val(6,2)=nd6Crds(1); N_coord.val(6,3)=nd6Crds(2);
    N_coord.val(7,1)=nd7Crds(0); N_coord.val(7,2)=nd7Crds(1); N_coord.val(7,3)=nd7Crds(2);
    N_coord.val(8,1)=nd8Crds(0); N_coord.val(8,2)=nd8Crds(1); N_coord.val(8,3)=nd8Crds(2);

    //N_coord.print();

    return N_coord;
  }

////#############################################################################
XC::BJtensor XC::EightNodeBrick::incr_disp(void) const
  {
    const int dimensions[]= {8,3};  // Xiaoyan changed from {20,3} to {8,3} for 8 nodes
    BJtensor increment_disp(2, dimensions, 0.0);

    //for( int i=0 ; i<8 ; i++ )   // Xiaoyan changed from 20 to 8 for 8 nodes
    //
    //  {
    //    // increment_disp.val(i+1,1)= nodes[ G_N_numbs[i] ].incremental_translation_x();
    //    // increment_disp.val(i+1,2)= nodes[ G_N_numbs[i] ].incremental_translation_y();
    //    // increment_disp.val(i+1,3)= nodes[ G_N_numbs[i] ].incremental_translation_z();
    //    // Xiaoyan changed to the following 09/27/00
    //    Vector IncremenDis= nodes[ G_N_numbs[i] ].getIncrDisp();
    //
    //    increment_disp.val(i+1,1)= IncremenDis(0);
    //    increment_disp.val(i+1,2)= IncremenDis(1);
    //    increment_disp.val(i+1,3)= IncremenDis(2);
    //
    //  }

    //Zhaohui using node pointers, which come from the XC::Domain
    //const XC::Vector &TotDis1= theNodes[0]->getTrialDisp();
    //const XC::Vector &incrdelDis1= theNodes[0]->getIncrDisp();
    //Have to get IncrDeltaDisp, not IncrDisp for cumulation of incr_disp

    const XC::Vector &IncrDis1= theNodes[0]->getIncrDeltaDisp();
    const XC::Vector &IncrDis2= theNodes[1]->getIncrDeltaDisp();
    const XC::Vector &IncrDis3= theNodes[2]->getIncrDeltaDisp();
    const XC::Vector &IncrDis4= theNodes[3]->getIncrDeltaDisp();
    const XC::Vector &IncrDis5= theNodes[4]->getIncrDeltaDisp();
    const XC::Vector &IncrDis6= theNodes[5]->getIncrDeltaDisp();
    const XC::Vector &IncrDis7= theNodes[6]->getIncrDeltaDisp();
    const XC::Vector &IncrDis8= theNodes[7]->getIncrDeltaDisp();

    //if( getTag() == 486 || getTag() == 566 || getTag() == 956)
    //{
    //std::cerr <<" \n\n element " << getTag() << std::std::endl;
    //std::cerr<<"\n tot node " << theNodes[0]->getTag() <<" x "<< TotDis1(0) <<" y "<< TotDis1(1) << " z "<< TotDis1(2);
    //
    //std::cerr<<"\n incr node " << theNodes[0]->getTag() <<" x "<< incrdelDis1(0) <<" y "<< incrdelDis1(1) << " z "<< incrdelDis1(2);
    //
    //std::cerr << "\nincr del node " << theNodes[0]->getTag() << " x " << IncrDis1(0) <<" y "<< IncrDis1(1) << " z "<< IncrDis1(2) << std::std::endl;
    //
    //std::cerr << " incr del node " << theNodes[1]->getTag() << " x " << IncrDis2(0) <<" y "<< IncrDis2(1) << " z "<< IncrDis2(2) << std::std::endl;
    //
    //std::cerr << " incr del node " << theNodes[2]->getTag() << " x " << IncrDis3(0) <<" y "<< IncrDis3(1) << " z "<< IncrDis3(2) << std::std::endl;
    //
    //std::cerr << " incr del node " << theNodes[3]->getTag() << " x " << IncrDis4(0) <<" y "<< IncrDis4(1) << " z "<< IncrDis4(2) << std::std::endl;
    //
    //std::cerr << " incr del node " << theNodes[4]->getTag() << " x " << IncrDis5(0) <<" y "<< IncrDis5(1) << " z "<< IncrDis5(2) << std::std::endl;
    //std::cerr << " incr del node " << theNodes[5]->getTag() << " x " << IncrDis6(0) <<" y "<< IncrDis6(1) << " z "<< IncrDis6(2) << std::std::endl;
    //std::cerr << " incr del node " << theNodes[6]->getTag() << " x " << IncrDis7(0) <<" y "<< IncrDis7(1) << " z "<< IncrDis7(2) << std::std::endl;
    //std::cerr << " incr del node " << theNodes[7]->getTag() << " x " << IncrDis8(0) <<" y "<< IncrDis8(1) << " z "<< IncrDis8(2) << std::std::endl;
    //}

    increment_disp.val(1,1)=IncrDis1(0); increment_disp.val(1,2)=IncrDis1(1);increment_disp.val(1,3)=IncrDis1(2);
    increment_disp.val(2,1)=IncrDis2(0); increment_disp.val(2,2)=IncrDis2(1);increment_disp.val(2,3)=IncrDis2(2);
    increment_disp.val(3,1)=IncrDis3(0); increment_disp.val(3,2)=IncrDis3(1);increment_disp.val(3,3)=IncrDis3(2);
    increment_disp.val(4,1)=IncrDis4(0); increment_disp.val(4,2)=IncrDis4(1);increment_disp.val(4,3)=IncrDis4(2);
    increment_disp.val(5,1)=IncrDis5(0); increment_disp.val(5,2)=IncrDis5(1);increment_disp.val(5,3)=IncrDis5(2);
    increment_disp.val(6,1)=IncrDis6(0); increment_disp.val(6,2)=IncrDis6(1);increment_disp.val(6,3)=IncrDis6(2);
    increment_disp.val(7,1)=IncrDis7(0); increment_disp.val(7,2)=IncrDis7(1);increment_disp.val(7,3)=IncrDis7(2);
    increment_disp.val(8,1)=IncrDis8(0); increment_disp.val(8,2)=IncrDis8(1);increment_disp.val(8,3)=IncrDis8(2);

    return increment_disp;
  }

////#############################################################################
XC::BJtensor XC::EightNodeBrick::total_disp(void) const
  {
    const int dimensions[]= {8,3};  // Xiaoyan changed from {20,3} to {8,3} for 8 nodes
    BJtensor total_disp(2, dimensions, 0.0);

    //Zhaohui using node pointers, which come from the XC::Domain
    const XC::Vector &TotDis1= theNodes[0]->getTrialDisp();
//    std::cerr<<"\ntot node " << theNodes[0]->getTag() <<" x "<< TotDis1(0) <<" y "<< TotDis1(1) << " z "<< TotDis1(2) << std::endl;
    const XC::Vector &TotDis2= theNodes[1]->getTrialDisp();
//    std::cerr << "tot node " << theNodes[1]->getTag() << " x " << TotDis2(0) <<" y "<< TotDis2(1) << " z "<< TotDis2(2) << std::endl;
    const XC::Vector &TotDis3= theNodes[2]->getTrialDisp();
//    std::cerr << "tot node " << theNodes[2]->getTag() << " x " << TotDis3(0) <<" y "<< TotDis3(1) << " z "<< TotDis3(2) << std::endl;
    const XC::Vector &TotDis4= theNodes[3]->getTrialDisp();
//    std::cerr << "tot node " << theNodes[3]->getTag() << " x " << TotDis4(0) <<" y "<< TotDis4(1) << " z "<< TotDis4(2) << std::endl;
    const XC::Vector &TotDis5= theNodes[4]->getTrialDisp();
//    std::cerr << "tot node " << theNodes[4]->getTag() << " x " << TotDis5(0) <<" y "<< TotDis5(1) << " z "<< TotDis5(2) << std::endl;
    const XC::Vector &TotDis6= theNodes[5]->getTrialDisp();
//    std::cerr << "tot node " << theNodes[5]->getTag() << " x " << TotDis6(0) <<" y "<< TotDis6(1) << " z "<< TotDis6(2) << std::endl;
    const XC::Vector &TotDis7= theNodes[6]->getTrialDisp();
//    std::cerr << "tot node " << theNodes[6]->getTag() << " x " << TotDis7(0) <<" y "<< TotDis7(1) << " z "<< TotDis7(2) << std::endl;
    const XC::Vector &TotDis8= theNodes[7]->getTrialDisp();
//    std::cerr << "tot node " << theNodes[7]->getTag() << " x " << TotDis8(0) <<" y "<< TotDis8(1) << " z "<< TotDis8(2) << std::endl;

    total_disp.val(1,1)=TotDis1(0); total_disp.val(1,2)=TotDis1(1);total_disp.val(1,3)=TotDis1(2);
    total_disp.val(2,1)=TotDis2(0); total_disp.val(2,2)=TotDis2(1);total_disp.val(2,3)=TotDis2(2);
    total_disp.val(3,1)=TotDis3(0); total_disp.val(3,2)=TotDis3(1);total_disp.val(3,3)=TotDis3(2);
    total_disp.val(4,1)=TotDis4(0); total_disp.val(4,2)=TotDis4(1);total_disp.val(4,3)=TotDis4(2);
    total_disp.val(5,1)=TotDis5(0); total_disp.val(5,2)=TotDis5(1);total_disp.val(5,3)=TotDis5(2);
    total_disp.val(6,1)=TotDis6(0); total_disp.val(6,2)=TotDis6(1);total_disp.val(6,3)=TotDis6(2);
    total_disp.val(7,1)=TotDis7(0); total_disp.val(7,2)=TotDis7(1);total_disp.val(7,3)=TotDis7(2);
    total_disp.val(8,1)=TotDis8(0); total_disp.val(8,2)=TotDis8(1);total_disp.val(8,3)=TotDis8(2);

    return total_disp;
  }


////#############################################################################
 XC::BJtensor XC::EightNodeBrick::total_disp(FILE *fp, double * u)
  {
    const int dimensions[]= {8,3};  // Xiaoyan changed from {20,3} to {8,3} for 8 nodes
    BJtensor total_disp(2, dimensions, 0.0);
    //    double totalx, totaly, totalz;
    //    totalx=0;
    //    totaly=0;
    //    totalz=0;

    //for( int i=0 ; i<8 ; i++ )  // Xiaoyan changed from 20 to 8 for 8 nodes
    //
    //  {
    //    // total_disp.val(i+1,1)= nodes[ G_N_numbs[i] ].total_translation_x(u);
    //    // total_disp.val(i+1,2)= nodes[ G_N_numbs[i] ].total_translation_y(u);
    //    // total_disp.val(i+1,3)= nodes[ G_N_numbs[i] ].total_translation_z(u);
    //    // Xiaoyan changed to the following 09/27/00
    //    Vector TotalTranDis= nodes[ G_N_numbs[i] ].getDisp();
    //
    //    total_disp.val(i+1,1)= TotalTranDis(0);
    //  total_disp.val(i+1,2)= TotalTranDis(1);
    //    total_disp.val(i+1,3)= TotalTranDis(2);
    //
    //  }

    //Zhaohui using node pointers, which come from the XC::Domain
    const XC::Vector &TotDis1= theNodes[0]->getTrialDisp();
    const XC::Vector &TotDis2= theNodes[1]->getTrialDisp();
    const XC::Vector &TotDis3= theNodes[2]->getTrialDisp();
    const XC::Vector &TotDis4= theNodes[3]->getTrialDisp();
    const XC::Vector &TotDis5= theNodes[4]->getTrialDisp();
    const XC::Vector &TotDis6= theNodes[5]->getTrialDisp();
    const XC::Vector &TotDis7= theNodes[6]->getTrialDisp();
    const XC::Vector &TotDis8= theNodes[7]->getTrialDisp();

    total_disp.val(1,1)=TotDis1(0); total_disp.val(1,2)=TotDis1(1);total_disp.val(1,3)=TotDis1(2);
    total_disp.val(2,1)=TotDis2(0); total_disp.val(2,2)=TotDis2(1);total_disp.val(2,3)=TotDis2(2);
    total_disp.val(3,1)=TotDis3(0); total_disp.val(3,2)=TotDis3(1);total_disp.val(3,3)=TotDis3(2);
    total_disp.val(4,1)=TotDis4(0); total_disp.val(4,2)=TotDis4(1);total_disp.val(4,3)=TotDis4(2);
    total_disp.val(5,1)=TotDis5(0); total_disp.val(5,2)=TotDis5(1);total_disp.val(5,3)=TotDis5(2);
    total_disp.val(6,1)=TotDis6(0); total_disp.val(6,2)=TotDis6(1);total_disp.val(6,3)=TotDis6(2);
    total_disp.val(7,1)=TotDis7(0); total_disp.val(7,2)=TotDis7(1);total_disp.val(7,3)=TotDis7(2);
    total_disp.val(8,1)=TotDis8(0); total_disp.val(8,2)=TotDis8(1);total_disp.val(8,3)=TotDis8(2);

    return total_disp;
  }


////#############################################################################
int XC::EightNodeBrick::get_global_number_of_node(int local_node_number)
{
  //return G_N_numbs[local_node_number];
  return theNodes.getTagNode(local_node_number);
}

////#############################################################################
int  XC::EightNodeBrick::get_Brick_Number(void)
{
  //return elem_numb;
  return this->getTag();
}

////#############################################################################
int * XC::EightNodeBrick::get_LM(void)
  {
    return LM;
  }

////#############################################################################
// returns nodal forces for given stress field in an element
XC::BJtensor XC::EightNodeBrick::nodal_forces(void) const
  {

//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in XC::BJtensor XC::EightNodeBrick::nodal_forces(" <<std::endl; this->Print(std::cerr);
//BJ//BJ

    int force_dim[]= {8,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor nodal_forces(2,force_dim,0.0);

    double r= 0.0;
    double rw= 0.0;
    double s= 0.0;
    double sw= 0.0;
    double t= 0.0;
    double tw= 0.0;

    short where= 0;
    double weight= 0.0;

    int dh_dim[]= {8,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(2, dh_dim, 0.0);

    stresstensor stress_at_GP(0.0);

    double det_of_Jacobian= 0.0;

    straintensor incremental_strain;

    static int disp_dim[]= {8,3};   // Xiaoyan changed from {20,3} to {8,3}
    BJtensor incremental_displacements(2,disp_dim,0.0); // \Delta u

    incremental_displacements= incr_disp();

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        rw= get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            sw= get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                tw= get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                // derivatives of local coordiantes with respect to local coordiantes
                dh= dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV= Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian= Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal= dh("ij") * JacobianINV("kj");

                //weight
                weight= rw * sw * tw * det_of_Jacobian;
    //std::cerr << " UCD det_of_Jacobian "<< det_of_Jacobian << std::endl;
                //..::printf("\n\nIN THE nodal forces ----**************** where= %d \n", where);
                //..::printf("                    GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
                //..                           GP_c_r,GP_c_s,GP_c_t);
                //..::printf("WEIGHT= %f", weight);
                //..::printf("determinant of Jacobian= %f", det_of_Jacobian);
                //..matpoint[where].report("Gauss Point\n");

                //..    just determine this BJtensor E yet and for each gauss point different !!!!!!!!!!!!
                //..    here should be based on incremental shifts somewhere
                //..    find incremental deformation (strain_increment) and then with it:
                //..
                //// BJtensor of incremental displacements taken from node objects
                //                incremental_displacements= incr_disp();
                //
                //// incremental strains at this Gauss point
                //                incremental_strain =
                //                  (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                //
                //                incremental_strain.null_indices();
                ////incremental_strain.reportshort("\n incremental_strain XC::BJtensor at GAUSS point\n");
                //
                ////                integr_type= (matpoint)->operator[](where).integration_type();
                ////                if( !strcmp(integr_type,"BakcwardEuler")

                //..  so send stress_increment here because old stress is stored within each
                //..  Gauss points and only send strain_increment to be transmitted
                //..  into the integration routine so that it will return the final voltage and then it can
                //..  that is, the ConstitutiveStiffnessTensor.
                //..  Actually send everything you have (incremental_strain, start_stress,
                //..  number_of_subincrements. . . into this Constitutive_tensor function
                //..  so let it go, depending on the model used and let
                //..  then there in each special model function return what it needs
                //..  (say Elastic returns Eelastic immediately and let's say MRS_Lade first
                //..  asks what method of integration to use and then depending on it
                //..  call function which integrals for that algorithm (ForwardEuler, BakcwardEuler,
                //..  SemiBackwardEuler ,. . . ) and then when the function returns voltage then
                //..  one wonders again what type of integration was at stake so the right one
                //..  ConstitutiveTensor and brings it back!

                //                   stress_at_GP= (GPstress)->operator[](where);
                //stress_at_GP= GPstress[where];

          //EPState *tmp_eps= (matpoint[where].matmodel)->getEPS();
    //stress_at_GP= tmp_eps->getStress();
    //std::cerr << "tmp_eps" << (*tmp_eps);

          //NDMaterial *tmp_ndm= (matpoint[where]).getNDMat();

    //if( tmp_eps ) {     //Elasto-plastic case

    //stress_at_GP= (matpoint[where].matmodel->getEPS())->getStress();

    //   EPState *tmp_eps= (matpoint[where].matmodel)->getEPS();
    //   stress_at_GP= tmp_eps->getStress();



//Guanzhou out 5-6-2004    incremental_strain =
//Guanzhou out 5-6-2004                     (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
//Guanzhou out 5-6-2004//    if(where == 0)
//Guanzhou out 5-6-2004//       //std::cerr << " In nodal_force delta_incremental_strain tag "<< getTag() <<"  " <<incremental_strain << std::endl;
//Guanzhou out 5-6-2004////    std::cerr << " el tag= "<< getTag();
//Guanzhou out 5-6-2004//
//Guanzhou out 5-6-2004    int err= ( matpoint[where].matmodel )->setTrialStrainIncr( incremental_strain);
//Guanzhou out 5-6-2004    if( err)
//Guanzhou out 5-6-2004      std::cerr << "XC::EightNodeBrick::getStiffnessTensor (tag: " << this->getTag() << "), not converged\n";


    //char *test= matpoint[where].matmodel->getType();
    // fmk - changing if so if into else block must be XC::Template3Dep
//    if(strcmp(matpoint[where].matmodel->getType(),"Template3Dep") != 0)
       stress_at_GP= matpoint[where].getStressTensor();

//         stress_at_GP.report("PROBLEM");
//         getchar();

//    else
//    {
//             //Some thing funny happened when getting stress directly from matpoint[where], i have to do it this way!
//       EPState *tmp_eps= ((Template3Dep *)(matpoint[where].matmodel))->getEPS();
//       stress_at_GP= tmp_eps->getStress();
//       //delete tmp_eps;
//           }

             //double  p= stress_at_GP.p_hydrostatic();
                //if( p < 0.0 )
          //{
          //  std::cerr << getTag();
          //  std::cerr << " ***p=    " << p << std::endl;
          //}

    //std::cerr << " nodal_force :: stress_at_GP " << stress_at_GP << std::endl;

    //}
    //else if( tmp_ndm ) { //Elastic case
               //    stress_at_GP= (matpoint[where].getNDMat())->getStressTensor();
    //}
    //else {
                 //   std::cerr << "XC::EightNodeBrick::nodal_forces (tag: %d), could not getStress", this->getTag();
    //   exit(1);
    //}

                //stress_at_GP.report("\n stress_at_GPtensor at GAUSS point for nodal forces \n");

                // nodal forces See Zienkievicz part 1 pp 108
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
XC::BJtensor XC::EightNodeBrick::iterative_nodal_forces(void) const
  {

//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in XC::BJtensor XC::EightNodeBrick::iterative_nodal_forces(void)" <<std::endl; this->Print(std::cerr);
//BJ//BJ

    int force_dim[]= {8,3}; // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor nodal_forces(2,force_dim,0.0);

    double r= 0.0;
    double rw= 0.0;
    double s= 0.0;
    double sw= 0.0;
    double t= 0.0;
    double tw= 0.0;

    short where= 0;
    double weight= 0.0;

    int dh_dim[]= {8,3};   // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(2, dh_dim, 0.0);

    stresstensor stress_at_GP(0.0);

    double det_of_Jacobian= 0.0;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        rw= get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            sw= get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                tw= get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                //.....
                //.....::printf("XC::EightNodeBrick::iterative_nodal_forces(void)  ----**************** where= %d \n", where);
                //.....::printf("UPDATE ");
                //.....::printf("   GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
                //.....                           GP_c_r,GP_c_s,GP_c_t);
                // derivatives of local coordiantes with respect to local coordiantes
                dh= dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV= Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian= Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal= dh("ij") * JacobianINV("kj");

                //weight
                weight= rw * sw * tw * det_of_Jacobian;

                //                   stress_at_GP= (GPstress)->operator[](where);
                //stress_at_GP= GPiterative_stress[where];

    //stress_at_GP= ( matpoint[where].getTrialEPS() )->getStress();
                stress_at_GP= matpoint[where].getStressTensor();
                stress_at_GP.reportshortpqtheta("\n iterative_stress at GAUSS point in iterative_nodal_force\n");

                // nodal forces See Zienkievicz part 1 pp 108
                nodal_forces =
                  nodal_forces + dhGlobal("ib")*stress_at_GP("ab")*weight;
                //nodal_forces.print("nf","\n XC::EightNodeBrick::iterative_nodal_forces Nodal Forces ~~~~\n");

              }
          }
      }


    return nodal_forces;

  }

////#############################################################################
// returns nodal forces for given constant stress field in the element
XC::BJtensor XC::EightNodeBrick::nodal_forces_from_stress(stresstensor & stress) const
  {


//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in XC::BJtensor XC::EightNodeBrick::nodal_forces_from_stress(stresstensor & stress)" <<std::endl; this->Print(std::cerr);
//BJ//BJ

    int force_dim[]= {8,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor nodal_forces(2,force_dim,0.0);

    double r= 0.0;
    double rw= 0.0;
    double s= 0.0;
    double sw= 0.0;
    double t= 0.0;
    double tw= 0.0;

    double weight= 0.0;

    int dh_dim[]= {8,3}; // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(2, dh_dim, 0.0);

    double det_of_Jacobian= 0.0;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        rw= get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            sw= get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                tw= get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                //--                where =
                //--                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                //.....
                //.....::printf("XC::EightNodeBrick::iterative_nodal_forces(void)  ----**************** where= %d \n", where);
                //.....::printf("UPDATE ");
                //.....::printf("   GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
                //.....                           GP_c_r,GP_c_s,GP_c_t);
                // derivatives of local coordiantes with respect to local coordiantes
                dh= dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV= Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian= Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal= dh("ij") * JacobianINV("kj");

                //weight
                weight= rw * sw * tw * det_of_Jacobian;

                //                   stress_at_GP= (GPstress)->operator[](where);
                //                stress_at_GP= GPiterative_stress[where];
                //GPiterative_stress[where].reportshortpqtheta("\n iterative_stress at GAUSS point in iterative_nodal_force\n");

                // nodal forces See Zienkievicz part 1 pp 108
                nodal_forces =
                  nodal_forces + dhGlobal("ib")*stress("ab")*weight;
                //nodal_forces.print("nf","\n XC::EightNodeBrick::iterative_nodal_forces Nodal Forces ~~~~\n");

              }
          }
      }

    return nodal_forces;

  }


////#############################################################################
// returns nodal forces for given incremental strain field in an element
// by using the linearized constitutive XC::BJtensor from the beginning of the step !
XC::BJtensor XC::EightNodeBrick::linearized_nodal_forces(void) const
  {
    int force_dim[]= {8,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor linearized_nodal_forces(2,force_dim,0.0);

    double r= 0.0;
    double rw= 0.0;
    double s= 0.0;
    double sw= 0.0;
    double t= 0.0;
    double tw= 0.0;

    short where= 0;
    double weight= 0.0;

    int dh_dim[]= {8,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor dh(2, dh_dim, 0.0);

    BJtensor Constitutive( 4, def_dim_4, 0.0);

    double det_of_Jacobian= 0.0;

    static int disp_dim[]= {8,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    BJtensor incremental_displacements(2,disp_dim,0.0);

    straintensor incremental_strain;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    stresstensor final_linearized_stress;
    //    stresstensor incremental_stress;
    // BJtensor of incremental displacements taken from node objects for this element !
    incremental_displacements= incr_disp();
    //incremental_displacements.print("disp","\n incremental_displacements XC::BJtensor at GAUSS point in iterative_Update\n");

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        rw= get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            sw= get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                tw= get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                // derivatives of local coordiantes with respect to local coordiantes
                dh= dh_drst_at(r,s,t);

                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV= Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian XC::BJtensor ( matrix )
                det_of_Jacobian= Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal= dh("ij") * JacobianINV("kj");

                //weight
                weight= rw * sw * tw * det_of_Jacobian;
                //..::printf("\n\nIN THE nodal forces ----**************** where= %d \n", where);
                //..::printf("                    GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
                //..                           GP_c_r,GP_c_s,GP_c_t);
                //..::printf("WEIGHT= %f", weight);
                //..::printf("determinant of Jacobian= %f", det_of_Jacobian);
                // incremental strains at this Gauss point
                // now in Update we know the incremental displacements so let's find
                // the incremental strain
                incremental_strain =
                 (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                incremental_strain.null_indices();
                //incremental_strain.reportshort("\n incremental_strain XC::BJtensor at GAUSS point in iterative_Update\n");

                //Constitutive= GPtangent_E[where];

          //EPState *tmp_eps= (matpoint[where]).getEPS();
          //NDMaterial *tmp_ndm= (matpoint[where]).getNDMat();

    //if( tmp_eps ) {     //Elasto-plastic case
    //    mmodel->setEPS( *tmp_eps );
    if( ! (matpoint[where].matmodel)->setTrialStrainIncr( incremental_strain)  )
      std::cerr << "XC::EightNodeBrick::linearized_nodal_forces (tag: " << this->getTag() << "), not converged\n";

    Constitutive= (matpoint[where].matmodel)->getTangentTensor();
          //    matpoint[where].setEPS( mmodel->getEPS() ); //Set the new EPState back
    //}
    //else if( tmp_ndm ) { //Elastic case
    //    (matpoint[where].p_matmodel)->setTrialStrainIncr( incremental_strain );
    //    Constitutive= (matpoint[where].p_matmodel)->getTangentTensor();
    //}
    //else {
                 //   std::cerr << "XC::EightNodeBrick::incremental_Update (tag: %d), could not getTangentTensor", this->getTag();
    //   exit(1);
    //}

    //Constitutive= ( matpoint[where].getEPS() )->getEep();
                //..//GPtangent_E[where].print("\n tangent E at GAUSS point \n");

                final_linearized_stress =
                  Constitutive("ijkl") * incremental_strain("kl");

                // nodal forces See Zienkievicz part 1 pp 108
                linearized_nodal_forces= linearized_nodal_forces +
                          dhGlobal("ib")*final_linearized_stress("ab")*weight;
                //::::                   nodal_forces.print("nf","\n\n Nodal Forces \n");

              }
          }
      }


    return linearized_nodal_forces;

  }

//#############################################################################
void XC::EightNodeBrick::report(char * msg)
  {
    const int nodes_in_brick= getNumExternalNodes();
    if( msg ) ::printf("** %s",msg);
    ::printf("\n XC::Element Number= %d\n", this->getTag() );
    ::printf("\n Number of nodes in a EightNodebrick= %d\n",
                                              nodes_in_brick);
    ::printf("\n Determinant of Jacobian (! ==0 before comp.)= %f\n",
                                                  determinant_of_Jacobian);

    ::printf("Node numbers \n");
    ::printf(
".....1.....2.....3.....4.....5.....6.....7.....8.....9.....0.....1.....2\n");
           for( int i=0 ; i<8 ; i++ )
      //::printf("%6d",G_N_numbs[i]);
      ::printf("%6d",theNodes.getTagNode(i));
    ::printf("\n");
    //           for( int j=8 ; j<20 ; j++ )
    //             ::printf("%6d",G_N_numbs[j]);     // Commented by Xiaoyan
    ::printf("\n\n");

    //    ::printf("Node existence array \n");
    //           for( int k=0 ; k<12 ; k++ )
    //             ::printf("%6d",node_existance[k]);
    //           ::printf("\n\n");          // Commented by Xiaoyan


    int total_number_of_Gauss_points= r_integration_order*
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

     //           for( int jn=8 ; jn<20 ; jn++ )
           //             (nodes[G_N_numbs[jn]]).report("nodes from within element (last 12)\n");
           // Commented by Xiaoyan
      }

    ::printf("\n\nGauss-Legendre integration order\n");
    ::printf("Integration order in r direction= %d\n",r_integration_order);
    ::printf("Integration order in s direction= %d\n",s_integration_order);
    ::printf("Integration order in t direction= %d\n\n",t_integration_order);



    for( int GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        for( int GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            for( int GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                short where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                ::printf("\n\n----------------**************** where= %d \n", where);
                ::printf("                    GP_c_r= %d,  GP_c_s= %d,  GP_c_t= %d\n",
                            GP_c_r,GP_c_s,GP_c_t);
                matpoint[where].report("Material Point\n");
                //GPstress[where].reportshort("stress at Gauss Point");
                //GPstrain[where].reportshort("strain at Gauss Point");
                //matpoint[where].report("Material model  at Gauss Point");
              }
          }
      }

  }


//#############################################################################
void XC::EightNodeBrick::reportshort(char * msg)
  {
    const int nodes_in_brick= getNumExternalNodes();
    if( msg ) ::printf("** %s",msg);
    ::printf("\n XC::Element Number= %d\n", this->getTag() );
    ::printf("\n Number of nodes in a XC::EightNodeBrick= %d\n",
                                              nodes_in_brick);
    ::printf("\n Determinant of Jacobian (! ==0 before comp.)= %f\n",
                                                  determinant_of_Jacobian);

    ::printf("Node numbers \n");
    ::printf(
".....1.....2.....3.....4.....5.....6.....7.....8.....9.....0.....1.....2\n");
           for( int i=0 ; i<8 ; i++ )
             //::printf("%6d",G_N_numbs[i]);
             ::printf( "%6d",theNodes.getTagNode(i) );

     ::printf("\n");
           //           for( int j=8 ; j<20 ; j++ )
           //             ::printf("%6d",G_N_numbs[j]);   //// Commented by Xiaoyan
           ::printf("\n\n");

           //    ::printf("Node existence array \n");
           //           for( int k=0 ; k<12 ; k++ )
           //             ::printf("%6d",node_existance[k]);     // Commented by Xiaoyan
           ::printf("\n\n");

  }




//#############################################################################
void XC::EightNodeBrick::reportPAK(char * msg)
  {
    if( msg ) ::printf("%s",msg);
    ::printf("%10d   ",  this->getTag());
           for( int i=0 ; i<8 ; i++ )
             ::printf( "%6d",theNodes.getTagNode(i) );
             //::printf("%6d",G_N_numbs[i]);

    printf("\n");
  }


//#############################################################################
void XC::EightNodeBrick::reportpqtheta(int GP_numb)
  {
    short where= GP_numb-1;
    matpoint[where].reportpqtheta("");
  }

////#############################################################################
//Vector XC::EightNodeBrick::reportLM(char * msg)
//  {
//    if( msg ) ::printf("%s",msg);
//    ::printf("Element # %d, LM->", this->get_Brick_Number());
//    for(int count= 0 ; count < 24 ; count++)
//      {
//        ::printf(" %d", LM[count]);
//      }
//    ::printf("\n");
//
//  }

//#############################################################################
//Compute gauss point coordinates
void XC::EightNodeBrick::computeGaussPoint()
  {
    //    if( msg ) ::printf("** %s\n",msg);

    // special case for 8 nodes only
    // special case for 8 nodes only
    int count;
    count= FixedOrder*FixedOrder*FixedOrder;
    //Vector Gsc(count*3+1); //+1: number of Gauss point in element
    Gsc8(0)= count;

    double r= 0.0;
    double s= 0.0;
    double t= 0.0;

    short where= 0;

    // special case for 8 nodes only
    static const int dim[]= {3, 8}; // static-> see ARM pp289-290
    static const int dimM[]= {3,  FixedOrder*FixedOrder*FixedOrder}; // found a bug: dimM depends on Order of Gauss Points Joey Yang March 02, 02
    BJtensor NodalCoord(2, dim, 0.0);
    BJtensor matpointCoord(2, dimM, 0.0);
    int h_dim[]= {24,3};   // Xiaoyan changed from {60,3} to {24,3} for 8 nodes
    BJtensor H(2, h_dim, 0.0);

    //for(int ncount= 1 ; ncount <= 8 ; ncount++ )
    ////  for(int ncount= 0 ; ncount <= 7 ; ncount++ )
    //  {
    //  //int global_node_number= get_global_number_of_node(ncount-1);
    //  // printf("global node num %d",global_node_number);
    //
    //    //   NodalCoord.val(1,ncount)= nodes[global_node_number].x_coordinate();
    //    //   NodalCoord.val(2,ncount)= nodes[global_node_number].y_coordinate();
    //    //   NodalCoord.val(3,ncount)= nodes[global_node_number].z_coordinate();
    //    // Xiaoyan changed to the following:  09/27/00
    //  Vector Coordinates= nodes[global_node_number].getCrds();
    //
    //    NodalCoord.val(1,ncount)= Coordinates(0);
    //    NodalCoord.val(2,ncount)= Coordinates(1);
    //    NodalCoord.val(3,ncount)= Coordinates(2);
    //printf("global point %d     x=%+.6e   y=%+.6e   z=%+.6e \n ", global_node_number,
    //                                                      NodalCoord.val(1,ncount),
    //                  NodalCoord.val(2,ncount),
    //                  NodalCoord.val(3,ncount));
    //}

    //Zhaohui using node pointers, which come from the XC::Domain
    const XC::Vector &nd1Crds= theNodes[0]->getCrds();
    const XC::Vector &nd2Crds= theNodes[1]->getCrds();
    const XC::Vector &nd3Crds= theNodes[2]->getCrds();
    const XC::Vector &nd4Crds= theNodes[3]->getCrds();
    const XC::Vector &nd5Crds= theNodes[4]->getCrds();
    const XC::Vector &nd6Crds= theNodes[5]->getCrds();
    const XC::Vector &nd7Crds= theNodes[6]->getCrds();
    const XC::Vector &nd8Crds= theNodes[7]->getCrds();

    NodalCoord.val(1,1)=nd1Crds(0); NodalCoord.val(2,1)=nd1Crds(1); NodalCoord.val(3,1)=nd1Crds(2);
    NodalCoord.val(1,2)=nd2Crds(0); NodalCoord.val(2,2)=nd2Crds(1); NodalCoord.val(3,2)=nd2Crds(2);
    NodalCoord.val(1,3)=nd3Crds(0); NodalCoord.val(2,3)=nd3Crds(1); NodalCoord.val(3,3)=nd3Crds(2);
    NodalCoord.val(1,4)=nd4Crds(0); NodalCoord.val(2,4)=nd4Crds(1); NodalCoord.val(3,4)=nd4Crds(2);
    NodalCoord.val(1,5)=nd5Crds(0); NodalCoord.val(2,5)=nd5Crds(1); NodalCoord.val(3,5)=nd5Crds(2);
    NodalCoord.val(1,6)=nd6Crds(0); NodalCoord.val(2,6)=nd6Crds(1); NodalCoord.val(3,6)=nd6Crds(2);
    NodalCoord.val(1,7)=nd7Crds(0); NodalCoord.val(2,7)=nd7Crds(1); NodalCoord.val(3,7)=nd7Crds(2);
    NodalCoord.val(1,8)=nd8Crds(0); NodalCoord.val(2,8)=nd8Crds(1); NodalCoord.val(3,8)=nd8Crds(2);


    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates

               H= H_3D(r,s,t);

          for(int encount=1 ; encount <= 8 ; encount++ )
                //         for(int encount=0 ; encount <= 7 ; encount++ )
           {
                  //  matpointCoord.val(1,where+1) =+NodalCoord.val(1,where+1) * H.val(encount*3-2,1);
                  //  matpointCoord.val(2,where+1) =+NodalCoord.val(2,where+1) * H.val(encount*3-1,2);
                  //  matpointCoord.val(3,where+1) =+NodalCoord.val(3,where+1) * H.val(encount*3-0,3);
                  matpointCoord.val(1,where+1) +=NodalCoord.val(1,encount) * H.val(encount*3-2,1);
                  //::printf("-- NO nodal, H_val :%d %+.2e %+.2e %+.5e\n", encount,NodalCoord.val(1,encount),H.val(encount*3-2,1),matpointCoord.val(1,where+1) );
                  matpointCoord.val(2,where+1) +=NodalCoord.val(2,encount) * H.val(encount*3-1,2);
                  matpointCoord.val(3,where+1) +=NodalCoord.val(3,encount) * H.val(encount*3-0,3);

      }

               //::printf("gauss point# %d   %+.6e %+.6e %+.6e \n", where+1,
               //                                        matpointCoord.val(1,where+1),
               //                                        matpointCoord.val(2,where+1),
               //                                        matpointCoord.val(3,where+1));

         Gsc8(where*3+1)= matpointCoord.val(1,where+1);
         Gsc8(where*3+2)= matpointCoord.val(2,where+1);
         Gsc8(where*3+3)= matpointCoord.val(3,where+1);

    //matpoint[where].reportTensor("");


              }
          }
      }
      //return Gsc8;
 }


////#############################################################################

//#############################################################################
//void XC::EightNodeBrick::reportTensor(char * msg)
// ZHaohui added to print gauss point coord. to file fp

void XC::EightNodeBrick::reportTensorF(FILE * fp)
  {
    //if( msg ) ::printf("** %s\n",msg);

    // special case for 8 nodes only
    // special case for 8 nodes only
    double r= 0.0;
    double s= 0.0;
    double t= 0.0;

    short where= 0;

    // special case for 8 nodes only
    static const int dim[]= {3, 8}; // static-> see ARM pp289-290
    BJtensor NodalCoord(2, dim, 0.0);
    BJtensor matpointCoord(2, dim, 0.0);
    int h_dim[]= {24,3};  // Xiaoyan changed from {60,3} to {24,3} for 8 nodes

    BJtensor H(2, h_dim, 0.0);

    //for(int ncount= 1 ; ncount <= 8 ; ncount++ )
    //  // for(int ncount= 0 ; ncount <= 7 ; ncount++ )
    //  {
    //  int global_node_number= get_global_number_of_node(ncount-1);
    //  // printf("global node num %d",global_node_number);
    //
    //    //        NodalCoord.val(1,ncount)= nodes[global_node_number].x_coordinate();
    //    //        NodalCoord.val(2,ncount)= nodes[global_node_number].y_coordinate();
    //    //        NodalCoord.val(3,ncount)= nodes[global_node_number].z_coordinate();
    //    // Xiaoyan changed to the following:  09/27/00
    //  Vector Coordinates= nodes[global_node_number].getCrds();
    //    NodalCoord.val(1,ncount)= Coordinates(0);
    //    NodalCoord.val(2,ncount)= Coordinates(1);
    //    NodalCoord.val(3,ncount)= Coordinates(2);
    //printf("global point %d     x=%+.6e   y=%+.6e   z=%+.6e \n ", global_node_number,
    //                                                      NodalCoord.val(1,ncount),
    //                  NodalCoord.val(2,ncount),
    //                  NodalCoord.val(3,ncount));
    //  }

    //Zhaohui using node pointers, which come from the XC::Domain
    const XC::Vector &nd1Crds= theNodes[0]->getCrds();
    const XC::Vector &nd2Crds= theNodes[1]->getCrds();
    const XC::Vector &nd3Crds= theNodes[2]->getCrds();
    const XC::Vector &nd4Crds= theNodes[3]->getCrds();
    const XC::Vector &nd5Crds= theNodes[4]->getCrds();
    const XC::Vector &nd6Crds= theNodes[5]->getCrds();
    const XC::Vector &nd7Crds= theNodes[6]->getCrds();
    const XC::Vector &nd8Crds= theNodes[7]->getCrds();

    NodalCoord.val(1,1)=nd1Crds(0); NodalCoord.val(2,1)=nd1Crds(1); NodalCoord.val(3,1)=nd1Crds(2);
    NodalCoord.val(1,2)=nd2Crds(0); NodalCoord.val(2,2)=nd2Crds(1); NodalCoord.val(3,2)=nd2Crds(2);
    NodalCoord.val(1,3)=nd3Crds(0); NodalCoord.val(2,3)=nd3Crds(1); NodalCoord.val(3,3)=nd3Crds(2);
    NodalCoord.val(1,4)=nd4Crds(0); NodalCoord.val(2,4)=nd4Crds(1); NodalCoord.val(3,4)=nd4Crds(2);
    NodalCoord.val(1,5)=nd5Crds(0); NodalCoord.val(2,5)=nd5Crds(1); NodalCoord.val(3,5)=nd5Crds(2);
    NodalCoord.val(1,6)=nd6Crds(0); NodalCoord.val(2,6)=nd6Crds(1); NodalCoord.val(3,6)=nd6Crds(2);
    NodalCoord.val(1,7)=nd7Crds(0); NodalCoord.val(2,7)=nd7Crds(1); NodalCoord.val(3,7)=nd7Crds(2);
    NodalCoord.val(1,8)=nd8Crds(0); NodalCoord.val(2,8)=nd8Crds(1); NodalCoord.val(3,8)=nd8Crds(2);

    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates

               H= H_3D(r,s,t);

          for(int encount=1 ; encount <= 8 ; encount++ )
                //         for(int encount=0 ; encount <= 7 ; encount++ )
         {
                  //  matpointCoord.val(1,where+1) =+NodalCoord.val(1,where+1) * H.val(encount*3-2,1);
                  //  matpointCoord.val(2,where+1) =+NodalCoord.val(2,where+1) * H.val(encount*3-1,2);
                  //  matpointCoord.val(3,where+1) =+NodalCoord.val(3,where+1) * H.val(encount*3-0,3);
                  matpointCoord.val(1,where+1) +=NodalCoord.val(1,encount) * H.val(encount*3-2,1);
                  //::printf("-- NO nodal, H_val :%d %+.2e %+.2e %+.5e\n", encount,NodalCoord.val(1,encount),H.val(encount*3-2,1),matpointCoord.val(1,where+1) );
                  matpointCoord.val(2,where+1) +=NodalCoord.val(2,encount) * H.val(encount*3-1,2);
                  matpointCoord.val(3,where+1) +=NodalCoord.val(3,encount) * H.val(encount*3-0,3);

         }

    fprintf(fp, "gauss point# %d   %+.6e %+.6e %+.6e \n", where+1,
                                                          matpointCoord.val(1,where+1),
                                                          matpointCoord.val(2,where+1),
                                                          matpointCoord.val(3,where+1));

    //matpoint[where].reportTensor("");


              }
          }
      }

 }



//=============================================================================
int XC::EightNodeBrick::getNumDOF(void) const
  {
    return 24;       //Changed from 2*4=8 to 3*8=24 Xiaoyan 07/06/00
  }

//=============================================================================
void XC::EightNodeBrick::setDomain (Domain *theDomain)
  {
    ElementBase<8>::setDomain(theDomain);
    theNodes.checkNumDOF(3,getTag());
  }

//=============================================================================
int XC::EightNodeBrick::commitState(void)
{

//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in int XC::EightNodeBrick::commitState ()" <<std::endl; this->Print(std::cerr);
//BJ//BJ

    int retVal= 0;


    // call element commitState to do any base class stuff
    if((retVal= this->XC::Element::commitState()) != 0) {
      std::cerr << "XC::EightNodeBrick::commitState () - failed in base class";
    }

    // int order= theQuadRule->getOrder();     // Commented by Xiaoyan

    int i;
    //int plastify= 0;
    //int j, k;      // Xiaoyan added k for three dimension


    // Loop over the integration points and commit the material states
    int count= r_integration_order* s_integration_order * t_integration_order;
    //for(i= 0; i < r_integration_order; i++)        // Xiaoyan changed order to
    //  for(j= 0; j < s_integration_order; j++)      // r_integration_order,
    //                  // s_integration_order, and
    //      for(k= 0; k < t_integration_order; k++)      // added t_integration_order,
    //         retVal += (GaussPtheMaterial[i][j][k]).commitState();

    Vector pp= getResistingForce();

    //if( this->getTag() == 1 || this->getTag() == 700)
    //{
      for(i= 0; i < count; i++)
      //for(i= 0; i < 0; i++)
        {
          retVal += matpoint[i].commitState();
          //if(i == 4 && strcmp(matpoint[i].matmodel->getType(),"Template3Dep") == 0)
//          stresstensor st;
//            stresstensor prin;
//            straintensor stn;
//            straintensor pl_stn;
//            straintensor stnprin;
//
//           //checking if element plastified
//           pl_stn= matpoint[i].getPlasticStrainTensor();
//
//           double  p_plastc= pl_stn.p_hydrostatic();
//           if(  fabs(p_plastc) > 0 )
//             {
//               plastify= 1;
//               break;
//             }
//
//           st= matpoint[i].getStressTensor();
//           prin= st.principal();
//           stn= matpoint[i].getStrainTensor();
//           stnprin= stn.principal();
//
//  //   std::cerr << "\nGauss Point: " << i << std::endl;
//  //   std::cerr << "sigma11: "<< st.cval(1, 1) << " "<< st.cval(1, 2) << " " << st.cval(1, 3) << std::endl;
//  //   std::cerr << "sigma21: "<< st.cval(2, 1) << " "<< st.cval(2, 2) << " " << st.cval(2, 3) << std::endl;
//  //    std::cerr << "sigma31: "<< st.cval(3, 1) << " "<< st.cval(3, 2) << " " << st.cval(3, 3) << std::endl << std::endl;
//
//     //std::cerr << "strain11: "<< stn.cval(1, 1) << " "<< stn.cval(1, 2) << " " << stn.cval(1, 3) << std::endl;
//     //std::cerr << "strain21: "<< stn.cval(2, 1) << " "<< stn.cval(2, 2) << " " << stn.cval(2, 3) << std::endl;
//      //std::cerr << "strain31: "<< stn.cval(3, 1) << " "<< stn.cval(3, 2) << " " << stn.cval(3, 3) << std::endl;
//
//     //double  p= -1*( prin.cval(1, 1)+ prin.cval(2, 2) +prin.cval(3, 3) )/3.0;
//     //double  ev= -1*( stnprin.cval(1, 1)+ stnprin.cval(2, 2) + stnprin.cval(3, 3) )/3.0;
//     //std::cerr << "   " << p;
//
//     //if(p < 0)
//     //  std::cerr  << "gs pnt:" << i << "  p="<< p;
//
//
//     double q;
//     //if( fabs(prin.cval(1, 1) - prin.cval(2, 2) ) <=  0.0001 )
//           if( fabs(prin.cval(1, 1) - prin.cval(2, 2) ) <=  0.001 )
//           {
//               q= prin.cval(1, 1) - prin.cval(3, 3);
//               //std::cerr << "1= 2";
//           }
//           else
//               q= prin.cval(3, 3) - prin.cval(1, 1);
//
//     //Triaxial compr.  fabs
//           //std::cerr << "     " << st.cval(2, 3); //tau_yz
//     //std::cerr << "     " << q;
//     ////----std::cerr << "     " << fabs(q);
//
//           //std::cerr << "     " << ev << std::endl;
//
//  //out22Jan2001   if(strcmp(matpoint[i].matmodel->getType(),"Template3Dep") == 0)
//  //out22Jan2001          {
//  //out22Jan2001           st= ( ((Template3Dep *)(matpoint[i].matmodel))->getEPS())->getStress();
//  //out22Jan2001           prin= st.principal();
//  //out22Jan2001    }
//  //out22Jan2001    else
//  //out22Jan2001    {
//  //out22Jan2001            st= matpoint[i].getStressTensor();
//  //out22Jan2001           prin= st.principal();
//  //out22Jan2001
//  //out22Jan2001    }
//
//      //double  p= st.p_hydrostatic();
//      //double  p= -1*( prin.cval(1, 1)+ prin.cval(2, 2) +prin.cval(3, 3) )/3.0;
//            //std::cerr << "\n " << prin.cval(1, 1) << "   " << prin.cval(2, 2) << "  " <<  prin.cval(3, 3) << std::endl;
//            //if( getTag() == 960)
//            //std::cerr << " El= " << getTag() << " , p    " << p << std::endl;
//
//      //printf(stderr, " Gauss Point i= %d ", (i+1));
//      //printf(stderr, " Gauss Point i= %d ", (i+1));
//
//
//            //if( p < 0 )
//      //{
//      //  std::cerr << getTag();
//      //  std::cerr << " ***p=    " << p << std::endl;
//      //}
//            //J2D
//            //std::cerr << "        " << st.q_deviatoric();
//
//            //double q;
//            //if( fabs(prin.cval(1, 1) - prin.cval(2, 2) ) <=  0.0001 )
//            //{
//            //    q= prin.cval(1, 1) - prin.cval(3, 3);
//            //    //std::cerr << "1= 2";
//            //}
//            //else
//            //    q= prin.cval(3, 3) - prin.cval(1, 1);
//
//            //Triaxial compr.
//            //std::cerr << "        " << q;
         }
      //}

      //std::cerr << this->getTag() << " " << plastify << std::endl;


      //std::cerr << " at elements " << this->getTag() << std::endl;


      //output nodal force
      //std::cerr << "    " << pp(2) << std::endl;
    //}
    return retVal;
}

//=============================================================================
int XC::EightNodeBrick::revertToLastCommit ()
{
  //  int order= theQuadRule->getOrder();  // Commented by Xiaoyan
    int i;
    //int j, k;     // Xiaoyan added k for three dimension
    int retVal= 0;

    // Loop over the integration points and revert to last committed material states
    int count= r_integration_order* s_integration_order * t_integration_order;
    //for(i= 0; i < r_integration_order; i++)       // Xiaoyan changed order to
    //  for(j= 0; j < s_integration_order; j++)     // r_integration_order,
    //      for(k= 0; k < t_integration_order; k++)     // s_integration_order, and
                       // added t_integration_order,
      //retVal += (theMaterial[i][j][k]).revertToLastCommit();

    for(i= 0; i < count; i++)
       retVal += matpoint[i].revertToLastCommit();


    return retVal;
}

//=============================================================================
int XC::EightNodeBrick::revertToStart ()
{
    int i;     // Xiaoyan added k for three dimension
    int retVal= 0;

    // Loop over the integration points and revert to last committed material states
    //for(i= 0; i < r_integration_order; i++)       // Xiaoyan changed order to
    //  for(j= 0; j < s_integration_order; j++)     // r_integration_order,
    //      for(k= 0; k < t_integration_order; k++)     // s_integration_order, and
                 // added t_integration_order,
    //      retVal += (theMaterial[i][j][k]).revertToLastCommit();

    int count= r_integration_order* s_integration_order * t_integration_order;

    for(i= 0; i < count; i++)
       retVal += matpoint[i].revertToStart();


    return retVal;

    // Loop over the integration points and revert to initial material states
   }


//=============================================================================
const XC::Matrix &XC::EightNodeBrick::getTangentStiff(void) const
{
//BJ/BJ
//BJ    std::cerr << "\n\n\n\n Print in const XC::Matrix &XC::EightNodeBrick::getTangentStiff ()" <<std::endl; this->Print(std::cerr);
//BJ//BJ



     BJtensor stifftensor= getStiffnessTensor();
     int Ki=0;
     int Kj=0;

     for( int i=1 ; i<=8 ; i++ )
     {
        for( int j=1 ; j<=8 ; j++ )
        {
           for( int k=1 ; k<=3 ; k++ )
           {
              for( int l=1 ; l<=3 ; l++ )
              {
                 Ki= k+3*(i-1);
                 Kj= l+3*(j-1);
                 K( Ki-1 , Kj-1 )= stifftensor.cval(i,k,l,j);
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
const XC::Matrix &XC::EightNodeBrick::getInitialStiff(void) const
  {
//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in const XC::Matrix &XC::EightNodeBrick::getInitialStiff ()" <<std::endl; this->Print(std::cerr);
//BJ//BJ


    if(!Ki)
      Ki= new Matrix(this->getTangentStiff());

    if(Ki == 0)
      {
        std::cerr << "FATAL XC::fElement::getInitialStiff() -";
        std::cerr << "ran out of memory\n";
        exit(-1);
      }
    if(isDead())
      (*Ki)*=dead_srf;
    return *Ki;
  }


//=============================================================================
//Get lumped mass
//const XC::Matrix &XC::EightNodeBrick::getMass ()
const XC::Matrix &XC::EightNodeBrick::getConsMass ()
{
     BJtensor masstensor= getMassTensor();
     //int Ki=0;
     //int Kj=0;

     //double tot_mass= 0.0;
     //double diag_mass= 0.0;
     double column_mass;

     for( int i=1 ; i<=24 ; i++ )
     {
        column_mass= 0.0;
  for( int j=1 ; j<=24 ; j++ )
        {

     //M( i-1 , j-1 )= masstensor.cval(i,j);

     column_mass += masstensor.cval(i,j);
     M( i-1 , j-1 )= 0;
     //tot_mass += M( i-1 , j-1 );
     //if(i == j)
     //   diag_mass += M( i-1 , j-1 );
        }
  M( i-1 , i-1 )= column_mass;

     }

     //std::cerr << " tot_mass= "<< tot_mass << " column_mass =" << column_mass << " diag_mass= " <<  diag_mass << std::endl;
     //std::cerr << "" << M.Output(std::cerr);
     //std::cerr << " M " << M;

    return M;
  }

//=============================================================================
//Get consistent mass
//const XC::Matrix &XC::EightNodeBrick::getConsMass ()
const XC::Matrix &XC::EightNodeBrick::getMass(void) const
{
     BJtensor masstensor= getMassTensor();
     //int Ki=0;
     //int Kj=0;

     //double tot_mass= 0.0;
     //double diag_mass= 0.0;
     //double column_mass;

     for( int i=1 ; i<=24 ; i++ )
     {
        //column_mass= 0.0;
  for( int j=1 ; j<=24 ; j++ )
        {
     M( i-1 , j-1 )= masstensor.cval(i,j);

     //column_mass += masstensor.cval(i,j);
     //M( i-1 , j-1 )= 0;
     //tot_mass += M( i-1 , j-1 );
     //if(i == j)
     //   diag_mass += M( i-1 , j-1 );
        }
  //M( i-1 , i-1 )= column_mass;

     }

     //std::cerr << " tot_mass= "<< tot_mass << " column_mass =" << column_mass << " diag_mass= " <<  diag_mass << std::endl;
     //std::cerr << "" << M.Output(std::cerr);
     //std::cerr << " M " << M;

    if(isDead())
      M*=dead_srf;
    return M;
  }

//=============================================================================
int XC::EightNodeBrick::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    //BJ//BJ
    //BJ    std::cerr << "\n\n\n\n Print in int XC::EightNodeBrick::addLoad(ElementalLoad *theLoad, double loadFactor)" <<std::endl; this->Print(std::cerr);
    //BJ//BJ


    //std::cerr << "XC::EightNodeBrick::addLoad - load type unknown for ele with tag: %d\n",
    //        this->getTag());

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
            Vector bforce(24);
            // Check for a quick return
            //std::cerr << "rho " << rho << std::endl;
            if(rho == 0.0)
              return 0;

            Vector baNodes(24), bfx(3);
            bfx(0)= bf(0) * loadFactor;
            bfx(1)= bf(1) * loadFactor;
            bfx(2)= bf(2) * loadFactor;

            baNodes(0)=  bfx(0);
            baNodes(1)=  bfx(1);
            baNodes(2)=  bfx(2);
            baNodes(3)=  bfx(0);
            baNodes(4)=  bfx(1);
            baNodes(5)=  bfx(2);
            baNodes(6)=  bfx(0);
            baNodes(7)=  bfx(1);
            baNodes(8)=  bfx(2);
            baNodes(9)=  bfx(0);
            baNodes(10)= bfx(1);
            baNodes(11)= bfx(2);
            baNodes(12)= bfx(0);
            baNodes(13)= bfx(1);
            baNodes(14)= bfx(2);
            baNodes(15)= bfx(0);
            baNodes(16)= bfx(1);
            baNodes(17)= bfx(2);
            baNodes(18)= bfx(0);
            baNodes(19)= bfx(1);
            baNodes(20)= bfx(2);
            baNodes(21)= bfx(0);
            baNodes(22)= bfx(1);
            baNodes(23)= bfx(2);

            //Form equivalent body force
            this->getMass();
            bforce.addMatrixVector(0.0, M, baNodes, 1.0);
            load.addVector(1.0, bforce, 1.0);
          }
        else
          {
            std::cerr << "XC::EightNodeBrick::addLoad() - 8NodeBrick " << this->getTag() << " unknown\n";
            return -1;
          }
      }
    return 0;
  }


//=============================================================================
int XC::EightNodeBrick::addInertiaLoadToUnbalance(const XC::Vector &accel)
{
  // Check for a quick return
  if(rho == 0.0)
    return 0;

  // Get R * accel from the nodes
  const XC::Vector &Raccel1= theNodes[0]->getRV(accel);
  const XC::Vector &Raccel2= theNodes[1]->getRV(accel);
  const XC::Vector &Raccel3= theNodes[2]->getRV(accel);
  const XC::Vector &Raccel4= theNodes[3]->getRV(accel);
        // Xiaoyan added the following four 09/27/00
  const XC::Vector &Raccel5= theNodes[4]->getRV(accel);
  const XC::Vector &Raccel6= theNodes[5]->getRV(accel);
  const XC::Vector &Raccel7= theNodes[6]->getRV(accel);
  const XC::Vector &Raccel8= theNodes[7]->getRV(accel);

    if(3 != Raccel1.Size() || 3 != Raccel2.Size() || 3 != Raccel3.Size() ||
         3 != Raccel4.Size() || 3 != Raccel5.Size() || 3 != Raccel6.Size() ||
  3 != Raccel7.Size() || 3 != Raccel8.Size()  ){
  // Xiaoyan changed 2 to 3 and added Racce15-18  09/27/00
      std::cerr << "XC::EightNodeBrick::addInertiaLoadToUnbalance matrix and vector sizes are incompatible\n";
      return -1;
    }

  static XC::Vector ra(24);  // Changed form 8 to 24(3*8)  Xiaoyan 09/27/00

  ra(0)=  Raccel1(0);
  ra(1)=  Raccel1(1);
  ra(2)=  Raccel1(2);
  ra(3)=  Raccel2(0);
  ra(4)=  Raccel2(1);
  ra(5)=  Raccel2(2);
  ra(6)=  Raccel3(0);
  ra(7)=  Raccel3(1);
  ra(8)=  Raccel3(2);
  ra(9)=  Raccel4(0);
  ra(10)= Raccel4(1);
  ra(11)= Raccel4(2);
      // Xiaoyan added the following 09/27/00
      ra(12)= Raccel5(0);
  ra(13)= Raccel5(1);
  ra(14)= Raccel5(2);
  ra(15)= Raccel6(0);
  ra(16)= Raccel6(1);
  ra(17)= Raccel6(2);
  ra(18)= Raccel7(0);
  ra(19)= Raccel7(1);
  ra(20)= Raccel7(2);
  ra(21)= Raccel8(0);
  ra(22)= Raccel8(1);
  ra(23)= Raccel8(2);


    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    // Mass matrix is computed in setDomain()

    //double column_mass= 0;
    //for(int i= 0; i < 24; i++)
    //   column_mass += M(1,i);
    //column_mass= column_mass/3.0;

    //std::cerr << " addInerti... column_mass " << column_mass << std::endl;

    //for(int i= 0; i < 24; i++)
    //    load(i) += -M(i,i)*ra(i);

    load.addMatrixVector(1.0, M, ra, -1.0);
    return 0;
}

//=============================================================================
const XC::Vector XC::EightNodeBrick::FormEquiBodyForce(void)
  {

//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in const XC::Vector XC::EightNodeBrick::FormEquiBodyForce(void)" <<std::endl; this->Print(std::cerr);
//BJ//BJ


    Vector bforce(24);

    // Check for a quick return
    //std::cerr << "rho " << rho << std::endl;
    if(rho == 0.0)
      return bforce;

    Vector baNodes(24);

    baNodes(0)=  bf(0);
    baNodes(1)=  bf(1);
    baNodes(2)=  bf(2);
    baNodes(3)=  bf(0);
    baNodes(4)=  bf(1);
    baNodes(5)=  bf(2);
    baNodes(6)=  bf(0);
    baNodes(7)=  bf(1);
    baNodes(8)=  bf(2);
    baNodes(9)=  bf(0);
    baNodes(10)= bf(1);
    baNodes(11)= bf(2);
    baNodes(12)= bf(0);
    baNodes(13)= bf(1);
    baNodes(14)= bf(2);
    baNodes(15)= bf(0);
    baNodes(16)= bf(1);
    baNodes(17)= bf(2);
    baNodes(18)= bf(0);
    baNodes(19)= bf(1);
    baNodes(20)= bf(2);
    baNodes(21)= bf(0);
    baNodes(22)= bf(1);
    baNodes(23)= bf(2);

    //Form equivalent body force
    bforce.addMatrixVector(0.0, M, baNodes, 1.0);
    //std::cerr << " baNodes " << baNodes;
    //std::cerr << " M " << M;
    //if(getTag() == 886)
    //std::cerr << " @@@@@ FormEquiBodyForce  " << bforce;

    return bforce;
  }


//=============================================================================
const XC::Vector &XC::EightNodeBrick::getResistingForce(void) const
  {
//BJ//BJ
//BJ    std::cerr << "\n\n\n\n Print in const XC::Vector &XC::EightNodeBrick::getResistingForce ()" <<std::endl; this->Print(std::cerr);
//BJ//BJ

    int force_dim[]= {8,3};
    BJtensor nodalforces(2,force_dim,0.0);

    nodalforces= nodal_forces();

    //converting nodalforce XC::BJtensor to vector
    for(int i= 0; i< 8; i++)
      for(int j= 0; j < 3; j++)
          P(i *3 + j)= nodalforces.cval(i+1, j+1);

    //std::cerr << "P" << P << '\n';
    //std::cerr << "load" << load << '\n';

    //P= P - load;
    P.addVector(1.0, load, -1.0);

    //std::cerr << "P-load" << P;
    if(isDead())
      P*=dead_srf;
    return P;
  }

//=============================================================================
const XC::Vector &XC::EightNodeBrick::getResistingForceIncInertia(void) const
  {
    // form resisting force
    this->getResistingForce();

    //
    // now add dynamic terms
    // P += M * a + C * v
    //

    if(rho != 0.0)
      {
	// form the mass matrix
	this->getMass();

	const Vector &accel1= theNodes[0]->getTrialAccel();
	const Vector &accel2= theNodes[1]->getTrialAccel();
	const Vector &accel3= theNodes[2]->getTrialAccel();
	const Vector &accel4= theNodes[3]->getTrialAccel();
	const Vector &accel5= theNodes[4]->getTrialAccel();
	const Vector &accel6= theNodes[5]->getTrialAccel();
	const Vector &accel7= theNodes[6]->getTrialAccel();
	const Vector &accel8= theNodes[7]->getTrialAccel();

	static Vector a(24);  // originally 8

	a(0)=  accel1(0);
	a(1)=  accel1(1);
	a(2)=  accel1(2);
	a(3)=  accel2(0);
	a(4)=  accel2(1);
	a(5)=  accel2(2);
	a(6)=  accel3(0);
	a(7)=  accel3(1);
	a(8)=  accel3(2);
	a(9)=  accel4(0);
	a(10)= accel4(1);
	a(11)= accel4(2);
	// Xiaoyn added the following 09/27/00
	a(12)= accel5(0);
	a(13)= accel5(1);
	a(14)= accel5(2);
	a(15)= accel6(0);
	a(16)= accel6(1);
	a(17)= accel6(2);
	a(18)= accel7(0);
	a(19)= accel7(1);
	a(20)= accel7(2);
	a(21)= accel8(0);
	a(22)= accel8(1);
	a(23)= accel8(2);

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
int XC::EightNodeBrick::sendSelf(Communicator &comm)
{
     // Not implemtented yet
     return 0;
}

//=============================================================================
int XC::EightNodeBrick::recvSelf(const Communicator &comm)
{
     // Not implemtented yet
     return 0;
}


//=============================================================================
void XC::EightNodeBrick::Print(std::ostream &s, int flag) const
{
  if(flag == 1) {

    s << "EightNodeBrick, element id:  " << this->getTag() << std::endl;
    s << "Connected external nodes:  " << theNodes;
    s << this->getResistingForce();

  } else {

    //report(" EightNodeBrick ");
    s << "EightNodeBrick, element id:  " << this->getTag() << std::endl;
    s << "Connected external nodes:  " << theNodes;

    int total_number_of_Gauss_points= r_integration_order*
                                       s_integration_order*
                                       t_integration_order;
    if( total_number_of_Gauss_points != 0 )
      {
     theNodes[0]->Print(s);
     theNodes[1]->Print(s);
     theNodes[2]->Print(s);
     theNodes[3]->Print(s);
     theNodes[4]->Print(s);
     theNodes[5]->Print(s);
           theNodes[6]->Print(s);
     theNodes[7]->Print(s);
    }
    s << "Element mass density:  " << rho << std::endl << std::endl;
    s << "Material model:  " << std::endl;

    for( int GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
    {
      for( int GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
      {
        for( int GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
        {
           // this short routine is supposed to calculate position of
           // Gauss point from 3D array of short's
           short where =
           ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

           s << "\n inside Gauss-Legendre loop: where= " << where << std::endl;
           s << " GP_c_r= " << GP_c_r << "GP_c_s= " << GP_c_s << " GP_c_t= " << GP_c_t << std::endl;
           matpoint[where].report("Material Point\n");
           //GPstress[where].reportshort("stress at Gauss Point");
           //GPstrain[where].reportshort("strain at Gauss Point");
           //matpoint[where].report("Material model  at Gauss Point");
        }
      }
    }
  }
}

//=============================================================================
 XC::Response * XC::EightNodeBrick::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
{
    //========================================================
    if(argv[0] == "force" || argv[0] == "forces")
    return new ElementResponse(this, 1, P);

    //========================================================
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
        return new ElementResponse(this, 2, K);

    //========================================================
    else if(argv[0] == "plasticGPC" || argv[0] == "plastifiedGPC")
    {
       //checking if element plastified
       //int count= r_integration_order* s_integration_order * t_integration_order;
       //straintensor pl_stn;
       //int plastify= 0;
       //
       //for(int i= 0; i < count; i++) {
       //  pl_stn= matpoint[i].getPlasticStrainTensor();
       //   double  p_plastc= pl_stn.p_hydrostatic();
       //
       //   if(  fabs(p_plastc) > 0 ) {
       //      plastify= 1;
       //      break;
       //   }
       //}

       return new ElementResponse(this, 3, InfoP);
    }
    //========================================================
    else if(argv[0] == "plastic" || argv[0] == "plastified")
    {
       return new ElementResponse(this, 31, InfoP1);
    }
    //========================================================
    else if(argv[0] == "stress" || argv[0] == "stresses")
    {
       return new ElementResponse(this, 4, InfoS);
    }
    //========================================================
    else if(argv[0] == "pq" || argv[0] == "PQ")
    {
       return new ElementResponse(this, 41, InfoSpq);
    }
    //Added 06-27-02 for p-q
    //========================================================
    else if(argv[0] == "stresspq" || argv[0] == "stressespq")
    {
       return new ElementResponse(this, 41, InfoSpq);
    }
    //Added 07-22-02 for all p-q, e_v_pl, xi
    //========================================================
    else if(argv[0] == "pqall" )
    {
       return new ElementResponse(this, 42, InfoSpq_all);
    }

    //========================================================
    else if(argv[0] == "gausspoint" || argv[0] == "GaussPoint")
    {
       return new ElementResponse(this, 5, Gsc8);
    }

    //========================================================
    /*else if(argv[0] == "material" || argv[0] == "integrPoint") {
      int pointNum= atoi(argv[1]);
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
int XC::EightNodeBrick::getResponse (int responseID, Information &eleInfo)
{
       switch (responseID)
        {

       case 1:
         return eleInfo.setVector(this->getResistingForce());

       case 2:
         return eleInfo.setMatrix(this->getTangentStiff());
       case 3:
               {
    //checking if element plastified
                 int count= r_integration_order* s_integration_order * t_integration_order;
    //std::cerr << count << std::endl;
                 //Vector Gsc(FixedOrder*FixedOrder*FixedOrder*3+1);  // 8*3 + count
    computeGaussPoint();
    //std::cerr << count << std::endl;

    //Vector Info(109); // count * 4 +1
    InfoP(0)= Gsc8(0); //Number of Gauss point

                 straintensor pl_stn;

    int plastify;
                 for(int i= 0; i < count; i++) {
                   plastify= 0;
         InfoP(i*4+1)= Gsc8(i*3+1); //x
         InfoP(i*4+2)= Gsc8(i*3+2); //y
         InfoP(i*4+3)= Gsc8(i*3+3); //z
                   pl_stn= matpoint[i].getPlasticStrainTensor();
                   //double  p_plastc= pl_stn.p_hydrostatic();
                   double  q_plastc= pl_stn.q_deviatoric();

         InfoP(i*4+4)= q_plastc; //plastify; //Plastified?

                 }
       return eleInfo.setVector( InfoP );
    //return plastify;

        }

     case 4:
        {
                 int count= r_integration_order* s_integration_order * t_integration_order;
    int i;
                stresstensor sts;
                 //Vector Gsc(81+1);  // 8*3 + count
    //Gsc= this->reportTensor("Gauss Point Coor.");

    //Vector Info(109 + 3 ); //Z values, x-disp. and corresponding avg. moment
    InfoS(0)= count;

                for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
                {
                    //r= get_Gauss_p_c( r_integration_order, GP_c_r );
                    for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
                    {
                        //s= get_Gauss_p_c( s_integration_order, GP_c_s );
                        //rs= (GP_c_r-1)*s_integration_order+GP_c_s-1;

      for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
                        {
              //for(int i= 0; i < count; i++)
                          i =
                             ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                          sts= matpoint[i].getStressTensor();
           InfoS(i*6+1)= sts.cval(1,1); //sigma_xx
           InfoS(i*6+2)= sts.cval(2,2); //sigma_yy
           InfoS(i*6+3)= sts.cval(3,3); //sigma_zz
           InfoS(i*6+4)= sts.cval(1,2); //Assign sigma_xy
           InfoS(i*6+5)= sts.cval(1,3); //Assign sigma_xz
           InfoS(i*6+6)= sts.cval(2,3); //Assign sigma_yz
            }
        }
    }
        return eleInfo.setVector( InfoS );
        }
     case 41:
        {
                 int count= r_integration_order* s_integration_order * t_integration_order;
    count= count / 2;
                stresstensor sts= matpoint[count].getStressTensor();
    InfoSpq(0) =sts.p_hydrostatic();
    InfoSpq(1) =sts.q_deviatoric();
        return eleInfo.setVector( InfoSpq );
        }
            case 42:
        {
          //int count= r_integration_order* s_integration_order * t_integration_order;
           int i;
                stresstensor sts, principle;

    //InfoSpq_all(0)= count;

                for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
                {
                    //r= get_Gauss_p_c( r_integration_order, GP_c_r );
                    for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
                    {
                        //s= get_Gauss_p_c( s_integration_order, GP_c_s );
                        //rs= (GP_c_r-1)*s_integration_order+GP_c_s-1;

      for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
                        {
              //for(int i= 0; i < count; i++)
                          i =
                             ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                          sts= matpoint[i].getStressTensor();
              InfoSpq_all(i*2+0) =sts.p_hydrostatic();
              //deviatoric stress sqrt(J2/3)
        InfoSpq_all(i*2+1) =sts.q_deviatoric();
        //deviator stress +/-
        principle= sts.principal();
              //InfoSpq_all(i*2+1) =principle.val(1,1)-principle.val(3,3);

                           if(i == 7)  {
          InfoSpq_all(i*2+2)= principle.val(1,1);
          //InfoSpq_all(i*2+3)= principle.val(2,2);
          InfoSpq_all(i*2+3)= principle.val(3,3);

          //Output volumetric strain for the eight Gauss point
          straintensor pl_stn;
          pl_stn= matpoint[i].getPlasticStrainTensor();
          //pl_stn= matpoint[i].getStrainTensor();
          InfoSpq_all(i*2+4)= pl_stn.Iinvariant1();
          double psi= matpoint[i].getpsi();
          InfoSpq_all(i*2+5)= psi;
        }
            }
        }
    }
        return eleInfo.setVector( InfoSpq_all );
        }
     case 5:
     {
    this->computeGaussPoint();
       return eleInfo.setVector(Gsc8);
     }

     case 31:
               {
    // Output element plastic info
                 int count= r_integration_order* s_integration_order * t_integration_order;

    InfoP1(0)= count; //Number of Gauss point

                 straintensor pl_stn;

                 for(int i= 0; i < count; i++) {
                   pl_stn= matpoint[i].getPlasticStrainTensor();
                   //double  p_plastc= pl_stn.p_hydrostatic();
                   double  q_plastc= pl_stn.q_deviatoric();

         InfoP1(i+1)= q_plastc; //plastify; //Plastified?

                 }
       return eleInfo.setVector( InfoP1 );

     }
     default:
       return -1;
  }
     //return ;
}




//=============================================================================

//const XC::Matrix&
//XC::EightNodeBrick::getTangentStiff ()
//{
//  int order= theQuadRule->getOrder();
//  const Vector &intPt= theQuadRule->getIntegrPointCoords();
//  const Vector &intWt= theQuadRule->getIntegrPointWeights();
//
//  const Vector &disp1= theNodes[0]->getTrialDisp();
//        const Vector &disp2= theNodes[1]->getTrialDisp();
//  const Vector &disp3= theNodes[2]->getTrialDisp();
//        const Vector &disp4= theNodes[3]->getTrialDisp();
//       // Xiaoyan added 5-8 07/06/00
//        const Vector &disp5= theNodes[4]->getTrialDisp();
//        const Vector &disp6= theNodes[5]->getTrialDisp();
//  const Vector &disp7= theNodes[6]->getTrialDisp();
//        const Vector &disp8= theNodes[7]->getTrialDisp();
//
//  static XC::Vector u(24);      //Changed from u(8) to u(24) Xiaoyn 07/06/00
//
//  u(0)= disp1(0);
//  u(1)= disp1(1);
//        u(2)= disp1(2);
//  u(3)= disp2(0);
//  u(4)= disp2(1);
//  u(5)= disp2(2);
//        u(6)= disp3(0);
//  u(7)= disp3(1);
//  u(8)= disp3(2);
//  u(9)= disp4(0);
//  u(10)= disp4(1);
//  u(11)= disp4(2);
//  u(12)= disp5(0);
//  u(13)= disp5(1);
//  u(14)= disp5(2);
//  u(15)= disp6(0);
//  u(16)= disp6(1);
//  u(17)= disp6(2);
//  u(18)= disp7(0);
//  u(19)= disp7(1);
//  u(20)= disp7(2);
//  u(21)= disp8(0);
//  u(22)= disp8(1);
//  u(23)= disp8(2);


//  static XC::Vector eps (6);      // Changed eps(3) to eps(6) Xiaoyan 07/06/00

//  K.Zero();

//  // Loop over the integration points
//  for(int i= 0; i < order; i++)
//  {
//    for(int j= 0; j < order; j++)
//    {
//
//      // Determine Jacobian for this integration point
//      this->setJacobian (intPt(i), intPt(j));
//
//      // Interpolate strains
//      this->formBMatrix (intPt(i), intPt(j));
//      eps= B*u;
//
//      // Set the material strain
//      (theMaterial[i][j])->setTrialStrain (eps);
//
//      // Get the material tangent
//      const XC::Matrix &D= (theMaterial[i][j])->getTangent();
//
//      // Form the Jacobian of the coordinate transformation
//      double detJ= this->formDetJ (intPt(i), intPt(j));
//
//      // Perform numerical integration
//      K= K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
//    }
//  }
//
//  K= K * thickness;
//
//  return K;
//}

//const XC::Matrix&
//XC::EightNodeBrick::getSecantStiff ()
//{
//  return K;
//}

//Commented by Xiaoyan     Use the form like Brick3d
//const XC::Matrix & XC::EightNodeBrick::getDamp ()
//{
//  return C;
//}
// Commented by Xiaoyan 08/04/00

//const XC::Matrix&
//XC::EightNodeBrick::getMass ()
//{
//  int order= theQuadRule->getOrder();
//  const Vector &intPt= theQuadRule->getIntegrPointCoords();
//  const Vector &intWt= theQuadRule->getIntegrPointWeights();
//
//  M.Zero();
//
//  int i, j;
//
//  // Loop over the integration points
//  for(i= 0; i < order; i++)
//  {
//    for(j= 0; j < order; j++)
//    {
//      // Determine Jacobian for this integration point
//      this->setJacobian (intPt(i), intPt(j));
//
//      // Interpolate strains
//      this->formNMatrix (intPt(i), intPt(j));
//
//      // Form the Jacobian of the coordinate transformation
//      double detJ= this->formDetJ (intPt(i), intPt(j));
//
//      // Perform numerical integration
//      M= M + (N^ N) * intWt(i)*intWt(j) * detJ;
//    }
//  }
//
//  M= M * thickness * rho;
//
//  // Lumped mass ... can be optional
//  for(i= 0; i < 24; i++)       // Changed 8 to 24  Xiaoyan 07/06/00
//  {
//    double sum= 0.0;
//    for(j= 0; j < 24; j++)    // Changed 8 to 24  Xiaoyan 07/06/00
//    {
//      sum += M(i,j);
//      M(i,j)= 0.0;
//    }
//    M(i,i)= sum;
//  }
//
//  return M;
//}
//
//const Vector&
//XC::EightNodeBrick::getResistingForce ()
//{
//  int order= theQuadRule->getOrder();
//  const Vector &intPt= theQuadRule->getIntegrPointCoords();
//  const Vector &intWt= theQuadRule->getIntegrPointWeights();
//
//  const Vector &disp1= theNodes[0]->getTrialDisp();
//        const Vector &disp2= theNodes[1]->getTrialDisp();
//  const Vector &disp3= theNodes[2]->getTrialDisp();
//        const Vector &disp4= theNodes[3]->getTrialDisp();
//  //6-8 added by Xiaoyan 07/06/00
//  const Vector &disp5= theNodes[4]->getTrialDisp();
//        const Vector &disp6= theNodes[5]->getTrialDisp();
//  const Vector &disp7= theNodes[6]->getTrialDisp();
//        const Vector &disp8= theNodes[7]->getTrialDisp();
//
//
//  static XC::Vector u(24);      //Changed from u(8) to u(24) Xiaoyn 07/06/00
//
//  u(0)= disp1(0);
//  u(1)= disp1(1);
//        u(2)= disp1(2);
//  u(3)= disp2(0);
//  u(4)= disp2(1);
//  u(5)= disp2(2);
//        u(6)= disp3(0);
//  u(7)= disp3(1);
//  u(8)= disp3(2);
//  u(9)= disp4(0);
//  u(10)= disp4(1);
//  u(11)= disp4(2);
//  u(12)= disp5(0);
//  u(13)= disp5(1);
//  u(14)= disp5(2);
//  u(15)= disp6(0);
//  u(16)= disp6(1);
//  u(17)= disp6(2);
//  u(18)= disp7(0);
//  u(19)= disp7(1);
//  u(20)= disp7(2);
//  u(21)= disp8(0);
//  u(22)= disp8(1);
//  u(23)= disp8(2);
//
//  eps (6);      //Changed eps(3) to eps(6) Xiaoyan 07/06/00
//
//  P.Zero();
//
//  // Loop over the integration points
//  for(int i= 0; i < order; i++)
//  {
//    for(int j= 0; j < order; j++)
//    {
//      // Determine Jacobian for this integration point
//      this->setJacobian (intPt(i), intPt(j));
//
//      // Interpolate strains
//      this->formBMatrix (intPt(i), intPt(j));
//      eps= B*u;
//
//      // Set the material strain
//      (theMaterial[i][j])->setTrialStrain (eps);
//
//      // Get material stress response
//      const Vector &sigma= (theMaterial[i][j])->getStress();
//
//      // Form the Jacobian of the coordinate transformation
//      double detJ= this->formDetJ (intPt(i), intPt(j));
//
//      // Perform numerical integration
//      P= P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
//    }
//  }
//
//  P= P * thickness * -1;
//
//  return P;
//}
//
//const XC::Vector&
//XC::EightNodeBrick::getResistingForceIncInertia ()
//{
//  // Yet to implement
//  return P;
//}
//
//
//
//void
//XC::EightNodeBrick::Print(std::ostream &s, int flag)
//{
//  s << "EightNodeBrick, element id:  " << this->getTag() << std::endl;
//  s << "Connected external nodes:  " << theNodes;
//  s << "Material model:  " << theMaterial[0][0]->getType() << std::endl;
//  s << "Element thickness:  " << thickness << std::endl;
//  s << "Element mass density:  " << rho << std::endl << std::endl;
//}
//
//
//int
//XC::EightNodeBrick::displaySelf (Renderer &theViewer, int displayMode, float fact)
//{
    // first determine the end points of the quad based on
    // the display factor (a measure of the distorted image)
    // store this information in 2 3d vectors v1 and v2
//        const Vector &end1Crd= theNodes[0]->getCrds();
//        const Vector &end2Crd= theNodes[1]->getCrds();
//  const Vector &end3Crd= theNodes[2]->getCrds();
//  const Vector &end4Crd= theNodes[3]->getCrds();
//  // 5-8 were added by Xiaoyan
//        const Vector &end5Crd= theNodes[4]->getCrds();
//        const Vector &end6Crd= theNodes[5]->getCrds();
//  const Vector &end7Crd= theNodes[6]->getCrds();
//  const Vector &end8Crd= theNodes[7]->getCrds();
////---------------------------------------------------------------
//      const Vector &end1Disp= theNodes[0]->getDisp();
//  const Vector &end2Disp= theNodes[1]->getDisp();
//  const Vector &end3Disp= theNodes[2]->getDisp();
//  const Vector &end4Disp= theNodes[3]->getDisp();
//
  // 5-8 were added by Xiaoyan
//        const Vector &end5Disp= theNodes[4]->getDisp();
//  const Vector &end6Disp= theNodes[5]->getDisp();
//  const Vector &end7Disp= theNodes[6]->getDisp();
//  const Vector &end8Disp= theNodes[7]->getDisp();
//
//  Vector v1(3);
//  Vector v2(3);
//  Vector v3(3);
//  Vector v4(3);
//  //5-8 added by Xiaoyan 07/06/00
//  Vector v5(3);
//  Vector v6(3);
//  Vector v7(3);
//  Vector v8(3);
//
//  for(int i= 0; i < 3; i++)      //Changed from i<2 to i<3, Xiaonyan 07/06/00
//  {
//    v1(i)= end1Crd(i) + end1Disp(i)*fact;
//    v2(i)= end2Crd(i) + end2Disp(i)*fact;
//    v3(i)= end3Crd(i) + end3Disp(i)*fact;
//    v4(i)= end4Crd(i) + end4Disp(i)*fact;
//
//    //5-8 added by Xiaoyan 07/06/00
//       v5(i)= end5Crd(i) + end1Disp(i)*fact;
//    v6(i)= end6Crd(i) + end2Disp(i)*fact;
//    v7(i)= end7Crd(i) + end3Disp(i)*fact;
//    v8(i)= end8Crd(i) + end4Disp(i)*fact;
//  }
//  int error= 0;
//
//  error += theViewer.drawLine (v1, v2, 1.0, 1.0);
//  error += theViewer.drawLine (v2, v3, 1.0, 1.0);
//  error += theViewer.drawLine (v3, v4, 1.0, 1.0);
//  error += theViewer.drawLine (v4, v5, 1.0, 1.0);   // 5-8 added by Xiaoyan 07/06/00
//  error += theViewer.drawLine (v5, v6, 1.0, 1.0);
//  error += theViewer.drawLine (v6, v7, 1.0, 1.0);
//  error += theViewer.drawLine (v7, v8, 1.0, 1.0);
//  error += theViewer.drawLine (v8, v1, 1.0, 1.0);
//
//  return error;
//}
// The following are all commented by  Xiaoyan. We use the Brick3D to form these

//
//void
//XC::EightNodeBrick::formNMatrix (double r, double s,double t)
////Changed xi, eta to r,s and added t Xiaoyan  07/06/00
//{
//  N.Zero();
//
////  N(0,0)= N(1,1)= 0.25*(1.0-xi)*(1.0-eta);    // N_1
////   N(0,2)= N(1,3)= 0.25*(1.0+xi)*(1.0-eta);    // N_2
////  N(0,4)= N(1,5)= 0.25*(1.0+xi)*(1.0+eta);    // N_3
////  N(0,6)= N(1,7)= 0.25*(1.0-xi)*(1.0+eta);    // N_4
//
////  Changed by Xiaoyan 07/06/00
// The shape functions have been changed from N(2,8) to N(3,24)
// I take the node order according to Bathe's book p344-345. Xiaoyan
//        N(0,0)=N(1,1)=N(2,2)=1/8.*(1.0+r)*(1.0+s)*(1.0+t);
//  N(0,3)=N(1,4)=N(2,5)=1/8.*(1.0-r)*(1.0+s)*(1.0+t);
//  N(0,6)=N(1,7)=N(2,8)=1/8.*(1.0-r)*(1.0-s)*(1.0+t);
//  N(0,9)=N(1,10)=N(2,11)=1/8.*(1.0+r)*(1.0-s)*(1.0+t);
//  N(0,12)=N(1,13)=N(2,14)=1/8.*(1.0+r)*(1.0+s)*(1.0-t);
//  N(0,15)=N(1,16)=N(2,17)=1/8.*(1.0-r)*(1.0+s)*(1.0-t);
//  N(0,18)=N(1,19)=N(2,20)=1/8.*(1.0-r)*(1.0-s)*(1.0-t);
//  N(0,21)=N(1,22)=N(2,23)=1/8.*(1.0+r)*(1.0-s)*(1.0-t);
// }
//void
//XC::EightNodeBrick::setJacobian (double r, double s, double t)
////Changed xi, eta to r,s and added t Xiaoyan 07/06/00
//{
//  const Vector &nd1Crds= theNodes[0]->getCrds();
//  const Vector &nd2Crds= theNodes[1]->getCrds();
//  const Vector &nd3Crds= theNodes[2]->getCrds();
//  const Vector &nd4Crds= theNodes[3]->getCrds();
//  // Xiaoyan added 5-8 07/06/00
//  const Vector &nd5Crds= theNodes[4]->getCrds();
//  const Vector &nd6Crds= theNodes[5]->getCrds();
//  const Vector &nd7Crds= theNodes[6]->getCrds();
//  const Vector &nd8Crds= theNodes[7]->getCrds();
//
////  J(0,0)= -nd1Crds(0)*(1.0-eta) + nd2Crds(0)*(1.0-eta) +
////        nd3Crds(0)*(1.0+eta) - nd4Crds(0)*(1.0+eta);
////
//  J(0,1)= -nd1Crds(0)*(1.0-xi) - nd2Crds(0)*(1.0+xi) +
//        nd3Crds(0)*(1.0+xi) + nd4Crds(0)*(1.0-xi);
//
//  J(1,0)= -nd1Crds(1)*(1.0-eta) + nd2Crds(1)*(1.0-eta) +
//        nd3Crds(1)*(1.0+eta) - nd4Crds(1)*(1.0+eta);
//
//  J(1,1)= -nd1Crds(1)*(1.0-xi) - nd2Crds(1)*(1.0+xi) +
//        nd3Crds(1)*(1.0+xi) + nd4Crds(1)*(1.0-xi);
//      J= J * 0.25;
//
//  // For 3D problem Jacobi XC::Matrix changed from J(2,2) to J(3,3)
//  // Xiaoyan  changed 07/06/00
//
//
//  J(0,0)= nd1Crds(0)*(1.0+s)*(1.0+t) - nd2Crds(0)*(1.0+s)*(1.0+t) -
//     nd3Crds(0)*(1.0-s)*(1.0+t) + nd4Crds(0)*(1.0-s)*(1.0+t) +
//     nd5Crds(0)*(1.0+s)*(1.0-t) - nd6Crds(0)*(1.0+s)*(1.0-t) -
//     nd7Crds(0)*(1.0-s)*(1.0-t) + nd8Crds(0)*(1.0-s)*(1.0-t);
//
//  J(0,1)= nd1Crds(1)*(1.0+s)*(1.0+t) - nd2Crds(1)*(1.0+s)*(1.0+t) -
//     nd3Crds(1)*(1.0-s)*(1.0+t) + nd4Crds(1)*(1.0-s)*(1.0+t) +
//     nd5Crds(1)*(1.0+s)*(1.0-t) - nd6Crds(1)*(1.0+s)*(1.0-t) -
//     nd7Crds(1)*(1.0-s)*(1.0-t) + nd8Crds(1)*(1.0-s)*(1.0-t);
//
//  J(0,2)= nd1Crds(2)*(1.0+s)*(1.0+t) - nd2Crds(2)*(1.0+s)*(1.0+t) -
//     nd3Crds(2)*(1.0-s)*(1.0+t) + nd4Crds(2)*(1.0-s)*(1.0+t) +
//     nd5Crds(2)*(1.0+s)*(1.0-t) - nd6Crds(2)*(1.0+s)*(1.0-t) -
//     nd7Crds(2)*(1.0-s)*(1.0-t) + nd8Crds(2)*(1.0-s)*(1.0-t);
//
//  J(1,0)= nd1Crds(0)*(1.0+r)*(1.0+t) + nd2Crds(0)*(1.0-r)*(1.0+t) -
//     nd3Crds(0)*(1.0-r)*(1.0+t) - nd4Crds(0)*(1.0+r)*(1.0+t) +
//     nd5Crds(0)*(1.0+r)*(1.0-t) + nd6Crds(0)*(1.0-r)*(1.0-t) -
//     nd7Crds(0)*(1.0-r)*(1.0-t) - nd8Crds(0)*(1.0+r)*(1.0-t);
//
//  J(1,1)= nd1Crds(1)*(1.0+r)*(1.0+t) + nd2Crds(1)*(1.0-r)*(1.0+t) -
//     nd3Crds(1)*(1.0-r)*(1.0+t) - nd4Crds(1)*(1.0+r)*(1.0+t) +
//     nd5Crds(1)*(1.0+r)*(1.0-t) + nd6Crds(1)*(1.0-r)*(1.0-t) -
//     nd7Crds(1)*(1.0-r)*(1.0-t) - nd8Crds(1)*(1.0+r)*(1.0-t);
//
//        J(1,2)= nd1Crds(2)*(1.0+r)*(1.0+t) + nd2Crds(2)*(1.0-r)*(1.0+t) -
//     nd3Crds(2)*(1.0-r)*(1.0+t) - nd4Crds(2)*(1.0+r)*(1.0+t) +
//     nd5Crds(2)*(1.0+r)*(1.0-t) + nd6Crds(2)*(1.0-r)*(1.0-t) -
//     nd7Crds(2)*(1.0-r)*(1.0-t) - nd8Crds(2)*(1.0+r)*(1.0-t);
//
//  J(2,0)= nd1Crds(0)*(1.0+r)*(1.0+s) + nd2Crds(0)*(1.0-r)*(1.0+s) +
//     nd3Crds(0)*(1.0-r)*(1.0-s) + nd4Crds(0)*(1.0+r)*(1.0-s) -
//     nd5Crds(0)*(1.0+r)*(1.0+s) - nd6Crds(0)*(1.0-r)*(1.0+s) -
//     nd7Crds(0)*(1.0-r)*(1.0-s) - nd8Crds(0)*(1.0+r)*(1.0-s);
//
//  J(2,1)= nd1Crds(1)*(1.0+r)*(1.0+s) + nd2Crds(1)*(1.0-r)*(1.0+s) +
//     nd3Crds(1)*(1.0-r)*(1.0-s) + nd4Crds(1)*(1.0+r)*(1.0-s) -
//     nd5Crds(1)*(1.0+r)*(1.0+s) - nd6Crds(1)*(1.0-r)*(1.0+s) -
//     nd7Crds(1)*(1.0-r)*(1.0-s) - nd8Crds(1)*(1.0+r)*(1.0-s);
//
//  J(2,2)= nd1Crds(2)*(1.0+r)*(1.0+s) + nd2Crds(2)*(1.0-r)*(1.0+s) +
//     nd3Crds(2)*(1.0-r)*(1.0-s) + nd4Crds(2)*(1.0+r)*(1.0-s) -
//     nd5Crds(2)*(1.0+r)*(1.0+s) - nd6Crds(2)*(1.0-r)*(1.0+s) -
//     nd7Crds(2)*(1.0-r)*(1.0-s) - nd8Crds(2)*(1.0+r)*(1.0-s);
//
//   J=J*0.125
//
//    // L= inv(J)  Changed from L(2,2) to L(3,3)  07/07/00
//
//  L(0,0)=-J(1,2)*J(2,1) + J(1,1)*J(2,2);
//  L(0.1)= J(0,2)*J(2,1) - J(0,1)*J(2,2);
//  L(0,3)=-J(0,2)*J(1,1) + J(0,1)*J(1,2);
//  L(1,0)= J(1,2)*J(2,0) - J(1,0)*J(2,2);
//  L(1,1)=-J(0,2)*J(2,0) + J(0,0)*J(2.2);
//  L(1,2)= J(0,2)*J(1,0) - J(0,0)*J(1,2);
//  L(2,0)=-J(1,1)*J(2,0) + J(1,0)*J(2,1);
//  L(2,1)= J(0,1)*J(2,0) - J(0,0)*J(2,1);
//  L(2,2)=-J(0,1)*J(1,0) + J(0,0)*J(1,1);
//  L=L/formDetJ(r,s,t)
//
//  L(0,0)= J(1,1);
//  L(1,0)= -J(0,1);
//  L(0,1)= -J(1,0);
//  L(1,1)= J(0,0);

//  L= L / formDetJ (xi, eta);
//}
//
//void
//XC::EightNodeBrick::formBMatrix (double r, double s, double t)
////Changed xi, eta to r,s and added t Xiaoyan  07/06/00
//{
//    B.Zero();
//
//    //Changed by Xiaoyan 07/06/00
//    double L00= L(0,0);
//    double L01= L(0,1);
//    double L02= L(0,1);
//    double L10= L(1,0);
//    double L11= L(1,1);
//    double L12= L(1,2);
//    double L20= L(2,0);
//    double L21= L(2,1);
//    double L22= L(2,2);
//
//    // See Cook, Malkus, Plesha p. 169 for the derivation of these terms
//    B(0,0)= L00*-0.25*(1.0-eta) + L01*-0.25*(1.0-xi);    // N_1,1
//    B(0,2)= L00*0.25*(1.0-eta) + L01*-0.25*(1.0+xi);    // N_2,1
//    B(0,4)= L00*0.25*(1.0+eta) + L01*0.25*(1.0+xi);    // N_3,1
//    B(0,6)= L00*-0.25*(1.0+eta) + L01*0.25*(1.0-xi);    // N_4,1
//
//    B(1,1)= L10*-0.25*(1.0-eta) + L11*-0.25*(1.0-xi);    // N_1,2
//    B(1,3)= L10*0.25*(1.0-eta) + L11*-0.25*(1.0+xi);    // N_2,2
//    B(1,5)= L10*0.25*(1.0+eta) + L11*0.25*(1.0+xi);    // N_3,2
//    B(1,7)= L10*-0.25*(1.0+eta) + L11*0.25*(1.0-xi);    // N_4,2
//
//    B(2,0)= B(1,1);
//    B(2,1)= B(0,0);
//    B(2,2)= B(1,3);
//    B(2,3)= B(0,2);
//    B(2,4)= B(1,5);
//    B(2,5)= B(0,4);
//    B(2,6)= B(1,7);
//    B(2,7)= B(0,6);
//}
//
//
//
////The derivative  of shape function to r,s,t respectly.
//// For example dh1dr means dh1/dr etc. Xiaoyan  07/07/00
//double  dh1dr=0.125*(1+s)*(1+t);
//double  dh1ds=0.125*(1+r)*(1+t);
//double  dh1dt=0.125*(1+r)*(1+s);
//
//double  dh2dr=-0.125*(1+s)*(1+t);
//double  dh2ds=0.125*(1-r)*(1+t);
//double  dh2dt=0.125*(1-r)*(1+s);
//
//double  dh3dr=-0.125*(1-s)*(1+t);
//double  dh3ds=-0.125*(1-r)*(1+t);
//double  dh3dt=0.125*(1-r)*(1-s);
//
//double  dh4dr=0.125*(1-s)*(1+t);
//double  dh4ds=-0.125*(1+r)*(1+t);
//double  dh4dt=0.125*(1+r)*(1-s);
//
//double  dh5dr=0.125*(1+s)*(1-t);
//double  dh5ds=0.125*(1+r)*(1-t);
//double  dh5dt=-0.125*(1+r)*(1+s);
//
//double  dh6dr=-0.125*(1+s)*(1-t);
//double  dh6ds=0.125*(1-r)*(1-t);
//double  dh6dt=-0.125*(1-r)*(1+s);
//
//double  dh7dr=-0.125*(1-s)*(1-t);
//double  dh7ds=-0.125*(1-r)*(1-t);
//double  dh7dt=-0.125*(1-r)*(1-s);
//
//double  dh8dr=0.125*(1-s)*(1-t);
//double  dh8ds=-0.125*(1+r)*(1-t);
//double  dh8dt=-0.125*(1+r)*(1-s);
//
//// B XC::Matrix B(6,24)
//B(0,0)=L00*dh1dr+L01*dh1ds+L02*dh1dt;
//B(0,3)=L00*dh2dr+L01*dh2ds+L02*dh2dt;
//B(0,6)=L00*dh3dr+L01*dh3ds+L02*dh3dt;
//B(0,9)=L00*dh4dr+L01*dh4ds+L02*dh4dt;
//B(0,12)=L00*dh5dr+L01*dh5ds+L02*dh5dt;
//B(0,15)=L00*dh6dr+L01*dh6ds+L02*dh6dt;
//B(0,18)=L00*dh7dr+L01*dh7ds+L02*dh7dt;
//B(0,21)=L00*dh8dr+L01*dh8ds+L02*dh8dt;
//
//B(1,1)=L10*dh1dr+L11*dh1ds+L12*dh1dt;
//B(1,4)=L10*dh2dr+L11*dh2ds+L12*dh2dt;
//B(1,7)=L10*dh3dr+L11*dh3ds+L12*dh3dt;
//B(1,10)=L10*dh4dr+L11*dh4ds+L12*dh4dt;
//B(1,13)=L10*dh5dr+L11*dh5ds+L12*dh5dt;
//B(1,16)=L10*dh6dr+L11*dh6ds+L12*dh6dt;
//B(1,19)=L10*dh7dr+L11*dh7ds+L12*dh7dt;
//B(1,22)=L10*dh8dr+L11*dh8ds+L12*dh8dt;
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
//B(3,12)=B(1,13);
//B(3,13)=B(0,12);
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
//B(5,12)=B(2,14);
//B(5,14)=B(0,12);
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
//B(3,12)=L00*dh5dr+L01*dh5ds+L02*dh5dt;
//B(3,15)=L00*dh6dr+L01*dh6ds+L02*dh6dt;
//B(3,18)=L00*dh7dr+L01*dh7ds+L02*dh7dt;
//B(3,21)=L00*dh8dr+L01*dh8ds+L02*dh8dt;
//double
//XC::EightNodeBrick::formDetJ (double r, double s, double t)
//{
//    return J(0,0)*J(1,1)*J(2,2)+J(1,0)*J(2,1)*J(0,2)+J(2,0)*J(0,1)*J(1,2)
//         - J(2,0)*J(1,1)*J(0,2)-J(0,0)*J(2,1)*J(1,2)-J(0,1)*J(1,0)*J(2,2);
//}


double XC::EightNodeBrick::get_Gauss_p_c(short order, short point_numb) const
  {
//  Abscissae coefficient of the Gaussian quadrature formula
// starting from 1 not from 0
    static double Gauss_coordinates[7][7];

    Gauss_coordinates[1][1]= 0.0 ;
    Gauss_coordinates[2][1]= -0.577350269189626;
    Gauss_coordinates[2][2]= -Gauss_coordinates[2][1];
    Gauss_coordinates[3][1]= -0.774596669241483;
    Gauss_coordinates[3][2]= 0.0;
    Gauss_coordinates[3][3]= -Gauss_coordinates[3][1];
    Gauss_coordinates[4][1]= -0.861136311594053;
    Gauss_coordinates[4][2]= -0.339981043584856;
    Gauss_coordinates[4][3]= -Gauss_coordinates[4][2];
    Gauss_coordinates[4][4]= -Gauss_coordinates[4][1];
    Gauss_coordinates[5][1]= -0.906179845938664;
    Gauss_coordinates[5][2]= -0.538469310105683;
    Gauss_coordinates[5][3]= 0.0;
    Gauss_coordinates[5][4]= -Gauss_coordinates[5][2];
    Gauss_coordinates[5][5]= -Gauss_coordinates[5][1];
    Gauss_coordinates[6][1]= -0.932469514203152;
    Gauss_coordinates[6][2]= -0.661209386466265;
    Gauss_coordinates[6][3]= -0.238619186083197;
    Gauss_coordinates[6][4]= -Gauss_coordinates[6][3];
    Gauss_coordinates[6][5]= -Gauss_coordinates[6][2];
    Gauss_coordinates[6][6]= -Gauss_coordinates[6][1];

    return Gauss_coordinates[order][point_numb];
 }

double XC::EightNodeBrick::get_Gauss_p_w(short order, short point_numb) const
  {
//  Weight coefficient of the Gaussian quadrature formula
// starting from 1 not from 0
    static double Gauss_weights[7][7]; // static data ??

    Gauss_weights[1][1]= 2.0;
    Gauss_weights[2][1]= 1.0;
    Gauss_weights[2][2]= 1.0;
    Gauss_weights[3][1]= 0.555555555555556;
    Gauss_weights[3][2]= 0.888888888888889;
    Gauss_weights[3][3]= Gauss_weights[3][1];
    Gauss_weights[4][1]= 0.347854845137454;
    Gauss_weights[4][2]= 0.652145154862546;
    Gauss_weights[4][3]= Gauss_weights[4][2];
    Gauss_weights[4][4]= Gauss_weights[4][1];
    Gauss_weights[5][1]= 0.236926885056189;
    Gauss_weights[5][2]= 0.478628670499366;
    Gauss_weights[5][3]= 0.568888888888889;
    Gauss_weights[5][4]= Gauss_weights[5][2];
    Gauss_weights[5][5]= Gauss_weights[5][1];
    Gauss_weights[6][1]= 0.171324492379170;
    Gauss_weights[6][2]= 0.360761573048139;
    Gauss_weights[6][3]= 0.467913934572691;
    Gauss_weights[6][4]= Gauss_weights[6][3];
    Gauss_weights[6][5]= Gauss_weights[6][2];
    Gauss_weights[6][6]= Gauss_weights[6][1];

    return Gauss_weights[order][point_numb];
  }


int XC::EightNodeBrick::update(void) //Note: Guanzhou finished the algorithm consistent with global incremental calculation Mar2005
  {

    double r= 0.0;
    double s= 0.0;
    double t= 0.0;

    short where= 0;

    static int dh_dim[]= {8,3};
    BJtensor dh(2, dh_dim, 0.0);

    static int disp_dim[]= {8,3};
    //GZ out XC::BJtensor incr_displacements(2,disp_dim,0.0);

    //GZ out straintensor incr_strain;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;

    //Guanzhou out incr_displacements= incr_disp();//Get incremental disp from domain

    BJtensor trial_disp(2,disp_dim,0.0);
    trial_disp= total_disp();//Guanzhou added, get trial disp from domain

    straintensor trial_strain;


    for( short GP_c_r= 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r= get_Gauss_p_c( r_integration_order, GP_c_r );

        for( short GP_c_s= 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s= get_Gauss_p_c( s_integration_order, GP_c_s );

            for( short GP_c_t= 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t= get_Gauss_p_c( t_integration_order, GP_c_t );

                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordiantes with respect to local coordiantes
                dh= dh_drst_at(r,s,t);
                // Jacobian XC::BJtensor ( matrix )
                Jacobian= Jacobian_3D(dh);
                //....                Jacobian.print("J");
                // Inverse of Jacobian XC::BJtensor ( matrix )
                JacobianINV= Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");
                // determinant of Jacobian XC::BJtensor ( matrix )
                //--                det_of_Jacobian= Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal= dh("ij") * JacobianINV("kj");
                // incremental strains at this Gauss point
                // now in Update we know the total displacements so let's find
                // the total strain
                trial_strain =
                    (dhGlobal("ib")*trial_disp("ia")).symmetrize11();
                trial_strain.null_indices();

                //Guanzhou out Mar2005 if( ( (matpoint[where].matmodel)->setTrialStrainIncr( incr_strain)) )
                if( ( (matpoint[where].matmodel)->setTrialStrain(trial_strain)) )
                  std::cerr << "XC::EightNodeBrick::update (tag: " << this->getTag() << "), Update Failed\n";
              }
          }
      }
    return 0;
  }

#endif
