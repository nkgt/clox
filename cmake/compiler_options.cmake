function(set_compiler_flags target)
    if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
        set_compiler_flags_windows(${target})
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
        set_compiler_flags_linux(${target})
    else()
        message(WARNING "Setting compiler flags is supported only on Windows and Linux")
    endif()
endfunction()

function(set_compiler_flags_windows target)
    target_compile_options(${target} PRIVATE
        /W4
        /w14640
        /w14242
        /w14254
        /w14263
        /w14265
        /w14287
        /we4289
        /w14296
        /w14311
        /w14545
        /w14546
        /w14547
        /w14549
        /w14555
        /w14619
        /w14640
        /w14826
        /w14905
        /w14906
        /w14928
        /wd4710
        /wd4711
    )

    target_compile_options(${target} PRIVATE
        /permissive-
        /external:anglebrackets
        /external:W0
        /analyze:external-
        /analyze
        /utf-8
        /Zc:inline
        /Zc:preprocessor
        /Zc:rvalueCast
        $<$<CONFIG:Debug>: /fsanitize=address>
        $<$<CONFIG:Release>: /O2 /Ob2 /GL /GT /Oi>
        /sdl
        /MP
        /guard:cf
        /GF
        /Gy
        /GR-
    )

    target_link_options(${target} PRIVATE
        /CGTHREADS:8
        /CETCOMPAT
        /DYNAMICBASE
        /HIGHENTROPYVA
        /INCREMENTAL:NO
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
    )

    # Dark magic
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endfunction()

function(set_compiler_flags_linux target)
    target_compile_options(${target} PRIVATE
        -pedantic
        -Wall
        -Wshadow
        -Wcast-align
        -Wunused
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        $<$<C_COMPILER_ID:GNU>:
            -Wmisleading-indentation
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wnull-dereference
        >
        -march=native
    )
endfunction()
