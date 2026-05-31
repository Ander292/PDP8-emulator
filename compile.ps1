param (
    [string]$mode = "full"
)
# full, run, build

$Assembler = "assembler"
$Emulator = "emulator"

# Directories

$RootDir            = Split-Path        -Parent             $MyInvocation.MyCommand.Path

$AssemblerDir       = Join-Path         $RootDir            $Assembler
$EmulatorDir        = Join-Path         $RootDir            $Emulator
$SharedDir          = Join-Path         $RootDir            "shared"

$AssetsDir          = Join-Path         $RootDir            "assets"

$BuildDir           = Join-Path         $RootDir            "build"
$BinDir             = Join-Path         $BuildDir           "bin"
$ObjDir             = Join-Path         $BuildDir           "obj"

$OutputExeA         = Join-Path         $BinDir             "$Assembler.exe"
$OutputExeE         = Join-Path         $BinDir             "$Emulator.exe"

# Creating general folders
New-Item -Type Directory -Force -Path   $BuildDir           | Out-Null
New-Item -Type Directory -Force -Path   $BinDir             | Out-Null
New-Item -Type Directory -Force -Path   $ObjDir             | Out-Null
New-Item -Type Directory -Force -Path   $AssemblerDir       | Out-Null
New-Item -Type Directory -Force -Path   $EmulatorDir        | Out-Null
New-Item -Type Directory -Force -Path   $AssetsDir          | Out-Null



if( ( $mode -eq "full" ) -OR ( $mode -eq "buildAssembler" ) -OR ( $mode -eq "build" ) )
{
    # Assembler
    $SrcDir             = Join-Path         $AssemblerDir       "src"
    $IncludeDir         = Join-Path         $AssemblerDir       "include"
    #$ResourceDir        = Join-Path         $AssemblerDir       "resources"

    $ObjLocal           = Join-Path         $ObjDir             $Assembler
    #$ResourcesObj       = Join-Path         $ObjDir             "resource.obj"

    

    # Creating platform folders if they are missing
    New-Item -Type Directory -Force -Path   $SrcDir             | Out-Null
    #New-Item -Type Directory -Force -Path   $ResourceDir        | Out-Null
    New-Item -Type Directory -Force -Path   $ObjLocal           | Out-Null

    [array]$Sources     = Get-ChildItem     "$SrcDir\*.c"       | ForEach-Object { $_.FullName }
    [array]$ObjC        = Get-ChildItem     "$SrcDir\*.c"       | ForEach-Object { $_.BaseName + ".obj" }

    #[array]$Resources   = Get-ChildItem     "$ResourceDir\*.rc" | ForEach-Object { $_.FullName }

    $IncludeFlags = @(
        "-I$IncludeDir"
        "-I$SharedDir"
    )

    #Flags for special libraries (if there are any)
    $LibFlags = @(
    )

    $CompilationFlags = @(
        #"-Wall"
        #"-pedantic"
        "-Wextra"
        #"-Wconversion"
        #"-Wundef"
        #"-Wstrict-overflow=5"
        "-fdiagnostics-show-option"
        "-g"
        #"-O"
    )

    $LinkerFlags = @(
        "-pedantic"
        "-g"
        #"-Wl,-Map=outputP.map"
        #"-m64"
        #"-mwindows"
    )

    # Compiling c code into obj file
    $i = 0
    $Sources.ForEach({
        $CurrentObj     = Join-Path         $ObjLocal           $ObjC[$i]

        gcc `
            $IncludeFlags `
            $_ -c `
            $CompilationFlags `
            -o $CurrentObj
        
        $i++
        if ($LASTEXITCODE -ne 0) {Write-Error "Error compiling $_";$ErrorCode = 1}
    })

    # # Compiling resources into a windows resource file:
    # Write-Host "Preparing resources..."
    # windres `
    #     $Resources `
    #     -O coff `
    #     -o $ResourcesObj

    [array]$Objs        = Get-ChildItem     "$ObjLocal\*.obj"   | ForEach-Object { $_.FullName }

    # Linking stage
    Write-Host "Linking..."
    gcc `
        $Objs `
        $IncludeFlags `
        $LibFlags `
        $LinkerFlags `
        -o $OutputExeA

    if ( ($LASTEXITCODE -ne 0) -OR ($ErrorCode -eq 1) ) {
        Write-Error "Assembler compilation failed!"
        exit(1)
    }

    Write-Host "Assembler compilation sucessful!"
}

if( ( $mode -eq "full" ) -OR ( $mode -eq "buildEmulator" ) -OR ( $mode -eq "build" ) )
{
    # Emulator layer:
    $SrcDir             = Join-Path         $EmulatorDir        "src"
    $IncludeDir         = Join-Path         $EmulatorDir        "include"
    #$ResourceDir        = Join-Path         $EmulatorDir        "resources"

    $ObjLocal           = Join-Path         $ObjDir             $Emulator



    [array]$Sources     = Get-ChildItem     "$SrcDir\*.c"       | ForEach-Object { $_.FullName }
    [array]$ObjC        = Get-ChildItem     "$SrcDir\*.c"       | ForEach-Object { $_.BaseName + ".obj" }


    # Creating platform folders if they are missing
    New-Item -Type Directory -Force -Path   $SrcDir             | Out-Null
    New-Item -Type Directory -Force -Path   $IncludeDir         | Out-Null
    New-Item -Type Directory -Force -Path   $SharedDir          | Out-Null
    #New-Item -Type Directory -Force -Path   $ResourceDir        | Out-Null

    New-Item -Type Directory -Force -Path   $ObjLocal           | Out-Null

    $IncludeFlags = @( #Flags that are included based on which libraries are there
        "-I$IncludeDir"
        "-I$SharedDir"
    )

    #Flags for special libraries (if there are any). Usually just dependencies for some windows functions
    $LibFlags = @(
    )

    $CompilationFlags = @(
        #"-Wall"
        #"-pedantic"
        "-Wextra"
        #"-Wconversion"
        #"-Wundef"
        #"-Wstrict-overflow=5"
        "-fdiagnostics-show-option"
        "-g"
        #"-O2"
    )

    $LinkerFlags = @(
        "-pedantic"
        "-g"
        #"-shared"
        #"-Wl,-Map=outputG.map"
        #"-Wl,--out-implib,libmy_lib.dll.a"
        #"-m64"
        #"-mwindows"
    )

    $ErrorCode = 0 # Will determine if linking is done

    # Compiling c code into obj file
    $i = 0
    $Sources.ForEach({
        $CurrentObj     = Join-Path         $ObjLocal           $ObjC[$i]

        gcc `
            $IncludeFlags `
            $_ -c `
            $CompilationFlags `
            -o $CurrentObj
        
        $i++
        if ($LASTEXITCODE -ne 0) {Write-Error "Error compiling $_";$ErrorCode = 1}
    })

    [array]$Objs        = Get-ChildItem     "$ObjLocal\*.obj"   | ForEach-Object { $_.FullName }

    # Linking stage
    Write-Host "Linking..."
    gcc `
        $Objs `
        $IncludeFlags `
        $LibFlags `
        $LinkerFlags `
        -o $OutputExeE

    if ( ($LASTEXITCODE -ne 0) -OR ($ErrorCode -eq 1) ) {
        Write-Error "Emulator compilation failed!"
        exit(1)
    }

    Write-Host "Emulator compilation sucessful!"
}

if ( ( $mode -eq "full" ) -OR ( $mode -eq "reloadAssets" ) ) {
    Write-Host "Copying assets..."
    $TargetAssetsDir = Join-Path $BinDir "assets"
    Remove-Item $TargetAssetsDir -Recurse -Force -ErrorAction SilentlyContinue
    Copy-Item $AssetsDir $TargetAssetsDir -Recurse -Force
}

if ( ( $mode -eq "full" ) -OR ( $mode -eq "runAssembler" ) ) {
    Write-Host "Running program..."
    & "$OutputExeA" "assets/in.txt"
}

if ( ( $mode -eq "full" ) -OR ( $mode -eq "runEmulator" ) ) {
    Write-Host "Running program..."
    & "$OutputExeE" "assets/out.bin" 200
}