# Switched on for our own targets only - the downloaded libraries keep their
# own settings, we are not going to fix warnings in code we did not write.
function(applyProjectWarnings targetName)
    if(MSVC)
        target_compile_options(${targetName} PRIVATE /W4 /permissive-)

        # Visual C++ calls std::getenv and std::localtime "unsafe". They are
        # used on purpose, because only they exist on every system.
        target_compile_definitions(${targetName} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${targetName} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion)
    endif()
endfunction()
