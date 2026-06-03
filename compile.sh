compile() {
    
    local CodeDir="$1"
    local Name="$2"

    local SrcDir="$CodeDir/src"
    local IncludeDir="$CodeDir/include"
    local ObjLocal="$ObjDir/$Name"

    local Sources=()
    local ObjC=()

    for file in $SrcDir/*.c;do
        Sources+=(`basename "$file"`)
        ObjC+=(`basename "$file" .c`.obj)
    done

    mkdir $SrcDir 2> /dev/null
    mkdir $IncludeDir 2> /dev/null
    mkdir $ObjLocal 2> /dev/null

    local -n CompFlags="$3"
    local -n LinkFlags="$4"

    #echo ${CompFlags[@]}
    #echo ${LinkFlags[@]}

    i=0
    for file in ${Sources[@]};do
        
    done
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
mkdir "$SharedDir" 2> /dev/null

case "$mode" in
    "build")
        CompilationFlags=(
            #"-Wall"
            #"-pedantic"
            "-Wextra"
            #"-Wconversion"
            #"-Wundef"
            #"-Wstrict-overflow=5"
            "-fdiagnostics-show-option"
            "-g"
            #"-O1"
        )

        LinkerFlags=(
            "-pedantic"
            "-g"
            #"-shared"
            #"-Wl,-Map=outputG.map"
            #"-Wl,--out-implib,libmy_lib.dll.a"
            #"-m64"
            #"-mwindows"
        )
        compile "$AssemblerDir" "$Assembler" "CompilationFlags" "LinkerFlags"

        CompilationFlags=(
            #"-Wall"
            #"-pedantic"
            "-Wextra"
            #"-Wconversion"
            #"-Wundef"
            #"-Wstrict-overflow=5"
            "-fdiagnostics-show-option"
            "-g"
            #"-O1"
        )

        LinkerFlags=(
            "-pedantic"
            "-g"
            #"-shared"
            #"-Wl,-Map=outputG.map"
            #"-Wl,--out-implib,libmy_lib.dll.a"
            #"-m64"
            #"-mwindows"
        )
        compile "$EmulatorDir" "$Emulator" "CompilationFlags" "LinkerFlags"
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