\c{Test tomado del ejemplo 5-001 del manual de verificación de SAP2000.}
\path{"/usr/local/lib/macros_xc"}

\c{Tipo de problema}
\incluye{"modelo/gdls_elasticidad3D.xci"}
\mdlr
  {
    \c{Definimos materiales}
    \materiales
      {
        \elastic_isotropic_3d["elast3d"]{ \E{1000000} \nu{0.25} \rho{0.0}}
      }
    \nodos
      { 
        \nod[1]{\coo{0.249,0.342,0.192}}
        \nod{\coo{0.826,0.288,0.288}}
        \nod{\coo{0.850,0.649,0.263}}
        \nod{\coo{0.273,0.750,0.230}}
        \nod{\coo{0.320,0.186,0.643}}
        \nod{\coo{0.677,0.305,0.683}}
        \nod{\coo{0.788,0.693,0.644}}
        \nod{\coo{0.165,0.745,0.702}}
        \nod{\coo{0,0,0}}
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
        \elem_brick[1]{\nodes{16,13,9,12,8,5,1,4}}
        \elem_brick{\nodes{13,14,10,9,5,6,2,1}}
        \elem_brick{\nodes{5,6,2,1,8,7,3,4}}
        \elem_brick{\nodes{7,3,4,8,15,11,12,16}}
        \elem_brick{\nodes{14,10,11,15,6,2,3,7}}
        \elem_brick{\nodes{14,13,16,15,6,5,8,7}}
        \elem_brick{\nodes{2,3,4,1,10,11,12,9}}
      }
    \nodos
      { 
        \nod[1]{\fix[0,1,2]{[0,0,0]} }
        \nod[2]{\fix[0,1,2]{[0,0,0]} }
        \nod[3]{\fix[0,1,2]{[0,0,0]} }
        \nod[4]{\fix[0,1,2]{[0,0,0]} }
        \nod[5]{\fix[0,1,2]{[0,0,0]} }
        \nod[6]{\fix[0,1,2]{[0,0,0]} }
        \nod[7]{\fix[0,1,2]{[0,0,0]} }
        \nod[8]{\fix[0,1,2]{[0,0,0]} }
        \nod[10]{\fix[0,1,2]{[0,0,0]} }
        \nod[11]{\fix[0,1,2]{[0,0,0]} }
        \nod[12]{\fix[0,1,2]{[0,0,0]} }
        \nod[14]{\fix[0,1,2]{[0,0,0]} }
        \nod[15]{\fix[0,1,2]{[0,0,0]} }
\c{
        \nod[9]{\fix[0,1,2]{[0,0,0]} }
        \nod[13]{\fix[0,1,2]{[0,0,0]} }
        \nod[16]{\fix[0,1,2]{[0,0,0]} }
  }

\c{
        \nod[10]{\fix[0,1,2]{[0.001,0.0005,0.0005]} }
        \nod[11]{\fix[0,1,2]{[0.0015,0.0015,0.001]} }
        \nod[12]{\fix[0,1,2]{[0.0005,0.001,0.0005]} }
        \nod[13]{\fix[0,1,2]{[0.0005,0.0005,0.001]} }
        \nod[14]{\fix[0,1,2]{[0.0015,0.001,0.0015]} }
        \nod[15]{\fix[0,1,2]{[0.002,0.002,0.002]} }
        \nod[16]{\fix[0,1,2]{[0.001,0.0015,0.0015]} }
}
\for_each
  {
    \print{"tag: ",@tag,"\n"}
  }
      }
  }
\c{Procedimiento de solución}
\sol_proc
  {
    \incluye{"solution/solucion1.xcm"}
  }