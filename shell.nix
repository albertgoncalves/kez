with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_12.stdenv; } {
    buildInputs = [
        cmake
        doxygen
        feh
        libGL
        pkg-config
        python3
        python3Packages.flake8
        renderdoc
        shellcheck
        xorg.libX11
        xorg.libXcursor
        xorg.libXext
        xorg.libXi
        xorg.libXinerama
        xorg.libXrandr
    ];
    shellHook = ''
        . .shellhook
    '';
}
