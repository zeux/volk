const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const vulkan_headers_dir = b.option([]const u8, "vulkan-headers-dir", "where to get vulkan headers") orelse null;

    // first build volk static lib
    const volk_lib = b.addStaticLibrary(.{
        .name = "volk",
        .target = target,
        .optimize = optimize,
    });

    volk_lib.addCSourceFile(.{
        .file = b.path("../../../volk.c"),
    });

    volk_lib.addIncludePath(b.path("."));
    volk_lib.addIncludePath(b.path("../../.."));

    if (vulkan_headers_dir) |dir| {
        volk_lib.addIncludePath(.{ .cwd_relative = dir });
    }

    // add platform-specific defines
    switch (target.result.os.tag) {
        .windows => volk_lib.root_module.addCMacro("VK_USE_PLATFORM_WIN32_KHR", ""),
        .linux => volk_lib.root_module.addCMacro("VK_USE_PLATFORM_XLIB_KHR", ""),
        .macos => volk_lib.root_module.addCMacro("VK_USE_PLATFORM_MACOS_MVK", ""),
        else => {},
    }

    volk_lib.linkLibC();

    // link with libdl on non-windows platforms
    if (target.result.os.tag != .windows) {
        volk_lib.linkSystemLibrary("dl");
    }

    // create test executable
    const exe = b.addExecutable(.{
        .name = "volk_static_lib_test",
        .target = target,
        .optimize = optimize,
    });

    exe.addCSourceFile(.{
        .file = b.path("main.c"),
    });

    // add include paths
    exe.addIncludePath(b.path("."));
    exe.addIncludePath(b.path("../../.."));

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

    // link with volk static lib
    exe.linkLibrary(volk_lib);

    // link with C library
    exe.linkLibC();

    // link with libdl on non-windows platforms
    if (target.result.os.tag != .windows) {
        exe.linkSystemLibrary("dl");
    }

    // install the executable
    b.installArtifact(exe);

    // create run step
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run-static", "Run the static library test");
    run_step.dependOn(&run_cmd.step);
}
