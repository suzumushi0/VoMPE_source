cmake_minimum_required(VERSION 3.14.0)

# suzumushi
option(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES "Enable VST 3 Plug-in Examples" OFF)
option(SMTG_ENABLE_VST3_HOSTING_EXAMPLES "Enable VST 3 Hosting Examples" OFF)
option(SMTG_CREATE_PLUGIN_LINK "" OFF)
set(SMTG_CXX_STANDARD 20 CACHE STRING "")

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "")

set(vst3sdk_SOURCE_DIR "C:/D/Repos/VST_SDK/vst3sdk")
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(VoMPE
    # This is your plug-in version number. Change it here only.
    # Version number symbols usable in C++ can be found in
    # source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
    VERSION 1.1.0.0 
    DESCRIPTION "VoMPE"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(VoMPE
    source/SODDL.h
    source/SOextparam.h
    source/SOextparam.cpp
    source/version.h
    source/VM1_3downsampler.h
    source/VMcids.h
    source/VMconfig.h
    source/VMcontroller.h
    source/VMcontroller.cpp 
    source/VMentry.cpp 
    source/VMextparam.h
    source/VMFFT.h
    source/VMLPC.h
    source/VMMIDI.h
    source/VMMIDI.cpp
    source/VMparam.h
    source/VMprocessor.h
    source/VMprocessor.cpp
    source/VMView.h
    source/VMViewcreator.cpp
    source/VMvst3editor.h
)

#- VSTGUI Wanted ----
if(SMTG_ENABLE_VSTGUI_SUPPORT)
    target_sources(VoMPE
        PRIVATE
            resource/VoMPE.uidesc
    )
    target_link_libraries(VoMPE
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(VoMPE
        RESOURCES
            "resource/VoMPE.uidesc"
    )
endif(SMTG_ENABLE_VSTGUI_SUPPORT)
# -------------------

smtg_target_add_plugin_snapshots (VoMPE
    RESOURCES
        resource/FFAE99B6C1EF50FD8A922F1B437EC7AE_snapshot.png
        resource/FFAE99B6C1EF50FD8A922F1B437EC7AE_snapshot_2.0x.png
)

target_link_libraries(VoMPE
    PRIVATE
        sdk
)

smtg_target_configure_version_file(VoMPE)

if(SMTG_MAC)
    smtg_target_set_bundle(VoMPE
        BUNDLE_IDENTIFIER foobar
        COMPANY_NAME "suzumushi"
    )
    smtg_target_set_debug_executable(VoMPE
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(VoMPE PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VoMPE)

        smtg_target_set_debug_executable(VoMPE
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
