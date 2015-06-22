\c{Tomado del control número 1 que figura en el apartado 6.7 del libro
«Los forjados reticulares: diseño, análisis, construcción y patología»
de Florentino Regalado Tesoro.}
\expr{CooMaxX= 10}
\expr{CooMaxY= 10}
\expr{xMidP= CooMaxX/2}
\expr{yMidP= CooMaxY/2}
\expr{xMidL= xMidP}
\expr{yMidL= CooMaxY}
\expr{NumDivI= @ceil(CooMaxX/0.25)}
\expr{NumDivJ= @ceil(CooMaxY/0.25)}
\expr{E= 2.1e10} \c{Módulo elástico en N/m2}
\expr{nu= 0.0} \c{Coeficiente de Poison}
\expr{G= E/2/(1+nu)}
\expr{thickness= 0.2} \c{Canto de la sección expresado en metros.}
\expr{unifLoad= 20e3} \c{Carga uniforme en N/m2.}
\expr{nLoad= unifLoad*CooMaxX*CooMaxY/NumDivI/NumDivJ} \c{Carga tributaria para cada nodo}

\def_prop["tagElemCentro","double"]{0}
\def_prop["tagElemLado","double"]{0}

\path{"/usr/local/lib/macros_xc"}
\incluye{"modelo/fija_nodo_6gdl.xcm"}
\incluye{"modelo/nodosRM3d.xci"}
\incluye{"modelo/cargas_nodo.xcm"}

\mdlr
  {
    \c{Definimos materiales}
    \materiales{ \elastic_material["elast",1]{\E{E}} }

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
    \loads{\casos{
        \constant_ts["ts"]{ \factor{1.0} } \c{Time series: constant_ts[nombre]{factor}}
        \set_current_time_series{"ts"}
        \load_pattern["0"]{} \c{load_pattern[codigo]{}}
        \set_current_load_pattern{"0"}
      }}
    \CargaNodosInterioresCara("f1",[0,0,-nLoad,0,0,0]){}
    \sets
      {
        \f1
          {
            \set{tagElemCentro= @getTagNearestElement(xMidP,yMidP,0.0)}
            \set{tagElemLado= @getTagNearestElement(xMidL,yMidL,0.0)}
            \nodos
              {
                \expr{nNodos= @size}
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
\fields
  {
    \def_field["disp"]
      {
        \setSet{"total"}
        \setComponentNames{["UX","UY","UZ","RX","RY","RZ"]}
        \setComponentDescriptions{["Mov. según x","Mov. según y","Mov. según z","Giro según x","Giro según y","Giro según z"]}
        \setComponentUnits{["in","in","in","rad","rad","rad"]}
        \setComponentsProperty{"getCommitDisp"}
      }
    \def_field["m1Medio"]
      {
        \setSet{"total"}
        \onElements{}
        \setComponentNames{["m1"]}
        \setComponentDescriptions{["Momento en torno al eje 1"]}
        \setComponentUnits{["Nm"]}
        \setComponentsProperty{"m1Medio"}
      }
    \def_field["m2"]
      {
        \setSet{"total"}
        \onGaussPoints{}
        \setComponentNames{["m1"]}
        \setComponentDescriptions{["Momento en torno al eje 2"]}
        \setComponentUnits{["Nm"]}
        \setComponentsProperty{"m2"}
      }
  }
\def_prop["fName","string"]{"/tmp/test_exporta_med03.med"}
\def_prop["spaceDim","double"]{-1}
\def_prop["meshDim","double"]{-1}
\def_prop["cooNames","lista"]{}
\def_prop["unitsNames","lista"]{}
\def_prop["numeroTiposCelda","double"]{}
\def_prop["tiposCelda","lista"]{}
\def_prop["nombresTiposCelda","lista"]{}
\def_prop["numeroCeldasTipo","double"]{}
\def_prop["numNodosCeldaTipo","double"]{}
\def_prop["numGruposNodos","double"]{}
\def_prop["numGruposElementos","double"]{}

\mdlr{\sets{\total{\calc_resisting_force{}}}}

\med_export
   {
     \vertices
       {
         \setCoordinatesNames{["X","Y","Z"]}
         \setCoordinatesUnits{["in","in","in"]}
       }
     \setName{"prueba03"}
     \write{@fName}
   }
\med_import
   {
     \setName{"prueba03"}
     \read{@fName}
     \set{spaceDim= @getSpaceDimension}
     \set{meshDim= @getMeshDimension}
     \set{cooNames= @getCoordinatesNames}
     \set{unitsNames= @getCoordinatesUnits}
     \set{numeroTiposCelda= @getNumberOfCellTypes}
     \set{tiposCelda= @getCellTypes}
     \set{nombresTiposCelda= @getCellTypeNames}
     \set{numeroCeldasTipo= @getNumCellsType(@tiposCelda[0])}
     \set{numNodosCeldaTipo= @mod(@tiposCelda[0],100)}
     \set{numGruposNodos= @getNumberOfGroups(@MED_NODE)}
     \set{numGruposElementos= @getNumberOfGroups(@MED_CELL)}
   }
\c{\sys{"rm -f " + @fName}}


\print{"spaceDim= ",@spaceDim,"\n"}
\print{"meshDim= ",@meshDim,"\n"}
\print{"cooNames= ",@cooNames,"\n"}
\print{"unitsNames= ",@unitsNames,"\n"}
\print{"numeroTiposCelda= ",@numeroTiposCelda,"\n"}
\print{"tiposCelda= ",@tiposCelda,"\n"}
\print{"nombresTiposCelda= ",@nombresTiposCelda,"\n"}
\print{"numeroCeldasTipo(",@tiposCelda[0],")= ",@numeroCeldasTipo,"\n"}
\print{"numeroNodosCeldaTipo(",@tiposCelda[0],")= ",@numNodosCeldaTipo,"\n"}
\print{"numGruposNodos= ",@numGruposNodos,"\n"}
\print{"numGruposElementos= ",@numGruposElementos,"\n"}
\c{
  }

\def_prop["ratio1","double"]{@spaceDim-3}
\def_prop["ratio2","double"]{@meshDim-2}

\if((@ratio1<1e-15) & (@ratio2<1e-15))
  {
    \then{\print{"test test_exporta_med03: pasa.\n"}}
    \else{\print{"test test_exporta_med03: falla.\n"}}
  }