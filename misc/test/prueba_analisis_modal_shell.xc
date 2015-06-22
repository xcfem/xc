\c{Tomado del Ejemplo VM66 del Ansys Verification Manual Release 9.0}
\path{"/usr/local/lib/macros_xc"}

\expr{L= 16} \c{Longitud de la ménsula en pulgadas.}
\expr{b= 4} \c{Ancho de la ménsula en pulgadas.}
\expr{h= 1} \c{Espesor de la ménsula en pulgadas.}
\def_prop["nuMat","double"]{0} \c{Coeficiente de Poisson.}
\def_prop["EMat","double"]{30E6} \c{Módulo de Young en psi.}
\def_prop["espChapa","double"]{h} \c{Espesor en m.}
\def_prop["area","double"]{b*@espChapa} \c{Área de la sección en m2}
\def_prop["inercia1","double"]{1/12*@espChapa*b^3} \c{Momento de inercia en m4}
\def_prop["inercia2","double"]{1/12*b*@espChapa^3} \c{Momento de inercia en m4}
\expr{dens= 0.000728} \c{Densidad del material en lb-sec2/in4}
\expr{m= b*h*dens}

\expr{NumDiv= 5}

\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales3D.xci"}
\mdlr
  {
    \c{Definimos materiales}
    \materiales{ \elastic_membrane_plate_section["elast"]{ \nu{@nuMat} \E{@EMat} \h{@espChapa} \rho{@espChapa*dens} } }
    \cad
      {
        \pnt[1]{ \pos{ \x{0.0} \y{0.0}} }
        \pnt[2]{ \pos{ \x{b} \y{0.0}} }
        \pnt[3]{ \pos{ \x{b} \y{L}} }
        \pnt[4]{ \pos{ \x{0} \y{L} } }
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
            \elem_shell_mitc4[1]{\materiales{}}
          }
      }
    \sets{\f1{\malla{}}}
    \c{Condiciones de contorno}
    \cad
      {
        \get_linea_extremos[1,2]
          { \fix_gdls[0,1,2,3,4,5]{[0,0,0,0,0,0]} } \c{UX,UY,UZ,RX,RY,RZ}
      }
  }

\def_prop["eig1","double"]{}
\def_prop["eig2","double"]{}

\def_prop["periodos","vector"]{}
\def_prop["modos","m_double"]{}
\def_prop["factoresParticipacionModal","vector"]{}
\def_prop["masasModalesEfectivas","vector"]{}
\def_prop["masaTotal","double"]{}
\def_prop["factoresDistribucion","m_double"]{}

\c{Procedimiento de solución}
\sol_proc
  {
\control
  {
    \solu_model["sm"]
      {
        \transformation_constraint_handler{}
        \default_numberer{\rcm{}}
      }
    \solu_method["smt","sm"]
      {
        \frequency_soln_algo{}
        \eigen_integrator[1.0,1,1.0,1.0]{}
        \c{\band_arpack_soe{\shift{0.0} \band_arpack_solver{\tol{1e-3}\num_max_iter{5}}}
        \full_gen_eigen_soe{\full_gen_eigen_solver{}}}
        \sym_band_eigen_soe{\sym_band_eigen_solver{}}
      }
  }    
    \modal_analysis["smt"]
      {
        \analyze{3}
        \set{periodos= @getPeriodos}
        \set{modos= @getNormalizedEigenvectors}
        \set{factoresParticipacionModal= @getModalParticipationFactors}
        \set{masasModalesEfectivas= @getEffectiveModalMasses}
        \set{masaTotal= @getTotalMass}
        \set{factoresDistribucion= @getDistributionFactors}
      }
  }

\print{"periodos: ",@periodos,"\n"}
\print{"modos: ",@modos,"\n"}
\print{"factoresParticipacionModal: ",@factoresParticipacionModal,"\n"}
\print{"masasModalesEfectivas: ",@masasModalesEfectivas,"\n"}
\print{"masaTotal: ",@masaTotal,"\n"}
\c{\print{"factoresDistribucion: ",@factoresDistribucion,"\n"}}
