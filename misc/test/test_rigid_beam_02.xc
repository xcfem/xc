\c{Test de elaboración propia}
\c{Barra rígida entre dos nodos.}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_6gdl.xcm"}

\c{Geometría}
\expr{L= 15} \c{Longitud de la barra expresada en metros}

\c{Carga}
\expr{F= 1.5e3} \c{Magnitud de la carga en kN}

\mdlr
  {    
    \nodos
      {

        \ncoo_def{3} \c{3 coordenadas (x,y,z) por nodo.}
        \ngdl_def{6} \c{6 gdl por nodo: (ux,uy,uz,thetax,thetay,thetaz) }
        \c{ sintaxis: nod[tag,num_gdl] }
        \nod[1]{\coo{0,0.0,0.0}}
        \nod{\coo{L,0.0,0.0}}
      }

    \c{Condiciones de contorno}
    \constraints
      {
        \fijaNodo6GDL(1){}
        \add_rigid_beam
          {
            \master_node{1}
            \slave_node{2}
          }
      }
    \c{Definimos cargas}
    \loads
      {
        \constant_ts["ts"]{ \factor{1.0} } \c{Time series: constant_ts[nombre]{factor}}
        \set_current_time_series{"ts"}
        \load_pattern["0"]{} \c{load_pattern[codigo]{}}
        \set_current_load_pattern{"0"}
        \nodal_load{ \nod{2} \val{F,0,0,0,0,0} } \c{nodal_load[<tag>]} \c{Carga sobre nodo}
        \add_to_domain{"0"} \c{Añadimos la hipótesis al dominio}
      }
  }

\c{Procedimiento de solución}
\sol_proc
  {
    \incluye{"solution/solucion1.xcm"}
  }

\mdlr
  {    
    \dom
       {
         \calculate_nodal_reactions{1}
         \nodos
           { 
             \nodo[1]{\disp{\expr{RX= @reac[0]}\expr{RY= @reac[1]}\expr{RZ= @reac[2]}} }
           }
       }
    \expr{ratio1= @abs(RX+F)/F}

    \print{"RX= ",RX,"\n"}
    \print{"RY= ",RY,"\n"}
    \print{"RZ= ",RZ,"\n"}
    \print{"ratio1= ",@tonum(ratio1),"\n"}
\c{
  }

    \if
      {
        \cond{ratio1<1e-11}
        \then{\print{"test rigid_beam_02: pasa.\n"}}
        \else{\print{"test rigid_beam_02: falla.\n"}}
      }
  }