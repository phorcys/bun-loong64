pub const URL = opaque {
    extern fn URL__fromJS(JSValue, *jsc.JSGlobalObject) ?*URL;
    extern fn URL__fromString(*bun.String) ?*URL;
    extern fn URL__protocol_out(*URL, *String) void;
    extern fn URL__href_out(*URL, *String) void;
    extern fn URL__username_out(*URL, *String) void;
    extern fn URL__password_out(*URL, *String) void;
    extern fn URL__search_out(*URL, *String) void;
    extern fn URL__host_out(*URL, *String) void;
    extern fn URL__hostname_out(*URL, *String) void;
    extern fn URL__port(*URL) u32;
    extern fn URL__deinit(*URL) void;
    extern fn URL__pathname_out(*URL, *String) void;
    extern fn URL__getHrefFromJS_out(JSValue, *jsc.JSGlobalObject, *String) void;
    extern fn URL__getHref_out(*String, *String) void;
    extern fn URL__getFileURLString_out(*String, *String) void;
    extern fn URL__getHrefJoin_out(*String, *String, *String) void;
    extern fn URL__pathFromFileURL_out(*String, *String) void;
    extern fn URL__hash_out(*URL, *String) void;
    extern fn URL__fragmentIdentifier_out(*URL, *String) void;

    /// Includes the leading '#'.
    pub fn hash(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__hash_out(url, &out);
        return out;
    }

    /// Exactly the same as hash, excluding the leading '#'.
    pub fn fragmentIdentifier(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__fragmentIdentifier_out(url, &out);
        return out;
    }

    pub fn hrefFromString(str: bun.String) String {
        jsc.markBinding(@src());
        var input = str;
        var out: String = undefined;
        URL__getHref_out(&input, &out);
        return out;
    }

    pub fn join(base: bun.String, relative: bun.String) String {
        jsc.markBinding(@src());
        var base_str = base;
        var relative_str = relative;
        var out: String = undefined;
        URL__getHrefJoin_out(&base_str, &relative_str, &out);
        return out;
    }

    pub fn fileURLFromString(str: bun.String) String {
        jsc.markBinding(@src());
        var input = str;
        var out: String = undefined;
        URL__getFileURLString_out(&input, &out);
        return out;
    }

    pub fn pathFromFileURL(str: bun.String) String {
        jsc.markBinding(@src());
        var input = str;
        var out: String = undefined;
        URL__pathFromFileURL_out(&input, &out);
        return out;
    }

    /// This percent-encodes the URL, punycode-encodes the hostname, and returns the result
    /// If it fails, the tag is marked Dead
    pub fn hrefFromJS(value: JSValue, globalObject: *jsc.JSGlobalObject) bun.JSError!String {
        jsc.markBinding(@src());
        var result: String = undefined;
        URL__getHrefFromJS_out(value, globalObject, &result);
        if (globalObject.hasException()) return error.JSError;
        return result;
    }

    pub fn fromJS(value: JSValue, globalObject: *jsc.JSGlobalObject) bun.JSError!?*URL {
        jsc.markBinding(@src());
        const result = URL__fromJS(value, globalObject);
        if (globalObject.hasException()) return error.JSError;
        return result;
    }

    pub fn fromUTF8(input: []const u8) ?*URL {
        return fromString(String.borrowUTF8(input));
    }
    pub fn fromString(str: bun.String) ?*URL {
        jsc.markBinding(@src());
        var input = str;
        return URL__fromString(&input);
    }
    pub fn protocol(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__protocol_out(url, &out);
        return out;
    }
    pub fn href(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__href_out(url, &out);
        return out;
    }
    pub fn username(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__username_out(url, &out);
        return out;
    }
    pub fn password(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__password_out(url, &out);
        return out;
    }
    pub fn search(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__search_out(url, &out);
        return out;
    }

    /// Returns the host WITHOUT the port.
    ///
    /// Note that this does NOT match JS behavior, which returns the host with the port. See
    /// `hostname` for the JS equivalent of `host`.
    ///
    /// ```
    /// URL("http://example.com:8080").host() => "example.com"
    /// ```
    pub fn host(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__host_out(url, &out);
        return out;
    }

    /// Returns the host WITH the port.
    ///
    /// Note that this does NOT match JS behavior which returns the host without the port. See
    /// `host` for the JS equivalent of `hostname`.
    ///
    /// ```
    /// URL("http://example.com:8080").hostname() => "example.com:8080"
    /// ```
    pub fn hostname(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__hostname_out(url, &out);
        return out;
    }
    /// Returns `std.math.maxInt(u32)` if the port is not set. Otherwise, `port`
    /// is guaranteed to be within the `u16` range.
    pub fn port(url: *URL) u32 {
        jsc.markBinding(@src());
        return URL__port(url);
    }
    pub fn deinit(url: *URL) void {
        jsc.markBinding(@src());
        return URL__deinit(url);
    }
    pub fn pathname(url: *URL) String {
        jsc.markBinding(@src());
        var out: String = undefined;
        URL__pathname_out(url, &out);
        return out;
    }

    extern fn URL__originLength(latin1_slice: [*]const u8, len: usize) u32;
    pub fn originFromSlice(slice: []const u8) ?[]const u8 {
        jsc.markBinding(@src());
        // a valid URL will not have ascii in the origin.
        const first_non_ascii = bun.strings.firstNonASCII(slice) orelse slice.len;
        const len = URL__originLength(
            slice[0..first_non_ascii].ptr,
            first_non_ascii,
        );
        if (len == 0) return null;
        return slice[0..len];
    }
};

const std = @import("std");

const bun = @import("bun");
const JSError = bun.JSError;
const String = bun.String;

const jsc = bun.jsc;
const JSGlobalObject = jsc.JSGlobalObject;
const JSValue = jsc.JSValue;
