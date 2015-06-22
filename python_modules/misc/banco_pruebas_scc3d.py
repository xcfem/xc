# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_6dof

# Define el modelo para probar una sección de fibras.
def modeloSecc3d(mdlr,nmbS):
  nodos= mdlr.getNodeLoader

  predefined_spaces.gdls_resist_materiales3D(nodos)
  nodos.defaultTag= 1 #El número del próximo nodo será 1.
  nodos.newNodeXYZ(1,0,0)
  nodos.newNodeXYZ(1,0,0)

  elementos= mdlr.getElementLoader
  elementos.dimElem= 1
  elementos.defaultMaterial= nmbS
  elementos.defaultTag= 1 #Tag for the next element.
  zls= elementos.newElement("zero_length_section",xc.ID([1,2]));
  return zls

# Define el modelo para probar una sección de fibras.
def nuevoZeroLengthSecc3d(mdlr, nmbS, tagNodo, tagElem):
  idNod1= tagNodo
  idNod2= (tagNodo+1)

   # Definimos nodos
  nodos.newNodeIDXYZ(idNod1,tagNodo,0,0)
  nodos.newNodeIDXYZ(idNod2,tagNodo,0,0)

  coacciones= mdlr.getConstraintLoader
  fix_node_6dof.fixNode6DOF(coacciones,idNod1)
  fix_node_6dof.Nodo6DOFGirosLibres(coacciones,idNod2)

  # Definimos elementos
  elementos= mdlr.getElementLoader
  elementos.defaultMaterial= nmbS
  elementos.defaultTag= tagElem #Tag for the next element.
  zls= elementos.newElement("zero_length_section",xc.ID([idNod1,idNod2]));

# '''
# Crea los elementos a partir de la tabla creada anteriormente
#    nmbDBase: Nombre de la base de datos donde se guardará la tabla.
#    nmbTableElem: Nombre de la tabla que contiene los índices de elemento.
#    nmbSeccion: Nombre de la sección que se asigna a cada elemento.
# '''
# def creaElemFromTable(nmbDBase, nmbTablaElem, nmbSeccion):
#     idElem= 0
#     idNodo= 0
#     \sqlite
#        { 
#          \nmbDBase
#            {
#              \defaultQuery
#                {
#                  \get_result{"select * from " + nmbTablaElem}
#                  \while
#                    {
#                      \cond{fetch_row}
#                      \bucle
#                        {
#                          idElem= getInt("ELEM")
#                          idNodo= 10*idElem
#                          \nuevoZeroLengthSecc3d(nmbSeccion,int(idNodo),int(idElem))
#                        }
#                    }
#                  \free_result
#                }
#            }
#        }
#   }

# '''
# Crea las cargas sobre cada uno de los elementos a partir de las tablas creadas anteriormente
#    nmbQuery: Nombre de la consulta que se empleara para obtener las cargas.
#    nmbTbEsf: Nombre de la tabla que contiene los índices de sección.
#    idSecc: Identificador de la sección.
# '''
# def creaCargasFromTable(nmbQuery, nmbTbEsf, idSecc):
#     idAccion= 
#     fieldN= 0.0
#     fieldQ1= 0.0
#     fieldQ2= 0.0
#     fieldM1= 0.0
#     fieldM2= 0.0
#     iNod= 10*idSecc+1
#     logging.info("Cargando sección: ",int(idSecc),"\n")
#     logging.info("Cargando nodo: ",int(iNod),"\n")
#     \nmbQuery
#       {
#         \get_result{"select * from "+ nmbTbEsf + " where idSecc = " + sqlValue(idSecc)}
#         \while
#           {
#             \cond{fetch_row}
#             \bucle
#               {
#                 idAccion= getStr("ACCION")
#                 fieldN= rowXXX['AXIL']
#                 fieldQ1= rowXXX['Q_1']
#                 fieldQ2= rowXXX['Q_2']
#                 fieldM1= rowXXX['M_1']
#                 fieldM2= rowXXX['M_2']
#                 \mdlr
#                   {
#                     \loads
#                       {
#                         \casos
#                           {
#                             \load_pattern[idAccion]
#                             \set_current_load_pattern{idAccion}
#                             \nodal_load{ \nod{int(iNod)} \val{fieldN,fieldQ1,fieldQ2,0,fieldM1,fieldM2} }
#                           }
#                       }
#                   }
#               }
#           }
#         \free_result
#       }
#   }
