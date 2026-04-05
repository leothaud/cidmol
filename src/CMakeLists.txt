add_subdirectory(compiler)
add_subdirectory(ast)
add_subdirectory(gssa)

addExecutable(main main.cpp
  cidmolCore
  cidmolAst
  cidmolFrontend
  cidmolGssa
)