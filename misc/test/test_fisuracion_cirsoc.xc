\c{Test de funcionamiento de la comprobación a fisuración de una sección de hormigón armado.
   Los resultados se comparan con los obtenidos del «Prontuario informático del hormigón armado».}

\c{Macros}
\path{"/usr/local/lib/macros_xc"}
\incluye{"materiales/ehe/auxEHE.xcm"}
\incluye{"misc/banco_pruebas_scc3d.xcm"}
\incluye{"solution/simple_newton_raphson_band_gen.xcm"} \c{Procedimiento de solución}


\incluye{"materiales/ehe/hormigonesEHE.xcm"}
\incluye{"materiales/ehe/acerosEHE.xcm"}
\incluye{"materiales/ehe/fisuracionEHE.xcm"}
\incluye{"modelo/fija_nodo_6gdl.xcm"}

\c{Coeficientes de seguridad.}
\expr{gammac= 1.5} \c{Coeficiente de minoración de la resistencia del hormigón.}
\expr{gammas= 1.15} \c{Coeficiente de minoración de la resistencia del acero.}

\def_prop["HA20","record"]{}
\HA20
  {
    \def_prop["fck","double"]{-20e6} \c{Resistencia característica a compresión del hormigón (en Pa).}
  }
\setupHormigon("HA20",gammac){}

\def_prop["ADN420","record"]{}
\ADN420
  {
    \def_prop["fyk","double"]{420e6}
    \def_prop["emax","double"]{0.074} \c{Deformación máxima (0.08 para B400S y 0.05 para B500S)}
  }
\setupAcero("ADN420",gammas){}

\expr{ancho= 0.25} \c{Ancho de la sección expresado en metros.}
\expr{canto= 0.25} \c{Canto de la sección expresado en metros.}
\expr{recub= 0.0554} \c{Recubrimiento de la sección expresado en metros.}
\expr{areaFi20=3.14e-4}
\expr{areaFi25=4.91e-4}

\expr{NDato= 550e3+300e3/2} \c{Axil para comprobar fisuración.}
\expr{MyDato= 1e3} \c{Momento para comprobar fisuración.}
\expr{MzDato= 0} \c{Momento para comprobar fisuración.}

\mdlr
  {
    \c{Definimos materiales}
    \materiales
      {
        \expr{tag= @defDiagKHormigon("HA20")}
        \expr{tag= @defDiagKAcero("ADN420")}
        \geom_secc["geomSecHA"]
          {
            \c{Hormigón}
            \regiones{\reg_cuad[@HA20.nmbDiagK]
              {
                \nDivIJ{10}
                \nDivJK{10}
                \pMin{-ancho/2,-canto/2}
                \pMax{ancho/2,canto/2}
              }}
            \c{Armadura}
            \armaduras
              {
                \capa_armadura_recta[@ADN420.nmbDiagK]
                  {
                    \numReinfBars{2}
                    \barArea{areaFi25}
                    \p1{recub-ancho/2,recub-canto/2}
                    \p2{ancho/2-recub,recub-canto/2}
                  }
                \capa_armadura_recta[@ADN420.nmbDiagK]
                  {
                    \numReinfBars{2}
                    \barArea{areaFi20}
                    \p1{recub-ancho/2,0}
                    \p2{ancho/2-recub,0}
                  }
                \capa_armadura_recta[@ADN420.nmbDiagK]
                  {
                    \numReinfBars{2}
                    \barArea{areaFi20}
                    \p1{0,recub-canto/2}
                    \p2{0,canto/2-recub}
                  }
                \capa_armadura_recta[@ADN420.nmbDiagK]
                  {
                    \numReinfBars{2}
                    \barArea{areaFi25}
                    \p1{recub-ancho/2,canto/2-recub}
                    \p2{ancho/2-recub,canto/2-recub}
                  }
               }
          }
        \fiber_section_3d["secHA"]
          {
            \def_section_repr{ \geom{"geomSecHA"}}
          }
      }
    \modeloSecc3d("secHA"){}
    \c{Condiciones de contorno}
    \constraints
      {
        \fijaNodo6GDL(1){}
        \Nodo6GDLMovXGirosYZLibres(2){}
      }
    \c{Definimos cargas}
    \loads
      {
        \constant_ts["ts"]{ \factor{1.0} } \c{Time series: constant_ts[nombre]{factor}}
        \set_current_time_series{"ts"}
        \load_pattern["0"]{}
        \set_current_load_pattern{"0"}
        \nodal_load{ \nod{2} \val{NDato,0,0,0,MyDato,MzDato} }
        \add_to_domain{"0"} \c{Añadimos la hipótesis al dominio}
      }
  }

\c{Procedimiento de solución}
\sol_proc
  {
    \static_analysis["smt"]{ \analyze{10} \expr{analOk= @result} }
  }


\def_prop["secHAParamsFis","record"]{}
\defParamsFis("secHAParamsFis"){}


\mdlr
  {
    \dom
       {
         \elementos
           {
             \elemento[1]
               {
                 \seccion
                   {
                     \calcApertCaracFis("secHAParamsFis",@HA20.tagDiagK,@ADN420.tagDiagK,@HA20.fctm){}
                   }
               }
           }
       }
  }


\secHAParamsFis
  {
    \expr{ratio1= @tonum((@sepBarrasTracc-0.15)/0.15)}
    \expr{ratio2= @tonum((@Wk-0.184632e-3)/0.184632e-3)}
  }

    \printParamFis("secHAParamsFis"){}
    \print{"ratio1= ",ratio1,"\n"}
    \print{"ratio2= ",ratio2,"\n"}
\c{
}


\if
  {
    \cond{(@abs(ratio1)<1e-5)}
    \then{\print{"test fisuracion_03: pasa.\n"}}
    \else{\print{"test fisuracion_03: falla.\n"}}
  }
