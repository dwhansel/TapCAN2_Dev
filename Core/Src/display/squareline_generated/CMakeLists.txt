SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_tapcan_280px_png.c)

add_library(ui ${SOURCES})
