#idf_component_register(SRCS "main.c"
#                    INCLUDE_DIRS ".")

set(COMPONENT_SRCS "main.c" 
        "app_httpd_mic"
        "Wav.cc"
        "i2s_acc.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_FILES
        "www/neko.wav")

register_component()
                    