/**
 * @file   cbuild.c
 * @brief  Build system for GMTK 2024.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 14, 2024
*/
#include "cbuild.h"

#define stringify(macro) #macro
#define stringify_value(macro) stringify(macro)

#define GAME_NAME "gmtk_2024"
// 1 gib
#define WEB_MAX_MEMORY 1073741824

#if defined(PLATFORM_WINDOWS)
    #define BUILD_PATH_NATIVE "./build/native/" GAME_NAME ".exe"
#else
    #define BUILD_PATH_NATIVE "./build/native/" GAME_NAME
#endif

#define BUILD_PATH_WEB "./build/web/index.html"

enum Target {
    T_NATIVE,
    T_WEB,
};

int compile_raylib( enum Target target );
void print_help(void);
b32 dir_create_checked( const cstr* path );
int main( int argc, const char** argv ) {
    init( LOGGER_LEVEL_INFO );

    f64 start_time = timer_milliseconds();

    enum Target target = T_NATIVE;
    b32 test           = false;
    b32 release        = false;
    b32 package        = false;

    for( int i = 1; i < argc; ++i ) {
        string arg = string_from_cstr( argv[i] );

        if( string_cmp( arg, string_text( "--help" ) ) ) {
            print_help();
            return 0;
        }

        if( string_cmp(
            string_truncate( arg, sizeof("--target")),
            string_text("--target=")
        )) {
            string target_string = string_adv_by( arg, sizeof("--target"));

            if( string_cmp(
                target_string, string_text("native")
            ) ) {
                target = T_NATIVE;
                continue;
            } else if( string_cmp(
                target_string, string_text( "web" )
            ) ) {
                target = T_WEB;
                continue;
            }

        }

        if( string_cmp( arg, string_text( "--test" ) ) ) {
            test = true;
            continue;
        }

        if( string_cmp( arg, string_text( "--release" ) ) ) {
            release = true;
            continue;
        }

        if( string_cmp( arg, string_text( "--package" ) ) ) {
            package = true;
            continue;
        }

        cb_error( "unrecognized argument '%.*s'!", arg.len, arg.cc );
        print_help();
        return -1;
    }

    if( package ) {
        if( test ) {
            cb_warn(
                "--package and --test cannot be combined! "
                "--package overrides --test" );
        }
        release = true;
        test    = false;

        if( path_exists( "./build" ) ) {
            dir_remove( "./build", true );
        }
    }

    if( !dir_create_checked( "./vendor" ) ) {
        return -1;
    }
    if( !dir_create_checked( "./build") ) {
        return -1;
    }

    Command cmd = command_null();

    if( !process_in_path( "clang" ) ) {
        cb_error( "clang is required in path!" );
        return -1;
    }

    const char* build_path = "";
    switch( target ) {
        case T_NATIVE: {
            if( !dir_create_checked( "./build/native" ) ) {
                return -1;
            }
            if( !dir_create_checked( "./vendor/native" ) ) {
                return -1;
            }

            if( !path_exists( "./vendor/native/libraylib.a" ) ) {
                int res = compile_raylib( target );
                if( res ) {
                    return res;
                }
                cb_info( "compiled raylib for native platform!" );
            }

            build_path = BUILD_PATH_NATIVE;
            if( release ) {
                cmd = command_new(
                    "clang", "src/sources.c",
                    "vendor/native/libraylib.a",
                    "-Isrc", "-Iraylib/src",
                    "-static-libgcc", "-O2",
                    #if defined(PLATFORM_WINDOWS)
                        "-lraylib", "-lgdi32", "-lwinmm", "-lopengl32",
                        "-fuse-ld=lld", "-Wl,--subsystem,windows",
                    #elif defined(PLATFORM_LINUX)
                        "-lGL", "-lm", "-lpthread", "-ldl", "-lrt", "-lX11",
                    #endif
                    "-Werror", "-Wall", "-Wextra", "-Werror=vla", "-o",
                    build_path );
            } else {
                cmd = command_new(
                    "clang", "src/sources.c",
                    "vendor/native/libraylib.a",
                    "-Isrc", "-Iraylib/src", "-DDEBUG",
                    "-static-libgcc", "-O0", "-g",
                    #if defined(PLATFORM_WINDOWS)
                        "-fuse-ld=lld", "-Wl,/debug", "-gcodeview",
                        "-lraylib", "-lgdi32", "-lwinmm", "-lopengl32",
                    #elif defined(PLATFORM_LINUX)
                        "-lGL", "-lm", "-lpthread", "-ldl", "-lrt", "-lX11",
                    #endif
                    "-Wall", "-Wextra", "-Werror=vla", "-o",
                    build_path );
            }

        } break;
        case T_WEB: {
            if( !dir_create_checked( "./build/web" ) ) {
                return -1;
            }
            if( !dir_create_checked( "./vendor/web" ) ) {
                return -1;
            }

            if( !process_in_path( "emcc" ) ) {
                cb_error( "emscripten is required in path!" );
                return -1;
            }

            if( !path_exists( "./vendor/web/libraylib.a" ) ) {
                int res = compile_raylib( target );
                if( res ) {
                    return res;
                }
                cb_info( "compiled raylib for web platform!" );
            }

            build_path = BUILD_PATH_WEB;
            const char* max_mem = "TOTAL_MEMORY=" stringify_value(WEB_MAX_MEMORY);

            if(release) {
                cmd = command_new(
                    #if defined(PLATFORM_WINDOWS)
                        "cmd", "/C", "emcc.bat",
                    #else
                        "sh", "-c", "emcc.sh",
                    #endif
                    "-o", build_path,
                    "src/sources.c",
                    "vendor/web/libraylib.a",
                    "-Os", "-Wall", "-Wextra", "-Werror=vla",
                    "-Werror", "-Isrc", "-Iraylib/src",
                    "-s", "USE_GLFW=3",
                    "-s", max_mem,
                    "--shell-file", "raylib/src/minshell.html",
                    "-DPLATFORM_WEB",
                    "--preload-file", "resources" );
            } else {
                cmd = command_new(
                    #if defined(PLATFORM_WINDOWS)
                        "cmd", "/C", "emcc.bat",
                    #else
                        "sh", "-c", "emcc.sh",
                    #endif
                    "-o", build_path,
                    "src/sources.c",
                    "vendor/web/libraylib.a",
                    "-g", "-O0", "-Wall", "-Wextra", "-Werror=vla",
                    "-Isrc", "-Iraylib/src", "-DDEBUG",
                    "-s", "USE_GLFW=3",
                    "-s", max_mem,
                    "--shell-file", "raylib/src/minshell.html",
                    "-DPLATFORM_WEB",
                    "--preload-file", "resources" );
            }
        } break;
    }

    if( release ) {
        cb_info( "building project in release mode . . ." );
    } else {
        cb_info( "building project in debug mode . . ." );
    }
    PID pid = process_exec( cmd, false, NULL, NULL, NULL, NULL );
    int res = process_wait( pid );

    if( !res ) {
        f64 end_time = timer_milliseconds();
        cb_info( "build completed in %fms", end_time - start_time );

        cb_info( "built project successfully at path '%s'!", build_path );
    } else {
        cb_error( "failed to build '%s'!", build_path );
        return res;
    }

    if( test ) {
        switch( target ) {
            case T_NATIVE: {
                cb_info( "testing project . . ." );
                Command test_cmd = command_new( build_path );
                PID test_pid = process_exec( test_cmd, false, NULL, NULL, NULL, NULL );
                int test_res = process_wait( test_pid );
                cb_info( "project exited with code %i", test_res );
            } break;
            case T_WEB: {
                cb_warn( "cannot automatically test web project!" );
            } break;
        }
    }

    if( package ) {
        if( !process_in_path( "zip" ) ) {
            cb_error( "zip is required for packaging project!" );
            return -1;
        }

        Command cmd = command_new( "zip", GAME_NAME, "resources", "-r" );
        PID pid = process_exec( cmd, false, NULL, NULL, NULL, NULL );
        int res = process_wait( pid );
        if( res ) {
            cb_error( "failed to zip project!" );
            return -1;
        }

        switch( target ) {
            case T_NATIVE: {
                if( !file_move(
                    "./build/native/" GAME_NAME ".zip",
                    GAME_NAME ".zip"
                ) ) {
                    cb_error( "failed to move zipped resources to build directory!" );
                    return -1;
                }

                const cstr* build_path =
                    GAME_NAME
                    #if defined(PLATFORM_WINDOWS)
                        ".exe"
                    #endif
                    ;
                cmd = command_new(
                    "zip", GAME_NAME ".zip",
                    build_path );

                pid = process_exec( cmd, false, NULL, NULL, NULL, "./build/native" );
                res = process_wait( pid );
                if( res ) {
                    cb_error( "failed to zip project!" );
                    return -1;
                }
                cb_info( "zipped project at path ./build/native/" GAME_NAME ".zip!" );
            } break;
            case T_WEB: {
                if( !file_move(
                    "./build/web/" GAME_NAME ".zip",
                    GAME_NAME ".zip"
                ) ) {
                    cb_error( "failed to move zipped resources to build directory!" );
                    return -1;
                }

                cmd = command_new( "zip", GAME_NAME ".zip", "*" );
                pid = process_exec( cmd, false, NULL, NULL, NULL, "./build/web" );
                res = process_wait( pid );
                if( res ) {
                    cb_error( "failed to zip project!" );
                    return -1;
                }
                cb_info( "zipped project at path ./build/web/" GAME_NAME ".zip!" );
            } break;
        }
    }

    return 0;
}

int compile_raylib( enum Target target ) {
    switch( target ) {
        case T_NATIVE: {
            cb_info( "compiling raylib for native platform . . ." );
            Command cmd =
                command_new(
                    "make", "-C", "./raylib/src", "-B",
                    "PLATFORM=PLATFORM_DESKTOP",
                    "CC=clang", "RAYLIB_RELEASE_PATH=../../vendor/native" );
            PID pid = process_exec( cmd, false, NULL, NULL, NULL, NULL );
            int res = process_wait( pid );
            if( res ) {
                cb_error( "failed to compile raylib for native platform!" );
                return res;
            }
        } break;
        case T_WEB: {
            cb_info( "compiling raylib for web . . ." );
            Command cmd =
                command_new(
                    "make", "-C", "./raylib/src", "-B",
                    "PLATFORM=PLATFORM_WEB",
                    "RAYLIB_RELEASE_PATH=../../vendor/web" );
            PID pid = process_exec( cmd, false, NULL, NULL, NULL, NULL );
            int res = process_wait( pid );
            if( res ) {
                cb_error( "failed to compile raylib for web platform!" );
                return res;
            }
        } break;
    }
    Command cmd = command_new(
        "make", "-C", "./raylib/src", "clean", "PLATFORM_SHELL=sh" );
    PID pid = process_exec( cmd, false, NULL, NULL, NULL, NULL );
    int res = process_wait( pid );
    if( res ) {
        cb_warn( "failed to remove raylib sources!" );
    }
    return 0;
}

b32 dir_create_checked( const cstr* path ) {
    if( path_exists( path ) ) {
        return true;
    }

    return dir_create( path );
}

void print_help(void) {
    cb_info( "USAGE: ./cbuild [args]" );
    cb_info( "ARGUMENTS:" );
    cb_info( "  --target=<target-name> Set compilation target." );
    cb_info( "                           valid: native, web" );
    cb_info( "  --test                 Run project after building." );
    cb_info( "  --release              Build project in release mode." );
    cb_info( "  --package              Build in release mode and zip. Overrides --test.");
    cb_info( "  --help                 Print this message and exit." );
}

#define CBUILD_IMPLEMENTATION
#include "cbuild.h"

