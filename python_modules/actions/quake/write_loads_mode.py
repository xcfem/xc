# -*- coding: utf-8 -*-

def write_loads_mode(preprocessor,fName, iModo, aceleraciones):
  '''Write the equivalent static loads oftained for each node in the mode 
  passed as parameter
  '''
  fName.write("\# Due to mass in nodes.)\n")
  nodes= preprocessor.getNodeLoader
  for n in nodes:
    if(n.tag>0):
      node_force= getEquivalentStaticLoad(iModo,aceleraciones[iModo-1])
      fName.write("\\nodal_load{ \\nod{",tag,"}")
      fName.write(" \\val{",node_force,"} }\n")

  fName.write("\n\n\# Due to mass in elements.)\n")
  elementos= preprocessor.getElementLoader
  for e in elementos:
    if(e.tag>0):
      numNodes= e.getNumExternalNodes
      tagsNodes= e.getTagsNodos
      forces= getEquivalentStaticNodalLoads(iModo,aceleraciones[iModo-1])
      node_force= 0
      for iNod in range(0,Nod<numNodes):
        fName.write("\\nodal_load{ \\nod{",tagsNodes[iNod],"}")
        node_force= getFila(forces,iNod+1)
        fName.write(" \\val{",node_force,"} }\n")
