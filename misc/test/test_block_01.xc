\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/gdls_elasticidad3D.xci"}
\mdlr
  {
    \nodos
      {
        \nod_semilla[0]{\coo{0.0,0.0,0.0}}
      }
    \cad
      {
        \pnt[1]{ \pos{ \x{0} \y{0} \z{0}} }
        \pnt{ \pos{ \x{1} \y{0} \z{0}} }
        \pnt{ \pos{ \x{1} \y{1} \z{0}} }
        \pnt{ \pos{ \x{0} \y{1} \z{0}} }
        \pnt{ \pos{ \x{0} \y{0} \z{1}} }
        \pnt{ \pos{ \x{1} \y{0} \z{1}} }
        \pnt{ \pos{ \x{1} \y{1} \z{1}} }
        \pnt{ \pos{ \x{0} \y{1} \z{1}} }


        \sup_cuadrilatera[1]{ \def_pnts{1,2,3,4} \ndivI{2} \ndivJ{1} } \c{Base}
        \sup_cuadrilatera{ \def_pnts{1,2,6,5} \ndivI{1} \ndivJ{1} }
        \sup_cuadrilatera{ \def_pnts{1,4,8,5} \ndivI{1} \ndivJ{1} }
        \sup_cuadrilatera{ \def_pnts{2,3,7,6} \ndivI{1} \ndivJ{1} }
        \sup_cuadrilatera{ \def_pnts{4,3,7,8} \ndivI{1} \ndivJ{1} }
        \sup_cuadrilatera{ \def_pnts{5,6,7,8} \ndivI{2} \ndivJ{1} } \c{Tapa}

        \block{ \def_caras{1,2,3,4,5,6} \malla{} }
      }
  }

