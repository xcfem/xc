\c{Tipo de problema}
\path{"/usr/local/lib/macros_lcmd"}
\path{"/usr/local/lib/macros_xc"}
\incluye{"materiales/ehe/areaBarrasEHE.xcm"}
\incluye{"materiales/seccion_fibras/defSeccionHASimple.xcm"}
\incluye{"materiales/seccion_fibras/plotGeomSeccion.xcm"}
\incluye{"materiales/seccion_fibras/informeGeomSeccion.xcm"}
\incluye{"materiales/parametrosSeccionRectangular.xcm"}
\incluye{"geom/plot.lcmdm"}

\incluye{"materiales/ehe/auxEHE.xcm"}
\incluye{"misc/banco_pruebas_scc3d.xcm"}
\incluye{"solution/simple_newton_raphson_band_gen.xcm"} \c{Procedimiento de solución}


\incluye{"materiales/ehe/hormigonesEHE.xcm"}
\incluye{"materiales/ehe/acerosEHE.xcm"}
\incluye{"materiales/ehe/fisuracionEHE.xcm"}
\incluye{"modelo/fija_nodo_6gdl.xcm"}

\c{Coeficientes de seguridad.}
\expr{gammac= 1.5} \c{Coeficiente de minoración de la resistencia del hormigón.}
\expr{gammas= 1.15} \c{Coeficiente de minoración de la resistencia del acero.}

\setupHormigon("HA25",gammac){}
\setupAcero("B500S",gammas){}

\expr{ancho= 1.0} \c{Ancho de la sección expresado en metros.}
\expr{canto= 0.25} \c{Canto de la sección expresado en metros.}
\expr{recub= 0.035} \c{Recubrimiento de la sección expresado en metros.}

\mdlr
  {
    \c{Definimos materiales}
    \materiales
      {
        \expr{tag= @defDiagKHormigon("HA25")}
        \expr{tag= @defDiagKAcero("B500S")}
      }
  }

\c{Losa horizontal. Armadura en dirección X (o radial)}
\def_prop["datosScc1LosC","record"]{}
\populateRecordSeccionHASimple("datosScc1LosC"){}
\datosScc1LosC
  {
    \set{nmbSeccion= "secHA1LosC"}
    \set{descSeccion= "Losa. Tramo Central. Sección normal al eje X."}
    \set{tipoHormigon= "HA25"} \set{canto= 0.25} \set{ancho= 1.0}
    \set{tipoArmadura= "B500S"}
    \set{nBarrasNeg= 5} \set{areaBarrasNeg= @areaFi10} \set{recubNeg= 0.025+0.010+0.010/2}
    \set{nBarrasPos= 5} \set{areaBarrasPos= @areaFi10} \set{recubPos= 0.025+0.010/2}
  }
\nuevo_archivo_salida["archTex"]{"informe_seccion.tex"}
\defSeccionHASimple("datosScc1LosC","k"){}
\informeGeomSeccion("datosScc1LosC","archTex","seccion.eps"){}
\cierra_archivo_salida{"archTex"}

