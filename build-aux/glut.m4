dnl I'd like this to be edited in -*- Autoconf -*- mode...
dnl
AC_DEFUN([GST_HAVE_GLUT], [

AC_REQUIRE([AC_PATH_XTRA])
AC_CHECK_HEADERS([GLUT/freeglut.h GL/freeglut.h GLUT/glut.h GL/glut.h])

AC_CACHE_CHECK([how to link with GLUT], gst_cv_glut_libs, [
  if test "$gst_cv_opengl_libs" = 'not found'; then
    gst_cv_glut_libs='not found'
  elif test $ac_cv_header_GLUT_glut_h = no && \
     test $ac_cv_header_GL_glut_h = no &&
     test $ac_cv_header_GL_freeglut_h = no &&
     test $ac_cv_header_GLUT_freeglut_h = no; then
    gst_cv_glut_libs='not found'
  else
    save_LIBS=$LIBS
    case $host in
      *-*-darwin*) gst_cv_glut_libs='-Wl,-framework,GLUT' ;;
      *) gst_cv_glut_libs="-lglut $X_LIBS $X_PRE_LIBS -lX11" ;;
    esac

    LIBS="$LIBS -lglut $gst_cv_opengl_libs"
    AC_LINK_IFELSE([AC_LANG_CALL([], [glutInit])],
                   [], [gst_cv_glut_libs='not found'])
    LIBS=$save_LIBS
  fi
])

if test "$gst_cv_glut_libs" != "not found"; then
  LIBGLUT="$gst_cv_glut_libs"
  AC_DEFINE(HAVE_GLUT, 1, [Define if your system has GLUT installed.])

  if test $ac_cv_header_GLUT_freeglut_h = yes; then
    gst_cv_glut_header_file='GLUT/freeglut.h'
  elif test $ac_cv_header_GL_freeglut_h = yes; then
    gst_cv_glut_header_file='GL/freeglut.h'
  elif test $ac_cv_header_GLUT_glut_h = yes; then
    gst_cv_glut_header_file='GLUT/glut.h'
  else
    gst_cv_glut_header_file='GL/glut.h'
  fi
  AC_DEFINE_UNQUOTED(GL_GLUT_H, [<$gst_cv_glut_header_file>], [Define to the #include directive for GLUT.])
fi
AC_SUBST(LIBGLUT)
])dnl
