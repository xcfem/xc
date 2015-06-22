\c{Tomado del example 2-005 del manual de verificación de SAP 2000}
\nverborrea{0}
\expr{NumDivI= 8}
\expr{NumDivJ= 8}
\expr{CooMaxX= 10}
\expr{CooMaxY= 2}
\expr{E= 17472000} \c{Módulo elástico en lb/in2}
\expr{nu= 0.3} \c{Coeficiente de Poison}
\expr{G= 6720000}
\expr{thickness= 0.0001} \c{Canto de la sección expresado en pulgadas.}
\expr{unifLoad= 0.0001} \c{Carga uniforme en lb/in2.}
\expr{ptLoad= 0.0004} \c{Carga puntual en lb.}
\expr{nLoad= unifLoad*CooMaxX*CooMaxY/NumDivI/NumDivJ} \c{Carga tributaria para cada nodo}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_6gdl.xcm"}

\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales3D.xci"}
\mdlr
  {
    \c{Definimos materiales}
    \materiales{ \elastic_material["elast",1]{\E{E}} }

    \nodos{ \nod_semilla[0]{\coo{0.0,0.0,0.0}} }

    \c{Definimos materiales}
    \materiales
      {
        \elastic_membrane_plate_section["memb1"]{ \nu{nu} \E{E} \h{thickness} \rho{0.0} }
      }
    \elementos
      {
        \elem_semilla
          {
            \nmb_material{"memb1"}
            \elem_corot_shell_mitc4[1]{}
          }
      }
    \cad
      {
        \pnt[1]{ \pos{ \x{0.0} \y{0.0} \z{0.0}} }
        \pnt[2]{ \pos{ \x{CooMaxX} \y{0.0} \z{0.0}} }
        \pnt[3]{ \pos{ \x{CooMaxX} \y{CooMaxY} \z{0.0}} }
        \pnt[4]{ \pos{ \x{0.0} \y{CooMaxY} \z{0.0}} }
        \sup_cuadrilatera[1]
          {
            \def_pnts{1,2,3,4}
            \ndivI{NumDivI}
            \ndivJ{NumDivJ}
          }
      }

    \sets
      {
        \f1
          {
            \malla{}
            \for_each_lado
              {
                \edge
                  { 
                    \fijaNodo6GDLLista(@getListTagNodos){}
                  }
              }
          }
      }
    \c{Definimos cargas}
    \loads
      {
        \casos
          {
            \constant_ts["ts"]{ \factor{1.0} } \c{Time series: constant_ts[nombre]{factor}}
            \set_current_time_series{"ts"}
            \load_pattern["0"]{} \c{load_pattern[codigo]{}}
            \set_current_load_pattern{"0"}
          }
      }
    \sets
      {
        \f1
          {
            \nodos
              {
                \expr{nNodos= @size}
                \capa[1]
                  {
                    \expr{nf= @tonum(@numFilas-1)}
                    \expr{nc= @tonum(@numCols-1)}
                    \for
                      {
                        \inicio{\expr{i=2} } \continua{i<=nf} \incremento{\expr{i=@tonum(i+1)}}
                        \bucle
                          {
                            \for
                              {
                                \inicio{\expr{j=2} } \continua{j<=nc} \incremento{\expr{j=@tonum(j+1)} }
                                \bucle
                                  {
                                    \nodo[i,j]
                                      {
                                        \expr{tagNod= @tag}
                                        \mdlr{\loads{\casos{\nodal_load{ \nod{tagNod} \val{0,0,-nLoad,0,0,0} }}}} \c{Carga tributaria}
                                      }
                                  }
                              }
                          }
                      }
                  }
              }
            \elementos
              {
                \expr{nElems= @size}
              }
          }
      }
    \loads
      {
        \add_to_domain{"0"} \c{Añadimos la hipótesis al dominio}
      }
  }
\c{Procedimiento de solución}
\sol_proc
  {
    \incluye{"solution/solucion1.xcm"}
    \current_analysis{ \expr{analOk= @result} }
  }
\mdlr
  {
    \sets
      {
        \f1
          {
            \nodos
              {
                \c{\print{"Nodo central: ",@nodo(1,NumDivI/2+1,NumDivJ/2+1).tag,"\n"}
                \print{"Coordenadas nodo central: ",@nodo(1,NumDivI/2+1,NumDivJ/2+1).coord,"\n"}}
                \nodo[1,NumDivI/2+1,NumDivJ/2+1]
                  {
                    \c{\print{"Nodo central: ",@tag,"\n"}
                    \print{"Coordenadas nodo central: ",@coord,"\n"}
                    \disp{\print{"Movs nodo central: ",@commit_disp,"\n"}}}
                    \disp{\expr{UZ= @commit_disp[2]}}
                  }
              }
          }
      }
  }

\expr{UZTeor= -2.6}
\expr{ratio1= @tonum(@abs((UZ-UZTeor)/UZTeor))}
\expr{ratio2= @tonum(@abs((nElems-64)/64))}

\c{
\print{"UZ= ",UZ,"\n"}
\print{"Num. nodos: ",nNodos,"\n"}
\print{"Num. elem: ",nElems,"\n"}
\print{"ratio1: ",ratio1,"\n"}
\print{"ratio2: ",ratio2,"\n"}
  }

\if
  {
    \cond{(@abs(ratio1)<5e-2) & (@abs(ratio2)<1e-15)}
    \then{\print{"test shell_mitc4_18: pasa.\n"}}
    \else{\print{"test shell_mitc4_18: FALLA.\n"}}
  }