
set(plug_sources
    include/plugcontroller.h
    source/plugcontroller.cpp
    #---
    include/plugids.h
    #---
    include/plugprocessor.h
    source/plugprocessor.cpp
    #---
    include/version.h
    #---
    source/plugfactory.cpp
    #---
    #---https://github.com/micknoise/Maximilian
    include/maximilian.h
)

#--- HERE change the target Name for your plug-in (for ex. set(target myDelay))-------
set(target Gilberts)

smtg_add_vst3plugin(${target} ${SDK_ROOT} ${plug_sources})
set_target_properties(${target} PROPERTIES ${SDK_IDE_MYPLUGINS_FOLDER})
target_link_libraries(${target} PRIVATE base sdk)

if(MAC)
    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS)
elseif(WIN)
    target_sources(${target} PRIVATE resource/plug.rc)
endif()
