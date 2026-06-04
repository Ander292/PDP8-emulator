compile() {
    
    local CodeDir="$1"
    local Name="$2"

    local SrcDir="$CodeDir/src"
    local IncludeDir="$CodeDir/include"
    local ObjLocal="$ObjDir/$Name"

    local Sources=()
    local Objs=()

    for file in $SrcDir/*.c;do
        Sources+=(`basename "$file"`)
        #ObjC+=(`basename "$file" .c`.obj)
    done

    mkdir $SrcDir 2> /dev/null
    mkdir $IncludeDir 2> /dev/null
    mkdir $ObjLocal 2> /dev/null

    local -n CompFlags="$3"
    local -n LinkFlags="$4"
    local IncFlags=(
        "-I$IncludeDir"
        "-I$SharedDir"
    )

    #echo $CodeDir
    #echo $Name
    #echo $SrcDir
    #echo $IncludeDir
    #echo $ObjLocal
    #echo ${Sources[@]}
    #echo ${IncFlags[@]}

    #echo ${Sources[@]}
    #echo ${CompFlags[@]}
    #echo ${LinkFlags[@]}


    #compiling
    for file in ${Sources[@]};do
        local CurrentObj=`basename "$file" .c`.obj
        local args="$SrcDir/$file -c ${IncFlags[@]} $CompilationFlags -o $ObjLocal/$CurrentObj"
        #echo "$args"
        gcc $args
    done

    #linking
    for file in "$ObjLocal/*.obj";do
        Objs+=( $file )
    done


    echo "Linking"
    gcc \
        "${Objs[@]}" \
        "${IncFlags[@]}" \
        "$LinkFlags" \
        -o "$BinDir/$Name"
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
BinDir="$BuildDir/bin"
ObjDir="$BuildDir/obj"

OutputExeA="$RootDir/$Assembler"
OutputExeE="$RootDir/$Emulator"

#echo $AssemblerDir
#echo $EmulatorDir
#echo $SharedDir
#echo $AssetsDir
#echo $BuildDir
#echo $BinDir
#echo $ObjDir
#echo $OutputExeA
#echo $OutputExeE

mkdir "$BuildDir" 2> /dev/null
mkdir "$BinDir" 2> /dev/null
mkdir "$ObjDir" 2> /dev/null
mkdir "$AssemblerDir" 2> /dev/null
mkdir "$EmulatorDir" 2> /dev/null
mkdir "$AssetsDir" 2> /dev/null
mkdir "$SharedDir" 2> /dev/null

CompilationFlagsA=(
    #"-Wall"
    #"-pedantic"
    "-Wextra"
    #"-Wconversion"
    #"-Wundef"
    #"-Wstrict-overflow=5"
    #"-fdiagnostics-show-option"
    "-g"
    #"-O1"
)

LinkerFlagsA=(
    "-pedantic"
    "-g"
    #"-shared"
    #"-Wl,-Map=outputG.map"
    #"-Wl,--out-implib,libmy_lib.dll.a"
    #"-m64"
    #"-mwindows"
)


CompilationFlagsE=(
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

LinkerFlagsE=(
    "-pedantic"
    "-g"
    #"-shared"
    #"-Wl,-Map=outputG.map"
    #"-Wl,--out-implib,libmy_lib.dll.a"
    #"-m64"
    #"-mwindows"
)

case "$mode" in
    "build")
        compile "$AssemblerDir" "$Assembler" "CompilationFlagsA" "LinkerFlagsA"
        compile "$EmulatorDir" "$Emulator" "CompilationFlagsE" "LinkerFlagsE"
        ;;
    "buildAssembler")
        compile "$AssemblerDir" "$Assembler" "CompilationFlagsA" "LinkerFlagsA"
        ;;
    "buildEmulator")
        compile "$EmulatorDir" "$Emulator" "CompilationFlagsE" "LinkerFlagsE"
        ;;    
    "run")
        # Not done yet
        ;;
    "full")
        compile "$AssemblerDir" "$Assembler" "CompilationFlagsA" "LinkerFlagsA"
        compile "$EmulatorDir" "$Emulator" "CompilationFlagsE" "LinkerFlagsE"
        ;;
    "runAssembler")
        #Not done yet
        "$BinDir/$Assembler"
        ;;
    "runEmulator")
        #Not done yet
        "$BinDir/$Emulator"
        ;;
esac