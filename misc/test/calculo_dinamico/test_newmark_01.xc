\c{Test tomado de un documento anónimo sobre cálculo dinámico.}

\path{"/usr/local/lib/macros_xc"}

\expr{m= 20/32.2/12} \c{Masa colocada sobre el muelle.}
\expr{k= 4.26} \c{Rigidez del muelle.}

\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales2D.xci"}
\incluye{"gnuplot/gnuplot_utils.xcm"}
\mdlr
  {    
    \nodos
      {
        \ncoo_def{1}
        \ngdl_def{1} \c{1 gdl por nodo: (ux) }
        \nod[1]{\coo{0}}
        \nod{\coo{0} \mass{[[m]]}}
      }
    
    \c{Definimos materiales}
    \materiales{ \elastic_material["elast"]{\E{k}} }
    
    \c{Definimos elementos}
    \elementos
      {
        \nmb_material{"elast"}
        \dim_elem{1}
        \c{ sintaxis: zero_length[<tag>] }
        \elem_zero_length[1]{ \nodes{1,2} }
      }
    \c{fin de la definición de elementos}
    
    \c{Condiciones de contorno}
    \constraints
      {
        \c{ sintaxis: fix[tag_nodo,tag_gdl,<tag>] }
        \fix[1,0] { \valor{0.0} } \c{Nodo 1}
      }

    \loads
      {
        \casos
          {
            \triangular_ts["ts"]{ \factor{1.0} \start{0.0} \finish{0.35} \period{1.4} \shift{0.35}}
            \set_current_time_series{"ts"}
            \load_pattern["0"]{}
            \set_current_load_pattern{"0"}
            \nodal_load{ \nod{2} \val{2} }
            \add_to_domain{"0"} \c{Añadimos la hipótesis al dominio}
\c{Imprimimos la carga.}
            \plotTimeSerie("ts","load.ps"){}
\sys{"gv load.ps &"}
          }
      }
\c{ Define RECORDERS -------------------------------------------------------------}
\nuevo_archivo_salida["DFree"]{"DFree.out"} \c{Desplazamiento del nodo libre}
    \dom
      {
        \prop_recorder
          {
            \nodos{2}
            \callback_record
              {
                \disp{\expr{d0= @commit_disp[0]}}
                \print["DFree"]{@committedTime," ",@tonum(d0),"\n"}
              }
            \callback_restart{\print["DFree"]{"Se llamó al método restart.\n"}}
          }
      }
  }

\sol_proc
  {
    \clearAll{}
    \control
      {
        \solu_model["sm"]
          {
            \default_numberer{\simple{}}
            \plain_handler{}
          }
        \solu_method["smt","sm"]
          {
            \newton_raphson_soln_algo{}
            \norm_disp_incr_conv_test{ \tol{1e-3} \print_flag{0} \max_num_iter{10}}
            \newmark_integrator
              {
                \gamma{0.5} \beta{1/4}
                \print{"beta= ",@getBeta,"\n"}
                \print{"gamma= ",@getGamma,"\n"}
                \print{"displ= ",@getDispl,"\n"}
                \factors
                  {
                    \print{"alphaM= ",@getAlphaM,"\n"}
                    \print{"betaK= ",@getBetaK,"\n"}
                    \print{"betaK0= ",@getBetaK0,"\n"}
                    \print{"betaKc= ",@getBetaKc,"\n"}
                  }
              }
            \band_gen_lin_soe{\band_gen_lin_lapack_solver{}}
          }
      }
    \direct_integration_analysis["smt"]{ \analyze{100,0.005} \expr{analOk= @result} }
  }

\c{Imprimimos el movimiento del nodo2.}
\cierra_archivo_salida{"DFree"}
\plotDataFile("./DFree.out","dfree.ps","x"){}
\sys{"gv dfree.ps &"}

