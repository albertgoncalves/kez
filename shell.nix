with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_14.stdenv; } {
    buildInputs = [
        feh
        glfw
        libGL
        mold
        python3
        python3Packages.flake8
        renderdoc
        shellcheck
        valgrind
    ];
    APPEND_LIBRARY_PATH = lib.makeLibraryPath [
        glfw
        libGL
    ];
    shellHook = ''
        export LD_LIBRARY_PATH="$APPEND_LIBRARY_PATH:$LD_LIBRARY_PATH"
        . .shellhook
    '';
}
