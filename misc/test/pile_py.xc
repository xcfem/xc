\mdlr
  {
    \c{Definimos materiales}
    \materiales
      {
        \elastic_material[1,elast]{\E{3000}}
        \py_simple1[1,py1]{\clay{} \Cd{0.1} \pult{10} \y50{0.05}}
      }

    \c{Definimos nodos}
    \ncoo_def_nod{2} \c{Dimensiones de los nodos}
    \ngdl_def_nod{3} \c{Grados de libertad de los nodos}
    \nod_semilla[0,3]{\coo{0.0,0.0}}
    \c{Definimos transformaciones geométricas}
    \geom_transf
      {
        \linear2d[0,lin]{}
      }
    \elem_semilla{ \elastic_beam_2d[1,lin]{ \A{0.017} \E{2e9} \I{0.0005} \h{1.0} } }
    \cad
      {
        \unif_grid[pilote]
          {
            \org{ \x{0.0} \y{6} \z{0} }
            \Lx{1} \Ly{-6} \Lz{1}
            \ndiv_x{0} \ndiv_y{6} \ndiv_z{0}
            \malla{}
            \crea_set_fila[1,1,1:5,tramo_enterr]
              {
                \print{"núm. nodos: ",@nnod,"\n"}
                \for_each_nod
                  {
                    \print{"  nodo: ",@tag," x= ",@coord[0],", y= ",@coord[1],"\n"}
                  }
              }
          }
        \unif_grid[terreno]
          {
            \org{ \x{0.0} \y{4} \z{0} }
            \Lx{1} \Ly{-4} \Lz{1}
            \ndiv_x{0} \ndiv_y{4} \ndiv_z{0}
            \crea_nodos{}
          }
      }
    \elem_semilla{ \zero_length[1,py1,2,1]{\print{"  dimensión: ",@dim,"\n"} } } \c{\zero_length[tag_elem,nmb_mat,dim_elem,dir]}
    \nverborrea{3}
    \cose(tramo_enterr,terreno){}
    \nverborrea{0}
    \dom
       {
    
          \print{"núm. nodos: ",@nnod,"\n"}
          \nods
            {
              \for_each
                {
                  \print{"  nodo: ",@tag," x= ",@coord[0],", y= ",@coord[1],"\n"}
                }
            }
          \print{"núm. elementos: ",@nelem,"\n"}
          \elems
            {
              \for_each
                {
                  \print{"  elem: ",@tag," tipo: ",@tipo," nod. I: ",@nod[0].tag," nod. J: ",@nod[1].tag,"\n"}
                }
            }
       }

  }