// Hardcoded module "node:os"
var tmpdir = function () {
  var env = Bun.env;

  tmpdir = function () {
    if (process.platform === "win32") {
      // using node implementation
      // https://github.com/nodejs/node/blob/ad5e2dab4c8306183685973387829c2f69e793da/lib/os.js#L186
      var path = env["TEMP"] || env["TMP"] || (env["SystemRoot"] || env["windir"]) + "\\temp";
      if (path.length > 1 && path[path.length - 1] === "\\" && !String.prototype.endsWith.$call(path, ":\\")) {
        path = String.prototype.slice.$call(path, 0, -1);
      }
      return path;
    }
    var path =
      env["TMPDIR"] || env["TMP"] || env["TEMP"] || (process.platform === "android" ? "/data/local/tmp" : "/tmp");
    const length = path.length;
    if (length > 1 && path[length - 1] === "/") path = path.slice(0, -1);
    return path;
  };

  tmpdir[Symbol.toPrimitive] = tmpdir;

  return tmpdir();
};

// os.cpus() is super expensive
// Specifically: getting the CPU speed on Linux is very expensive
// Some packages like FastGlob only bother to read the length of the array
// so instead of actually populating the entire object
// we turn them into getters
function lazyCpus({ cpus, hostCpuCount }) {
  return () => {
    const array = new Array(hostCpuCount);
    function populate() {
      const results = cpus();
      const length = results.length;
      array.length = length;
      for (let i = 0; i < length; i++) {
        array[i] = results[i];
      }
    }

    for (let i = 0; i < array.length; i++) {
      // This is technically still observable via
      // Object.getOwnPropertyDescriptors(), but it should be okay.
      const instance = {
        get model() {
          if (array[i] === instance) populate();
          return array[i].model;
        },
        set model(value) {
          if (array[i] === instance) populate();
          array[i].model = value;
        },

        get speed() {
          if (array[i] === instance) populate();
          return array[i].speed;
        },

        set speed(value) {
          if (array[i] === instance) populate();
          array[i].speed = value;
        },

        get times() {
          if (array[i] === instance) populate();
          return array[i].times;
        },
        set times(value) {
          if (array[i] === instance) populate();
          array[i].times = value;
        },

        toJSON() {
          if (array[i] === instance) populate();
          return array[i];
        },
      };

      array[i] = instance;
    }

    return array;
  };
}

// all logic based on `process.platform` and `process.arch` is inlined at bundle time
function bound(binding) {
  const currentArch = function () {
    return globalThis["process"].arch;
  };
  const currentPlatform = function () {
    return globalThis["process"].platform;
  };

  return {
    availableParallelism: function () {
      return navigator.hardwareConcurrency;
    },
    arch: function () {
      return currentArch();
    },
    cpus: lazyCpus(binding),
    endianness: function () {
      const arch = currentArch();
      return arch === "arm64" || arch === "x64" || arch === "loong64" ? "LE" : "LE";
    },
    freemem: binding.freemem,
    getPriority: binding.getPriority,
    homedir: binding.homedir,
    hostname: binding.hostname,
    loadavg: binding.loadavg,
    networkInterfaces: binding.networkInterfaces,
    platform: function () {
      return currentPlatform();
    },
    release: binding.release,
    setPriority: binding.setPriority,
    get tmpdir() {
      return tmpdir;
    },
    totalmem: binding.totalmem,
    type: function () {
      const platform = currentPlatform();
      return platform === "win32"
        ? "Windows_NT"
        : platform === "darwin"
          ? "Darwin"
          : platform === "linux" || platform === "android"
            ? "Linux"
            : platform === "freebsd"
              ? "FreeBSD"
              : "Unknown";
    },
    uptime: binding.uptime,
    userInfo: binding.userInfo,
    version: binding.version,
    machine: function () {
      // TODO: linux arm64 should also return "aarch64" (Node/uname compat) —
      // separate PR to avoid behavior change in the Android port.
      // FreeBSD: uname -m returns MACHINE ("arm64"/"amd64"), not MACHINE_ARCH.
      const arch = currentArch();
      const platform = currentPlatform();
      return arch === "arm64"
        ? platform === "android"
          ? "aarch64"
          : "arm64"
        : arch === "x64"
          ? platform === "freebsd"
            ? "amd64"
            : "x86_64"
          : arch === "loong64"
            ? "loongarch64"
            : arch;
    },
    devNull: currentPlatform() === "win32" ? "\\\\.\\nul" : "/dev/null",
    get EOL() {
      return currentPlatform() === "win32" ? "\r\n" : "\n";
    },
    constants: $processBindingConstants.os,
  };
}

const out = bound($rust("node_os.rs", "createNodeOsBinding"));

symbolToStringify(out, "arch");
symbolToStringify(out, "availableParallelism");
symbolToStringify(out, "endianness");
symbolToStringify(out, "freemem");
symbolToStringify(out, "homedir");
symbolToStringify(out, "hostname");
symbolToStringify(out, "platform");
symbolToStringify(out, "release");
symbolToStringify(out, "tmpdir");
symbolToStringify(out, "totalmem");
symbolToStringify(out, "type");
symbolToStringify(out, "uptime");
symbolToStringify(out, "version");
symbolToStringify(out, "machine");

function symbolToStringify(obj, key) {
  $assert(obj[key] !== undefined, `Missing ${key}`);
  obj[key][Symbol.toPrimitive] = function (_hint: string) {
    return obj[key]();
  };
}

export default out;
