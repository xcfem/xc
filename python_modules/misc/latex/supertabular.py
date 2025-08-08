# -*- coding: utf-8 -*-
''' Code related to supertabular LaTeX environment.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

def get_supertabular_head_tail(headTitles,justif,caption,tit2ndLine=None,label=None):
    '''Return the heading tex lines for a long table

    :param headTitles: list of titles for the table (first line e.g. ['tit1','tit2',...]
    :param justif: justification of columns (e.g. 'lcr')
    :param caption: caption text (e.g. 'Caption text')
    :param tit2ndLine:  list of titles, if any, for the second line of the head titles (defaults to None)
    :param label: label for the table (defaults to None)
    '''
    tableHead='\\begin{center} \n  '
    
    tableHead+='\\tablefirsthead{ \n'
    head='\\hline \n '
    for tit in headTitles:
        head+=tit+' & '
    head=head.removesuffix('& ')
    head+=' \\\\ \n'
    if tit2ndLine:
        for tit in tit2ndLine:
            head+=tit+' & '
        head=head.removesuffix('& ')
        head+=' \\\\ \n'
    #head+=' \\hline \n'
    tableHead+=head +' \\hline } \n'
    tableHead+='\\tablehead{'+head + '\\hline } \n'
    tableHead+='\\tabletail{ \\hline \\multicolumn{'+str(len(headTitles))+'}{r}{\\emph{Continúa \\ldots}}\\\\}  \n'
    tableHead+='\\tablelasttail{\\hline} \n'
    tableHead+='\\topcaption{'+caption+'} \n'
    if label:
        tableHead+='\\label{'+label+'} \n'
    tableHead+='\\begin{supertabular}{|'
    tableHead+=justif + '|} \n'
    tableTail='\\end{supertabular} \n'
    tableTail+='\\end{center} \n'
    return tableHead,tableTail

def cabeceraSupertabular(nmbArch,numCampos,defCampos,idsCampos,caption):
    'cabeceraSupertabular(nmbArch,numCampos,defCampos,idsCampos,caption):'
    nmbArch.write("\\begin{center}\n")
    nmbArch.write("\\tablefirsthead{%\n")
    nmbArch.write("\\hline\n")
    nmbArch.write(idsCampos)
    nmbArch.write("\\hline}\n")
    nmbArch.write("\\tablehead{%\n")
    nmbArch.write("\\hline\n")
    nmbArch.write("\\multicolumn{",numCampos,"}{|l|}{\\small\\sl \\ldots continúa}\\\\\n")
    nmbArch.write("\\hline\n")
    nmbArch.write(idsCampos)
    nmbArch.write("\\hline}\n")
    nmbArch.write("\\tabletail{%\n")
    nmbArch.write("\\hline\n")
    nmbArch.write("\\multicolumn{",numCampos,"}{|r|}{\\small\\sl continúa \\ldots}\\\\\n")
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
