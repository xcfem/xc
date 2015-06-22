\c{Tomado del example B16 del manual de verificación de Solvia}
\nverborrea{0}
\expr{NumDivI= 6}
\expr{NumDivJ= 6}
\expr{CooMaxX= 24}
\expr{CooMaxY= 24}
\expr{E= 1e7} \c{Módulo elástico psi}
\expr{nu= @sqrt(0.1)} \c{Coeficiente de Poison}
\expr{thickness= 0.12} \c{Canto de la sección expresado en pulgadas.}
\expr{unifLoad= 3.125} \c{Carga uniforme en psi.}
\expr{nLoad= unifLoad*CooMaxX*CooMaxY/NumDivI/NumDivJ} \c{Carga tributaria para cada nodo}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_6gdl.xcm"}
\incluye{"solution/simple_newton_raphson_band_gen.xcm"} \c{Procedimiento de solución}

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
                    \Nodo6GDLGirosLibresLista(@getListTagNodos){}
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
    \static_analysis["smt"]{ \analyze{10} \expr{analOk= @result} }
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

\expr{UZTeor= -@tonum(3.5*thickness)}
\expr{ratio1= @tonum(@abs((UZ-UZTeor)/UZTeor))}
\expr{ratio2= @tonum(@abs((nElems-(NumDivI*NumDivJ))/(NumDivI*NumDivJ)))}

\print{"UZ= ",UZ,"\n"}
\print{"UZTeor= ",UZTeor,"\n"}
\print{"Num. nodos: ",nNodos,"\n"}
\print{"Num. elem: ",nElems,"\n"}
\print{"ratio1: ",ratio1,"\n"}
\print{"ratio2: ",ratio2,"\n"}
\c{
  }

\if
  {
    \cond{(@abs(ratio1)<5e-2) & (@abs(ratio2)<1e-15)}
    \then{\print{"test corot_shell_mitc4_10: pasa.\n"}}
    \else{\print{"test corot_shell_mitc4_10: FALLA.\n"}}
  }