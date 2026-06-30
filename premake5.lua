workspace "reCOM"
    configurations { "Debug", "Release" }
    platforms { "PS2" }
	defaultplatform "x86"

project "fts"
    kind "ConsoleApp"
    language "C++"

	flags { "MultiProcessorCompile" }

    includedirs {
		"src"
	}

    files {
        "src/Apps/FTS/**.cpp",
		"src/Apps/FTS/**.h",
        "src/gamez/**.cpp",
		"src/gamez/**.h",
        "src/rsa/**.cpp",
        "src/rsa/**.h",
        "src/rt/**.cpp",
        "src/rt/**.h",
        "src/posix/**.cpp",
        "src/posix/**.h"
    }

    filter { "configurations:Debug" }
        defines { "DEBUG", "NOGAME" }
        symbols "On"
		targetdir "output/debug"

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"
		targetdir "output/release"
		targetname ("fts64")

    filter { "platforms:PS2" }
        system "bsd"
        architecture "mips"
		targetname ("SCUS-972")
		targetextension (".05")