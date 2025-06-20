const std = @import("std");

fn str(s: []const u8) []const u8 {
    return s;
}

fn envHeadersDir(alloc: std.mem.Allocator) ?[]const u8 {
    const sdk = std.process.getEnvVarOwned(alloc, "VULKAN_SDK") catch null;
    if (sdk) |path| {
        defer alloc.free(path);
        return std.fs.path.join(alloc, &.{ path, "include" }) catch null;
    }
    return null;
}

fn systemHeadersDir() ?[]const u8 {
    const hdr = "/usr/include/vulkan/vulkan.h";
    const file = std.fs.cwd().openFile(hdr, .{}) catch return null;
    file.close();
    return str("/usr/include");
}

fn locateHeaders(b: *std.Build) []const u8 {
    const arg = b.option([]const u8, "vulkan-headers-dir", "Explicit Vulkan headers dir") orelse "";
    if (arg.len > 0) return arg;

    if (envHeadersDir(b.allocator)) |p| return p;
    if (systemHeadersDir()) |p| return p;

    std.log.err(
        \\Volk build: Vulkan headers not found.
        \\Pass -Dvulkan-headers-dir=/path or set $VULKAN_SDK.
    , .{});
    @panic("missing Vulkan headers");
}

fn linkLoader(lib: *std.Build.Step.Compile, target: std.Build.ResolvedTarget) void {
    if (target.result.os.tag != .windows) lib.linkSystemLibrary("dl");
}

pub fn build(b: *std.Build) void {
    const opt_headers_only = b.option(bool, "headers_only", "Just interface lib") orelse false;
    const opt_install = b.option(bool, "install", "Install headers/lib") orelse false;
    const static_defines = b.option([]const []const u8, "static_defines", "Extra compile-time defines") orelse &[_][]const u8{};

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const vk_inc = locateHeaders(b);

    const int_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });
    int_mod.addIncludePath(b.path("."));
    int_mod.addIncludePath(.{ .cwd_relative = vk_inc });

    var static_lib: ?*std.Build.Step.Compile = null;
    if (!opt_headers_only or opt_install) {
        static_lib = b.addLibrary(.{
            .name = "volk",
            .root_module = int_mod,
        });
        static_lib.?.addCSourceFile(.{ .file = b.path("volk.c") });
        for (static_defines) |d| static_lib.?.root_module.addCMacro(d, "");
        linkLoader(static_lib.?, target);
    }

    b.addNamedLazyPath("vulkan_include_dir", .{ .cwd_relative = vk_inc });

    if (opt_install) {
        const is = b.getInstallStep();
        const hdr = b.addInstallFile(b.path("volk.h"), "include/volk.h");
        const src = b.addInstallFile(b.path("volk.c"), "include/volk.c");
        is.dependOn(&hdr.step);
        is.dependOn(&src.step);
        if (static_lib) |lib| is.dependOn(&(b.addInstallArtifact(lib, .{}).step));
    }
}
