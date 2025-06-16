set(zhdrs
    zbuffer.h
    zfeatures.h
    tinygl_api.h
    ktx_loader.h
    lvgl_bridge.h
)

install(FILES ${zhdrs} DESTINATION ${INCLUDE_DIR})
install(FILES GL/gl.h GL/tinygl_ext.h DESTINATION ${INCLUDE_DIR}/TGL)
