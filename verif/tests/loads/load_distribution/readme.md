# Load tests
Tests that verify different techniques to distribute loads over nodes or elements.

# Center of rigidity
To compute the Center of Rigidity (CR) of a finite element (FE) model, the most common approach is to compute the rotations due to unit loads along the X and Y axes and unit torsional moment at an arbitrary point.

## Step-by-Step Computational Method
To find the CR coordinates $(X_{CR}, Y_{CR})$ at a specific floor level, the FE solver generates three independent, fictitious static load cases applied at an arbitrary point (usually the Center of Mass, $(X_M, Y_M)$) on that floor: [2, 3, 4]

## 1. Define the 3 Static Unit Load Cases

* Load Case 1 (Force in X): Apply a global unit lateral force ($F_x = 1$) at the arbitrary master node $(X_M, Y_M)$. Record the resulting global rotation of the diaphragm around the vertical Z-axis, denoted as $R_{zx}$. [2, 3, 4] 
* Load Case 2 (Force in Y): Apply a global unit lateral force ($F_y = 1$) at the same arbitrary master node $(X_M, Y_M)$. Record the resulting global rotation of the diaphragm, denoted as $R_{zy}$. [2, 3] 
* Load Case 3 (Moment around Z): Apply a global unit torsional moment ($M_z = 1$) at the same master node. Record the resulting global rotation of the diaphragm, denoted as $R_{zz}$. [2, 3] 
* 

## 2. Calculate the Center of Rigidity

By definition, the Center of Rigidity is the point where a lateral force causes pure translation and zero floor rotation. Using the structural rotations captured from the unit load cases, back-calculate the lever arms to locate this point relative to your arbitrary loading node: [3, 6, 7] 
$$X_{CR} = X_M - \frac{R_{zy}}{R_{zz}}$$ 
$$Y_{CR} = Y_M + \frac{R_{zx}}{R_{zz}}$$ 
(Note: Sign conventions may toggle depending on the global coordinate orientation of your finite element program, but the absolute offset remains the same). [3, 8] 
------------------------------


## References

- [Structural load](https://en.wikipedia.org/wiki/Structural_load)
- [Distributing coupling elements](https://web.mit.edu/calculix_v2.7/CalculiX/ccx_2.7/doc/ccx/node189.html)
- [ABAQUS Distributing coupling elements](https://classes.engineering.wustl.edu/2009/spring/mase5513/abaqus/docs/v6.6/books/stm/default.htm?startat=ch03s09ath99.html)
- [Distributing couplings implementation in Fortran](https://github.com/Dhondtguido/CalculiX/blob/4a7be12f5513878dcf34d04d2bde1d74b94dcd6d/src/distributingcouplings.f#L51)
- [Statically Equivalent Loads](https://portwooddigital.com/2025/04/14/statically-equivalent-loads/)
- [Temperature gradient](https://manual.midasuser.com/EN_Common/Gen/885/index.htm#Start/05_Load/08_Temperature_Loads/Temperature_Gradient.htm)

### Center of rigidity
[1] [https://bentleysystems.service-now.com](https://bentleysystems.service-now.com/community?id=kb_article&sysparm_article=KB0112771)
[2] [https://docs.bentley.com](https://docs.bentley.com/LiveContent/web/RAM%20Structural%20System%20Help-v4/en/GUID-E851A16F-2B2F-4AE8-95E7-0DE49F8240F8.html)
[3] [https://structuralacademy.com](https://structuralacademy.com/article/en/Center-of-Mass-Center-of-Rigidity)
[4] [https://web.wiki.csiamerica.com](https://web.wiki.csiamerica.com/wiki/spaces/etabs/pages/1474591/Center+of+rigidity)
[5] [https://web.wiki.csiamerica.com](https://web.wiki.csiamerica.com/wiki/spaces/etabs/pages/1474781/Center+of+rigidity+for+semi-rigid+diaphragm)
[6] [https://fawadnajam.com](https://fawadnajam.com/why-the-location-of-center-of-rigidity-changes-for-each-story-even-for-symmetric-structures/)
[7] [https://www.linkedin.com](https://www.linkedin.com/pulse/computation-centre-rigidity-using-space-gass-tien-cuong-nguyen)
[9] [https://help.idecad.com](https://help.idecad.com/ideCAD/center-of-rigidity)
[10] [https://www.eng-tips.com](https://www.eng-tips.com/threads/center-of-rigidity-calcution.278872/)
[11] [https://docs.bentley.com](https://docs.bentley.com/LiveContent/web/STAAD.Pro%20Help-v19/en/GUID-6965D1D2-6098-4246-A6C3-44A8D71C8A1D.html)
