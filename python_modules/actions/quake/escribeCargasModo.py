# -*- coding: utf-8 -*-
'''
Escribe las acciones estáticas equivalentes que se obtienen para
cada nodo del modelo y que corresponden al modo que se pasa como parámetro
'''
def escribeCargasModo(mdlr,fName, iModo, aceleraciones):
  fName.write("\# Debidas a masas en nodos.)\n")
  nodos= mdlr.getNodeLoader
  for n in nodos:
    if(n.tag>0):
      fuerza_nodo= getEquivalentStaticLoad(iModo,aceleraciones[iModo-1])
      fName.write("\\nodal_load{ \\nod{",tag,"}")
      fName.write(" \\val{",fuerza_nodo,"} }\n")

  fName.write("\n\n\# Debidas a masas en elementos.)\n")
  elementos= mdlr.getElementLoader
  for e in elementos:
    if(e.tag>0):
      numNodes= e.getNumExternalNodes
      tagsNodos= e.getTagsNodos
      fuerzas= getEquivalentStaticNodalLoads(iModo,aceleraciones[iModo-1])
      fuerza_nodo= 0
      for iNod in range(0,Nod<numNodes):
        fName.write("\\nodal_load{ \\nod{",tagsNodos[iNod],"}")
        fuerza_nodo= getFila(fuerzas,iNod+1)
        fName.write(" \\val{",fuerza_nodo,"} }\n")
