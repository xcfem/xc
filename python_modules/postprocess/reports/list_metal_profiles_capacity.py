# -*- coding: utf-8 -*-

def printTNListingHeaderSteelShape(archivo, tit):
  '''Open the normal stresses listing.'''
  archivo.write("\\begin{center}\n")
  archivo.write("\\begin{footnotesize}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & $M_y$  & $M_z$  & FC \\\\\n")
  archivo.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|l|}{\\small\\sl \\ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & $M_y$  & $M_z$  & FC \\\\\n")
  archivo.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tabletail{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|r|}{\\small\\sl continúa\\ldots}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablelasttail{\\hline}\n")
  archivo.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")


def printTNListingEndSteelShape(archivo):
  '''Closes the listing.'''
  archivo.write("\\end{supertabular}\n")
  archivo.write("\\end{footnotesize}\n")
  archivo.write("\\end{center}\n")


def listSteelShapeTNStrengthElasticRange(xcSet, fName, titulo):
    '''
    Print the results of normal stresses checking in an element set
    whose material is a steel shape.
    '''
    printTNListingHeaderSteelShape(fName,titulo) 
    elems= xcSet.elements
    tagFCTNCP= 0
    FCTNCP= 0
    HIPCPTN= ""
    for e in elems:
        fName.write(e.tag," & ",HIPCPTN," & ",'{:5.2f}',format(e.getProp("NCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("MyCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("MzCP")/1e3)," & ",'{:3.2f}'.format(e.getProp("FCTNCP")),"\\\\\n")
        if(e.getProp("FCTNCP")>FCTNCP):
            FCTNCP= e.getProp("FCTNCP")
            HIPCPTN= e.getProp("HIPCPTN")
            tagFCTNCP= e.tag
    fName.write("\\hline\n")
    fName.write("\\multicolumn{6}{|c|}{Caso pésimo}\\\\\n")
    fName.write("\\hline\n")
    fName.write(tagFCTNCP," & ",HIPCPTN," &  &  &  & ",'{:3.2f}'.format(FCTNCP),"\\\\\n")

    printTNListingEndSteelShape(fName) 



def printVListingHeaderSteelShape(archivo, tit):
  '''Open the shear listing.'''
  archivo.write("\\begin{center}\n")
  archivo.write("\\begin{footnotesize}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $V_y$  & $V_z$  & FC \\\\\n")
  archivo.write("      &      & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|l|}{\\small\\sl \\ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $V_y$  & $V_z$  & FC \\\\\n")
  archivo.write("      &      & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tabletail{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|r|}{\\small\\sl continúa\\ldots}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablelasttail{\\hline}\n")
  archivo.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")

def listSteelShapeVStrength(xcSet, fName, titulo):
    '''Print the results of shear stress checking in an element set
    whose material is a steel shape.
    '''
    printVListingHeaderSteelShape(fName,titulo) 
    elems= xcSet.elements
    tagFCVCP= 0
    FCVCP= 0
    HIPCPV= ""
    for e in elems:
        fName.write(e.tag," & ",HIPCPV," & ",'{:5.2f}'.format(e.getProp("VyCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("VzCP")/1e3)," & ",'{:3.2f}'.format(e.getProp("FCVCP")),"\\\\\n")
        if(e.getProp("FCVCP")>FCVCP):
            FCVCP= e.getProp("FCVCP")
            HIPCPV= e.getProp("HIPCPV")
            tagFCVCP= e.tag
    fName.write("\\hline\n")
    fName.write("\\multicolumn{5}{|c|}{Caso pésimo}\\\\\n")
    fName.write("\\hline\n")
    fName.write(tagFCVCP," & ",HIPCPV," &  &  & ",format(e.getProp("FCVCP"),'{:3.2f}'),"\\\\\n")
    printTNListingEndSteelShape(fName) 

