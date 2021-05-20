      SUBROUTINE SKYPF1 (INDEX,JMAX,JMIN,MCOLS,MROWS,MFUNIT,DGPNT,
     &                   BLOCK,NBLOCK,LBLOCK,A,B,D,RW,TW,INFO) 
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPF1 updates (A) with the blocks already factorized            *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    INDEX  (aiw) : indexes of the columns copied into (RW)           *
C*    JMAX   (sii) : index of the last  column of (A)                  *
C*    JMIN   (sii) : index of the first column of (A)                  *
C*    MCOLS  (sii) : maximum number of columns stored in (RW)          *
C*    MROWS  (sii) : maximum number of rows    stored in (RW)          *
C*    MFUNIT (sii) : file unit for (B)                                 *
C*    DGPNT  (aii) : addresses of the diagonal coefficients            *
C*    BLOCK  (aii) : blocking information                              *
C*    NBLOCK (sii) : number of blocks                                  *
C*    LBLOCK (sii) : index of the current block                        *
C*    A      (arb) : block being   factorized                          *
C*    B      (arw) : block already factorized                          *
C*    D      (ari) : reciprocal of the pivots                          *
C*    RW     (arw) : work array, MROWSxMCOLS, for storing submatrices  *
C*    TW     (arw) : work array, MROWSxMROWS, for storing submatrices  *
C*    INFO   (sio) : information on the execution of SKYPF1            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                                                                     *
C*  - SUBPROGRAMS :                                                    *
C*    SKYPCI,SKYPRI,SKYPSB,SKYPSP                                      *
C*                                                                     *
C*  - BLAS kernels :                                                   *
C*    DGEMV,DSCAL,DTRSM                                                *
C*                                                                     *
C*  - INTRINSIC FUNCTIONS :                                            *
C*    MAX0,MIN0                                                        *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),INDEX(*),INFO,JMAX,JMIN,
     &                 LBLOCK,MCOLS,MFUNIT,MROWS,NBLOCK
      DOUBLE PRECISION A(*),B(*),D(*),RW(MROWS,*),TW(MROWS,*)
C
C==== local variables ==================================================
C
      INTEGER          IABOVE,IBELOW,IBLOCK,ILEFT,ILOWER,IMAX,IMIN,
     &                 INEXT,IRIGHT,IUPPER,J,JABOVE,JBELOW,JFIRST,
     &                 JLEFT,JLOWER,JNEXT,JRIGHT,JSIZE,JSPAN,
     &                 JSTART,JUPPER,NCOLS,NROWS
C
C==== subprograms ======================================================
C
      INTEGER          SKYPCI,SKYPRI
C
C==== intrinsic functions ==============================================
C
      INTEGER          MAX0,MIN0
C
C**** executable statements ********************************************
C
      INFO = 0
C
      IF ( NBLOCK .GT. 1 ) REWIND (UNIT=MFUNIT,ERR=1)
C
C.... JABOVE is the minimum row-index above the row with index JMIN ....
C.... JBELOW is the maximum row-index above the row with index JMIN ....
C
      JABOVE = BLOCK(3,LBLOCK)
      JBELOW = JMIN - 1
C
C.... previous LBLOCK-1 factorized blocks ..............................
C
      DO 010 IBLOCK = 1,LBLOCK-1
C
         IMIN   = BLOCK(1,IBLOCK)
         IMAX   = BLOCK(2,IBLOCK) 
	 IABOVE = BLOCK(3,IBLOCK)
	 IBELOW = BLOCK(1,IBLOCK) - 1
C
         IF ( IMAX .LT. JABOVE ) THEN 
C
C.......... this block can be skipped ..................................
C
            READ (UNIT=MFUNIT,ERR=1)
C
         ELSE
C
C.......... this block is taken into account ...........................
C
            READ (UNIT=MFUNIT,ERR=1) (B(J),J=1,DGPNT(IMAX)) 
C
C.......... rows in (B) with indexes less than IMIN ....................
C
            DO 020 ILOWER = IABOVE,IBELOW,MROWS
C
               IUPPER = MIN0(ILOWER+MROWS-1,IBELOW)
               NROWS  = IUPPER - ILOWER + 1
C
C............. update (A) using submatrices of (B) .....................
C
               INEXT = IMIN
               JLEFT = SKYPCI(DGPNT,IUPPER,JMIN,JMAX,JMIN)
C
               IF ( JLEFT .GT. 0 ) THEN
C
                  IRIGHT = SKYPCI(DGPNT,IUPPER,IMAX,INEXT,IMIN)
                  JRIGHT = SKYPCI(DGPNT,IUPPER,JMAX,JLEFT,JMIN)
C
                  DO 030 WHILE ( INEXT.LE.IRIGHT )
C
                     ILEFT = SKYPCI(DGPNT,IUPPER,INEXT,IRIGHT,IMIN)
                     INEXT = ILEFT
C
C................... copy the submatrix of (B) into (RW) ...............
C
                     CALL SKYPSB (DGPNT,ILOWER,IUPPER,INDEX,INEXT,
     &                            IRIGHT,IMIN,IMAX,MCOLS,
     &                            MROWS,NCOLS,B,RW,'R')
C
C................... update the j-th column of (A) .....................
C
                     DO 040 J = JLEFT,JRIGHT
                        JSTART = SKYPRI(DGPNT,J,JMIN)
                        JSTART = MAX0(ILOWER,JSTART)
                        JSIZE  = ILOWER + NROWS - JSTART
                        IF ( JSIZE .GT. 0 ) THEN
                           CALL DGEMV ('T',JSIZE,NCOLS,-1.0D0,
     &                                RW(JSTART-ILOWER+1,1),MROWS,
     &                                A(DGPNT(J)+JSTART-J),1,+1.0D0,
     &                                A(DGPNT(J)+ILEFT -J),1)
                        END IF
  040                CONTINUE
C
  030             CONTINUE
C
               END IF
C
  020       CONTINUE
C
C.......... rows in (B) with indexes between IMIN and IMAX .............
C
            DO 050 ILOWER = IMIN,IMAX,MROWS
C
               IUPPER = MIN0(ILOWER+MROWS-1,IMAX)
               JLEFT  = SKYPCI(DGPNT,IUPPER,JMIN,JMAX,JMIN)
C
               IF ( JLEFT .GT. 0 ) THEN
C
C................ shadow of the diagonal submatrix of (B) into (A) .....
C
                  JRIGHT = SKYPCI(DGPNT,IUPPER,JMAX,JLEFT,JMIN)
C
C................ copy the submatrix of (B) into (TW) ..................
C
                  CALL SKYPSB (DGPNT,ILOWER,IUPPER,INDEX,ILOWER,
     &                         IUPPER,IMIN,IUPPER,MROWS,
     &                         MROWS,NROWS,B,TW,'T')
C
                  DO 060 WHILE ( JLEFT .LE. JRIGHT )
C
C................... copy the submatrix of (A) into (RW) ...............
C
                     CALL SKYPSB (DGPNT,ILOWER,IUPPER,INDEX,JLEFT,
     &                            JRIGHT,JMIN,IUPPER,MCOLS,
     &                            MROWS,NCOLS,A,RW,'R')
C
C................... updating: (RW)=(TW')**(-1)*(RW) ...................
C
                     CALL DTRSM  ('L','U','T','U',NROWS,NCOLS,1.0D0,
     &                            TW,MROWS,RW,MROWS)
C
C................... copy (RW) back into (A) ...........................
C
                     CALL SKYPSP (DGPNT,ILOWER,IUPPER,INDEX,JMIN,
     &                            MCOLS,MROWS,NCOLS,A,RW,'R')
C
  060             CONTINUE 
C
C................ shadow of the nondiagonal submatrix of (B) into (A) ..
C
                  IF ( IMAX .GT. IUPPER ) THEN
C
                     INEXT = SKYPCI(DGPNT,IUPPER,IUPPER+1,IMAX,IMIN)
                     JLEFT = SKYPCI(DGPNT,IUPPER,JMIN,JMAX,JMIN)
C
                     IF ( ( INEXT .GT. 0 ) .AND. ( JLEFT .GT. 0 ) ) THEN
C
                        IRIGHT = SKYPCI(DGPNT,IUPPER,IMAX,INEXT,IMIN)
                        JRIGHT = SKYPCI(DGPNT,IUPPER,JMAX,JLEFT,JMIN)
C
                        DO 070 WHILE ( INEXT .LE. IRIGHT )
C
                           ILEFT = SKYPCI(DGPNT,IUPPER,INEXT,IMAX,IMIN)
                           INEXT = ILEFT
C
C......................... copy the submatrix of (B) into (RW) .........
C
                           CALL SKYPSB (DGPNT,ILOWER,IUPPER,INDEX,INEXT,
     &                                  IRIGHT,IMIN,IMAX,MCOLS,
     &                                  MROWS,NCOLS,B,RW,'R')
C
C......................... update the j-th column of (A) ...............
C
                           DO 080 J = JLEFT,JRIGHT
                              JSTART = SKYPRI(DGPNT,J,JMIN)
                              JSTART = MAX0(ILOWER,JSTART)
                              JSIZE  = ILOWER + NROWS - JSTART
                              IF ( JSIZE .GT. 0 ) THEN
                              CALL DGEMV ('T',JSIZE,NCOLS,-1.0D0,
     &                                    RW(JSTART-ILOWER+1,1),MROWS,
     &                                    A(DGPNT(J)+JSTART-J),1,+1.0D0,
     &                                    A(DGPNT(J)+ILEFT -J),1)
                              END IF
  080                      CONTINUE
C
  070                   CONTINUE
C
                     END IF
C
                  END IF
C
               END IF
C
  050       CONTINUE
C
         END IF
C
  010 CONTINUE
C
      IF ( LBLOCK .EQ. 1 ) RETURN
C
C.... submatrices of (A) with row-indexes from JABOVE to JBELOW ........
C
      JFIRST = JMIN
C
      DO 090 WHILE ( JFIRST .LE. JMAX ) 
C
         JRIGHT = MIN0(JFIRST+MROWS-1,JMAX)
C
         DO 100 JLOWER = JABOVE,JBELOW,MROWS
C
C.......... factorize the submatrices of (A) ...........................
C
            JUPPER = MIN0(JLOWER+MROWS-1,JBELOW)
            NROWS  = JUPPER - JLOWER + 1
C
            JLEFT = SKYPCI(DGPNT,JUPPER,JFIRST,JRIGHT,JMIN)
            JNEXT = JLEFT
C
            IF ( JLEFT .GT. 0 ) THEN
C
C............. copy the submatrix of (A) into (RW) .....................
C
               CALL SKYPSB (DGPNT,JLOWER,JUPPER,INDEX,JNEXT,JRIGHT,JMIN,
     &                      JMAX,MROWS,MROWS,NCOLS,A,RW,'R')
C
C............. scale (RW) ..............................................
C
               DO 110 J = JLOWER,JUPPER
                  IF ( D(J) .NE. 0.0D0 ) THEN
                     CALL DSCAL (NCOLS,D(J),RW(J-JLOWER+1,1),MROWS)
                  END IF   
  110          CONTINUE
C
C............. update the j-th column of (A) ...........................
C
               DO 120 J = JLEFT,JMAX   
                  JSTART = SKYPRI(DGPNT,J,JMIN)
                  JSTART = MAX0(JLOWER,JSTART)
                  JSIZE  = JLOWER + NROWS - JSTART
                  IF ( JSIZE .GT. 0 ) THEN
                     JSPAN = MIN0(NCOLS,J-JLEFT+1)
                     CALL DGEMV ('T',JSIZE,JSPAN,-1.0D0,
     &                           RW(JSTART-JLOWER+1,1),MROWS,
     &                           A(DGPNT(J)+JSTART-J),1,+1.0D0,
     &                           A(DGPNT(J)+JLEFT -J),1)
                  END IF
  120          CONTINUE
C
C............. copy (RW) back into (A) .................................
C
               CALL SKYPSP (DGPNT,JLOWER,JUPPER,INDEX,JMIN,
     &                      MROWS,MROWS,NCOLS,A,RW,'R')
C
            END IF
C
  100    CONTINUE
C
         JFIRST = JRIGHT + 1
C
  090 CONTINUE
C
      RETURN
C
C.... an IO error has occurred .........................................
C
    1 INFO = 1
C
      RETURN
C
C**** end of SKYPF1 ****************************************************
C
      END
