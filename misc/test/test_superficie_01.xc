\expr{R= 2.0}
\expr{cos45= @cos(@deg2rad(45))}
\expr{sin45= cos45}
\expr{E= 30e6} \c{Módulo elástico en psi}
\expr{nu= 0.3} \c{Coeficiente de Poison}
\expr{rho= 0.0} \c{Densidad}

\def_prop["area","double"]{0}

\mdlr
  {
    \materiales
      {
        \elastic_material["elast",1]{\E{3000}}
        \elastic_membrane_plate_section["prueba"]{ \nu{nu} \E{E} \h{0.25} \rho{rho} }
      }
    \nodos{ \nod_semilla[0]{\coo{0,0,0}} }
    \elementos
      {
        \elem_semilla
          {
            \nmb_material{"prueba"}
            \elem_shell_mitc4[1]{}
          }
      }
    \cad
      {
        \pnt[1]{ \pos{ \x{R} \y{0.0} \z{0.0}} }
        \pnt{ \pos{ \x{(R*cos45)} \y{(R*sin45)} \z{0.0}} }
        \pnt{ \pos{ \x{0.0} \y{R} \z{0.0}} }
        \pnt{ \pos{ \x{R} \y{0.0} \z{0.5}} }
        \pnt{ \pos{ \x{(R*cos45)} \y{(R*sin45)} \z{0.5}} }
        \pnt{ \pos{ \x{0.0} \y{R} \z{0.5}} }
        \pnt{ \pos{ \x{R} \y{0.0} \z{1.0}} }
        \pnt{ \pos{ \x{(R*cos45)} \y{(R*sin45)} \z{1.0}} }
        \pnt{ \pos{ \x{0.0} \y{R} \z{1.0}} }

        \sup_cuadrilatera
          {
            \def_matriz_pnts{[[1,2,3],[4,5,6]]}
            \ndivI{10}  \ndivJ{1}
          }
        \sup_cuadrilatera
          {
            \def_matriz_pnts{[[4,5,6],[7,8,9]]}
            \ndivI{10}  \ndivJ{1}
          }
      }
    \sets{ \total{\superficies{\for_each{\malla{}} }}}
    \dom
      {
        \elementos
          {\for_each{\set{area= @area+@getArea}}}
      }
  }

\def_prop["ratio1","double"]{(@area-@PI)/@PI}

\print{"area= ",@area,"\n"}
\print{"ratio1= ",@ratio1,"\n"}

\if(@ratio1<=5e-5)
  {
    \then{\print{"test malla superficies 05: pasa.\n"}}
    \else{\print{"test malla superficies 05: FALLA.\n"}}
  }