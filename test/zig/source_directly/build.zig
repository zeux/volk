const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const vulkan_headers_dir = b.option([]const u8, "vulkan-headers-dir", "Where to get vulkan headers") orelse null;

    // create test executable
    const exe = b.addExecutable(.{
        .name = "volk_source_direct_test",
        .target = target,
        .optimize = optimize,
    });

    // add source files
    exe.addCSourceFile(.{
        .file = b.path("main.c"),
    });

    exe.addCSourceFile(.{
        .file = b.path("../../../volk.c"),
    });

    // add include paths
    // FIXME: if we call build from one directory up/down it will not find volk.h properly.
    exe.addIncludePath(b.path("./"));
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

    const run_step = b.step("run-source-direct", "Run the source direct test");
    run_step.dependOn(&run_cmd.step);
}
