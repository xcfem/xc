# -*- coding: utf-8 -*-

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaEsfuerzosBarrasSet(preprocessor,nmbComb, setName, fmt, fName):
  str= "" 
  k= 0
  s= preprocessor.getSets.getSet(setName)
  elems= s.elements
  for e in elements:
    str= nmbComb+" & "+e.tag+" & "
    k= 0
    sections= e.getSections()
    for s in sections:
      fName.write(str,k," & ")
      fName.write(fmt.format(e.getProp("N")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
      fName.write(fmt.format(e.getProp("Mx")/1e3)," & ",fmt.format(e.getProp("My")/1e3)," & ",fmt.format(e.getProp("Mz")/1e3),"\\\\\n")
      k=k+1


# Imprime los esfuerzos de los elementos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaEsfuerzosBarrasLineas(preprocessor, nmbComb, setNameLineas, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+setNameLineas
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,9,defCampos,idsCampos,caption) 

  s= preprocessor.getSets.getSet(setNameLineas)
  lines= s.getLines()
  for l in lines:
    nmb= l.getName() 
    fName.write("\hline\n")
    fName.write("\multicolumn{9}{|l|}{Esfuerzos en las barras de la linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listaEsfuerzosBarrasSet(nmbComb,nmb,fmt,fName) 
  cierraSupertabular(fName)

def listaEsfuerzosFCBarrasSet(preprocessor, nmbComb, setName, fmt, fName, nmbDiag):
  '''Print internal forces and capacity factor of the elements contained in the
     argument set.'''
  str= "" 
  k= 0
  s= preprocessor.getSets.getSet(setName)
  elems= s.elements
  for e in elements:
    str= nmbComb+" & "+e.tag+" & "
    k= 0
    sections= e.getSections()
    for s in sections:
      fName.write(str,k," & ")
      fName.write(fmt.format(e.getProp("N")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
      fName.write(fmt.format(e.getProp("Mx")/1e3)," & ",fmt.format(e.getProp("My")/1e3)," & ",fmt.format(e.getProp("Mz")/1e3)," & ",fmt.format(getCapacityFactor(nmbDiag)),"\\\\\n")
      k=k+1

def listaEsfuerzosFCBarrasLineas(nmbComb, setNameLineas, fmt, fName, encab, tit, nmbDiag):
  '''Print internal forces and capacity factor of the elements contained in the
     lines of the argument set.'''
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+setNameLineas
  defCampos= "|l|r|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz & FC \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m & - "
  cabeceraSupertabular(fName,10,defCampos,idsCampos,caption) 

  s= preprocessor.getSets.getSet(setNameLineas)
  lines= s.getLines()
  for l in lines:
    nmb= l.getName()
    fName.write("\hline\n")
    fName.write("\multicolumn{10}{|l|}{Esfuerzos en las barras de la linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listaEsfuerzosFCBarrasSet(nmbComb,nmb,fmt,fName,nmbDiag) 
  cierraSupertabular(fName) 


# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaDatosEsfuerzosTrussSet(nmbComb, setName, fmt, fName):
  s= preprocessor.getSets.getSet(setName)
  elems= s.elements
  for e in elements:
    fName.write(nmbComb," & ",e.tag," & ",fmt.format(e.getStrain()*1e2)," & ",fmt.format(e.getStress()/1e6)," & ",fmt.format(e.getN()/1e3),"\\\\\n")

# Imprime los esfuerzos de los elementos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaEsfuerzosTrussSet(nmbComb, setName, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+setName
  defCampos= "|l|r|r|r|r|"
  idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
  cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 
  listaDatosEsfuerzosTrussSet(nmbComb,setName,fmt,fName) 
  cierraSupertabular(fName) 

# Imprime los esfuerzos de los elementos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaEsfuerzosTrussLineas(nmbComb, setNameLineas, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Barras del conjunto: "+setNameLineas
  defCampos= "|l|r|r|r|r|"
  idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
  cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 

  s= preprocessor.getSets.getSet(setNameLineas)
  lines= s.getLines()
  for l in lines:
    nmb= l.getName()
    fName.write("\hline\n")
    fName.write("\multicolumn{5}{|l|}{Esfuerzos en elementos linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listaDatosEsfuerzosTrussSet(nmbComb,nmb,fmt,fName) 
  cierraSupertabular(fName) 


# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def listaEsfuerzosZeroLenghtSet(nmbComb, setName, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Elementos del conjunto: "+setName
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
  s= preprocessor.getSets.getSet(setName)
  elems= s.elements
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
def listaEsfuerzosElasticBeam3dSet(preprocessor, nmbComb, setName, fmt, fName):
  s= preprocessor.getSets.getSet(setName)
  elems= s.elements
  for e in elements:
    str= nmbComb+" & "+e.tag+" & "
    fName.write(str,1," & ")
    fName.write(fmt.format(e.getProp("N1")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
    fName.write(fmt.format(e.getProp("T")/1e3)," & ",fmt.format(e.getProp("My1")/1e3)," & ",fmt.format(e.getProp("Mz1")/1e3),"\\\\\n")
    fName.write(str,2," & ")
    fName.write(fmt.format(e.getProp("N2")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
    fName.write(fmt.format(e.getProp("T")/1e3)," & ",fmt.format(e.getProp("My2")/1e3)," & ",fmt.format(e.getProp("Mz2")/1e3),"\\\\\n")
