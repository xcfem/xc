\c{Test tomado del ejemplo 5-001 del manual de verificación de SAP2000.}
\path{"/usr/local/lib/macros_xc"}

\c{Tipo de problema}
\incluye{"modelo/gdls_elasticidad3D.xci"}
\mdlr
  {
    \c{Definimos materiales}
    \materiales
      {
        \elastic_isotropic_3d["elast3d"]{ \E{1e6} \nu{0.25} \rho{0.0}}
      }
    \nodos
      { 
        \nod[9]{\coo{0,0,0}}
        \nod{\coo{1,0,0}}
        \nod{\coo{1,1,0}}
        \nod{\coo{0,1,0}}
        \nod{\coo{0,0,1}}
        \nod{\coo{1,0,1}}
        \nod{\coo{1,1,1}}
        \nod{\coo{0,1,1}}
      }
    \elementos
      {
        \nmb_material{"elast3d"}
        \elem_brick[1]{\nodes{9,10,11,12,13,14,15,16}}
      }
    \nodos
      { 
        \nod[9]{\fix[0,1,2]{[0,0,0]} }
        \nod[10]{\fix[0,1,2]{[0,0,0]} }
        \nod[11]{\fix[0,1,2]{[0,0,0]} }
        \nod[12]{\fix[0,1,2]{[0,0,0]} }
      }
    \nodos
      { 
        \nod[9]{\fix[0,1,2]{[0,0,0]} }
        \nod[10]{\fix[0,1,2]{[0.001,0.0005,0.0005]} }
        \nod[11]{\fix[0,1,2]{[0.0015,0.0015,0.001]} }
        \nod[12]{\fix[0,1,2]{[0.0005,0.001,0.0005]} }
        \nod[13]{\fix[0,1,2]{[0.0005,0.0005,0.001]} }
        \nod[14]{\fix[0,1,2]{[0.0015,0.001,0.0015]} }
        \nod[15]{\fix[0,1,2]{[0.002,0.002,0.002]} }
        \nod[16]{\fix[0,1,2]{[0.001,0.0015,0.0015]} }
      }

  }
\c{Procedimiento de solución}
\sol_proc
  {
    \incluye{"solution/solucion1.xcm"}
  }

\def_prop["avgStress11","double"]{}

\mdlr
  {
    \elementos
      {
        \element[1]{\set{avgStress11= @getAvgStress(1,1)}}
      }
  }

\print{"tensión media 11: ",@avgStress11,"\n"}
