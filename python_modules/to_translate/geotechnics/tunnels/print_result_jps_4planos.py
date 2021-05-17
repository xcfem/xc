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
\JP0000{ \processPolyhedralAngle("JP0000",nmbTun){} }
\JP0001{ \processPolyhedralAngle("JP0001",nmbTun){} }
\JP0010{ \processPolyhedralAngle("JP0010",nmbTun){} }
\JP0011{ \processPolyhedralAngle("JP0011",nmbTun){} }
\JP0100{ \processPolyhedralAngle("JP0100",nmbTun){} }
\JP0101{ \processPolyhedralAngle("JP0101",nmbTun){} }
\JP0110{ \processPolyhedralAngle("JP0110",nmbTun){} }
\JP0111{ \processPolyhedralAngle("JP0111",nmbTun){} }
\JP1000{ \processPolyhedralAngle("JP1000",nmbTun){} }
\JP1001{ \processPolyhedralAngle("JP1001",nmbTun){} }
\JP1010{ \processPolyhedralAngle("JP1010",nmbTun){} }
\JP1011{ \processPolyhedralAngle("JP1011",nmbTun){} }
\JP1100{ \processPolyhedralAngle("JP1100",nmbTun){} }
\JP1101{ \processPolyhedralAngle("JP1101",nmbTun){} }
\JP1110{ \processPolyhedralAngle("JP1110",nmbTun){} }
\JP1111{ \processPolyhedralAngle("JP1111",nmbTun){} }
