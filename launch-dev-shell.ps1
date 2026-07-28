#Requires -Version 5.1

<#
.SYNOPSIS
Opens an official Windows C++ development environment for MC-LAB-CORE.

.DESCRIPTION
This command locates and delegates to the environment activation supplied by
MSYS2 or Visual Studio. It never recreates vendor environment variables and
never modifies the machine or user PATH.

The new shell starts in the MC-LAB-CORE repository root. Configure, build, test,
and quality commands launched from that shell inherit one coherent toolchain
environment.

.PARAMETER Environment
The development environment to open:

  ucrt64  - MSYS2 UCRT64 with GCC and the UCRT runtime.
  clang64 - MSYS2 CLANG64 with LLVM, LLD, libc++, and the UCRT runtime.
  msvc    - Visual Studio Developer PowerShell.

.PARAMETER Check
Diagnoses the selected vendor environment without opening an interactive shell.

.EXAMPLE
./launch-dev-shell.ps1 --help

.EXAMPLE
./launch-dev-shell.ps1 ucrt64

.EXAMPLE
./launch-dev-shell.ps1 clang64 -Check

.NOTES
Use -WhatIf to preview the shell-opening operation after discovery succeeds.
#>

[CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Low')]
param(
    [Parameter(Position = 0)]
    [ValidateSet(
        'ucrt64',
        'clang64',
        'msvc',
        '--help'
    )]
    [string] $Environment,

    [Parameter()]
    [Alias('h')]
    [switch] $Help,

    [Parameter()]
    [switch] $Check
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Write-MCLabCoreInformation {
    param(
        [Parameter(Mandatory = $true)]
        [AllowEmptyString()]
        [string] $Message
    )

    Write-Information `
        -MessageData $Message `
        -InformationAction Continue
}

function Resolve-MCLabCoreRepositoryRoot {
    # launch-dev-shell.ps1 is the repository-level developer entry point.
    # PSScriptRoot is therefore already the repository root; do not walk to
    # its parent.
    $repositoryRoot = $PSScriptRoot
    $presetEntryPoint = Join-Path $repositoryRoot 'CMakePresets.json'

    if (-not (Test-Path -LiteralPath $presetEntryPoint -PathType Leaf)) {
        throw @"
MC-LAB-CORE repository root could not be determined.

Expected preset entry point:
  $presetEntryPoint

Run the script from an intact MC-LAB-CORE checkout.
"@
    }

    return (Resolve-Path -LiteralPath $repositoryRoot).Path
}

function Resolve-MSys2Launcher {
    $candidateRoots = [System.Collections.Generic.List[string]]::new()

    foreach ($variableName in @('MC_LAB_CORE_MSYS2_ROOT', 'MSYS2_ROOT')) {
        $variableValue = [Environment]::GetEnvironmentVariable($variableName)

        if (-not [string]::IsNullOrWhiteSpace($variableValue)) {
            $candidateRoots.Add($variableValue)
        }
    }

    $launcherFromPath = Get-Command 'msys2_shell.cmd' -ErrorAction SilentlyContinue

    if ($null -ne $launcherFromPath) {
        $candidateRoots.Add((Split-Path -Parent $launcherFromPath.Source))
    }

    if (-not [string]::IsNullOrWhiteSpace($env:SystemDrive)) {
        $candidateRoots.Add((Join-Path $env:SystemDrive 'msys64'))
        $candidateRoots.Add((Join-Path $env:SystemDrive 'tools\msys64'))
    }

    foreach ($candidateRoot in ($candidateRoots | Select-Object -Unique)) {
        $launcher = Join-Path $candidateRoot 'msys2_shell.cmd'

        if (Test-Path -LiteralPath $launcher -PathType Leaf) {
            return (Resolve-Path -LiteralPath $launcher).Path
        }
    }

    throw @"
MSYS2 was not found.

Install MSYS2 from:
  https://www.msys2.org/

The standard installation path is C:\msys64. For another location, set:
  MC_LAB_CORE_MSYS2_ROOT=<installation-directory>

This script does not install software or modify PATH automatically.
"@
}

function Get-MSys2EnvironmentStatus {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet('ucrt64', 'clang64')]
        [string] $Name,

        [Parameter(Mandatory = $true)]
        [string] $Launcher
    )

    $msys2Root = Split-Path -Parent $Launcher
    $environmentRoot = Join-Path $msys2Root $Name
    $binaryDirectory = Join-Path $environmentRoot 'bin'

    $requirements = if ($Name -eq 'ucrt64') {
        @(
            [pscustomobject]@{
                Program = 'g++.exe'
                Package = 'mingw-w64-ucrt-x86_64-toolchain'
            }
            [pscustomobject]@{
                Program = 'cmake.exe'
                Package = 'mingw-w64-ucrt-x86_64-cmake'
            }
            [pscustomobject]@{
                Program = 'ninja.exe'
                Package = 'mingw-w64-ucrt-x86_64-ninja'
            }
            [pscustomobject]@{
                Program = 'clang-format.exe'
                Package = 'mingw-w64-ucrt-x86_64-clang'
            }
        )
    } else {
        @(
            [pscustomobject]@{
                Program = 'clang++.exe'
                Package = 'mingw-w64-clang-x86_64-toolchain'
            }
            [pscustomobject]@{
                Program = 'cmake.exe'
                Package = 'mingw-w64-clang-x86_64-cmake'
            }
            [pscustomobject]@{
                Program = 'ninja.exe'
                Package = 'mingw-w64-clang-x86_64-ninja'
            }
            [pscustomobject]@{
                Program = 'clang-format.exe'
                Package = 'mingw-w64-clang-x86_64-clang'
            }
        )
    }

    $missingRequirements = @(
        foreach ($requirement in $requirements) {
            $programPath = Join-Path $binaryDirectory $requirement.Program

            if (-not (Test-Path -LiteralPath $programPath -PathType Leaf)) {
                $requirement
            }
        }
    )

    $gitPath = Join-Path $msys2Root 'usr\bin\git.exe'

    if (-not (Test-Path -LiteralPath $gitPath -PathType Leaf)) {
        $missingRequirements += [pscustomobject]@{
            Program = 'git.exe'
            Package = 'git'
        }
    }

    $missingPackages = @(
        $missingRequirements |
            Select-Object -ExpandProperty Package -Unique
    )

    return [pscustomobject]@{
        BinaryDirectory = $binaryDirectory
        Complete = ($missingRequirements.Count -eq 0)
        MissingPrograms = @(
            $missingRequirements |
                Select-Object -ExpandProperty Program
        )
        MissingPackages = $missingPackages
        InstallCommand = if ($missingPackages.Count -gt 0) {
            'pacman -S --needed ' + ($missingPackages -join ' ')
        } else {
            $null
        }
    }
}

function Get-VisualStudioInstallationGuidance {
    return @"
Open Visual Studio Installer and install or modify an instance with:
  - Desktop development with C++
  - MSVC x64/x86 build tools
  - Windows SDK
  - C++ CMake tools for Windows
  - C++ Clang tools for Windows (required by clang-cl presets)

Visual Studio Installer can be found at:
  https://visualstudio.microsoft.com/downloads/
"@
}

function Resolve-VisualStudioDeveloperShell {
    $installationGuidance = Get-VisualStudioInstallationGuidance
    $programFilesX86 = [Environment]::GetEnvironmentVariable(
        'ProgramFiles(x86)'
    )

    if ([string]::IsNullOrWhiteSpace($programFilesX86)) {
        throw @"
The ProgramFiles(x86) environment variable was not found in this shell.

$installationGuidance
"@
    }

    $vswhere = Join-Path $programFilesX86 `
        'Microsoft Visual Studio\Installer\vswhere.exe'

    if (-not (Test-Path -LiteralPath $vswhere -PathType Leaf)) {
        throw @"
Visual Studio instance discovery is unavailable.

Expected:
  $vswhere

$installationGuidance

Visual Studio Installer supplies vswhere automatically.
"@
    }

    $instanceJson = & $vswhere `
        -latest `
        -prerelease `
        -products '*' `
        -requires 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' `
        -format json

    $instanceJsonText = $instanceJson -join [Environment]::NewLine

    if (
        $LASTEXITCODE -ne 0 `
        -or [string]::IsNullOrWhiteSpace($instanceJsonText)
    ) {
        throw @"
No Visual Studio installation with the Microsoft C++ x86/x64 build tools was
found.

$installationGuidance
"@
    }

    $instance = @($instanceJsonText | ConvertFrom-Json) |
        Select-Object -First 1

    if (
        $null -eq $instance `
        -or [string]::IsNullOrWhiteSpace($instance.instanceId) `
        -or [string]::IsNullOrWhiteSpace($instance.installationPath)
    ) {
        throw 'Visual Studio instance metadata is incomplete.'
    }

    $developerShell = Join-Path $instance.installationPath `
        'Common7\Tools\Launch-VsDevShell.ps1'

    if (-not (Test-Path -LiteralPath $developerShell -PathType Leaf)) {
        throw @"
The Visual Studio Developer PowerShell launcher is missing.

Expected:
  $developerShell

$installationGuidance

Repair the selected Visual Studio installation.
"@
    }

    return [pscustomobject]@{
        DeveloperShell = (Resolve-Path -LiteralPath $developerShell).Path
        DisplayName = $instance.displayName
        InstanceId = $instance.instanceId
        InstallationPath = $instance.installationPath
        InstallationVersion = $instance.installationVersion
    }
}

function Start-MSys2Environment {
    [CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Low')]
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet('ucrt64', 'clang64')]
        [string] $Name,

        [Parameter(Mandatory = $true)]
        [string] $RepositoryRoot,

        [Parameter()]
        [switch] $CheckOnly
    )

    $launcher = Resolve-MSys2Launcher
    $environmentStatus = Get-MSys2EnvironmentStatus `
        -Name $Name `
        -Launcher $launcher
    $displayName = "MSYS2 $($Name.ToUpperInvariant())"
    $arguments = @(
        '-defterm'
        '-here'
        '-no-start'
        "-$Name"
    )

    Write-MCLabCoreInformation 'MC-LAB-CORE development environment'
    Write-MCLabCoreInformation "  Environment : $displayName"
    Write-MCLabCoreInformation "  Launcher    : $launcher"
    Write-MCLabCoreInformation "  Repository  : $RepositoryRoot"

    if (-not $environmentStatus.Complete) {
        Write-Warning "$displayName is installed but its MC-LAB-CORE build environment is incomplete."
        Write-MCLabCoreInformation (
            '  Missing     : ' +
            ($environmentStatus.MissingPrograms -join ', ')
        )
        Write-MCLabCoreInformation ''
        Write-MCLabCoreInformation 'After the new shell opens, install the missing components:'
        Write-MCLabCoreInformation "  $($environmentStatus.InstallCommand)"
        Write-MCLabCoreInformation ''
        Write-MCLabCoreInformation 'Then verify:'
        Write-MCLabCoreInformation '  cmake --version'
        Write-MCLabCoreInformation '  ninja --version'
        Write-MCLabCoreInformation ''
    } else {
        Write-MCLabCoreInformation '  Build tools : ready'
    }

    $presetName = if ($Name -eq 'ucrt64') {
        'windows-ucrt64-gcc-debug'
    } else {
        'windows-clang64-debug'
    }

    Write-MCLabCoreInformation ''
    Write-MCLabCoreInformation 'After the development shell opens, configure, build, and test with:'
    Write-MCLabCoreInformation "  cmake --workflow --preset $presetName --fresh"

    if ($CheckOnly) {
        Write-MCLabCoreInformation ''
        Write-MCLabCoreInformation 'Check completed. No shell was opened.'
        Write-MCLabCoreInformation 'To open it, run:'
        Write-MCLabCoreInformation "  .\launch-dev-shell.ps1 $Name"
        return
    }

    if ($PSCmdlet.ShouldProcess($displayName, 'Open interactive shell')) {
        Start-Process `
            -FilePath $launcher `
            -ArgumentList $arguments `
            -WorkingDirectory $RepositoryRoot
    }
}

function Start-VisualStudioEnvironment {
    [CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Low')]
    param(
        [Parameter(Mandatory = $true)]
        [string] $RepositoryRoot,

        [Parameter()]
        [switch] $CheckOnly
    )

    $visualStudio = Resolve-VisualStudioDeveloperShell
    $developerShell = $visualStudio.DeveloperShell
    # MC-LAB-CORE intentionally supports native Windows x86-64 only.
    $targetArchitecture = 'amd64'
    $hostArchitecture = 'amd64'

    $visualStudioMajor = [int](
    $visualStudio.InstallationVersion.Split('.')[0]
    )

    $visualStudioYear = switch ($visualStudioMajor) {
        17 { '2022' }
        18 { '2026' }
        default { $null }
    }

    $recommendedWorkflow = if ($null -ne $visualStudioYear) {
        "windows-visualstudio-$visualStudioYear-debug"
    }
    else {
        'windows-msvc-debug'
    }

    $powerShellExecutable = (Get-Process -Id $PID).Path
    $escapedDeveloperShell = $developerShell.Replace("'", "''")
    $escapedInstanceId = $visualStudio.InstanceId.Replace("'", "''")
    $escapedRepositoryRoot = $RepositoryRoot.Replace("'", "''")

    $startupCommand = @"
& '$escapedDeveloperShell' -VsInstanceId '$escapedInstanceId' -Arch $targetArchitecture -HostArch $hostArchitecture -SkipAutomaticLocation
if (-not `$?) {
    throw 'Visual Studio Developer PowerShell initialization failed.'
}

Set-Location -LiteralPath '$escapedRepositoryRoot'

Write-Information '' -InformationAction Continue
Write-Information 'MC-LAB-CORE Visual Studio developer environment is ready.' -InformationAction Continue
Write-Information '' -InformationAction Continue
Write-Information 'Detected Visual Studio build environment:' -InformationAction Continue
Write-Information '  Generator : Visual Studio $visualStudioMajor $visualStudioYear' -InformationAction Continue
Write-Information '' -InformationAction Continue
Write-Information 'Recommended configure, build, and test workflow:' -InformationAction Continue
Write-Information '  cmake --workflow --preset $recommendedWorkflow --fresh' -InformationAction Continue
Write-Information '' -InformationAction Continue
Write-Information 'To discover clang-cl and quality alternatives:' -InformationAction Continue
Write-Information '  cmake --list-presets=configure' -InformationAction Continue
"@

    $encodedCommand = [Convert]::ToBase64String(
        [Text.Encoding]::Unicode.GetBytes($startupCommand)
    )

    Write-MCLabCoreInformation 'MC-LAB-CORE development environment'
    Write-MCLabCoreInformation (
        "  Environment : $($visualStudio.DisplayName) Developer PowerShell"
    )
    Write-MCLabCoreInformation (
        "  Version     : $($visualStudio.InstallationVersion)"
    )
    Write-MCLabCoreInformation (
        "  Generator   : Visual Studio $visualStudioMajor $visualStudioYear"
    )
    Write-MCLabCoreInformation (
        "  Instance    : $($visualStudio.InstanceId)"
    )
    Write-MCLabCoreInformation '  Target      : x64'
    Write-MCLabCoreInformation "  Launcher    : $developerShell"
    Write-MCLabCoreInformation "  Repository  : $RepositoryRoot"

    if ($CheckOnly) {
        Write-MCLabCoreInformation ''
        Write-MCLabCoreInformation (
            'Recommended configure, build, and test workflow:'
        )
        Write-MCLabCoreInformation (
            "  cmake --workflow --preset $recommendedWorkflow --fresh"
        )
        Write-MCLabCoreInformation ''
        Write-MCLabCoreInformation (
            'Check completed. No shell was opened.'
        )
        Write-MCLabCoreInformation 'To open it, run:'
        Write-MCLabCoreInformation (
            '  .\launch-dev-shell.ps1 msvc'
        )
        return
    }

    if (
        $PSCmdlet.ShouldProcess(
            'Visual Studio Developer PowerShell',
            'Open interactive shell'
        )
    ) {
        Start-Process `
            -FilePath $powerShellExecutable `
            -ArgumentList @(
                '-NoExit'
                '-NoLogo'
                '-EncodedCommand'
                $encodedCommand
            ) `
            -WorkingDirectory $RepositoryRoot
    }
}

if ($Help -or $Environment -eq '--help') {
    Get-Help `
        -Name $PSCommandPath `
        -Detailed

    return
}

if ([string]::IsNullOrWhiteSpace($Environment)) {
    throw @"
A development environment is required.

Usage:
  ./launch-dev-shell.ps1 <environment> [-Check]

Supported environments:
  ucrt64
  clang64
  msvc

For detailed help:
  ./launch-dev-shell.ps1 --help
"@
}

if ($env:OS -ne 'Windows_NT') {
    throw 'launch-dev-shell.ps1 supports Windows hosts only.'
}

$repositoryRoot = Resolve-MCLabCoreRepositoryRoot

switch ($Environment) {
    'ucrt64' {
        Start-MSys2Environment `
            -Name 'ucrt64' `
            -RepositoryRoot $repositoryRoot `
            -CheckOnly:$Check
    }
    'clang64' {
        Start-MSys2Environment `
            -Name 'clang64' `
            -RepositoryRoot $repositoryRoot `
            -CheckOnly:$Check
    }
    'msvc' {
        Start-VisualStudioEnvironment `
            -RepositoryRoot $repositoryRoot `
            -CheckOnly:$Check
    }
}
