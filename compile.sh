compile() {
    local CodeDir="$1"
    local Name="$2"

    local SrcDir="$CodeDir/src"
    local IncludeDir="$CodeDir/include"
    
    local ObjLocal="$ObjDir/$Name"

    Sources=( * )
}

mode="$full"
#full, run, build

if [ $# -ne 1 ]; then
    echo "Usage: `basename $0` <mode>"
    exit 1
else
    mode=$1
fi

Assembler="assembler"
Emulator="emulator"

RootDir=`pwd`

AssemblerDir="$RootDir/$Assembler"
EmulatorDir="$RootDir/$Emulator"
SharedDir="$RootDir/shared"

AssetsDir="$RootDir/assets"

BuildDir="$RootDir/build"
BinDir="$RootDir/bin"
ObjDir="$RootDir/obj"

OutputExeA="$RootDir/$Assembler"
OutputExeE="$RootDir/$Emulator"

mkdir "$BuildDir" 2> /dev/null
mkdir "$BinDir" 2> /dev/null
mkdir "$ObjDir" 2> /dev/null
mkdir "$AssemblerDir" 2> /dev/null
mkdir "$EmulatorDir" 2> /dev/null
mkdir "$AssetsDir" 2> /dev/null

case "$mode" in
    "build")
        compile "$AssemblerDir" "$Assembler"
        compile "$EmulatorDir" "$Emulator"
        ;;
    "run")

        ;;
    "full")

        ;;
    "runAssembler")

        ;;
    "runEmulator")

        ;;
esac