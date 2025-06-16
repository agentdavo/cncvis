set(zhdrs
    zbuffer.h
    zfeatures.h
    tinygl_api.h
    ktx_loader.h
    lvgl_bridge.h
)

install(FILES ${zhdrs} DESTINATION ${INSTALL_INCLUDE_DIR})
install(FILES GL/gl.h GL/tinygl_ext.h DESTINATION ${INSTALL_INCLUDE_DIR}/TGL)
