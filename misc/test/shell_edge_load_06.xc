\c{Test de elaboración propia para comprobar las cargas
sobre bordes de elemento en una línea.}
\nverborrea{0}
\expr{NumDivI= 4}
\expr{NumDivJ= 1}
\expr{CooMaxX= NumDivI}
\expr{CooMaxY= 1}
\expr{E= 1e6}
\expr{nu= 0.3}
\expr{G= 6720000}
\expr{thickness= 0.1} \c{Canto de la sección expresado en pulgadas.}
\expr{unifLoad= 10} \c{Carga uniforme en lb/in2.}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_6gdl.xcm"}

\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales3D.xci"}
\mdlr
  {
    \c{Definimos materiales}
    \materiales
      {
        \elastic_membrane_plate_section["memb1"]{ \nu{nu} \E{E} \h{thickness} \rho{0.0} }
      }
    \nodos{ \nod_semilla[0]{\coo{0.0,0.0,0.0}} }
    \elementos
      {
        \elem_semilla
          {
            \nmb_material{"memb1"}
            \elem_shell_mitc4[1]{}
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
        \sup_cuadrilatera[1]
          {
            \for_each_line
              {
                \print{"linea: ",@nombre," p1: ",@getTagP1,"\n"}
              }
          }
      }
\fin{}

    \sets
      {
        \f1
          {
            \malla{}
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

\expr{UZTeor= -5.60}
\expr{ratio1= @tonum(@abs((UZ-UZTeor)/UZTeor))}
\expr{ratio2= @tonum(@abs((nElems-64)/64))}

\c{
\print{"UZ= ",UZ,"\n"}
\print{"Num. nodos: ",nNodos,"\n"}
\print{"Num. elem: ",nElems,"\n"}
\print{"ratio1: ",ratio1,"\n"}
  }

\if
  {
    \cond{(@abs(ratio1)<4e-2) & (@abs(ratio2)<1e-9)}
    \then{\print{"test shell_mitc4_06: pasa.\n"}}
    \else{\print{"test shell_mitc4_06: FALLA.\n"}}
  }