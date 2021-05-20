      INTEGER FUNCTION SKYPRI (DGPNT,J,JMIN)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPRI finds the row-index of the first non-zero in column J     *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    DGPNT (aii) : addresses of the diagonal coefficients             *
C*    J     (sii) : index of the column                                *
C*    JMIN  (sii) : index of the first column of the block             *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER DGPNT(*),J,JMIN
C
C**** executable statements ********************************************
C
      IF ( J .EQ. JMIN ) THEN
         SKYPRI = J - DGPNT(J) + 1
      ELSE
         SKYPRI = J - DGPNT(J) + 1 + DGPNT(J-1)
      END IF
C
      RETURN
C
C**** end of SKYPRI ****************************************************
C
      END
