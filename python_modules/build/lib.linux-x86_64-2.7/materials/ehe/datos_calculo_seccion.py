# -*- coding: utf-8 -*-

esfN= 0.0 # Esfuerzo axil
esfMy= 0.0 # Momento flector en torno al eje y.
esfMz= 0.0 # Momento flector en torno al eje z.
fibraCEpsMin= 0.0 # Fibra de hormigón con deformación mínima.
epsCMin= 0.0 # Valor de la deformación mínima en el hormigón.
fibraCEpsMax= 0.0 # Fibra de hormigón con deformación máxima.
epsCMax= 0.0 # Valor de la deformación máxima en el hormigón.
Ac= 0.0 # Área de hormigón.
sgxd= 0.0
fibraSEpsMax= 0.0 # Fibra de acero con deformación mínima.
epsSMax= 0.0 # Valor de la deformación máxima en el acero.
AsPas= 0.0 # Área de la armadura pasiva.
AsPasTracc= 0.0 # Área de la armadura pasiva a tracción.
T= 0.0 # Resultante de las tracciones.
tipoSolic= 0.0 # Tipo de la solicitación.
strTipoSolic= "nil" # Descripción del tipo de solicitación.
aprovSeccTN= 0.0 # Aprovechamiento de la sección frente a tensiones normales.
cumpleTN= 0.0 # Verdadero si cumple frente a tensiones normales.
z= 0.0 # Brazo mecánico.
Vu1= 0.0
Vcu= 0.0
Vsu= 0.0
Vu2= 0.0
Vu= 0.0
cumpleV= 0.0 # Verdadero si cumple frente a cortante.
aprovSeccV= 0.0 # Aprovechamiento de la sección frente a cortante.
PropVrDato= 0.0

# def datCalcSecc(fOut, nmbTablaResultTN, nmbTablaResultV, idSecc, idElem, idComb, rutinaVerifCuantias):
#     aprovTNPrevio= 0.0
#     aprovVPrevio= 0.0
#     strSQLTN= "select * from " + nmbTablaResultTN + " where idSecc= " + sqlValue(int(idSecc))
#     strSQLV= "select * from " + nmbTablaResultV + " where idSecc= " + sqlValue(int(idSecc))
#     tbTN= nmbTablaResultTN
#     tbV= nmbTablaResultV
#     \queryResultPrevio
#       {
#         \get_result{strSQLTN}
#         \while
#           {
#             \cond{fetch_row}
#             \bucle
#               {
#                 aprovTNPrevio= rowXXX['aprovTN']
#               }
#           }
#         \free_result
#         \get_result{strSQLV}
#         \while
#           {
#             \cond{fetch_row}
#             \bucle
#               {
#                 aprovVPrevio= rowXXX['aprovV']
#               }
#           }
#         \free_result
#       }
#     print("aprovTNPrevio",aprovTNPrevio,"\n")
#     esfN= N
#     esfMy= My
#     esfMz= Mz

#     fibraCEpsMin= hormigon.IMinProp("material.strain")
#     epsCMin= ((hormigon.fibra(fibraCEpsMin)).material).strain # Deformación mínima en el hormigón.
#     fibraCEpsMax= hormigon.IMaxProp("material.strain")
#     epsCMax= ((hormigon.fibra(fibraCEpsMax)).material).strain # Deformación máxima en el hormigón.
#     Ac= hormigon.area

#     sgxd= esfN/area+esfMz/Iz*yCdg+esfMy/Iy*zCdg



#     AsPas= armadura.area # Area de armadura a tracción
#     fibraSEpsMax= armadura.IMaxProp("material.strain")
#     epsSMax= ((armadura.fibra(fibraSEpsMax)).material).strain # Deformación máxima en el acero.

#     AsPasTracc= armaduraTracc.area # Area de armadura a tracción
#     T= getResultanteTracc # Resultante tracciones en la sección

#     tipoSolic= tipoSolicitacion(epsCMin,epsSMax)
#     strTipoSolic= strTipoSolicitacion(tipoSolic)

#     aprovSeccTN= aprovTN(tipoSolic,epsCMin,epsCMax,epsSMax)
#     cumpleTN= aprovSeccTN<=1.0

#     if(aprovTNPrevio<aprovSeccTN):
#         \then
#           {
#             aprovTNPrevio= aprovSeccTN
#             \writeUpdateResultTN(fOut,tbTN,idSecc,idElem,idComb,esfN,esfMy,esfMz,tipoSolic,epsCMin,epsCMax,epsSMax, aprovSeccTN)
#           }
#         else: }
#       }
#     # Brazo mecánico.
#     if(T!=0.0):
#         z= esfMz/T}
#         else: z= 0.9*d}
#       }

#     Vu1= getVu1(abs(HA25.fcd),esfN,Ac,b0,d,alpha,theta)
#     Vcu= getVcu(abs(HA25.fck),esfN,Ac,b0,d,theta,AsPasTracc,B500S.fyd,0.0,0.0,sgxd,0.0)
#     Vsu= getVsu(z,alpha,theta,AsTrsv,B500S.fyd)
#     Vu2= getVu2(abs(HA25.fck),esfN,Ac,b0,d,z,alpha,theta,AsPasTracc,B500S.fyd,0.0,0.0,sgxd,0.0,AsTrsv,B500S.fyd)
#     Vu= min(Vu1,Vu2)
#     cumpleV= VrDato<=Vu
#     aprovSeccV= VrDato/Vu
#     PropVrDato= VrDato
#     if(aprovVPrevio<aprovSeccV):
#         \then
#           {
#             aprovVPrevio= aprovSeccV
#             \writeUpdateResultV(fOut,tbV,idSecc,idElem,idComb,esfN,PropVrDato,Vu1,Vu2,Vu,aprovSeccV)
#           }
#         else: }
#       }
#     \rutinaVerifCuantias(Ac,AsPas,abs(HA25.fcd),B500S.fyd,esfN)
#   }

# # Comprobamos las secciones para una hipótesis
# def compruebaSeccionesHipot(idComb, nmbTablaSecc, rutinaVerifCuantias, fOut, nmbTablaResultTN, nmbTablaResultV):
#     idScc= 0.0
#     idElem= 0.0
#     idNodo= 0.0
#     \sqlite
#       {
#         \nmbDatabase
#           {
#             \querySecciones
#               {
#                 \get_result{"select * from " + nmbTablaSecc}
#                 \while
#                   {
#                     \cond{fetch_row}
#                     \bucle
#                       {
#                         idScc= rowXXX['ELEM']
#                         idElem= floor(idScc/10)
#                         idNodo= 10*idScc+1
#                         \mdlr
#                           {
#                             if((analOk==0.0)):
#                                 \then
#                                   {
#                                     \dom
#                                       {
#                                         \calculate_nodal_reactions{1}
#                         print("Comprobando sección: ",int(idScc),"\n")
#                         print("Nodo: ",int(idNodo),"\n")
#                                         \nodos{nod= nodos.getNode(idNodo){VrDato= sqrt(reac[1]^2+reac[2]^2)} }  # Obtención del cortante.
#                                         \elementos{\elemento[idScc]{\seccion
#                                          {
#                                            \resel["armaduraTracc","armadura"]{(material.stress>0)} # Armadura traccionada.
#                                            \datCalcSecc(fOut,nmbTablaResulTN,nmbTablaResulV,idScc,idElem,idComb,rutinaVerifCuantias)
#                                          }}}# Comprobamos seccion
#                                       }
#                                   }
#                                 else:
#                                   {
#                                     print("Fallo en sección: ",ind(idScc), ", elemento: ",int(idElem)," combinación: ",idComb,"\n")
#                                     \fin
#                                   }
#                               }
#                           }
#                       }
#                   }
#                 \free_result
#               }
#            }
#       }
#   }

# def compruebaSecciones(nmbDatabase, nmbTablaEsfuerzos, nmbQueryEsfuerzos, nmbTablaComb, nmbQueryCombinaciones, nmbQueryUpdate, nmbFileUpdate, nmbTablaSecc, rutinaVerifCuantias, nmbTablaResultTN, nmbTablaResultV):
#     \creaCargasSecciones(nmbDatabase,nmbTablaEsfuerzos,nmbQueryEsfuerzos,nmbTablaSecc)
#     listaIdComb= 
#     listaDescompComb= 
#     \getListaCombinaciones(nmbDatabase,nmbQueryCombinaciones,"listaIdComb","listaDescompComb")
#     \sqlite
#       {
#         \nmbDatabase
#           {
#             \newQuery{"querySecciones"}
#             \newQuery{"queryResultPrevio"}
#             \newQuery{"queryResultUpdate"}
#           }
#       }
#     idComb= 0.0
#     descomp= ""
#     nComb= listaIdComb.size
#     i= 0.0
#        for i in range(0,nComb+1):
#         \bucle
#            {
#              idComb= listaIdComb[i]
#              print("idComb= ",idComb,"\n")
#              descomp= listaDescompComb[i]
#              print("descomp= ",descomp,"\n")
#              # Lanzamos el calculo de la combinación.
#              \mdlr
#                {
#                  \loads
#                    {
#                      \combinacion[idComb]{\descomp{descomp}}
#                      \add_to_domain{idComb}
#                    }
#                  \sol_proc{\static_analysis["smt"]{ \analyze{1} analOk= result} }
#                }
#              \nuevo_archivo_salida["sqlOut"]{nmbFileUpdate}
#              \compruebaSeccionesHipot(idComb,nmbTablaSecc,rutinaVerifCuantias,"sqlOut",nmbTablaResultTN,nmbTablaResultV)
#              \cierra_archivo_salida{"sqlOut"}
#              \sqlite
#                {
#                  \nmbDatabase
#                     {
#                       \queryResultUpdate
#                         {
#                           execfile(nmbFileUpdate) # Actualizamos datos secciones
#                         }
#                     }
#                }
#              \borra_archivo_si_existe{nmbFileUpdate}
#              \mdlr
#                {
#                  \loads
#                    {
#                      \remove_from_domain{idComb} # Quitamos la combinación del dominio
#                      \clear_combinaciones
#                    }
#                  \dom{\revert_to_start}
#                }
#           }
#       }
#   }
