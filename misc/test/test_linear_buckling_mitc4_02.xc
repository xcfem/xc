\c{Tomado del ejemplo B43 del «SOLVIA Verification Manual».
Análisis de pandeo linealizado de una placa rigidizada.}
\path{"/usr/local/lib/macros_xc"}
\incluye{"materiales/parametrosSeccionRectangular.xcm"}

\def_prop["EMat","double"]{2.1e11} \c{Módulo de Young en N/m2.}
\def_prop["nuMat","double"]{0.3} \c{Coeficiente de Poisson.}
\def_prop["espChapa","double"]{0.005} \c{Espesor en m.}
\def_prop["F","double"]{500000} \c{Fuerza en N/m.}
\def_prop["dens","double"]{7850*@espChapa} \c{Densidad kg/m2.}

\def_prop["Brig","double"]{1.073e-2/2} \c{Ancho del rigidizador en m.}
\def_prop["Hrig","double"]{4e-2} \c{Canto del rigidizador en m.}
\def_prop["densRig","double"]{7850*@Brig*@Hrig} \c{Densidad del rigidizador kg/m.}

\c{Definición de la sección del rigidizador}
\def_prop["sccRig","record"]{}
\sccRig
  {
    \def_prop["nmb","string"]{"rgidizador"} \c{Nombre para la sección.}
    \def_prop["b","double"]{@Brig} \c{Ancho de la sección expresado en m.}
    \def_prop["h","double"]{@Hrig} \c{Canto de la sección expresado en m.}
    \def_prop["E","double"]{@EMat} \c{Módulo de Young del material en kp/cm2.}
    \def_prop["nu","double"]{@nuMat} \c{Coeficiente de Poisson del material.}
  }
\setupSeccRectang("sccRig"){}

\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales3D.xci"}
\mdlr
  {
    \c{Definimos materiales}
    \materiales{ \elastic_membrane_plate_section["elast"]{ \nu{@nuMat} \E{@EMat} \h{@espChapa} \rho{@dens} } }
    \cad
      {
        \pnt[1]{ \pos{ \x{0.0} \y{0.00}} }
        \pnt[2]{ \pos{ \x{0.0} \y{0.25}} }
        \pnt[3]{ \pos{ \x{0.5} \y{0.00}} }
        \pnt[4]{ \pos{ \x{0.5} \y{0.25} } }
        \sup_cuadrilatera[1]
          {
            \def_pnts{4,2,1,3}
            \ndivI{3}
            \ndivJ{2}
          }
      }
    \nodos{ \nod_semilla[0]{\coo{0.0,0.0,0.0}} }
    \elementos
      {
        \elem_semilla
          {
            \nmb_material{"elast"}
            \elem_shell_mitc4[1]{}
          }
      }
    \sets{\f1{\malla{}}}

    \geom_transf{ \corot3d["corot"]{\set_xz_vector{\coo{0,1,0}}} }
    \defSeccShElastica3d(@sccRig,0){}
    \elementos{\elem_semilla{ \nmb_material{"sccRig"} \nmb_transf{"corot"} \elem_elastic_beam_3d[1]{ \rho{@densRig}} }}

    \c{Condiciones de contorno}
    \cad
      {
        \get_linea_extremos[1,3]
          { \malla{} } \c{Mallado del rigidizador}
        \get_linea_extremos[2,1]
          { \fix_gdls[0,4,5]{[0,0,0]} } \c{UX,RY,RZ}
        \get_linea_extremos[1,3]
          { \fix_gdls[1,3,5]{[0,0,0]} } \c{UY,RY,RZ}
        \get_linea_extremos[3,4]
          { \fix_gdls[2,5]{[0,0]} } \c{UZ,RZ}
        \get_linea_extremos[4,2]
          { \fix_gdls[2,5]{[0,0]} } \c{UZ,RZ}
      }
    \c{Definimos cargas}
    \loads
      {
        \casos
          {
            \constant_ts["ts"]{ \factor{1.0} } \c{Time series: constant_ts[nombre]{factor}}
            \set_current_time_series{"ts"}
            \load_pattern["0"]{}
            \set_current_load_pattern{"0"}
          }
      }
    \cad
      {
        \pnt[3]{ \nodo{ \load{[-21460,0,0,0,0,0]} } }
      }
  }

