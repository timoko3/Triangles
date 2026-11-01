в nix отдельно нужно устанавливать nixGL для поиска функций драйвера видеокарты 
```sh
nix run github:nix-community/nixGL#nixGLIntel -- \
  ./build/opengl-demo/opengl_demo
```