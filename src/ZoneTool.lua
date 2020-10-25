ZoneTool = {}

function ZoneTool:include()
    includedirs {
        path.join(ProjectFolder(), "ZoneTool")
    }
end

function ZoneTool:link()
    self:include()
	links {
		"ZoneTool"
	}
end

function ZoneTool:project()
    local folder = ProjectFolder();

    project "ZoneTool-Generator"
        kind "ConsoleApp"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "ZoneTool/stdafx.cpp"))

        files {
            path.join(folder, "ZoneTool/**.h"),
            path.join(folder, "ZoneTool/**.hpp"),
            path.join(folder, "ZoneTool/**.cpp")
        }

        -- Linked projects
        self:include()
        
        -- ThirdParty
        zlib:link()
        chaiscript:link()

        if _OPTIONS["set-version"] then
            defines {
                "ZONETOOL_VERSION=\"" .. _OPTIONS["set-version"] .. "\""
            }
        end
end