{
  description = "CMAKE/GLFW/GCC Development Environment Flake";
  # Need to address this issue to make this flake fully functional:
  # https://github.com/NixOS/nixpkgs/issues/139943

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    emsPatch = {
      url = "file+https://raw.githubusercontent.com/NixOS/nixpkgs/refs/heads/nixos-unstable/pkgs/development/compilers/emscripten/0001-emulate-clang-sysroot-include-logic.patch";
      flake = false;
    };
    cachePatch = {
      url = "file+https://raw.githubusercontent.com/NixOS/nixpkgs/refs/heads/nixos-unstable/pkgs/development/compilers/emscripten/locate_cache.sh";
      flake = false;
    };
  };

  outputs = { self , nixpkgs, emsPatch, cachePatch ,... }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {
        inherit system;
    };
    NPM_CONFIG_PREFIX = "npm-global";
  in {
    devShells."${system}".default = pkgs.mkShell {
      # All dependencies that we'll need to build
      packages = with pkgs; [
        cmake
        gcc
        glfw
        glew
        gtk4
        libdecor
        gtkmm4
        gtkd
        nodejs_24
        nodePackages.pnpm
        pkgs.python314

        pkgs.llvm
        pkgs.clang
        pkgs.wasm
        (pkgs.callPackage ./emscripten/default.nix {patchDir = emsPatch; patchCache = cachePatch;})
      ];

      shellHook = ''
        mkdir -p ${NPM_CONFIG_PREFIX}
        npm set prefix ${NPM_CONFIG_PREFIX}
        export PATH="${NPM_CONFIG_PREFIX}/bin:$PATH"
        cp -r ${pkgs.emscripten}/share/emscripten/cache ~/.emscripten_cache
        chmod u+rwX -R ~/.emscripten_cache
        export EM_CACHE=~/.emscripten_cache
      '';
    };
  };
}
