const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // volk path option
    const volk_path = b.option([]const u8, "volk-path", "path to installed volk headers") orelse null;
    if (volk_path == null) {
        std.debug.print("volk-path is required for installed test\n", .{});
        return;
    }

    // vulkan headers path option
    const vulkan_headers_dir = b.option([]const u8, "vulkan-headers-dir", "where to get vulkan headers") orelse null;

    // create test executable
    const exe = b.addExecutable(.{
        .name = "volk_installed_test",
        .target = target,
        .optimize = optimize,
    });

    exe.addCSourceFile(.{
        .file = b.path("main.c"),
    });

    // add include paths
    exe.addIncludePath(.{ .cwd_relative = volk_path.? });

    if (vulkan_headers_dir) |dir| {
        exe.addIncludePath(.{ .cwd_relative = dir });
    }

    // add platform-specific defines
    switch (target.result.os.tag) {
        .windows => exe.root_module.addCMacro("VK_USE_PLATFORM_WIN32_KHR", ""),
        .linux => exe.root_module.addCMacro("VK_USE_PLATFORM_XLIB_KHR", ""),
        .macos => exe.root_module.addCMacro("VK_USE_PLATFORM_MACOS_MVK", ""),
        else => {},
    }

    // link with C library
    exe.linkLibC();

    // link with libdl on non-windows platforms
    if (target.result.os.tag != .windows) {
        exe.linkSystemLibrary("dl");
    }

    exe.addCSourceFile(.{
        .file = .{ .cwd_relative = b.fmt("{s}/volk.c", .{volk_path.?}) },
    });

    // install the executable
    b.installArtifact(exe);

    // create run step
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run-installed", "Run the installed headers test");
    run_step.dependOn(&run_cmd.step);
}
