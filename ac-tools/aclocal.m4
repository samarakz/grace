dnl ACX_SAVE_STATE/ACX_RESTORE_STATE
dnl Save/restore flags
dnl 
dnl ACX_SAVE_STATE
AC_DEFUN(ACX_SAVE_STATE,
[
    save_CFLAGS=$CFLAGS
    save_CPPFLAGS=$CPPFLAGS
    save_LDFLAGS=$LDFLAGS
    save_LIBS=$LIBS
])
dnl ACX_RESTORE_STATE
AC_DEFUN(ACX_RESTORE_STATE,
[
    CFLAGS=$save_CFLAGS
    CPPFLAGS=$save_CPPFLAGS
    LDFLAGS=$save_LDFLAGS
    LIBS=$save_LIBS
])


AC_DEFUN(ACX_CHECK_CC_FLAGS,
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether ${CC-cc} accepts $1, ac_$2,
[echo 'void f(){}' > conftest.c
if test -z "`${CC-cc} $1 -c conftest.c 2>&1`"; then
	ac_$2=yes
else
	ac_$2=no
fi
rm -f conftest*
])
if test "$ac_$2" = yes; then
	:
	$3
else
	:
	$4
fi
])

dnl **** Check for gcc strength-reduce bug ****
AC_DEFUN(ACX_GCC_STRENGTH_REDUCE,
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK( "for gcc strength-reduce bug", ac_cv_c_gcc_strength_bug,
  AC_TRY_RUN([
    int main(void) {
      static int Array[[3]];
      unsigned int B = 3;
      int i;
      for(i=0; i<B; i++) Array[[i]] = i - 3;
      exit( Array[[1]] != -2 );
    }
    ],

    ac_cv_c_gcc_strength_bug="no",
    ac_cv_c_gcc_strength_bug="yes",
    ac_cv_c_gcc_strength_bug="yes")
  )
  
  if test "$ac_cv_c_gcc_strength_bug" = "yes"
  then
    :
    $1
  else
    :
    $2
  fi
])

dnl **** Checks for FPU arithmetics
AC_DEFUN(ACX_CHECK_FPU,
[
AC_CACHE_CHECK( "for FPU arithmetics type", ac_cv_c_fpu_arithmetics_type,
                  AC_TRY_RUN([
#include <stdio.h>
#include <string.h>

#define LOG2EA 0.44269504088896340735992
#define ACCURACY "%1.4f"

typedef union {unsigned short s[[4]]; double d;} XTYPE;

XTYPE X[[]] = { 
              {{0,0,0,0}},			    /* Unknown             */
              {{0x3fdc,0x551d,0x94ae,0x0bf8}}, 	    /* Big endian     IEEE */
              {{0x0bf8,0x94ae,0x551d,0x3fdc}}, 	    /* Littile endian IEEE */
              {{0037742,0124354,0122560,0057703}}   /* DEC                 */
            };
            
int main (void)
{
    int i; char TMPSTR[[1024]]; char LOG2EA_STR[[80]];
    i = 0;
 
    sprintf(LOG2EA_STR, ACCURACY, LOG2EA);
 
    for (i=3; i >= 0; i--)
    {
      	sprintf(TMPSTR, ACCURACY, X[[i]].d);
      	if (strcmp(TMPSTR, LOG2EA_STR) == 0) {
    	    break;
      	}
    }
 
    exit(i);
}],
    ac_cv_c_fpu_arithmetics_type="Unknown",
    [case "$?" in
      "1"[)] ac_cv_c_fpu_arithmetics_type="Big endian IEEE" ;;
      "2"[)] ac_cv_c_fpu_arithmetics_type="Little endian IEEE" ;;
      "3"[)] ac_cv_c_fpu_arithmetics_type="DEC" ;;
    esac],
    ac_cv_c_fpu_arithmetics_type="Unknown") )

case "$ac_cv_c_fpu_arithmetics_type" in
  "DEC")                AC_DEFINE(HAVE_DEC_FPU) ;;
  "Little endian IEEE") AC_DEFINE(HAVE_LIEEE_FPU) ;;
  "Big endian IEEE")    AC_DEFINE(HAVE_BIEEE_FPU) ;;
esac

])


AC_DEFUN(ACX_ANSI_TYPES,
[
  dnl **** Check which ANSI integer type is 16 bit
  AC_CACHE_CHECK( "which ANSI integer type is 16 bit", ac_16bit_type,
                  AC_TRY_RUN([
  int main(void) {
    if (sizeof(short)==2)
      return(0);
    else if (sizeof(int)==2)
      return(1);
    else
      return(2);
  }], ac_16bit_type="short", ac_16bit_type="int", ac_16bit_type=))
  if test "$ac_16bit_type" = "short"
  then
    T1_AA_TYPE16="short"
  else
    T1_AA_TYPE16="int"
  fi

  dnl **** Check which ANSI integer type is 32 bit
  AC_CACHE_CHECK( "which ANSI integer type is 32 bit", ac_32bit_type,
                  AC_TRY_RUN([
  int main(void) {
    if (sizeof(int)==4)
      return(0);
    else if (sizeof(long)==4)
      return(1);
    else
      return(2);
  }], ac_32bit_type="int", ac_32bit_type="long", ac_32bit_type=))
  if test "$ac_32bit_type" = "int"
  then
    T1_AA_TYPE32="int"
  else
    T1_AA_TYPE32="long"
  fi

  dnl **** Check which ANSI integer type is 64 bit 
  AC_CACHE_CHECK( "which ANSI integer type is 64 bit", ac_64bit_type,
                  AC_TRY_RUN([
  int main(void) {
    if (sizeof(long)==8)
      return(0);
    else
      return(1);
  }], ac_64bit_type="long", ac_64bit_type="<none>", ac_64bit_type=))
  if test "$ac_64bit_type" = "long"
  then
    T1_AA_TYPE64="long"
  else
    T1_AA_TYPE64=
  fi
])

dnl **** Check for buggy realloc()
AC_DEFUN(ACX_CHECK_REALLOC,
[
AC_CACHE_CHECK( "whether realloc is buggy", ac_cv_c_realloc_bug,
                  AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
int main(void) {
  void *ptr;
  ptr = NULL;
  ptr = realloc(ptr, 1);
  exit(ptr == NULL);
}],
  ac_cv_c_realloc_bug="no",
  ac_cv_c_realloc_bug="yes",
  ac_cv_c_realloc_bug="yes") )
if test "$ac_cv_c_realloc_bug" = "yes"
  then
    :
    $1
  else
    :
    $2
fi
])

dnl **** Check for F77 or higher compiler
AC_DEFUN(ACX_PROG_FC,
[
AC_PROG_F77
if test "x$F77" != "x"; then
  FC=$F77
else
  forts="xlf fort77 pgf77 f90 xlf90 pgf90 f95"
  for fc in $forts
  do
    AC_CHECK_PROG(FC, $fc, "$fc")
    if test "x$FC" != "x"; then
      break
    fi
  done
fi
])

dnl **** Check whether Fortran adds underscore to external symbol names ****
AC_DEFUN(ACX_CHECK_FC_UNDERSCORE,
[
AC_REQUIRE([ACX_PROG_FC])
AC_CACHE_CHECK("whether Fortran adds underscore to external symbol names",
               ac_cv_c_fc_underscore,
cat > conftest_fc.f <<EOF
      CALL ACTEST ()
      STOP
      END
EOF

cat > conftest_c.c <<EOF
void actest_(void)
{
    return;
}
EOF

$CC $CFLAGS -c conftest_c.c > /dev/null 2>&1
$FC conftest_fc.f conftest_c.o -o conftest $LIBS > /dev/null 2>&1

case "$?" in
  0[)] ac_cv_c_fc_underscore="yes" ;;
  *[)] ac_cv_c_fc_underscore="no" ;;
esac
rm -f conftest*
)

if test "$ac_cv_c_fc_underscore" = "yes"
  then
    :
    $1
  else
    :
    $2
fi
])

dnl ICE_CHECK_DECL (FUNCTION, HEADER-FILE...)
dnl -----------------------------------------
dnl
dnl If FUNCTION is available, define `HAVE_FUNCTION'.  If it is declared
dnl in one of the headers named in the whitespace-separated list
dnl HEADER_FILE, define `HAVE_FUNCTION_DECL` (in all capitals).
dnl
AC_DEFUN(ICE_CHECK_DECL,
[
changequote(,)dnl
ice_tr=`echo $1 | tr '[a-z]' '[A-Z]'`
changequote([,])dnl
ice_have_tr=HAVE_$ice_tr
ice_have_decl_tr=${ice_have_tr}_DECL
ice_have_$1=no
AC_CHECK_FUNCS($1, ice_have_$1=yes)
if test "${ice_have_$1}" = yes; then
AC_MSG_CHECKING(for $1 declaration in $2)
AC_CACHE_VAL(ice_cv_have_$1_decl,
[
ice_cv_have_$1_decl=no
changequote(,)dnl
ice_re_params='[a-zA-Z_][a-zA-Z0-9_]*'
ice_re_word='(^|[^a-zA-Z_0-9_])'
changequote([,])dnl
for header in $2; do
# Check for ordinary declaration
AC_EGREP_HEADER([${ice_re_word}$1 *\(], $header,
	ice_cv_have_$1_decl=yes)
if test "$ice_cv_have_$1_decl" = yes; then
	break
fi
# Check for "fixed" declaration like "getpid _PARAMS((int))"
AC_EGREP_HEADER([${ice_re_word}$1 *$ice_re_params\(\(], $header,
	ice_cv_have_$1_decl=yes)
if test "$ice_cv_have_$1_decl" = yes; then
	break
fi
done
])
AC_MSG_RESULT($ice_cv_have_$1_decl)
if test "$ice_cv_have_$1_decl" = yes; then
AC_DEFINE_UNQUOTED(${ice_have_decl_tr})
fi
fi
])dnl

dnl --- *@-mdw_CHECK_MANYLIBS-@* ---
dnl
dnl Author:     Mark Wooding
dnl
dnl Synopsis:   mdw_CHECK_MANYLIBS(FUNC, LIBS, [IF-FOUND], [IF-NOT-FOUND])
dnl
dnl Arguments:  FUNC = a function to try to find
dnl             LIBS = a whitespace-separated list of libraries to search
dnl             IF-FOUND = what to do when the function is found
dnl             IF-NOT-FOUND = what to do when the function isn't found
dnl
dnl Use:        Searches for a library which defines FUNC.  It first tries
dnl             without any libraries; then it tries each library specified
dnl             in LIBS in turn.  If it finds a match, it adds the
dnl             appropriate library to `LIBS'.
dnl
dnl             This is particularly handy under DIREIX: if you link with
dnl             `-lnsl' then you get non-NIS-aware versions of getpwnam and
dnl             so on, which is clearly a Bad Thing.
dnl
dnl Modified:   by Evgeny Stambulchik to add (found) libraries to `LIBS'
dnl             *only* if `IF-FOUND' is absent. As well, if no additional
dnl             library is needed for `FUNC', `mdw_cv_lib_$1' sets to "".

AC_DEFUN(mdw_CHECK_MANYLIBS,
[AC_CACHE_CHECK([for library containing $1], [mdw_cv_lib_$1],
[mdw_save_LIBS="$LIBS"
mdw_cv_lib_$1="no"
AC_TRY_LINK(,[$1()], [mdw_cv_lib_$1="none required"])
test "$mdw_cv_lib_$1" = "no" && for i in $2; do
LIBS="-l$i $mdw_save_LIBS"
AC_TRY_LINK(,[$1()],
[mdw_cv_lib_$1="-l$i"
break])
done
LIBS="$mdw_save_LIBS"])
if test "$mdw_cv_lib_$1" != "no"; then
  if test "x$3" != "x"; then
    test "$mdw_cv_lib_$1" = "none required" && mdw_cv_lib_$1=""
    $3
  else
    test "$mdw_cv_lib_$1" = "none required" || LIBS="$mdw_cv_lib_$1 $LIBS"
  fi
else :
  $4
fi])
dnl ICE_FIND_MOTIF
dnl --------------
dnl
dnl Find Motif libraries and headers
dnl Put Motif include directory in motif_includes,
dnl put Motif library directory in motif_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl
AC_DEFUN(ICE_FIND_MOTIF,
[
AC_REQUIRE([AC_PATH_XTRA])
motif_includes=
motif_libraries=
AC_ARG_WITH(motif,
[  --without-motif              do not use Motif widgets])
dnl Treat --without-motif like
dnl --without-motif-includes --without-motif-libraries.
if test "$with_motif" = "no"
then
motif_includes=no
motif_libraries=no
fi
AC_ARG_WITH(motif-includes,
[  --with-motif-includes=DIR    Motif include files are in DIR],
motif_includes="$withval")
AC_ARG_WITH(motif-libraries,
[  --with-motif-libraries=DIR   Motif libraries are in DIR],
motif_libraries="$withval")
AC_MSG_CHECKING(for Motif)
#
#
# Search the include files.
#
if test "$motif_includes" = ""; then
AC_CACHE_VAL(ice_cv_motif_includes,
[
ice_motif_save_LIBS="$LIBS"
ice_motif_save_CFLAGS="$CFLAGS"
ice_motif_save_CPPFLAGS="$CPPFLAGS"
ice_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <Xm/Xm.h>],[int a;],
[
# Xm/Xm.h is in the standard search path.
ice_cv_motif_includes=
],
[
# Xm/Xm.h is not in the standard search path.
# Locate it and put its directory in `motif_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_motif_includes=no
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
           "${prefix}"/*/include /usr/*/include /usr/local/*/include \
           "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/Xm/Xm.h"; then
ice_cv_motif_includes="$dir"
break
fi
done
])
#
LIBS="$ice_motif_save_LIBS"
CFLAGS="$ice_motif_save_CFLAGS"
CPPFLAGS="$ice_motif_save_CPPFLAGS"
LDFLAGS="$ice_motif_save_LDFLAGS"
])
motif_includes="$ice_cv_motif_includes"
fi
#
#
# Now for the libraries.
#
if test "$motif_libraries" = ""; then
AC_CACHE_VAL(ice_cv_motif_libraries,
[
ice_motif_save_LIBS="$LIBS"
ice_motif_save_CFLAGS="$CFLAGS"
ice_motif_save_CPPFLAGS="$CPPFLAGS"
ice_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_LINK([#include <Xm/Xm.h>],[XtToolkitInitialize();],
[
# libXm.a is in the standard search path.
ice_cv_motif_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `motif_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_motif_libraries=no
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
           /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
           /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
           /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
           "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
           "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXm.* 2> /dev/null`" != ""; then
ice_cv_motif_libraries="$dir"
break
fi
done
])
#
LIBS="$ice_motif_save_LIBS"
CFLAGS="$ice_motif_save_CFLAGS"
CPPFLAGS="$ice_motif_save_CPPFLAGS"
LDFLAGS="$ice_motif_save_LDFLAGS"
])
#
motif_libraries="$ice_cv_motif_libraries"
fi
# Add Motif definitions to X flags
#
if test "$motif_includes" != "" && test "$motif_includes" != "$x_includes" && test "$motif_includes" != "no"
then
X_CFLAGS="-I$motif_includes $X_CFLAGS"
fi
if test "$motif_libraries" != "" && test "$motif_libraries" != "$x_libraries" && test "$motif_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$motif_libraries -R $motif_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$motif_libraries -R$motif_libraries $X_LIBS";;
  *)      X_LIBS="-L$motif_libraries $X_LIBS";;
esac
fi
#
#
motif_libraries_result="$motif_libraries"
motif_includes_result="$motif_includes"
test "$motif_libraries_result" = "" &&
  motif_libraries_result="in default path"
test "$motif_includes_result" = "" &&
  motif_includes_result="in default path"
test "$motif_libraries_result" = "no" &&
  motif_libraries_result="(none)"
test "$motif_includes_result" = "no" &&
  motif_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $motif_libraries_result, headers $motif_includes_result])
])dnl

dnl ICE_CHECK_LESSTIF
dnl -----------------
dnl
dnl Define `HAVE_LESSTIF' if the Motif library is actually a LessTif library
dnl
AC_DEFUN(ICE_CHECK_LESSTIF,
[
AC_MSG_CHECKING(whether the Motif library is actually a LessTif library)
AC_CACHE_VAL(ice_cv_have_lesstif,
AC_EGREP_CPP(yes,
[#include <Xm/Xm.h>
#ifdef LesstifVersion
yes
#endif
], ice_cv_have_lesstif=yes, ice_cv_have_lesstif=no))
AC_MSG_RESULT($ice_cv_have_lesstif)
if test "$ice_cv_have_lesstif" = yes; then
AC_DEFINE(HAVE_LESSTIF)
fi
])dnl


dnl ACX_CHECK_XMVERSIONSTRING
dnl --------------
AC_DEFUN(ACX_CHECK_XMVERSIONSTRING,
[
  AC_CACHE_CHECK( "whether _XmVersionString[] can be referred to",
    acx_cv__xmversionstring,
    AC_TRY_LINK([#include <stdio.h>],
                [extern char _XmVersionString[[]]; printf("%s\n", _XmVersionString);],
                [acx_cv__xmversionstring="yes"],
                [acx_cv__xmversionstring="no"]
    )
    if test "$acx_cv__xmversionstring" = "yes"
    then
      AC_DEFINE(HAVE__XMVERSIONSTRING)
      $1
    else
      :
      $2
    fi
  )
])dnl

dnl ACX_CHECK_T1LIB
dnl --------------
AC_DEFUN(ACX_CHECK_T1LIB,
[
  AC_CACHE_CHECK( "for T1lib \>= $1", acx_cv_t1lib,
    ACX_SAVE_STATE
    LIBS="-lt1 -lm $LIBS"
    AC_TRY_RUN([
#include <string.h>
#include <t1lib.h>
      int main(void) {
        char *vlib;
        vlib = T1_GetLibIdent();
        if (strcmp(vlib, "[$1]") < 0) {
          exit(1);
        }
        exit(0);
      }
      ],

      acx_cv_t1lib="yes",
      acx_cv_t1lib="no",
      acx_cv_t1lib="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_t1lib" = "yes"
  then
    T1_LIB="-lt1"
    $2
  else
    T1_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_ZLIB
dnl --------------
AC_DEFUN(ACX_CHECK_ZLIB,
[
  AC_ARG_WITH(zlib_library,
  [  --with-zlib-library=OBJ      use OBJ as ZLIB library [-lz]],
  zlib_library="$withval")
  if test "x$zlib_library" = "x"
  then
    zlib_library=-lzlib
  fi

  AC_CACHE_CHECK( "for zlib \>= $1", acx_cv_zlib,
    AC_CACHE_VAL(acx_cv_zlib_library, acx_cv_zlib_library=$zlib_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_zlib_library $LIBS"
    AC_TRY_RUN([
#include <string.h>
#include <zlib.h>
      int main(void) {
        char *vlib, *vinc;
        vlib = zlibVersion();
        vinc = ZLIB_VERSION;
        if (strcmp(vinc, "[$1]") < 0) {
          exit(1);
        }
        if (strcmp(vinc, vlib) != 0) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_zlib="yes",
      acx_cv_zlib="no",
      acx_cv_zlib="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_zlib" = "yes"
  then
    Z_LIB="$acx_cv_zlib_library"
    $2
  else
    Z_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_JPEG
dnl --------------
AC_DEFUN(ACX_CHECK_JPEG,
[
  AC_ARG_WITH(jpeg_library,
  [  --with-jpeg-library=OBJ      use OBJ as JPEG library [-ljpeg]],
  jpeg_library="$withval")
  if test "x$jpeg_library" = "x"
  then
    jpeg_library=-ljpeg
  fi
  
  AC_CACHE_CHECK( "for IJG JPEG software \>= $1", acx_cv_jpeg,
    AC_CACHE_VAL(acx_cv_jpeg_library, acx_cv_jpeg_library=$jpeg_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_jpeg_library $LIBS"
    AC_TRY_RUN([
#include <stdio.h>
#include <jpeglib.h>
      int main(void) {
        int vinc;
        vinc = JPEG_LIB_VERSION;
        if (vinc < [$1]) {
          exit(1);
        }
        exit(0);
      }
      ],

      acx_cv_jpeg="yes",
      acx_cv_jpeg="no",
      acx_cv_jpeg="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_jpeg" = "yes"
  then
    JPEG_LIB=$acx_cv_jpeg_library
    $2
  else
    JPEG_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_PNG
dnl --------------
AC_DEFUN(ACX_CHECK_PNG,
[
  AC_ARG_WITH(png_library,
  [  --with-png-library=OBJ       use OBJ as PNG library [-lpng]],
  png_library="$withval")
  if test "x$png_library" = "x"
  then
    png_library=-lpng
  fi

  AC_CACHE_CHECK( "for libpng \>= $1", acx_cv_png,
    AC_CACHE_VAL(acx_cv_png_library, acx_cv_png_library=$png_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_png_library $Z_LIB $LIBS"
    AC_TRY_RUN([
#include <string.h>
#include <png.h>
      int main(void) {
        char *vlib, *vinc;
        vlib = png_libpng_ver;
        vinc = PNG_LIBPNG_VER_STRING;
        if (strcmp(vinc, "[$1]") < 0) {
          exit(1);
        }
        if (strcmp(vinc, vlib) != 0) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_png="yes",
      acx_cv_png="no",
      acx_cv_png="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_png" = "yes"
  then
    PNG_LIB="$acx_cv_png_library"
    $2
  else
    PNG_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_TIFF
dnl --------------
AC_DEFUN(ACX_CHECK_TIFF,
[
  AC_ARG_WITH(tiff_library,
  [  --with-tiff-library=OBJ      use OBJ as TIFF library [-ltiff]],
  tiff_library="$withval")
  if test "x$tiff_library" = "x"
  then
    tiff_library=-ltiff
  fi

  AC_CACHE_CHECK( "for libtiff \>= $1", acx_cv_tiff,
    AC_CACHE_VAL(acx_cv_tiff_library, acx_cv_tiff_library=$tiff_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_tiff_library $JPEG_LIB $Z_LIB -lm $LIBS"
    AC_TRY_RUN([
#include <tiffio.h>
      int main(void) {
        int vinc;
        (void) TIFFGetVersion();
        vinc = TIFFLIB_VERSION;
        if (vinc < [$1]) {
          exit(1);
        }
        exit(0);
      }
      ],

      acx_cv_tiff="yes",
      acx_cv_tiff="no",
      acx_cv_tiff="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_tiff" = "yes"
  then
    TIFF_LIB="$acx_cv_tiff_library"
    $2
  else
    TIFF_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_PDFLIB
dnl --------------
AC_DEFUN(ACX_CHECK_PDFLIB,
[
  AC_ARG_WITH(pdf_library,
  [  --with-pdf-library=OBJ       use OBJ as PDFlib library [-lpdf]],
  pdf_library="$withval")
  if test "x$pdf_library" = "x"
  then
    pdf_library=-lpdf
  fi

  AC_CACHE_CHECK( "for PDFlib \>= $1", acx_cv_pdflib,
    AC_CACHE_VAL(acx_cv_pdf_library, acx_cv_pdf_library=$pdf_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_pdf_library $TIFF_LIB $JPEG_LIB $PNG_LIB $Z_LIB $LIBS"
    AC_TRY_RUN([
#include <pdflib.h>
      int main(void) {
        char *vinc;
        int vlibn, vincn;
        vlibn = 100*PDF_get_majorversion() + PDF_get_minorversion();
        vincn = 100*PDFLIB_MAJORVERSION + PDFLIB_MINORVERSION;
        vinc = PDFLIB_VERSIONSTRING;
        if (strcmp(vinc, "[$1]") < 0) {
          exit(1);
        }
        if (vincn != vlibn) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_pdflib="yes",
      acx_cv_pdflib="no",
      acx_cv_pdflib="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_pdflib" = "yes"
  then
    PDF_LIB="$acx_cv_pdf_library"
    $2
  else
    PDF_LIB=
    $3
  fi
])dnl
