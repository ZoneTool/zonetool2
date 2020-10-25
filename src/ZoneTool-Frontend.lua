ZoneTool_Frontend = {}

function ZoneTool_Frontend:include()
    includedirs {
        path.join(ProjectFolder(), "ZoneTool-Frontend")
    }
end

function ZoneTool_Frontend:link()
    self:include()
end

function ZoneTool_Frontend:project()
    local folder = ProjectFolder();

    project "ZoneTool-Frontend"
        kind "ConsoleApp"
        language "C++"
        dependson "ZoneTool-Generator"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "ZoneTool-Frontend/stdafx.cpp"))

        files {
            path.join(folder, "ZoneTool-Frontend/**.h"),
            path.join(folder, "ZoneTool-Frontend/**.hpp"),
            path.join(folder, "ZoneTool-Frontend/**.cpp")
        }

        -- Linked projects
        self:include()
        
        -- ThirdParty
        zlib:link()

        if _OPTIONS["set-version"] then
            defines {
                "ZONETOOL_VERSION=\"" .. _OPTIONS["set-version"] .. "\""
            }
        end

        filter "toolset:msc*"
            prebuildcommands {
                "if \"%COMPUTERNAME%\" == \"DESKTOP-CDFBECH\" ( \"Y:\\projects\\zonetool\\zonetool-transpiler\\build\\bin\\ZoneTool-Generator-x86-debug.exe\" \"Y:\\projects\\zonetool\\zonetool-transpiler\\src\\ZoneTool-Frontend\\linkers\\iw4.hpp\" \"Y:\\projects\\zonetool\\zonetool-transpiler\\src\\ZoneTool-Frontend\\linkers\\iw4.cpp\" \"iw4\" )",
            }
        filter {}
end