# -*- coding: utf-8 -*-
from __future__ import print_function

print("\\section{",caption," Análisis cinemático de cuñas y bloques.}\n")
print("\\begin{center}\n")
print("\\begin{tabular}{|l|c|c|}\n")
print("\\hline\n")
print("Familia & Buzamiento & Dir. buzamiento\\\\\n")
print("\\hline\n")
print("P1 &", rad2deg(alpha1)," & ",rad2deg(beta1),"\\\\\n")
print("P2 &", rad2deg(alpha2)," & ",rad2deg(beta2),"\\\\\n")
print("P3 &", rad2deg(alpha3)," & ",rad2deg(beta3),"\\\\\n")
print("\\hline\n")
print("\\multicolumn{3}{|l|}{Tunnel axis: ",tunnelAxisVector,"}\\\\\n")
print("\\hline\n")
print("\\end{tabular}\n")
print("\\end{center}\n")
\JP000{ \processPolyhedralAngle("JP000",nmbTun){} }
\JP001{ \processPolyhedralAngle("JP001",nmbTun){} }
\JP010{ \processPolyhedralAngle("JP010",nmbTun){} }
\JP011{ \processPolyhedralAngle("JP011",nmbTun){} }
\JP100{ \processPolyhedralAngle("JP100",nmbTun){} }
\JP101{ \processPolyhedralAngle("JP101",nmbTun){} }
\JP110{ \processPolyhedralAngle("JP110",nmbTun){} }
\JP111{ \processPolyhedralAngle("JP111",nmbTun){} }
