
\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_3gdl.xcm"}

\expr{NumDiv= 10}
\expr{CooMax= 100}
\def_prop["Px","double"]{250}
\def_prop["Py","double"]{10000}
\def_prop["A","double"]{10*10}
\def_prop["E","double"]{200e3}
\def_prop["I","double"]{1/12*10^4}
\def_prop["nu","double"]{0.3}
\def_prop["G","double"]{@E/(2*(1+@nu))}

\def_prop["nodExtremo","double"]{0}
\def_prop["nodArranque","double"]{0}

\mdlr
  {
    \nodos
      {
        \ncoo_def{2} \c{Dimensiones de los nodos}
        \ngdl_def{3} \c{Grados de libertad de los nodos}
        \nod_semilla[0]{\coo{0.0,0.0}}
      }

    \c{Definimos transformaciones geométricas}
    \geom_transf
      {
        \corot2d["lin"]{}
      }
    
    \materiales
      {
        \elasticShearSection2d["seccion"]{\paramSeccion{ \A{@A} \E{@E} \G{@G} \I{@I} \alpha{5/6} }
      }
    \c{Definimos elemento semilla}
    \elementos
      {
        \elem_semilla
          {
            \nmb_transf{"lin"}

            \c{\elastic_beam_2d[1]{\print{"I= ",@I,"\n"} \A{100} \E{200e3} \I{@I} \h{10}}}
            \nmb_material{"seccion"}
            \num_sec{3}
            \force_beam_column_2d[1]{}
          }
      }
    \c{fin de la definición de elementos}

    \cad
      {
        \pnt[1]{ \pos{ \x{0.0} \y{0.0}} }
        \pnt[2]{ \pos{ \x{0.0} \y{CooMax} } }
        \linea[1]
          {
            \p1{1} \p2{2} \ndiv{NumDiv}
          }
      }
    \sets{ \total{\malla{}} }

    \cad
      {
        \c{Definimos condiciones de contorno.}
        \pnt[1] { \set{nodArranque= @tag_nodo} }
        \pnt[2] { \set{nodExtremo= @tag_nodo} }
      }

    \constraints{\fijaNodo3GDL(@nodArranque){}}

    \loads
      {
        \linear_ts["ts"]{ \factor{1.0} }
        \set_current_time_series{"ts"}
        \load_pattern["0"]{} \c{load_pattern[codigo]{}}
        \set_current_load_pattern{"0"}
        \nodal_load{ \nod{@nodExtremo} \val{-@Px,-@Py,0} }
        \add_to_domain{"0"} \c{Añadimos la hipótesis (load_pattern) al dominio}
      }
  }
\expr{Nstep= 10}  		\c{ apply gravity in 10 steps}
\expr{DInc= 1./Nstep} 	\c{ first load increment}
\sol_proc
  {
    \control
      {
        \solu_model["sm"]
          {
            \plain_handler{}
            \simple_numberer{}
          }
        \solu_method["smt","sm"]
          {
            \newton_raphson_soln_algo{}
            \c{\norm_disp_incr_conv_test{ \tol{1.0e-6} \print_flag{6} \max_num_iter{1000}}}
            \norm_unbalance_conv_test{ \tol{1.0e-6} \print_flag{5} \max_num_iter{1000}}
            \c{\load_control_integrator[dLambda1,<Jd,minLambda,maxLambda>]{}}
            \load_control_integrator{\dLambda1{DInc} }
            \band_gen_lin_soe{\band_gen_lin_lapack_solver{}}
          }
      }
    \static_analysis["smt"]{ \analyze{Nstep} \expr{analOk= @result} }
  }
\mdlr
  {
    \dom
       {
         \calculate_nodal_reactions{}
         \nodos
           {
             \nodo[@nodExtremo]{\disp{\expr{deltax= @commit_disp[0]} \expr{deltay= @commit_disp[1]}} }
             \nodo[@nodArranque]{\expr{RX= @reac[0]}\expr{RY= @reac[1]}\expr{MZ= @reac[2]} }
           }
         \elementos
           {
             \elemento[NumDiv-1]
               {
                 \calc_resisting_force{}
\coordTransf
  {
    \print{"alpha: ",@rad2deg(@alpha),"\n"}
    \print{"theta: ",@rad2deg(@theta),"\n"}
  }
\nverborrea{0}
\print{"Resisting force: ",@getResistingForce,"\n"}
               }
             \elemento[0]
               {
                 \calc_resisting_force{}
\print{"Resisting force: ",@getResistingForce,"\n"}
                 \secciones
                   {
                     \seccion[0]
                       {
                         \expr{V= @Vy}
                         \expr{N0= @N}
                         \expr{M= @Mz}
\print{"Axil: ",@N,"\n"}
\print{"Momento: ",@Mz,"\n"}
\print{"Cortante: ",@Vy,"\n"}
                       }
                   }
               }
           }
       }
  }

\expr{deltaXLinTeor= @tonum(-@Px*CooMax^3/3/@E/@I)}
\expr{deltaYLinTeor= @tonum(@Py*CooMax/@E/@A)}
\expr{dif= @tonum(deltaXLinTeor-deltax)}
\expr{MZ1Teor= @tonum(-@Px*(CooMax-deltay))}
\expr{MZ2Teor= @tonum(@Py*deltax)}
\expr{MZTeor= @tonum(MZ1Teor+MZ2Teor)}
\expr{ratioM= @tonum((MZTeor-MZ)/MZTeor)}
\print{"analOk= ",@tonum(analOk),"\n"}
\print{"deltax= ",@tonum(deltax),"\n"}
\print{"deltaXLinTeor= ",deltaXLinTeor,"\n"}
\print{"dif= ",dif,"\n"}
\print{"deltay= ",@tonum(deltay),"\n"}
\print{"deltaYLinTeor= ",deltaYLinTeor,"\n"}
\print{"RX= ",@tonum(RX),"\n"}
\print{"RY= ",@tonum(RY),"\n"}
\print{"MZ= ",@tonum(MZ),"\n"}
\print{"MZ1Teor= ",@tonum(MZ1Teor),"\n"}
\print{"MZ2Teor= ",@tonum(MZ2Teor),"\n"}
\print{"MZTeor= ",@tonum(MZTeor),"\n"}
\print{"ratioM= ",@tonum(ratioM),"\n"}
