<<<<<<< HEAD
#include_directories(../fex)
add_definitions(-DHAVE_STDINT_H)
include_directories(~/src/emsdk/emscripten/1.38.11/system/include/emscripten)

# was only used while trying to find things that still needed linking in the
# port back to CMake

#SET ( CMAKE_CXX_FLAGS "-Wl,-z,defs" CACHE STRING "compile flags" FORCE)
=======

if(NOT GME_BUILD_SHARED AND NOT GME_BUILD_STATIC)
    message(FATAL_ERROR "Both shared and static builds disabled. Please enable build of shared and/or static build of the GME.")
endif()

if(GME_ZLIB)
    find_package(ZLIB QUIET)
endif()

>>>>>>> db7344ebf (abc)
# List of source files required by libgme and any emulators
# This is not 100% accurate (Fir_Resampler for instance) but
# you'll be OK.
set(libgme_SRCS Blip_Buffer.cpp
<<<<<<< HEAD
                Classic_Emu.cpp
                Data_Reader.cpp
                Dual_Resampler.cpp
                Effects_Buffer.cpp
                Fir_Resampler.cpp
                Resampler.cpp
                Rom_Data.cpp
                gme.cpp
                Gme_Loader.cpp
                Gme_File.cpp
                M3u_Playlist.cpp
                Multi_Buffer.cpp
                Music_Emu.cpp
                blargg_errors.cpp
                blargg_common.cpp
                Track_Filter.cpp
                )

# static builds need to find static zlib (and static forms of other needed
# libraries.  Ensure CMake looks only for static libs if we're doing a static
# build.  See https://stackoverflow.com/a/44738756
if(NOT BUILD_SHARED_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif()

find_package(ZLIB QUIET)

# Ay_Apu is very popular around here
if (USE_GME_AY OR USE_GME_KSS)
    set(libgme_SRCS ${libgme_SRCS}
                Ay_Apu.cpp
                Ay_Core.cpp
=======
                Blip_Buffer.h
                Classic_Emu.cpp
                Classic_Emu.h
                Data_Reader.cpp
                Data_Reader.h
                Dual_Resampler.cpp
                Dual_Resampler.h
                Effects_Buffer.cpp
                Effects_Buffer.h
                Fir_Resampler.cpp
                Fir_Resampler.h
                gme.cpp
                gme.h
                gme_types.h
                Gme_File.cpp
                Gme_File.h
                M3u_Playlist.cpp
                M3u_Playlist.h
                Multi_Buffer.cpp
                Multi_Buffer.h
                Music_Emu.cpp
                Music_Emu.h
                blargg_common.h
                blargg_config.h
                blargg_endian.h
                blargg_source.h
                )

# Ay_Apu is very popular around here
if(USE_GME_AY OR USE_GME_KSS)
    list(APPEND libgme_SRCS
                Ay_Apu.cpp
                Ay_Apu.h
>>>>>>> db7344ebf (abc)
        )
endif()

# so is Ym2612_Emu
<<<<<<< HEAD
if (USE_GME_VGM OR USE_GME_GYM)
    set(libgme_SRCS ${libgme_SRCS}
#                Ym2612_Emu.cpp
                fm2612.c
                fm.c
                fmopl.cpp
                ymdeltat.cpp
                Ym2612_Emu_MAME.cpp
                Ymf262_Emu.cpp
                dbopl.cpp
                # not sure this is the optimal location for this:
                # it might belong in some special "OPL" definition or something
                Ym3812_Emu.cpp
                Ym2610b_Emu.cpp
                Ym2608_Emu.cpp
                # these two probably should really have a special case, since
                # I know this is used in the NES VRC7 chip as well as the
                # Japanese SMS/Mark III FM module
                Ym2413_Emu.cpp
                ym2413.c
                Sms_Fm_Apu.cpp
                Ym2151_Emu.cpp
                ym2151.c
                Ym2203_Emu.cpp
                Ymz280b_Emu.cpp
                ymz280b.c
# done in MAME
#                Ym2612_Emu.cpp
                Z80_Cpu.cpp
                # Extremely unsure this goes here
                SegaPcm_Emu.cpp
                segapcm.c
        )
endif()

# Less accurate than the other implementation, only one should be defined at a time.
if (USE_GENS)
  set(libgme_SRCS ${libgme_SRCS}
                Ym2612_Emu_Gens.cpp
    )
endif()

# But none are as popular as Sms_Apu
if (USE_GME_VGM OR USE_GME_GYM OR USE_GME_KSS)
    set(libgme_SRCS ${libgme_SRCS}
                Sms_Apu.cpp
        )
endif()

if (USE_GME_AY)
    set(libgme_SRCS ${libgme_SRCS}
              # Ay_Apu.cpp included earlier
                Ay_Cpu.cpp
                Ay_Emu.cpp
        )
endif()

if (USE_GME_GBS)
    set(libgme_SRCS ${libgme_SRCS}
                Gb_Apu.cpp
                Gb_Cpu.cpp
                Gbs_Cpu.cpp
                Gb_Oscs.cpp
                Gbs_Emu.cpp
                Gbs_Core.cpp
        )
endif()

if (USE_GME_GYM)
    set(libgme_SRCS ${libgme_SRCS}
              # Sms_Apu.cpp included earlier
              # Ym2612_Emu.cpp included earlier
                Gym_Emu.cpp
        )
endif()

if (USE_GME_HES)
    set(libgme_SRCS ${libgme_SRCS}
                Hes_Apu.cpp
                Hes_Cpu.cpp
                Hes_Emu.cpp
                Hes_Core.cpp
        )
endif()

if (USE_GME_KSS)
    set(libgme_SRCS ${libgme_SRCS}
              # Ay_Apu.cpp included earlier
              # Sms_Apu.cpp included earlier
                Kss_Cpu.cpp
                Kss_Emu.cpp
                Kss_Scc_Apu.cpp
                Kss_Core.cpp
        )
endif()

if (USE_GME_NSF OR USE_GME_NSFE)
    set(libgme_SRCS ${libgme_SRCS}
                Nes_Apu.cpp
                Nes_Cpu.cpp
                Nes_Fme7_Apu.cpp
                Nes_Namco_Apu.cpp
                Nes_Oscs.cpp
                Nes_Vrc6_Apu.cpp
                Nsf_Emu.cpp
                Nsf_Core.cpp
                Nsf_Impl.cpp
                Nsf_Cpu.cpp
                Nes_Fds_Apu.cpp
                Nes_Vrc7_Apu.cpp
        )
endif()

if (USE_GME_NSFE)
    set(libgme_SRCS ${libgme_SRCS}
                Nsfe_Emu.cpp
        )
endif()

if (USE_GME_SAP)
    set(libgme_SRCS ${libgme_SRCS}
                Sap_Apu.cpp
                Sap_Cpu.cpp
                Sap_Emu.cpp
        )
endif()

if (USE_GME_SPC)
    set(libgme_SRCS ${libgme_SRCS}
                Spc_Emu.h
                Spc_Emu.cpp
                Spc_Filter.cpp
                Spc_Sfm.cpp
                higan/smp/smp.cpp
                higan/dsp/dsp.cpp
                higan/dsp/SPC_DSP.cpp
                higan/smp/timing.cpp
                higan/smp/memory.cpp
                higan/processor/spc700/spc700.cpp
#                higan/processor/spc700/algorithms.cpp
#                higan/processor/spc700/disassembler.cpp
#                higan/processor/spc700/instructions.cpp
                Bml_Parser.cpp
        )
endif()

if (USE_GME_VGM)
    set(libgme_SRCS ${libgme_SRCS}
              # Sms_Apu.cpp included earlier
              # Ym2612_Emu.cpp included earlier
                Vgm_Emu.cpp
                Vgm_Core.cpp
                dac_control.c
#                Vgm_Emu.cpp
#                Vgm_Emu_Impl.cpp
#                Ym2413_Emu.cpp
        )
endif()

# ADDED ON LATER:
# SGC - Hope I'm doing this right
if (USE_GME_SGC)
    set(libgme_SRCS ${libgme_SRCS}
                Sgc_Emu.cpp
                Sgc_Cpu.cpp
                Sgc_Impl.cpp
                Sgc_Core.cpp
        )
endif()

# MISC: SHOULD REALLY GO IN THEIR OWN SECTION, BUT I AM TIRED OF FIXING THIS CRAP:
set(libgme_SRCS ${libgme_SRCS}
  #C140 Namco System 2
  C140_Emu.cpp
  c140.c
  #Other
  # sega CD I think?
  Rf5C68_Emu.cpp
  rf5c68.c
  scd_pcm.c
  Rf5C164_Emu.cpp
  
  Pwm_Emu.cpp
  pwm.c
  
  Okim6258_Emu.cpp
  okim6258.c
  Okim6295_Emu.cpp
  okim6295.c
  
  K051649_Emu.cpp
  k051649.c
  
  K053260_Emu.cpp
  k053260.c
  
  K054539_Emu.cpp
  k054539.c
  
  Qsound_Apu.cpp
  qmix.c
  
  # And the ones that still escaped my attention
  # (discovered via some shell script magic):
  Downsampler.cpp
  Hes_Apu_Adpcm.cpp
  Opl_Apu.cpp
  Sap_Core.cpp
  Upsampler.cpp
  )

# These headers are part of the generic gme interface.
set (EXPORTED_HEADERS gme.h)

# On some platforms we may need to change headers or whatnot based on whether
# we're building the library or merely using the library. The following is
# only defined when building the library to allow us to tell which is which.
add_definitions(-DBLARGG_BUILD_DLL)

# For the gme_types.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Add library to be compiled.
add_library(gme ${libgme_SRCS})

if(ZLIB_FOUND)
    message(" ** ZLib library located, compressed file formats will be supported")
    target_compile_definitions(gme PRIVATE -DHAVE_ZLIB_H)
    target_include_directories(gme PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(gme ${ZLIB_LIBRARIES})
    # Is not to be installed though

    set(PKG_CONFIG_ZLIB -lz) # evaluated in libgme.pc.in
else()
    message("ZLib library not found, disabling support for compressed formats such as VGZ")
endif()

# The version is the release.  The "soversion" is the API version.  As long
# as only build fixes are performed (i.e. no backwards-incompatible changes
# to the API), the SOVERSION should be the same even when bumping up VERSION.
# The way gme.h is designed, SOVERSION should very rarely be bumped, if ever.
# Hopefully the API can stay compatible with old versions.
set_target_properties(gme
    PROPERTIES VERSION ${GME_VERSION}
               SOVERSION 0)

install(TARGETS gme LIBRARY DESTINATION lib${LIB_SUFFIX}
                    RUNTIME DESTINATION bin  # DLL platforms
                    ARCHIVE DESTINATION lib) # DLL platforms

# Run during cmake phase, so this is available during make
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gme_types.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/gme_types.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libgme.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libgme.pc @ONLY)

install(FILES ${EXPORTED_HEADERS} DESTINATION include/gme)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgme.pc DESTINATION lib${LIB_SUFFIX}/pkgconfig)

=======
if(USE_GME_VGM OR USE_GME_GYM)
    if(GME_YM2612_EMU STREQUAL "Nuked")
        add_definitions(-DVGM_YM2612_NUKED)
        list(APPEND libgme_SRCS
                    Ym2612_Nuked.cpp
                    Ym2612_Nuked.h
            )
        message(STATUS "VGM/GYM: Nuked OPN2 emulator will be used")
    elseif(GME_YM2612_EMU STREQUAL "MAME")
        add_definitions(-DVGM_YM2612_MAME)
        list(APPEND libgme_SRCS
                    Ym2612_MAME.cpp
                    Ym2612_MAME.h
            )
        message(STATUS "VGM/GYM: MAME YM2612 emulator will be used")
    else()
        add_definitions(-DVGM_YM2612_GENS)
        list(APPEND libgme_SRCS
                    Ym2612_GENS.cpp
                    Ym2612_GENS.h
            )
        message(STATUS "VGM/GYM: GENS 2.10 emulator will be used")
    endif()
endif()

# But none are as popular as Sms_Apu
if(USE_GME_VGM OR USE_GME_GYM OR USE_GME_KSS)
    list(APPEND libgme_SRCS
                Sms_Apu.cpp
                Sms_Apu.h
        )
endif()

if(USE_GME_AY)
    list(APPEND libgme_SRCS
              # Ay_Apu.cpp included earlier
                Ay_Cpu.cpp
                Ay_Cpu.h
                Ay_Emu.cpp
                Ay_Emu.h
        )
endif()

if(USE_GME_GBS)
    list(APPEND libgme_SRCS
                Gb_Apu.cpp
                Gb_Apu.h
                Gb_Cpu.cpp
                Gb_Cpu.h
                Gb_Oscs.cpp
                Gb_Oscs.h
                Gbs_Emu.cpp
                Gbs_Emu.h
                gb_cpu_io.h
        )
endif()

if(USE_GME_GYM)
    list(APPEND libgme_SRCS
              # Sms_Apu.cpp included earlier
              # Ym2612_Emu.cpp included earlier
                Gym_Emu.cpp
                Gym_Emu.h
        )
endif()

if(USE_GME_HES)
    list(APPEND libgme_SRCS
                Hes_Apu.cpp
                Hes_Apu.h
                Hes_Cpu.cpp
                Hes_Cpu.h
                Hes_Emu.cpp
                Hes_Emu.h
                hes_cpu_io.h
        )
endif()

if(USE_GME_KSS)
    list(APPEND libgme_SRCS
              # Ay_Apu.cpp included earlier
              # Sms_Apu.cpp included earlier
                Kss_Cpu.cpp
                Kss_Cpu.h
                Kss_Emu.cpp
                Kss_Emu.h
                Kss_Scc_Apu.cpp
                Kss_Scc_Apu.h
        )
endif()

if(USE_GME_NSF OR USE_GME_NSFE)
    list(APPEND libgme_SRCS
                Nes_Apu.cpp
                Nes_Apu.h
                Nes_Cpu.cpp
                Nes_Cpu.h
                Nes_Fme7_Apu.cpp
                Nes_Fme7_Apu.h
                Nes_Namco_Apu.cpp
                Nes_Namco_Apu.h
                Nes_Oscs.cpp
                Nes_Oscs.h
                Nes_Vrc6_Apu.cpp
                Nes_Vrc6_Apu.h
                Nes_Fds_Apu.cpp
                Nes_Fds_Apu.h
                Nes_Vrc7_Apu.cpp
                Nes_Vrc7_Apu.h
                ext/emu2413.c
                ext/emu2413.h
                ext/panning.c
                ext/panning.h
                ext/emutypes.h
                ext/2413tone.h
                ext/vrc7tone.h
                Nsf_Emu.cpp
                Nsf_Emu.h
        )
endif()

if(USE_GME_NSFE)
    list(APPEND libgme_SRCS
                Nsfe_Emu.cpp
                Nsfe_Emu.h
        )
endif()

if(USE_GME_SAP)
    list(APPEND libgme_SRCS
                Sap_Apu.cpp
                Sap_Cpu.cpp
                Sap_Emu.cpp
                sap_cpu_io.h
        )
endif()

if(USE_GME_SPC)
    list(APPEND libgme_SRCS
                Snes_Spc.cpp
                Snes_Spc.h
                Spc_Cpu.cpp
                Spc_Cpu.h
                Spc_Dsp.cpp
                Spc_Dsp.h
                Spc_Emu.cpp
                Spc_Emu.h
                Spc_Filter.cpp
                Spc_Filter.h
        )
    if(GME_SPC_ISOLATED_ECHO_BUFFER)
        add_definitions(-DSPC_ISOLATED_ECHO_BUFFER)
    endif()
endif()

if(USE_GME_VGM)
    list(APPEND libgme_SRCS
              # Sms_Apu.cpp included earlier
              # Ym2612_Emu.cpp included earlier
                Vgm_Emu.cpp
                Vgm_Emu.h
                Vgm_Emu_Impl.cpp
                Vgm_Emu_Impl.h
                Ym2413_Emu.cpp
                Ym2413_Emu.h
        )
endif()

# These headers are part of the generic gme interface.
set (EXPORTED_HEADERS gme.h)

# while building a macOS framework, exported headers must be in the source
# list, or the header files aren't copied to the bundle.
if(GME_BUILD_FRAMEWORK)
    set(libgme_SRCS ${libgme_SRCS} ${EXPORTED_HEADERS})
endif()

# Extract symbols from gme.exports
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/gme.exports" gme_exports_strings)
set(gme_exports)
foreach(gme_export_string ${gme_exports_strings})
    string(STRIP "${gme_export_string}" gme_export_string)
    string(SUBSTRING "${gme_export_string}" 0 1 gme_export_first_char)
    if(gme_export_string AND NOT gme_export_first_char STREQUAL "#")
        list(APPEND gme_exports ${gme_export_string})
    endif()
endforeach()


add_library(gme_deps INTERFACE)

foreach(LIB ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES})
    if(IS_ABSOLUTE ${LIB} AND EXISTS ${LIB})
        list(APPEND IMPLICITS_LIST "${LIB}")
    elseif(LIB MATCHES "-l:lib.*.a")
        string(LENGTH ${LIB} LIBLEN)
        math(EXPR LIBLEN "${LIBLEN}-8")
        string(SUBSTRING ${LIB} 6 ${LIBLEN} DIRECT_LIB)
        list(APPEND IMPLICITS_LIST "-l${DIRECT_LIB}")
    elseif(LIB MATCHES "-l.*")
        list(APPEND IMPLICITS_LIST "${LIB}")
    else()
        list(APPEND IMPLICITS_LIST "-l${LIB}")
    endif()
endforeach()
if(IMPLICITS_LIST)
    # blacklist of libraries that should not be in Libs.private
    list(REMOVE_ITEM IMPLICITS_LIST "-lmingwex"
         "-lmingw32" "-lmoldname" "-lmsvcrt" "-ladvapi32" "-lshell32"
         "-luser32" "-lkernel32")
    string(REPLACE ";" " " PC_LIBS "${IMPLICITS_LIST}")
else()
    set(PC_LIBS "")
endif(IMPLICITS_LIST)

if(GME_ZLIB)
    if(ZLIB_FOUND)
        message(STATUS "ZLib library located, compressed file formats will be supported")
        target_compile_definitions(gme_deps INTERFACE HAVE_ZLIB_H)
        target_link_libraries(gme_deps INTERFACE ZLIB::ZLIB)
        # Is not to be installed though
        list(APPEND PC_LIBS -lz) # for libgme.pc
    else()
        message(STATUS "** ZLib library not found, disabling support for compressed formats such as VGZ")
    endif()
else()
    message(STATUS "Zlib-Compressed formats excluded")
endif()

if(NOT MSVC)
    # Link with -no-undefined, if available
    if(NOT APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES ".*OpenBSD.*")
        cmake_push_check_state()
        set(CMAKE_REQUIRED_FLAGS "-Wl,-no-undefined")
            check_cxx_source_compiles("int main(void) { return 0;}" LINKER_SUPPORTS_NO_UNDEFINED)
        cmake_pop_check_state()
    endif()

    # Link to libm, if necessary
    check_cxx_source_compiles("
        #include <math.h>
        int main(int argc, char** argv) {
            return (int) pow(argc, 2.5);
        }" LIBM_NOT_NEEDED)
    if(NOT LIBM_NOT_NEEDED)
        cmake_push_check_state()
        set(CMAKE_REQUIRED_LIBRARIES "-lm")
        check_cxx_source_compiles("
            #include <math.h>
            int main(int argc, char** argv) {
                return (int) pow(argc, 2.5);
            }" HAVE_LIBM)
        cmake_pop_check_state()
        if(HAVE_LIBM)
            list(APPEND PC_LIBS -lm) # for libgme.pc
        endif()
    endif()
endif()

# Add a version script to hide the c++ STL
if(APPLE)
    set(gme_syms "")
    foreach(gme_export ${gme_exports})
        set(gme_syms "${gme_syms}_${gme_export}\n")
    endforeach()

    # Use an intermediate 'gme_generated.sym' file to avoid a relink every time CMake runs
    set(temporary_sym_file "${CMAKE_CURRENT_BINARY_DIR}/gme_generated.sym")
    set(generated_sym_file "${CMAKE_CURRENT_BINARY_DIR}/gme.sym")
    file(WRITE "${temporary_sym_file}" "${gme_syms}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${temporary_sym_file}" "${generated_sym_file}")
elseif(UNIX)
    set(gme_syms "{\n  global:\n")
    foreach(gme_export ${gme_exports})
        set(gme_syms "${gme_syms}    ${gme_export};\n")
    endforeach()
    set(gme_syms "${gme_syms}  local: *;\n};\n")

    # Use an intermediate 'gme_generated.sym' file to avoid a relink every time CMake runs
    set(temporary_sym_file "${CMAKE_CURRENT_BINARY_DIR}/gme_generated.sym")
    set(generated_sym_file "${CMAKE_CURRENT_BINARY_DIR}/gme.sym")
    file(WRITE "${temporary_sym_file}" "${gme_syms}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${temporary_sym_file}" "${generated_sym_file}")

    cmake_push_check_state()
    set(CMAKE_REQUIRED_FLAGS "-Wl,-version-script='${generated_sym_file}'")
    check_cxx_source_compiles("int main() { return 0;}" LINKER_SUPPORTS_VERSION_SCRIPT)
    cmake_pop_check_state()
endif()


set(INSTALL_TARGETS)

# Shared properties for shared and static builds of libGME
macro(set_gme_props gme_target)
    list(APPEND INSTALL_TARGETS ${gme_target})

    set_property(TARGET ${gme_target} PROPERTY C_VISIBILITY_PRESET "hidden")
    set_property(TARGET ${gme_target} PROPERTY VISIBILITY_INLINES_HIDDEN TRUE)
    set_property(TARGET ${gme_target} PROPERTY CXX_VISIBILITY_PRESET "hidden")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_definitions(${gme_target} PRIVATE LIBGME_VISIBILITY)
    endif()

    target_compile_definitions(${gme_target} PRIVATE GEN_TYPES_H)
    if(WORDS_BIGENDIAN)
        target_compile_definitions(${gme_target} PRIVATE BLARGG_BIG_ENDIAN=1)
    else()
        target_compile_definitions(${gme_target} PRIVATE BLARGG_LITTLE_ENDIAN=1)
    endif()

    # Try to protect against undefined behavior from signed integer overflow
    # This has caused miscompilation of code already and there are other
    # potential uses; see https://bitbucket.org/mpyne/game-music-emu/issues/18/
    #                    (https://github.com/libgme/game-music-emu/issues/20 )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${gme_target} PRIVATE -fwrapv)
    endif()

    target_include_directories(${gme_target}
        PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"  # For gen_types.h
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>"
        INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    if(NOT MSVC)
        # Link with -no-undefined, if available
        if(APPLE)
            set_property(TARGET ${gme_target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-undefined,error")
        elseif(NOT CMAKE_SYSTEM_NAME MATCHES ".*OpenBSD.*" AND LINKER_SUPPORTS_NO_UNDEFINED)
            set_property(TARGET ${gme_target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-no-undefined")
        endif()
    endif()

endmacro()

# Building the Shared version of GME
if(GME_BUILD_SHARED)
    add_library(gme_shared SHARED ${libgme_SRCS})
    set_target_properties(gme_shared PROPERTIES OUTPUT_NAME gme)

    if(WIN32)
        set_property(TARGET gme_shared PROPERTY DEFINE_SYMBOL BLARGG_BUILD_DLL)
    endif()

    set_gme_props(gme_shared)

    # Add a version script to hide the c++ STL
    if(APPLE)
        set_property(TARGET gme_shared APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-exported_symbols_list,'${generated_sym_file}'")
        set_property(TARGET gme_shared APPEND PROPERTY LINK_DEPENDS "${generated_sym_file}")
    elseif(UNIX AND LINKER_SUPPORTS_VERSION_SCRIPT)
        set_property(TARGET gme_shared APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-version-script='${generated_sym_file}'")
        set_property(TARGET gme_shared APPEND PROPERTY LINK_DEPENDS "${generated_sym_file}")
    endif()

    target_link_libraries(gme_shared PRIVATE gme_deps)

    if(HAVE_LIBM)
        target_link_libraries(gme_shared PRIVATE -lm)
    endif()

    # The version is the release.  The "soversion" is the API version.  As long
    # as only build fixes are performed (i.e. no backwards-incompatible changes
    # to the API), the SOVERSION should be the same even when bumping up VERSION.
    # The way gme.h is designed, SOVERSION should very rarely be bumped, if ever.
    # Hopefully the API can stay compatible with old versions.
    set_target_properties(gme_shared PROPERTIES VERSION ${GME_VERSION} SOVERSION 0)

    # macOS framework build
    if(GME_BUILD_FRAMEWORK)
        set_target_properties(gme_shared
            PROPERTIES FRAMEWORK TRUE
                       FRAMEWORK_VERSION A
                       MACOSX_FRAMEWORK_IDENTIFIER net.mpyne.gme
                       VERSION ${GME_VERSION}
                       SOVERSION 0
                       PUBLIC_HEADER "${EXPORTED_HEADERS}")
    endif()
endif()

# Building the Static version of GME
if(GME_BUILD_STATIC)
    # Static build.
    add_library(gme_static STATIC ${libgme_SRCS})

    # Static builds need to find static zlib (and static forms of other needed
    # libraries.  Ensure CMake looks only for static libs if we're doing a static
    # build.  See https://stackoverflow.com/a/44738756
    if(MSVC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()

    if(MSVC AND GME_BUILD_SHARED)
        set_target_properties(gme_static PROPERTIES OUTPUT_NAME gme-static)
    else()
        set_target_properties(gme_static PROPERTIES OUTPUT_NAME gme)
    endif()

    set_gme_props(gme_static)

    target_link_libraries(gme_static PUBLIC gme_deps)

    if(HAVE_LIBM)
        target_link_libraries(gme_static PUBLIC -lm)
    endif()
endif()

if(GME_BUILD_SHARED)
    add_library(gme::gme ALIAS gme_shared)
else()
    add_library(gme::gme ALIAS gme_static)
endif()


install(TARGETS ${INSTALL_TARGETS}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # DLL platforms
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # DLL platforms
        FRAMEWORK DESTINATION /Library/Frameworks)   # macOS framework


# Run during cmake phase, so this is available during make
configure_file(gme_types.h.in gen_types.h @ONLY)

set(TMP_PC_LIBS "")
foreach(PC_LIB ${PC_LIBS})
    if(NOT "${TMP_PC_LIBS}" STREQUAL "")
        set(TMP_PC_LIBS "${TMP_PC_LIBS} ${PC_LIB}")
    else()
        set(TMP_PC_LIBS "${PC_LIB}")
    endif()
endforeach()
set(PC_LIBS "${TMP_PC_LIBS}")
unset(TMP_PC_LIBS)

configure_file(libgme.pc.in libgme.pc @ONLY)

install(FILES ${EXPORTED_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gme)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgme.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
>>>>>>> db7344ebf (abc)
