## Small nametag using Pi Zero W and various OLED panels

### Backend
Build with cmake

Run with:
- `sudo ./nametag` (due to GPIO permissions)

Useful snippets for x-builds:
 - `TOOLCHAIN=armv6-rpi-linux-gnueabihf`
 - `TOOLDIR=${HOME}/.local/x-tools/${TOOLCHAIN}`
 - configure (freetype?): `CC=${TOOLDIR}/bin/${TOOLCHAIN}-gcc CXX=${TOOLDIR}/bin/${TOOLCHAIN}-g++ ./configure --prefix=${TOOL_DIR}/${TOOLCHAIN}/sysroot/usr $@`
 - ffmpeg: `CC=${TOOLDIR}/bin/${TOOLCHAIN}-gcc CXX=${TOOLDIR}/bin/${TOOLCHAIN}-g++ ./configure --prefix=${TOOLDIR}/${TOOLCHAIN}/sysroot/usr --arch=armel --target-os=linux --enable-cross-compile --cross-prefix=${TOOLDIR}/bin/${TOOLCHAIN}-`
 - ./cross-cmake.sh
 - uSockets: `CC=${TOOLDIR}/bin/${TOOLCHAIN}-gcc CXX=${TOOLDIR}/bin/${TOOLCHAIN}-g++ PREFIX=${TOOLDIR}/${TOOLCHAIN}/ CXXFLAGS=-I${TOOLDIR}/${TOOLCHAIN}/sysroot/usr/include/ LDFLAGS=-L${TOOLDIR}/${TOOLCHAIN}/sysroot/usr/lib make`

Notes:
 - Ensure your video is already in desired size
   - From testing, h264 decode for 256x64 in software takes about ~5ms. 480x360 requires about 33ms
   - In combination with 4.5ms copy time, this means <10ms, allowing for up to 100fps
