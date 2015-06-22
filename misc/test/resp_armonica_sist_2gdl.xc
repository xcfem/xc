\c{Test de elaboración propia}
\expr{k1= 1.0e4} \c{Rigidez del muelle 1}
\expr{k2= 1.5e4} \c{Rigidez del muelle 2}
\expr{l= 1} \c{Distancia entre nodos}
\expr{Fo= 1} \c{Amplitud de la fuerza}

\c{Definición del modelo}
\mdlr
  {
    \c{Definimos nodos}
    \ncoo_def_nod{2}
    \c{ sintaxis: nod[tag,num_gdl] }
    \nod[1,2]{\coo{0,0} }
    \nod[2,2]{\coo{l,0.0} \mass{[[1,0][0,1]]}} \c{Coordenadas del nodo y su matriz de masas}
    \nod[3,2]{\coo{2*l,0.0} \mass{[[1,0][0,1]]}} \c{Coordenadas del nodo y su matriz de masas}
    \c{fin de la definición de nodos}
    
    \c{Definimos materiales}
    \materiales{ \elastic_material[k1,1]{\E{k1}} }
    \materiales{ \elastic_material[k2,1]{\E{k2}} }
    
    \c{Definimos elementos}
    \elementos
      {
        \c{ sintaxis: muelle[tag,nmb_mat,dim] }
        \muelle[1,k1,2]{ \nodes{1,2} }
        \muelle[2,k2,2]{ \nodes{2,3} }
      }
    \c{fin de la definición de elementos}
    
    \c{Condiciones de contorno}
    \constraints
      {
        \c{ sintaxis: fix[tag_nodo,tag_gdl,<tag>] }
        \fix[1,0] { \valor{0.0} } \c{Nodo 1}
        \fix[1,1] { \valor{0.0} }
        \fix[2,1] { \valor{0.0} } \c{Nodo 2}
        \fix[3,1] { \valor{0.0} } \c{Nodo 3}
      }

    \c{Definimos cargas}
    \loads
      {
        \constant_ts[ts]{ \factor{1.0} } \c{Time series: constant_ts[nombre]{factor}}
        \load_pattern[0,ts]{} \c{load_pattern[tag,nmb_time_series]{}}
        \nodal_load[2,1,0]{ \val{F,0} } \c{nodal_load[idNodo,tag,idLoadPattern]} \c{Carga sobre nodo}
        \add_to_domain{0} \c{Añadimos la hipótesis al dominio}
      }
  }

\c{Procedimiento de solución}
\sol_proc
  {
    \linear_soln_algo{}
    \load_control_integrator[1.0,1,1.0,1.0]{}
    \penalty_constraint_handler[1.0e12,1.0e12]{}
    \rcm_numberer{}
    \band_spd_lin_soe{\band_spd_lin_lapack_solver{}}
    \static_analysis{ \analyze{1} }
  }

\mdlr
  {
    \dom
       {
          \calculate_nodal_reactions{1}
          \nods
            {
              \nodo[2]{\expr{deltax= @commit_disp[0]} \expr{deltay= @commit_disp[1]} }
              \nodo[1]{\expr{R= @reac[0]} }
            }
         \elems
           {
             \elemento[1]
               {
                 \calc_resisting_force{}
                 \expr{Ax= @strain} \c{Alargamiento del muelle}
               }
           }
       }
    \expr{ratio1= -R/F}
    \expr{ratio2= (K*deltax)/F}
    \expr{ratio3= deltax/Ax}
\c{
    \print{"R= ",$R,"\n"}
    \print{"dx= ",$deltax,"\n"}
    \print{"dy= ",$deltay,"\n"}
    \print{"Ax= ",$Ax,"\n"}
    \print{"ratio1= ",$tonum(ratio1),"\n"}
    \print{"ratio2= ",$tonum(ratio2),"\n"}
    \print{"ratio3= ",$tonum(ratio3),"\n"}
  }  
    \if
      {
        \cond{(abs(ratio1-1.0)<1e-5) and (abs(ratio2-1.0)<1e-5) and (abs(ratio3-1.0)<1e-5)}
        \then{\print{"test muelle_01: pasa.\n"}}
        \else{\print{"test muelle_01: falla.\n"}}
      }
  }