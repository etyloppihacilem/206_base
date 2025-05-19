cat << EOF > .clangd
[
  {
    "arguments": [
      "/usr/bin/arm-none-eabi-gcc",
      "-mcpu=cortex-m3",
      "-mthumb",
      "-c",
      "-fno-common",
      "-fmessage-length=0",
      "-Wall",
      "-fno-exceptions",
      "-ffunction-sections",
      "-fdata-sections",
      "-g",
      "-DTARGET_LPC1769",
      "-DTOOLCHAIN_GCC_ARM",
      "-DNDEBUG",
      "-D__CORTEX_M3",
      "-std=gnu99",
      "-I.",
      "-I./LPC1769",
      "-I./include",
      "-I/usr/lib/gcc/arm-none-eabi/14.1.0/include",
      "-I/usr/lib/gcc/arm-none-eabi/14.1.0/include-fixed",
      "-I/usr/lib/gcc/arm-none-eabi/14.1.0/../../../../arm-none-eabi/include",
      "-o",
      "build/params.o",
      "src/params.c"
    ],
    "directory": "$(pwd)",
    "file": "$(realpath src/params.c)",
    "output": "$(pwd)/build/params.o"
  }
]
EOF

mkdir -p include
