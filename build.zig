const std = @import("std");

fn findvk(b: *std.Build, explicit_dir: ?[]const u8) ?[]const u8 {
    // first try explicit directory if provided
    if (explicit_dir) |dir| {
        return dir;
    }

    // finally check VULKAN_SDK environment variable
    if (std.process.getEnvVarOwned(b.allocator, "VULKAN_SDK")) |sdk_path| {
        const include_path = std.fs.path.join(
            b.allocator,
            &.{ sdk_path, "include" },
        ) catch return null;

        defer b.allocator.free(include_path);
        return b.dupe(include_path);
    } else |_| {}

    // not found
    return null;
}

fn reqvk(b: *std.Build, vulkan_headers_dir: ?[]const u8) []const u8 {
    if (findvk(b, vulkan_headers_dir)) |dir| {
        return dir;
    }

    std.debug.print("ERROR: Vulkan headers not found!\n", .{});
    std.debug.print("Please set VULKAN_SDK environment variable or provide --vulkan-headers-dir option\n", .{});
    @panic("Vulkan headers required but not found");
}

pub fn build(b: *std.Build) void {
    // project version
    const version = std.SemanticVersion{ .major = 0, .minor = 3, .patch = 13 };

    // options
    const static_defines = b.option([]const []const u8, "static_defines", "Additional defines for volk static library") orelse &[_][]const u8{};
    const pull_in_vulkan = b.option(bool, "pull_in_vulkan", "Vulkan as transitive dependency") orelse true;
    const install = b.option(bool, "install", "Create installation targets") orelse false;
    const headers_only = b.option(bool, "headers_only", "Add interface library only") orelse false;
    const vulkan_headers_dir = b.option([]const u8, "vulkan_headers_dir", "Where to get vulkan headers") orelse null;

    // standard build options
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // require vulkan headers
    const vulkan_include_dir = if (pull_in_vulkan)
        reqvk(b, vulkan_headers_dir)
    else
        null;

    // -----------------------------------------------
    // static library

    var volk_lib: ?*std.Build.Step.Compile = null;

    if (!headers_only) {
        volk_lib = std.Build.Step.Compile.create(b, .{
            .name = "volk",
            .version = version,
            .kind = .lib,
            .linkage = .static,
            .root_module = b.createModule(.{
                .target = target,
                .optimize = optimize,
            }),
        });

        volk_lib.?.addCSourceFile(.{
            .file = b.path("volk.c"),
        });

        volk_lib.?.addIncludePath(b.path("."));

        // add platform-specific defines
        switch (target.result.os.tag) {
            .windows => volk_lib.?.root_module.addCMacro("VK_USE_PLATFORM_WIN32_KHR", ""),
            .linux => volk_lib.?.root_module.addCMacro("VK_USE_PLATFORM_XLIB_KHR", ""),
            .macos => volk_lib.?.root_module.addCMacro("VK_USE_PLATFORM_MACOS_MVK", ""),
            else => {},
        }

        // add defines if provided
        for (static_defines) |define| {
            volk_lib.?.root_module.addCMacro(define, "");
        }

        // add vulkan headers if enabled
        if (pull_in_vulkan) {
            volk_lib.?.addIncludePath(.{ .cwd_relative = vulkan_include_dir.? });
        }

        volk_lib.?.linkLibC();

        // link with libdl on non-windows platforms
        if (target.result.os.tag != .windows) {
            volk_lib.?.linkSystemLibrary("dl");
        }

        // make the public module inherit the include paths from volk_lib
        if (pull_in_vulkan) {
            volk_lib.?.installHeader(b.path("volk.h"), "volk.h");
        }

        b.installArtifact(volk_lib.?);
    } else {
        // even in headers-only mode, install the header
        const header_install = b.addInstallFile(b.path("volk.h"), "include/volk.h");
        b.getInstallStep().dependOn(&header_install.step);
    }

    // export Vulkan SDK path for consumers
    if (vulkan_include_dir != null) {
        b.addNamedLazyPath("vulkan_include_dir", .{ .cwd_relative = vulkan_include_dir.? });
    }

    // -----------------------------------------------
    // installation support

    const install_step = b.getInstallStep();

    if (install) {
        // install header files
        const header_install = b.addInstallFile(b.path("volk.h"), "include/volk.h");
        const source_install = b.addInstallFile(b.path("volk.c"), "include/volk.c");

        install_step.dependOn(&header_install.step);
        install_step.dependOn(&source_install.step);
    }
}
