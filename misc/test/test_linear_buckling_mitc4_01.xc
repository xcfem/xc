\c{Tomado del Ejemplo B46 del SOLVIA Verification Manual}
\path{"/usr/local/lib/macros_xc"}

\expr{L= 4} \c{Longitud de la columna en metros}
\expr{b= 0.2} \c{Ancho de la sección en metros}
\def_prop["nuMat","double"]{0.3} \c{Coeficiente de Poisson.}
\def_prop["EMat","double"]{30E9} \c{Módulo de Young en N/m2.}
\def_prop["espChapa","double"]{0.2} \c{Espesor en m.}
\def_prop["area","double"]{b*@espChapa} \c{Área de la sección en m2}
\def_prop["inercia","double"]{1/12*b*@espChapa^3} \c{Momento de inercia en m4}
\expr{P= -10000} \c{Carga vertical sobre la columna.}

\expr{NumDiv= 4}

\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales3D.xci"}
\mdlr
  {
    \c{Definimos materiales}
    \materiales{ \elastic_membrane_plate_section["elast"]{ \nu{@nuMat} \E{@EMat} \h{@espChapa} } }
    \cad
      {
        \pnt[1]{ \pos{ \x{0.0} \z{0.00}} }
        \pnt[2]{ \pos{ \x{b} \z{0.0}} }
        \pnt[3]{ \pos{ \x{b} \y{L}} }
        \pnt[4]{ \pos{ \x{b} \y{L} } }
        \sup_cuadrilatera[1]
          {
            \def_pnts{1,2,3,4}
            \ndivI{1}
            \ndivJ{NumDiv}
          }
      }
    \nodos{ \nod_semilla[0]{\coo{0.0,0.0,0.0}} }
    \elementos
      {
        \elem_semilla
          {
            \nmb_material{"elast"}
            \elem_corot_shell_mitc4[1]{}
          }
      }
    \sets{\f1{\malla{}}}
    \c{Condiciones de contorno}
    \cad
      {
        \get_linea_extremos[1,2]
          { \fix_gdls[0,1,2]{[0,0,0]} } \c{UX,UY,UZ}
        \get_linea_extremos[3,4]
          { \fix_gdls[0,1]{[0,0]} } \c{UY,UY}
      }
    \c{Definimos cargas}
    \loads
      {
        \casos
          {
            \constant_ts["ts"]{ \factor{1.0} } \c{Time series: constant_ts[nombre]{factor}}
            \set_current_time_series{"ts"}
            \load_pattern["0"]{} \c{load_pattern[codigo]{}}
            \set_current_load_pattern{"0"}
          }
      }
    \cad
      {
        \pnt[3]{ \nodo{\load{[0,0,P/2,0,0,0]}}}
        \pnt[4]{ \nodo{\load{[0,0,P/2,0,0,0]}}}
      }
    \loads{ \add_to_domain{"0"}} \c{Añadimos la hipótesis al dominio}

  }

\c{Procedimiento de solución}
\sol_proc
  {
\control
  {
    \solu_model["sm"]
      {
        \penalty_constraint_handler{ \alphaSP{1.0e15} \alphaMP{1.0e15} }
        \default_numberer{\rcm{}}
      }
    \solu_method["smt","sm"]
      {
        \newton_raphson_soln_algo{}
        \norm_disp_incr_conv_test{ \print_flag{1} \tol{1e-4} \max_num_iter{100} }
        \load_control_integrator[1.0,1,1.0,1.0]{}
        \band_spd_lin_soe{\band_spd_lin_lapack_solver{}}
      }
    \solu_method["buck","sm"]
      {
        \linear_buckling_soln_algo{}
        \linear_buckling_integrator{}
        \band_arpackpp_soe{\shift{0.0} \band_arpackpp_solver{}}
      }
  }

\linear_buckling_analysis["smt","buck"]
  {
    \num_modes{2}
    \analyze{2}
  }
  }

\mdlr
  {
    \dom
       {
         \expr{eig1= @eigen_value(0)}
         \nodos
           {
             \nodo[2]{\disp{\expr{deltay= @commit_disp[1]}} }
           }
       }
  }
  

\expr{deltayTeor= P*L/(@EMat*@area)}
\expr{ratio1= (deltay-deltayTeor)/deltayTeor}
\expr{blCalc= eig1*P}
\expr{blTeor= -1*@PI^2*@EMat*@inercia/(L^2)}
\expr{ratio2= (blCalc-blTeor)/blTeor}

\print{"deltay= ",@tonum(deltay),"\n"}
\print{"deltayTeor= ",@tonum(deltayTeor),"\n"}
\print{"eig1= ",@tonum(eig1),"\n"}
\print{"ratio1= ",@tonum(ratio1),"\n"}
\print{"blCalc= ",@tonum(blCalc/1e6)," MN \n"}
\print{"blTeor= ",@tonum(blTeor/1e6)," MN \n"}
\print{"ratio2= ",@tonum(ratio2),"\n"}
 \c{
 }

\if((@abs(ratio1)<1e-5) & (@abs(ratio2)<0.06))
  {
    \cond{(@abs(ratio1)<1e-5) & (@abs(ratio2)<0.06)}
    \then{\print{"test linear_buckling_column 01: pasa.\n"}}
    \else{\print{"test linear_buckling_column 01: FALLA.\n"}}
  }