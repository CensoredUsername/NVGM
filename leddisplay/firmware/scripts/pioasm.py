"""
Custom pioasm compiler script for platformio.
(c) 2025 by P.Z.

"""
from os.path import join
import glob
import sys
Import("env")

install_msg = """tool-pioasm-rp2040-earlephilhower not found!

Visit https://github.com/earlephilhower/pico-quick-toolchain/releases/
and copy the link to your OS correspoding version.

The tool is no longer in the Platformio repository and has to be installed
manually using a direct link.

Either install the tool globally (change the address to your version):
pio pkg install -g --tool https://github.com/earlephilhower/pico-quick-toolchain/releases/download/4.0.1/x86_64-linux-gnu.pioasm-efe2103.240929.tar.gz

of add in the platfromio.ini

platform_packages = tool-pioasm-rp2040-earlephilhower @ https://github.com/earlephilhower/pico-quick-toolchain/releases/download/4.0.1/x86_64-linux-gnu.pioasm-efe2103.240929.tar.gz"""

platform = env.PioPlatform()
PROJ_SRC = env["PROJECT_SRC_DIR"]
PIO_FILES = glob.glob(join(PROJ_SRC, '*.pio'), recursive=True)

if PIO_FILES:
    print("==============================================")
    print('PIO ASSEMBLY COMPILER')
    try:
        PIOASM_DIR = platform.get_package_dir("tool-pioasm-rp2040-earlephilhower")
    except:
        print(install_msg)
        sys.exit()

    if PIOASM_DIR is not None:
        PIOASM_EXE = join(PIOASM_DIR, "pioasm")
        print(f"pio files found:")
        for filename in PIO_FILES:
            env.Execute(PIOASM_EXE + f' -o c-sdk {filename} {filename}.h')
        print("==============================================")
    else:
        print(install_msg)
        sys.exit()