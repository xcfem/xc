\c{ --------------------------------------------------------------------------------------------------}
\c{ Example 2. 2D cantilever column, static pushover}
\c{			Silvia Mazzoni & Frank McKenna, 2006}

\c{    ^Y}
\c{    |}
\c{    2       __ }
\c{    |          | }
\c{    |          |}
\c{    |          |}
\c{  (1)       LCol}
\c{    |          |}
\c{    |          |}
\c{    |          |}
\c{  =1=      _|_  -------->X}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_3gdl.xcm"}

\c{ SET UP ----------------------------------------------------------------------------}
\c{ units: kip, inch, sec}

\c{ create data directory}
\def_prop["dataDir","string"]{"verif/PushData"}
\if
  {
    \cond{!@existeArchivo(@dataDir)}
    \then{\sys{"mkdir "+@dataDir} }
  }
\mdlr
  {
    \nodos
      {
        \ncoo_def{2} \c{Dimensiones de los nodos}
        \ngdl_def{3} \c{Grados de libertad de los nodos}
      }
  }

\c{ define GEOMETRY -------------------------------------------------------------}
\expr{LCol= 432} \c{ column length}
\expr{Weight= 2000} 		\c{ superstructure weight}
\c{ define section geometry}
\expr{HCol= 60} 		\c{ Column Depth}
\expr{BCol= 60}		\c{ Column Width}

\c{ calculated parameters}
\expr{PCol= Weight} 		\c{ nodal dead-load weight per column}
\expr{g= 386.4}			\c{ g.}
\expr{Mass= @tonum(PCol/g)}		\c{ nodal mass}
\c{ calculated geometry parameters}
\expr{ACol= @tonum(BCol*HCol*1e6)} \c{ cross-sectional area, make stiff}
\expr{IzCol= @tonum(1./12.*BCol*@pow(HCol,3))} \c{ Column moment of inertia}

\c{ nodal coordinates:}
\mdlr
  {
    \nodos
      {
        \nod[1]{\coo{0,0}}
        \nod[2]
          {
            \coo{0,LCol}
            \mass{[[Mass,0,0],[0,Mass,0],[0,0,1e-9]]}
          }
      }
    \constraints{\fijaNodo3GDL(1){}}
  }

\c{ Define ELEMENTS -------------------------------------------------------------}
\c{ Material parameters}
\expr{fc= -4.} 		\c{ CONCRETE Compressive Strength (+Tension, -Compression)}
\expr{Ec= 1.17253926926*57*@sqrt(-fc*1000)} 	\c{ Concrete Elastic Modulus (the term in sqr root needs to be in psi)}

\c{ define geometric transformation: performs a linear geometric transformation of beam stiffness and resisting force from the basic system to the global-coordinate system}
\expr{ColTransfTag= 1} 			\c{ associate a tag to column transformation}
\mdlr
  {
    \geom_transf
      {
        \linear2d["lin"]{}
      }
    \elementos
      {
        \nmb_transf{"lin"} \c{Transformación de coordenadas para los nuevos elementos}
        \c{ sintaxis: elastic_beam_3d[<tag>] }
        \elastic_beam_2d[1]{ \nodes{1,2} \A{ACol} \E{Ec} \I{IzCol} }
      }
  }

\c{ Define RECORDERS -------------------------------------------------------------}
\nuevo_archivo_salida["DFree"]{@dataDir + "/DFree.out"} \c{Desplazamiento del nodo libre}
\mdlr
  {    
    \dom
      {
        \prop_recorder
          {
            \nodos{2}
            \callback_record
              {
                \disp{\expr{d0= @commit_disp[0]} \expr{d1=  @commit_disp[1]} \expr{d2=  @commit_disp[2]}}
                \print["DFree"]{@committedTime,",",@tonum(d0),",",@tonum(d1),",",@tonum(d2),"\n"}
              }
            \callback_restart{\print["DFree"]{"Se llamó al método restart.\n"}}
          }
      }
  }
\nuevo_archivo_salida["DBase"]{@dataDir + "/DBase.out"} \c{Desplazamiento del nodo soporte}
\mdlr
  {    
    \dom
      {
        \prop_recorder
          {
            \nodos{1}
            \callback_record
              {
                \disp{\expr{d0= @commit_disp[0]} \expr{d1=  @commit_disp[1]} \expr{d2=  @commit_disp[2]}}
                \print["DBase"]{@lastCommitTag,",",@lastTimeStamp,",",@tonum(d0),",",@tonum(d1),",",@tonum(d2),"\n"}
              }
            \callback_restart{\print["DBase"]{"Se llamó al método restart.\n"}}
          }
      }
  }
\nuevo_archivo_salida["RBase"]{@dataDir + "/RBase.out"} \c{Reacciones en el nodo soporte}
\mdlr
  {    
    \dom
      {
        \callback_commit{\calculate_nodal_reactions{1}}
        \prop_recorder
          {
            \nodos{1}
            \callback_record
              {
                \expr{r0= @reac[0]} \expr{r1=  @reac[1]} \expr{r2=  @reac[2]}
                \print["RBase"]{@currentTime,",",@tonum(r0),",",@tonum(r1),",",@tonum(r2),"\n"}
              }
            \callback_restart{\print["RBase"]{"Se llamó al método restart.\n"}}
          }
      }
  }

\nuevo_archivo_salida["FCol"]{@dataDir + "/FCol.out"} \c{Reacciones en el nodo soporte}
\mdlr
  {    
    \dom
      {
        \callback_commit{\calculate_nodal_reactions{1}}
        \prop_recorder
          {
            \elementos{1}
            \callback_record
              {
                \print["FCol"]{@currentTime,",",@getResistingForce,"\n"}
              }
            \callback_restart{\print["FCol"]{"Se llamó al método restart.\n"}}
          }
      }
  }
\c{
recorder Drift -file Data/Drift.out -time -iNode 1 -jNode 2 -dof 1   -perpDirn 2 ;	\c{ lateral drift}}

\c{ define GRAVITY -------------------------------------------------------------}
\mdlr
  {
    \loads
      {
        \linear_ts["ts"]{ \factor{1.0} } \c{Time series: linear_ts[nombre]{factor}}
        \set_current_time_series{"ts"}
        \load_pattern["0"]{} \c{load_pattern[codigo]{}}
        \set_current_load_pattern{"0"}
        \nodal_load{ \nod{2} \val{0,-PCol,0} }
        \add_to_domain{"0"} \c{Añadimos la hipótesis al dominio}
      }
  }



\c{ ------------------------------------------------- apply gravity load}
\expr{Tol= 1.0e-8}			\c{ convergence tolerance for test}
\expr{NstepGravity= 10}  		\c{ apply gravity in 10 steps}
\expr{DGravity= 1./NstepGravity} 	\c{ first load increment}
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
            \norm_disp_incr_conv_test{ \tol{Tol} \print_flag{6} \max_num_iter{10}}
            \c{\load_control_integrator[dLambda1,<Jd,minLambda,maxLambda>]{}}
            \load_control_integrator{\dLambda1{DGravity} }
            \band_gen_lin_soe{\band_gen_lin_lapack_solver{}}
          }
      }
    \static_analysis["smt"]{ \analyze{NstepGravity} \expr{analOk= @result} }
  }



\mdlr{\dom{\set_load_const{0.0}}} \c{maintain constant gravity loads and reset time to zero}

\print{"Model Built\n"}

\c{ STATIC PUSHOVER ANALYSIS --------------------------------------------------------------------------------------------------}

\c{ we need to set up parameters that are particular to the model.}
\expr{IDctrlNode= 2}			\c{ node where displacement is read for displacement control}
\expr{IDctrlDOF= 0}			\c{ degree of freedom of displacement read for displacement contro}
\expr{Dmax= 0.01*LCol}	\c{ maximum displacement of pushover. push to 10% drift.}
\expr{Dincr= 0.001*LCol} \c{ displacement increment for pushover. you want this to be very small, but not too small to slow down the analysis}

\c{ create load pattern for lateral pushover load}
\expr{Hload= Weight}\c{ define the lateral load as a proportion of the weight so that the pseudo time equals the lateral-load coefficient when using linear load pattern}
\print{"Hload= ",Hload,"\n"}
\mdlr
  {
    \loads
      {
        \load_pattern["200"]{}	\c{ define load pattern -- generalized}
	\nodal_load{ \nod{2} \val{Hload,0.0,0.0}} \c{ define lateral load in static lateral analysis}
        \add_to_domain{"200"} \c{Añadimos la hipótesis al dominio}
      }
  }

\expr{Tol= 1.e-8} \c{ Convergence Test: tolerance}
\expr{maxNumIter= 6} \c{ Convergence Test: maximum number of iterations that will be performed before "failure to converge" is returned}
\expr{printFlag= 0} \c{ Convergence Test: flag used to print information on convergence (optional)        # 1: print information on each step }

\sol_proc
  {
    \control
      {
        \solu_method["smt","sm"]
          {
            \displacement_control_integrator{\nod{IDctrlNode} \dof{IDctrlDOF} \dU1{0.1} }
          }
      }
  }


\expr{Nsteps= @int(Dmax/Dincr)}        \c{ number of pushover analysis steps}
\c{  ---------------------------------    perform Static Pushover Analysis}
\sol_proc
  {
    \static_analysis["smt"]{ \analyze{1000} \expr{analOk= @result} }
  }





\c{ ---------------------------------- in case of convergence problems}
\if
  {
    \cond{analOk != 0}
    \then{\print{"Fallo\n"}
  }
\print{"DonePushover\n"}
\cierra_archivo_salida{"DFree"}
\cierra_archivo_salida{"DBase"}
\cierra_archivo_salida{"RBase"}
\cierra_archivo_salida{"FCol"}
