# -*- coding: utf-8 -*-

def cabeceraSupertabular(nmbArch,numCampos,defCampos,idsCampos,caption):
    'cabeceraSupertabular(nmbArch,numCampos,defCampos,idsCampos,caption):'
    nmbArch.write("\\begin{center}\n")
    nmbArch.write("\\tablefirsthead{%\n")
    nmbArch.write("\\hline\n")
    nmbArch.write(idsCampos)
    nmbArch.write("\\hline}\n")
    nmbArch.write("\\tablehead{%\n")
    nmbArch.write("\\hline\n")
    nmbArch.write("\\multicolumn{",numCampos,"}{|l|}{\\small\\sl \ldots continúa}\\\\\n")
    nmbArch.write("\\hline\n")
    nmbArch.write(idsCampos)
    nmbArch.write("\\hline}\n")
    nmbArch.write("\\tabletail{%\n")
    nmbArch.write("\\hline\n")
    nmbArch.write("\\multicolumn{",numCampos,"}{|r|}{\\small\\sl continúa\ldots}\\\\\n")
    nmbArch.write("\\hline}\n")
    nmbArch.write("\\tablelasttail{\\hline}\n")
    nmbArch.write(caption,"\\\\\n")
    nmbArch.write("\\begin{supertabular}{",defCampos,"}\n")
    return

def cierraSupertabular(nmbArch):
    'cierraSupertabular(nmbArch):'
    nmbArch.write("\\end{supertabular}\n")
    nmbArch.write("\\end{center}\n")
    return
