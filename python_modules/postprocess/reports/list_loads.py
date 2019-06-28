# -*- coding: utf-8 -*-

def UniformLoadList(preprocessor,loadPattern, archivo, carga, fmt):
  elemTags= loadPattern.getElementTags()
  for i in elemTags:
    e= preprocessor.getElementHandler.getElement(i)
    archivo.write(e.tag," & ",loadPattern," & unif. & & ",fmt.format(carga[0]/1e3)," & ",fmt.format(carga[1]/1e3)," & ",fmt.format(carga[2]/1e3),"\\\\\n")

def PunctualLoadList(preprocessor,loadPattern, archivo, carga, fmt):
  xCarga= x
  elemTags= loadPattern.getElementTags()
  for i in elemTags:
    e= preprocessor.getElementHandler.getElement(i)
    archivo.write(e.tag," & ",loadPattern," & punt. & ",xCarga," & ",fmt.format(carga[0]/1e3)," & ",fmt.format(carga[1]/1e3)," & ",fmt.format(carga[2]/1e3),"\\\\\n")

def ElementLoadList(loadPattern, archivo, fmt):
  caption= "Cargas sobre elementos"
  defCampos= "|r|c|c|r|r|r|r|"
  idsCampos= "Id & Acc. & Type & x & Fx & Fy &Fz \\\\\n - & - & - &  & kN/m & kN/m & kN/m "
  cabeceraSupertabular(archivo,7,defCampos,idsCampos,caption)
  loads= preprocessor.getLoadPatterns
  print "Número de patrones de carga: ",numLoadPatterns
  print "Número de cargas en elementos: ", loadPattern.getNumEleLoads()
  eleLoads= loadPattern.getEleLoads()
  for l in eleLoads:
    if(l.category=="nodef"):
      carga= l.getGlobalForces
    elif(l.category=="uniform"):
      UniformLoadList(loadPattern,archivo,carga,fmt)
    elif(l.category=="punctual"):
      PunctualLoadList(loadPattern,archivo,carga,fmt)
  cierraSupertabular(archivo) 

def listNodalLoads(loadPattern, archivo, fmt):
  caption= "Nodal loads"
  defCampos= "|r|c|c|r|r|r|r|"
  idsCampos= "Id & Acc. & Type & x & Fx & Fy &Fz \\\\\n - & - & - &  & kN & kN & kN "
  cabeceraSupertabular(archivo,7,defCampos,idsCampos,caption) 
  print "Number of nodal loads: ", loadPattern.getNumNodalLoads()
  nodalLoads= loadPattern.getNodalLoads()
  for l in nodalLoads:
    carga= l.load
    archivo.write(l.nod.tag," & ",loadPattern," & nod. &  & ",fmt.format(carga[0]/1e3)," & ",fmt.format(carga[1]/1e3)," & ",fmt.format(carga[2]/1e3),"\\\\\n")
  cierraSupertabular(archivo) 


