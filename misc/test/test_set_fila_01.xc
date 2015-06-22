\nverborrea{0}
\expr{NumDivI= 3}
\expr{NumDivJ= 2}
\expr{CooMax= 10}
\expr{E= 30e6} \c{Módulo elástico en psi}
\expr{nu= 0.3} \c{Coeficiente de Poison}
\expr{rho= 0.0} \c{Densidad}
\mdlr
  {
    \c{Definimos materiales}
    \materiales{ \elastic_material["elast",1]{\E{3000}} }

    \nodos
      {

        \ncoo_def{2} \c{2 coordenadas (x,y) por nodo.}
        \ngdl_def{2} \c{2 gdl por nodo: (ux,uy) }
        \nod_semilla[0]{\coo{0.0,0.0}}
      }
    \c{Definimos materiales}
    \materiales{\elastic_isotropic_plane_stress_2d["elast2d"]{ \E{E} \nu{nu} \rho{rho}} }
    \elementos
      {
        \elem_semilla
          {
            \nmb_material{"elast2d"}
            \quad4n{}
          }
      }
    \cad
      {
        \pnt[1]{ \pos{ \x{0.0} \y{0.0} \z{0.0}} }
        \pnt[2]{ \pos{ \x{CooMax} \y{0.0} \z{0.0}} }
        \pnt[3]{ \pos{ \x{CooMax} \y{CooMax} \z{0.0}} }
        \pnt[4]{ \pos{ \x{0.0} \y{CooMax} \z{0.0}} }
        \sup_cuadrilatera[1]
          {
            \def_pnts{1,2,3,4}
            \ndivI{NumDivI}
            \ndivJ{NumDivJ}
          }
      }

    \sets{ \f1{\malla{}} }


    \cad
      {
        \sup_cuadrilatera[1]
          {
            \expr{nnodCuadr= @getNumNodos}
            \expr{nelemCuadr= @getNumElementos}
            \crea_set_fila[[1,1],[1,3],[1,1],"fila1"]
              {
                \print{"núm. nodos: ",@nnod,"\n"}
                \for_each_nod
                  {
                    \print{"  nodo: ",@tag," x= ",@coord[0],", y= ",@coord[1],"\n"}
                  }
              }
            \crea_set_fila[[2,1],[3,1],[1,1],"fila2"]
              {
                \print{"núm. nodos: ",@nnod,"\n"}
                \for_each_nod
                  {
                    \print{"  nodo: ",@tag," x= ",@coord[0],", y= ",@coord[1],"\n"}
                  }
              }
          }
      }
