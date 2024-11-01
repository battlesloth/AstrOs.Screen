SET(SOURCES screens/ui_loadingscreen.c
    screens/ui_mainscreen.c
    screens/ui_settingsscreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_button_png.c
    images/ui_img_gear_png.c
    fonts/ui_font_StarJedi_28.c
    fonts/ui_font_StarJedi_72.c)

add_library(ui ${SOURCES})
