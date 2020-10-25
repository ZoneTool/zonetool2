newoption {
	trigger = "set-version",
	description = "Sets the version information of zonetool",
}

-- Functions for locating commonly used folders
local _DependencyFolder = path.getabsolute("dep")
function DependencyFolder()
	return path.getrelative(os.getcwd(), _DependencyFolder)
end

local _ProjectFolder = path.getabsolute("src")
function ProjectFolder()
	return path.getrelative(os.getcwd(), _ProjectFolder)
end

-- ========================
-- Workspace
-- ========================
workspace "zonetool"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin"
	targetname "%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}"
	warnings "Off"

	configurations {
		"debug",
		"release",
	}

	platforms {
		"x86",
		"x64"
	}

	filter "platforms:x86"
		architecture "x86"
		defines "CPU_32BIT"
	filter {}

	filter "platforms:x64"
		architecture "x86_64"
		defines "CPU_64BIT"
	filter {}

	buildoptions "/std:c++latest"
	systemversion "latest"
	symbols "On"
	editandcontinue "Off"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks"
	}

	filter "configurations:debug"
		optimize "Debug"
		defines {
			"DEBUG",
			"_DEBUG",
		}
	filter {}

	filter "configurations:release"
		optimize "Full"
		defines {
			"NDEBUG",
		}
		flags {
			"FatalCompileWarnings",
		}
	filter{}

	includedirs {
		ProjectFolder()
	}

	startproject "ZoneTool-Frontend"

	-- ========================
	-- Dependencies
	-- ========================

	include "dep/zlib.lua"

	-- All projects here should be in the thirdparty folder
	group "thirdparty"

	zlib:project()

	-- Reset group
	group ""

	-- ========================
	-- Projects
	-- ========================

	include "src/ZoneTool.lua"
	include "src/ZoneTool-Frontend.lua"

	ZoneTool:project()
	ZoneTool_Frontend:project()
