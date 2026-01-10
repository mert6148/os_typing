<# Non-interactive PowerShell wrapper for os_controlsystem (Windows)
   - Attempts to build with cl or g++ if binary missing
   - Returns os_controlsystem's exit code
#>

param(
    [string]$ConfigPath = "tests\hardening_config.json"
)

$here = Split-Path -Parent $MyInvocation.MyCommand.Definition
Push-Location $here\..\.. | Out-Null

$bin = Join-Path $PWD "os_controlsystem.exe"
$src = Join-Path $PWD "scripts\c-c++\os_controlsystem.cpp"

if (-not (Test-Path $bin)) {
    Write-Output "os_controlsystem binary not found, attempting to build..."
    if (Get-Command cl -ErrorAction SilentlyContinue) {
        cl /nologo /EHsc /Fe:os_controlsystem.exe $src
    } elseif (Get-Command g++ -ErrorAction SilentlyContinue) {
        g++ -std=c++17 -O2 -o os_controlsystem.exe $src
    } else {
        Write-Error "No suitable compiler found (cl or g++). Cannot build os_controlsystem."
        exit 2
    }
}

$arg = "--checks all --config $ConfigPath"
Write-Output "Running os_controlsystem $arg"
$proc = Start-Process -FilePath $bin -ArgumentList $arg -NoNewWindow -Wait -PassThru
exit $proc.ExitCode
