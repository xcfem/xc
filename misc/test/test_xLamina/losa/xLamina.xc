\path{"/usr/local/lib/macros_xc"}

\incluye{"materiales/xLamina/extrae_acciones.xcm"}
\incluye{"solution/resuelve_combinacion.xcm"}
\incluye{"materiales/xLamina/postproceso_xLamina.xcm"}
\incluye{"materiales/xLamina/modelo.xcm"}
\incluye{"materiales/xLamina/calculo_tn.xcm"}
\incluye{"listados/listados_factor_capacidad.xcm"}

\def_prop["nmbArch","string"]{"losa"} \c{XXX AQUI SE ESCRIBE EL NOMBRE (SIN EXTENSIÓN) DEL LISTADO DE ANSYS}
\extraeDatosLST(@nmbArch+".lst"){} 


\xLaminaConstruyeModelo("../materiales_hormigon_acero.xci","diag_interaccion_scc1.xci","diag_interaccion_scc2.xci"){}
\xLaminaCalculaTN("../def_hip_elu.xci","../calc_hip_elu.xci"){}
