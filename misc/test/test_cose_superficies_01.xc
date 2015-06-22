\expr{E= 30e6} \c{Módulo elástico en psi}
\expr{nu= 0.3} \c{Coeficiente de Poison}
\expr{rho= 0.0} \c{Densidad}
\mdlr
  {
    \materiales
      {
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
        \pnt[1]{ \pos{ \x{0.0} \y{0.0} \z{0.0}} }
        \pnt{ \pos{ \x{1.0} \y{0.0} \z{0.0}} }
        \pnt{ \pos{ \x{2.0} \y{0.0} \z{0.0}} }
        \pnt{ \pos{ \x{3.0} \y{0.0} \z{0.0}} }
        \esquema3d[0]
          {
            \dim{2,2,4}
            \set_pnt[1,1,1]{1}
            \set_pnt[1,1,2]{2}
            \set_pnt[1,1,3]{3}
            \set_pnt[1,1,4]{4}

            \def_prop["rangoCopia","record"]{}
            \rangoCopia
              {
                \def_prop["rangoCapas","vector"]{[1,1]}
                \def_prop["rangoFilas","vector"]{[1,1]}
                \def_prop["rangoColumnas","vector"]{[1,4]}
                \def_prop["offsetIndices","vector"]{[0,1,0]}
                \def_prop["vectorOffset","vector3d"]{[0,1,0]}
              }
            \copia_rango{@rangoCopia}
            \rangoCopia
              {
                \set{rangoFilas= [1,2]}
                \set{offsetIndices=[1,0,0]}
                \set{vectorOffset= [0,0,1]}
              }
            \copia_rango{@rangoCopia}
            \def_prop["rangoCaras","record"]{}
            \rangoCaras
              {
                \def_prop["rangoCapas","vector"]{[1,1]}
                \def_prop["rangoFilas","vector"]{[1,2]}
                \def_prop["rangoColumnas","vector"]{[1,4]}
                \def_prop["offsetIndices","vector"]{[[0,0,0],[0,0,1],[0,1,1],[0,1,0]]}
                \def_prop["dispNodos","string"]{"2x2"}
                \def_prop["elemSizeI","double"]{"0.5"}
                \def_prop["elemSizeJ","double"]{"0.5"}
              }
            \crea_cuadrilateros{@rangoCaras}
          }
      }
  }
