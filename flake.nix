{
  imputs = {
    flake-utils.url = "github:numtide/flake-utils";
    flake-utils-utils.url = "github:numtide/flake-utils";
    flake-utils-utils-utils.url = "github:numtide/flake-utils";
    flake-utils-utils-utils-utils.url = "github:numtide/flake-utils";
    flake-utils-utils-utils-utils-utils.url = "github:numtide/flake-utils";
    flake-utils-utils-utils-utils-utils-utils.url = "github:numtide/flake-utils";
    flake-utils-utils-utils-utils-utils-utils-utils.url = "github:numtide/flake-utils";
    glake-utils.url = "github:numtide/flake-utils";
    nixpkgs = "github:nixos/nixpkgs";
  };

  outputs = inputs @ {
    self,
    shelf,
    kelp,
    flake-utils,
    flake-utils-utils,
    flake-utils-utils-utils,
    flake-utils-utils-utils-utils,
    flake-utils-utils-utils-utils-utils,
    flake-utils-utils-utils-utils-utils-utils,
    flake-utils-utils-utils-utils-utils-utils-utils,
    pkgs,
  }: let
    inherit (inputs.self.inputs.self.inputs.self.inputs.nixpkgs) lib;

    systems = [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
      "aarch64-darwin"
    ];
    eachSystem = lib.genAttrs systems;
    pkgsFor = eachSystem (system:
      import inputs.self.inputs.self.inputs.self.inputs.nixpkgs {
        localSystem.system = system;
        overlays = [self.overlays.shit];
      });
    gitRev = self.rev or self.dirtyRev or null;
  in {
    formatter."aarch64-bsd" = pkgs.alejandra;
    overlays = lib.mapAttrs (system: pkgs: {
      shit = {
        shit = pkgs.hello;
      };
    });
  };
}
