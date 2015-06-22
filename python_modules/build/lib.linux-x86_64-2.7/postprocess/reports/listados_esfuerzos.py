# -*- coding: utf-8 -*-

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaEsfuerzosBarrasSet(mdlr,nmbComb, nmbSet, fmt, fName):
  str= "" 
  k= 0
  s= mdlr.getSetLoader.getSet(nmbSet)
  elems= s.getElements()
  for e in elements:
    str= nmbComb+" & "+e.tag+" & "
    k= 0
    secciones= e.getSections()
    for s in secciones:
      fName.write(str,k," & ")
      fName.write(fmt.format(e.getProp("N")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
      fName.write(fmt.format(e.getProp("Mx")/1e3)," & ",fmt.format(e.getProp("My")/1e3)," & ",fmt.format(e.getProp("Mz")/1e3),"\\\\\n")
      k=k+1


# Imprime los esfuerzos de los elementos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaEsfuerzosBarrasLineas(mdlr, nmbComb, nmbSetLineas, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+nmbSetLineas
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,9,defCampos,idsCampos,caption) 

  s= mdlr.getSetLoader.getSet(nmbSetLineas)
  lines= s.getLines()
  for l in lines:
    nmb= l.getName() 
    fName.write("\hline\n")
    fName.write("\multicolumn{9}{|l|}{Esfuerzos en las barras de la linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listaEsfuerzosBarrasSet(nmbComb,nmb,fmt,fName) 
  cierraSupertabular(fName)

# Imprime los esfuerzos y el factor de capacidad de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaEsfuerzosFCBarrasSet(mdlr, nmbComb, nmbSet, fmt, fName, nmbDiag):
  str= "" 
  k= 0
  s= mdlr.getSetLoader.getSet(nmbSet)
  elems= s.getElements()
  for e in elements:
    str= nmbComb+" & "+e.tag+" & "
    k= 0
    secciones= e.getSections()
    for s in secciones:
      fName.write(str,k," & ")
      fName.write(fmt.format(e.getProp("N")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
      fName.write(fmt.format(e.getProp("Mx")/1e3)," & ",fmt.format(e.getProp("My")/1e3)," & ",fmt.format(e.getProp("Mz")/1e3)," & ",fmt.format(getFactorCapacidad(nmbDiag)),"\\\\\n")
      k=k+1

# Imprime los esfuerzos y el factor de capacidad de los elementos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaEsfuerzosFCBarrasLineas(nmbComb, nmbSetLineas, fmt, fName, encab, tit, nmbDiag):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+nmbSetLineas
  defCampos= "|l|r|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz & FC \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m & - "
  cabeceraSupertabular(fName,10,defCampos,idsCampos,caption) 

  s= mdlr.getSetLoader.getSet(nmbSetLineas)
  lines= s.getLines()
  for l in lines:
    nmb= l.getName()
    fName.write("\hline\n")
    fName.write("\multicolumn{10}{|l|}{Esfuerzos en las barras de la linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listaEsfuerzosFCBarrasSet(nmbComb,nmb,fmt,fName,nmbDiag) 
  cierraSupertabular(fName) 


# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaDatosEsfuerzosTrussSet(nmbComb, nmbSet, fmt, fName):
  s= mdlr.getSetLoader.getSet(nmbSet)
  elems= s.getElements()
  for e in elements:
    fName.write(nmbComb," & ",e.tag," & ",fmt.format(e.getStrain()*1e2)," & ",fmt.format(e.getStress()/1e6)," & ",fmt.format(e.getAxil()/1e3),"\\\\\n")

# Imprime los esfuerzos de los elementos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaEsfuerzosTrussSet(nmbComb, nmbSet, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+nmbSet
  defCampos= "|l|r|r|r|r|"
  idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
  cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 
  listaDatosEsfuerzosTrussSet(nmbComb,nmbSet,fmt,fName) 
  cierraSupertabular(fName) 

# Imprime los esfuerzos de los elementos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaEsfuerzosTrussLineas(nmbComb, nmbSetLineas, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+nmbSetLineas
  defCampos= "|l|r|r|r|r|"
  idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
  cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 

  s= mdlr.getSetLoader.getSet(nmbSetLineas)
  lines= s.getLines()
  for l in lines:
    nmb= l.getName()
    fName.write("\hline\n")
    fName.write("\multicolumn{5}{|l|}{Esfuerzos en elementos linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listaDatosEsfuerzosTrussSet(nmbComb,nmb,fmt,fName) 
  cierraSupertabular(fName) 


# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaEsfuerzosZeroLenghtSet(nmbComb, nmbSet, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Elementos del conjunto: "+nmbSet
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & Id & N & Vx & Vy & T & Mx & My \\\\\n - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption) 

  str= "" 
  k= 0
  VX= 0
  VY= 0
  N= 0
  T= 0
  momY= 0
  momZ= 0
  s= mdlr.getSetLoader.getSet(nmbSet)
  elems= s.getElements()
  for e in elements:
    str= nmbComb+" & "+e.tag+" & "
    mats= e.getMaterials()
    VX= mats[0].getStress()
    VY= mats[1].getStress()
    N= mats[2].getStress()
    T= mats[3].getStress()
    momY= mats[4].getStress()
    momZ= mats[5].getStress()

    fName.write(str)
    fName.write(fmt.format(N/1e3)," & ",fmt.format(VX/1e3)," & ",fmt.format(VY/1e3)," & ")
    fName.write(fmt.format(T/1e3)," & ",fmt.format(momY/1e3)," & ",fmt.format(momZ/1e3),"\\\\\n")
  cierraSupertabular(fName) 

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaEsfuerzosElasticBeam3dSet(mdlr, nmbComb, nmbSet, fmt, fName):
  s= mdlr.getSetLoader.getSet(nmbSet)
  elems= s.getElements()
  for e in elements:
    str= nmbComb+" & "+e.tag+" & "
    fName.write(str,1," & ")
    fName.write(fmt.format(e.getProp("N1")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
    fName.write(fmt.format(e.getProp("T")/1e3)," & ",fmt.format(e.getProp("My1")/1e3)," & ",fmt.format(e.getProp("Mz1")/1e3),"\\\\\n")
    fName.write(str,2," & ")
    fName.write(fmt.format(e.getProp("N2")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
    fName.write(fmt.format(e.getProp("T")/1e3)," & ",fmt.format(e.getProp("My2")/1e3)," & ",fmt.format(e.getProp("Mz2")/1e3),"\\\\\n")
