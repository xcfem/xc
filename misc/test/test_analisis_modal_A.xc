\c{Test para comprobar el análisis modal espectral
tomado de la publicación de Andrés Sáez Pérez: «Estructuras III»
 E.T.S. de Arquitectura de Sevilla (España).}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_3gdl.xcm"}

\def_prop["masaPorPlanta","double"]{134.4e3}
\def_prop["matrizMasasNodo","m_double"]{[[@masaPorPlanta,0,0],
                                         [0,@masaPorPlanta,0],
                                         [0,0,0]]}
\def_prop["Ehorm","double"]{200000*1e5} \c{Módulo elástico del hormigón.}

\def_prop["Bbaja","double"]{0.45} \c{Escuadría de los pilares.}
\def_prop["Ibaja","double"]{1/12*@Bbaja^4} \c{Inercia de la sección.}
\def_prop["Hbaja","double"]{4} \c{Altura de la planta baja.}
\def_prop["B1a","double"]{0.40} \c{Escuadría de los pilares.}
\def_prop["I1a","double"]{1/12*@B1a^4} \c{Inercia de la sección.}
\def_prop["H","double"]{3} \c{Altura del resto de plantas.}
\def_prop["B3a","double"]{0.35} \c{Escuadría de los pilares.}
\def_prop["I3a","double"]{1/12*@B3a^4} \c{Inercia de la sección.}


\def_prop["kPlBaja","double"]{20*12*@Ehorm*@Ibaja/(@Hbaja^3)}
\def_prop["kPl1a","double"]{20*12*@Ehorm*@I1a/(@H^3)}
\def_prop["kPl2a","double"]{@kPl1a}
\def_prop["kPl3a","double"]{20*12*@Ehorm*@I3a/(@H^3)}
\def_prop["kPl4a","double"]{@kPl3a}

\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales2D.xci"}
\mdlr
  {
    \nodos
      {
        \nod[0]{\coo{0,0} \mass{@matrizMasasNodo} \def_prop["gdlsCoartados","vector"]{[0,1,2]}}
        \nod{\coo{0,4} \mass{@matrizMasasNodo} \def_prop["gdlsCoartados","vector"]{[1,2]}}
        \nod{\coo{0,4+3} \mass{@matrizMasasNodo} \def_prop["gdlsCoartados","vector"]{[1,2]}}
        \nod{\coo{0,4+3+3} \mass{@matrizMasasNodo} \def_prop["gdlsCoartados","vector"]{[1,2]}}
        \nod{\coo{0,4+3+3+3} \mass{@matrizMasasNodo} \def_prop["gdlsCoartados","vector"]{[1,2]}}
        \nod{\coo{0,4+3+3+3+3} \mass{@matrizMasasNodo} \def_prop["gdlsCoartados","vector"]{[1,2]}}
        \for_each{  \fix[@gdlsCoartados]{[0,0,0]} }
      }    
    \c{Definimos materiales}
    \materiales
       {
         \elastic_section_2d["sccPlBaja"]{\paramSeccion{ \A{20*@Bbaja*@Bbaja} \E{@Ehorm} \I{20*@Ibaja} }}
         \elastic_section_2d["sccPl1a"]{\paramSeccion{ \A{20*@B1a*@B1a} \E{@Ehorm} \I{20*@I1a} }} 
         \elastic_section_2d["sccPl2a"]{\paramSeccion{ \A{20*@B1a*@B1a} \E{@Ehorm} \I{20*@I1a} }} 
         \elastic_section_2d["sccPl3a"]{\paramSeccion{ \A{20*@B3a*@B3a} \E{@Ehorm} \I{20*@I3a} }} 
         \elastic_section_2d["sccPl4a"]{\paramSeccion{ \A{20*@B3a*@B3a} \E{@Ehorm} \I{20*@I3a} }}
       }

    \c{Definimos transformaciones geométricas}
    \geom_transf{ \linear2d["lin"]{} }

    \c{Definimos elementos}
    \elementos
      {
        \nmb_transf{"lin"} \nmb_material{"sccPlBaja"}
        \elem_elastic_beam_2d[1]{ \nodes{0,1} \h{@Bbaja} }
        \nmb_material{"sccPl1a"} \elem_elastic_beam_2d{ \nodes{1,2} \h{@B1a} }
        \nmb_material{"sccPl2a"} \elem_elastic_beam_2d{ \nodes{2,3} \h{@B1a} }
        \nmb_material{"sccPl3a"} \elem_elastic_beam_2d{ \nodes{3,4} \h{@B3a} }
        \nmb_material{"sccPl4a"} \elem_elastic_beam_2d{ \nodes{4,5} \h{@B3a} }
      }
  }

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
            \band_arpack_soe
             {
               \shift{0.0} 
               \band_arpack_solver{\tol{1e-5}\num_max_iter{100}}
             }
          }
      }    
    \modal_analysis["smt"]
      {
        \analyze{4}
        \set{periodos= @getPeriodos}
        \set{modos= @getNormalizedEigenvectors}
        \set{factoresParticipacionModal= @getModalParticipationFactors}
        \set{masasModalesEfectivas= @getEffectiveModalMasses}
        \set{masaTotal= @getTotalMass}
        \set{factoresDistribucion= @getDistributionFactors}
      }
  }

\def_prop["periodosTeor","vector"]{[0.468,0.177,0.105,0.084]}
\def_prop["ratio1","double"]{@abs(@periodos-@periodosTeor)}
\def_prop["modosEjemplo","m_double"]{[[0.323,-0.764,-0.946,0.897],
                                      [0.521,-0.941,-0.378,-0.251],
                                      [0.685,-0.700,0.672,-0.907],
                                      [0.891,0.241,1.000,1.000],
                                      [1.000,1.000,-0.849,-0.427]]}
\def_prop["resta","m_double"]{(@modos-@modosEjemplo)}
\def_prop["ratio2","double"]{@abs(@resta)}

\c{
\print{"kPlBaja= ",@kPlBaja,"\n"}
\print{"kPl1a= ",@kPl1a,"\n"}
\print{"kPl3a= ",@kPl3a,"\n"}
\print{"periodos: ",@periodos,"\n"}
\print{"ratio1= ",@ratio1,"\n"}
\print{"modos: ",@modos,"\n"}
\print{"resta: ",@resta,"\n"}
\print{"ratio2= ",@ratio2,"\n"}
\print{"factoresParticipacionModal: ",@factoresParticipacionModal,"\n"}
}

\if((@ratio1<1e-3) & (@ratio2<5e-2))
  {
    \then{\print{"test analisis modal 03: pasa.\n"}}
    \else{\print{"test analisis modal 03: falla.\n"}}
 }