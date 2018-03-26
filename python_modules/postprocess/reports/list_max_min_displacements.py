# -*- coding: utf-8 -*-

'''
Imprime valores extremos de los desplazamientos of the nodes del conjunto
cuyo nombre se pasa como parámetro.
'''
def listNodeUMaxUMin(preprocessor,setName, tit, fmt, fName):
  caption= tit
  defCampos= "|l|r|r|r|r|"
  idsCampos= "IdN & $U_{max}$ & Comb. & $U_{min}$ & Comb. \\\\\n - & mm & - & mm & - \\\\\n"
  cabeceraSupertabular(fName,5,defCampos,idsCampos,caption)
  s= preprocessor.getSetLoader.getSet(setName)
  nodes= s.getNodes()
  tagUMaxMax= 0
  UMaxMax= 0
  CombUMaxMax= None
  tagUMinMin= 0
  UMinMin= 0
  CombUMinMin= None 
  for n in nodes:
    fName.write(tag," & ",fmt.format(UMax*1e3)," & ",CombUMax," & ",fmt.format(UMin*1e3)," & ",CombUMin,"\\\\\n")
    if(UMax>UMaxMax): 
      UMaxMax= UMax
      CombUMaxMax= CombUMax
      tagUMaxMax= tag
    if(UMin<UMinMin):
       UMinMin= UMin
       CombUMinMin= CombUMin
       tagUMinMin= tag
  fName.write("\\hline\n")
  fName.write("\\multicolumn{5}{|c|}{Valores extremos}\\\\\n")
  fName.write("\\hline\n")
  fName.write(tagUMaxMax," & ",fmt.format(UMaxMax*1e3)," & ",CombUMaxMax," & - & - \\\\\n")
  fName.write("\\hline\n")
  fName.write(tagUMinMin," & - & - &",fmt.format(UMinMin*1e3)," & ",CombUMinMin,"\\\\\n")
  cierraSupertabular(fName) 


'''
Imprime valores extremos de los desplazamientos of the nodes del conjunto
cuyo nombre se pasa como parámetro.
'''
def listNodeVMaxVMin(setName, tit, fmt, fName):
  caption= tit
  defCampos= "|l|r|r|r|r|"
  idsCampos= "IdN & $V_{max}$ & Comb. & $V_{min}$ & Comb. \\\\\n - & mm & - & mm & - \\\\\n"
  cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 
  tagVMaxMax= 0
  VMaxMax= 0
  CombVMaxMax= None
  tagVMinMin= 0
  VMinMin= 0
  CombVMinMin= None
 
  s= preprocessor.getSetLoader.getSet(setName)
  nodes= s.getNodes()
  for n in nodes:
    fName.write(tag," & ",fmt.format(VMax*1e3)," & ",CombVMax," & ",fmt.format(VMin*1e3)," & ",CombVMin,"\\\\\n")
    if(VMax>VMaxMax):
      VMaxMax= VMax
      CombVMaxMax= CombVMax
      tagVMaxMax= tag
    if(VMin<VMinMin):
      VMinMin= VMin
      CombVMinMin= CombVMin
      tagVMinMin= tag
  fName.write("\\hline\n")
  fName.write("\\multicolumn{5}{|c|}{Valores extremos}\\\\\n")
  fName.write("\\hline\n")
  fName.write(tagVMaxMax," & ",fmt.format(VMaxMax*1e3)," & ",CombVMaxMax," & - & - \\\\\n")
  fName.write("\\hline\n")
  fName.write(tagVMinMin," & - & - &",fmt.format(VMinMin*1e3)," & ",CombVMinMin,"\\\\\n")
  cierraSupertabular(fName) 

'''
Imprime valores extremos de los desplazamientos of the nodes del conjunto
cuyo nombre se pasa como parámetro.
'''
def listNodeWMaxWMin(preprocessor, setName, tit, fmt, fName):
  caption= tit
  defCampos= "|l|r|r|r|r|"
  idsCampos= "IdN & $W_{max}$ & Comb. & $W_{min}$ & Comb. \\\\\n - & mm & - & mm & - \\\\\n"
  cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 
  tagWMaxMax= 0
  WMaxMax= 0
  CombWMaxMax= None
  tagWMinMin= 0
  WMinMin= 0
  CombWMinMin= None
 
  s= preprocessor.getSetLoader.getSet(setName)
  nodes= s.getNodes()
  for n in nodes:
    fName.write(tag," & ",fmt.format(WMax*1e3)," & ",CombWMax," & ",fmt.format(WMin*1e3)," & ",CombWMin,"\\\\\n")
    if(WMax>WMaxMax):
       WMaxMax= WMax
       CombWMaxMax= CombWMax
       tagWMaxMax= tag
    if(WMin<WMinMin): 
      WMinMin= WMin
      CombWMinMin= CombWMin
      tagWMinMin= tag

  fName.write("\\hline\n")
  fName.write("\\multicolumn{5}{|c|}{Valores extremos}\\\\\n")
  fName.write("\\hline\n")
  fName.write(tagWMaxMax," & ",fmt.format(WMaxMax*1e3)," & ",CombWMaxMax," & - & - \\\\\n")
  fName.write("\\hline\n")
  fName.write(tagWMinMin," & - & - &",fmt.format(WMinMin*1e3)," & ",CombWMinMin,"\\\\\n")
  cierraSupertabular(fName) 
