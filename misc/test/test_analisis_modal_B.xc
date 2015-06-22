\c{Test para comprobar el análisis modal espectral
tomado de la publicación de Andrés Sáez Pérez: «Estructuras III»
 E.T.S. de Arquitectura de Sevilla (España).}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_3gdl.xcm"}
\incluye{"acciones/sismo/escribeCargasModo.xcm"}

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
         \elastic_section_2d["sccPlBaja"]{\paramSeccion{ \A{20*@Bbaja*@Bbaja} \E{@Ehorm} \I{20*@Ibaja}  }}
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
        \elem_elastic_beam_2d[1]{ \nodes{0,1} \h{@Bbaja} \rho{1}}
        \nmb_material{"sccPl1a"} \elem_elastic_beam_2d{ \nodes{1,2} \h{@B1a}  \rho{1}}
        \nmb_material{"sccPl2a"} \elem_elastic_beam_2d{ \nodes{2,3} \h{@B1a}  \rho{1}}
        \nmb_material{"sccPl3a"} \elem_elastic_beam_2d{ \nodes{3,4} \h{@B3a}  \rho{1}}
        \nmb_material{"sccPl4a"} \elem_elastic_beam_2d{ \nodes{4,5} \h{@B3a}  \rho{1}}
      }
  }

\def_prop["periodos","vector"]{}
\def_prop["aceleraciones","vector"]{} \c{Acelaración máxima para cada modo.}
\def_prop["modos","m_double"]{}
\def_prop["factoresParticipacionModal","vector"]{}
\def_prop["masasModalesEfectivas","vector"]{}
\def_prop["masaTotal","double"]{}
\def_prop["factoresDistribucion","m_double"]{}

\def_prop["beta","double"]{0.29} \c{Ductilidad alta.}
\def_prop["Ki","vector"]{[1,1.33,1.88]}

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
               \band_arpack_solver{\tol{0.1}\num_max_iter{10000}}
             }
          }
      }    
    \modal_analysis["smt"]
      {
        \espectro
          {
            \def_prop["ac","double"]{0.69} \c{Aceleración de cálculo.}
            \def_prop["T0","double"]{0.24}
            \def_prop["T1","double"]{0.68}
            \def_prop["meseta","double"]{2.28}
            \punto{0.0,1.0}
            \punto{@T0,@meseta}
            \def_var["t","double"]{}
            \for(t=@T1;@t<2.0;t=@t+1)
              {\bucle{
                \punto{@t,@meseta*@T1/@t}
              }}
            \multiplicaCte{@ac}
          }
        \analyze{3}          
        \set{periodos= @getPeriodos}
        \espectro{\set{aceleraciones= @valores(@getPeriodos)}}
        \set{modos= @getNormalizedEigenvectors}
        \set{factoresDistribucion= @getDistributionFactors}
      }
  }

\def_prop["cargaModo1","vector"]{[0,0,0]}
\def_prop["cargaModo2","vector"]{[0,0,0]}
\def_prop["cargaModo3","vector"]{[0,0,0]}
\mdlr
  {
    \dom
      {
        \set{factoresParticipacionModal= @getModalParticipationFactors}
        \set{masasModalesEfectivas= @getEffectiveModalMasses}
        \set{masaTotal= @getTotalMass}
      }
    \nodos
      {
        \for_each
          {
            \if(@tag>0)
               {\then{
                
                 \set{cargaModo1= @cargaModo1+@beta*@Ki[0]*@getEquivalentStaticLoad(1,@aceleraciones[0])}
                 \set{cargaModo2= @cargaModo2+@beta*@Ki[1]*@getEquivalentStaticLoad(2,@aceleraciones[1])}
                 \set{cargaModo3= @cargaModo3+@beta*@Ki[2]*@getEquivalentStaticLoad(3,@aceleraciones[2])}
               }}
           }
      }
  }
\nuevo_archivo_salida["cargasModo1"]{"cargas_modo1.xci"}
\escribeCargasModo("cargasModo1",1,@aceleraciones){}
\cierra_archivo_salida{"cargasModo1"}
\fin{}

 

\def_prop["periodosTeor","vector"]{[0.468,0.177,0.105]}
\def_prop["ratio1","double"]{@abs(@periodos-@periodosTeor)}
\def_prop["modosEjemplo","m_double"]{[[0.323,-0.764,-0.946],
                                      [0.521,-0.941,-0.378],
                                      [0.685,-0.700,0.672],
                                      [0.891,0.241,1.000],
                                      [1.000,1.000,-0.849]]}
\def_prop["resta","m_double"]{(@modos-@modosEjemplo)}
\def_prop["ratio2","double"]{@abs(@resta)}
\def_prop["ratio3","double"]{@abs(@masaTotal-5*@masaPorPlanta)/5/@masaPorPlanta}
\c{Los valores de los 3 primeros factores de distribución (3 primeras columnas)
   se tomaron del ejemplo de referencia. Los otros dos (que no se dan en el
   ejemplo) son los que se obtienen del programa.} 
\def_prop["factoresDistribEjemplo","m_double"]{[[0.419,0.295,0.148],
                                                [0.676,0.363,0.059],
                                                [0.889,0.27,-0.105],
                                                [1.157,-0.093,-0.156],
                                                [1.298,-0.386,0.133]]}
\set{resta= @factoresDistribucion-@factoresDistribEjemplo}
\def_prop["ratio4","double"]{@abs(@resta)}
\def_prop["ratio5","double"]{@sqrt(@sqr(@cargaModo1[0]-273523)+@sqr(@cargaModo2[0]-31341)+@sqr(@cargaModo3[0]-6214))/273523}

\print{"kPlBaja= ",@kPlBaja,"\n"}
\print{"kPl1a= ",@kPl1a,"\n"}
\print{"kPl3a= ",@kPl3a,"\n"}
\print{"periodos: ",@periodos,"\n"}
\print{"aceleraciones= ",@aceleraciones,"\n"}          
\print{"ratio1= ",@ratio1,"\n"}
\print{"modos: ",@modos,"\n"}
\print{"resta: ",@resta,"\n"}
\print{"ratio2= ",@ratio2,"\n"}
\print{"factoresParticipacionModal: ",@factoresParticipacionModal,"\n"}
\print{"masasModalesEfectivas: ",@masasModalesEfectivas,"\n"}
\print{"masaTotal: ",@masaTotal,"\n"}
\print{"ratio3= ",@ratio3,"\n"}
\print{"factoresDistribucion: ",@factoresDistribucion,"\n"}
\print{"ratio4= ",@ratio4,"\n"}
\print{"\n  carga estática equivalente modo 1: ",@cargaModo1,"\n"}
\print{"  carga estática equivalente modo 2: ",@cargaModo2,"\n"}
\print{"  carga estática equivalente modo 3: ",@cargaModo3,"\n"}
\print{"ratio5= ",@ratio5,"\n"}
\c{
}

\if((@ratio1<1e-3) & (@ratio2<5e-2) & (@ratio3<1e-2) & (@ratio4<5e-2) & (@ratio5<5e-2))
  {
    \then{\print{"test analisis modal B: pasa.\n"}}
    \else{\print{"test analisis modal B: falla.\n"}}
 }