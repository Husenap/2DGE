message("-- External Project: imgui")
include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG 4f5aac319e3561284833db90f35d218de8b282c1
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    set(src_imgui
        "${imgui_SOURCE_DIR}/imconfig.h"
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui.h"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_internal.h"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/imstb_rectpack.h"
        "${imgui_SOURCE_DIR}/imstb_textedit.h"
        "${imgui_SOURCE_DIR}/imstb_truetype.h"
        "${imgui_SOURCE_DIR}/examples/imgui_impl_dx11.h"
        "${imgui_SOURCE_DIR}/examples/imgui_impl_dx11.cpp"
        "${imgui_SOURCE_DIR}/examples/imgui_impl_win32.h"
        "${imgui_SOURCE_DIR}/examples/imgui_impl_win32.cpp"
        )
        
    add_library(imgui STATIC ${src_imgui})
    
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
        
    set_target_properties(imgui PROPERTIES FOLDER "thirdparty")

endif()