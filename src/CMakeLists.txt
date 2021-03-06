SET (hawkey_SRCS
    errno.c
    goal.c
    iutil.c
    package.c
    packagelist.c
    packageset.c
    query.c
    reldep.c
    repo.c
    sack.c
    selector.c
    stringarray.c
    util.c)

ADD_LIBRARY(libhawkey SHARED ${hawkey_SRCS})
TARGET_LINK_LIBRARIES(libhawkey ${SOLV_LIBRARY} ${SOLVEXT_LIBRARY})
TARGET_LINK_LIBRARIES(libhawkey ${EXPAT_LIBRARY} ${ZLIB_LIBRARY} ${RPMDB_LIBRARY})
SET_TARGET_PROPERTIES(libhawkey PROPERTIES OUTPUT_NAME "hawkey")
SET_TARGET_PROPERTIES(libhawkey PROPERTIES SOVERSION 0)

ADD_EXECUTABLE(hth hth.c)
TARGET_LINK_LIBRARIES(hth libhawkey
			   ${SOLV_LIBRARY}
			   ${SOLVEXT_LIBRARY}
			   ${EXPAT_LIBRARY}
			   ${ZLIB_LIBRARY}
			   ${RPMDB_LIBRARY})

ADD_SUBDIRECTORY(python)

SET(HAWKEY_headers
    errno.h
    goal.h
    package.h
    packagelist.h
    packageset.h
    query.h
    reldep.h
    repo.h
    sack.h
    selector.h
    types.h
    util.h
    stringarray.h
    version.h)

IF (CMAKE_SIZEOF_VOID_P MATCHES "8")
  SET (LIB_SUFFIX "64")
ENDIF (CMAKE_SIZEOF_VOID_P MATCHES "8")
SET (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")

INSTALL(FILES ${HAWKEY_headers} DESTINATION include/hawkey)
INSTALL(TARGETS libhawkey LIBRARY DESTINATION ${LIB_INSTALL_DIR})

CONFIGURE_FILE("hawkey.pc.cmake" "${CMAKE_BINARY_DIR}/hawkey.pc" @ONLY)
CONFIGURE_FILE("version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/version.h" @ONLY)

INSTALL(FILES "${CMAKE_BINARY_DIR}/hawkey.pc"
	DESTINATION "${LIB_INSTALL_DIR}/pkgconfig")
