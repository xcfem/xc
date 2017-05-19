# -*- coding: utf-8 -*-
'''
Escribe las acciones estáticas equivalentes que se obtienen para
cada nodo del modelo y que corresponden al modo que se pasa como parámetro
'''
def escribeCargasModo(preprocessor,fName, iModo, aceleraciones):
  fName.write("\# Debidas a masas en nodos.)\n")
  nodos= preprocessor.getNodeLoader
  for n in nodos:
    if(n.tag>0):
      node_force= getEquivalentStaticLoad(iModo,aceleraciones[iModo-1])
      fName.write("\\nodal_load{ \\nod{",tag,"}")
      fName.write(" \\val{",node_force,"} }\n")

  fName.write("\n\n\# Debidas a masas en elementos.)\n")
  elementos= preprocessor.getElementLoader
  for e in elementos:
    if(e.tag>0):
      numNodes= e.getNumExternalNodes
      tagsNodos= e.getTagsNodos
      forces= getEquivalentStaticNodalLoads(iModo,aceleraciones[iModo-1])
      node_force= 0
      for iNod in range(0,Nod<numNodes):
        fName.write("\\nodal_load{ \\nod{",tagsNodos[iNod],"}")
        node_force= getFila(forces,iNod+1)
        fName.write(" \\val{",node_force,"} }\n")
