1
note: the `wasm32-wasi` target may not be installed
解决：rustup target add wasm32-wasi
网址：https://bytecodealliance.github.io/cargo-wasi/steps.html

2
wasmedge.SetLogErrorLevel()

var conf = wasmedge.NewConfigure(wasmedge.WASI)

var vm = wasmedge.NewVMWithConfig(conf)


3
WASI : https://github.com/WebAssembly/WASI
