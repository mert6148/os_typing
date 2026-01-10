Param(
    [switch]$Test
)

function Has-Command([string]$name){
    return (Get-Command $name -ErrorAction SilentlyContinue) -ne $null
}

if (Has-Command "gcc") {
    $cmd = "gcc -DOS_CHECKING_TEST -Isrc -o os_checking src\os_checking.c"
    $cmd2 = "gcc -Isrc -o terminal_test src\terminal_test.c src\terminal.c"
} elseif (Has-Command "cl") {
    # MSVC: cl is picky about arguments and current env; assume developer has Developer Command Prompt set up
    $cmd = "cl /nologo /D OS_CHECKING_TEST /I src src\\os_checking.c /link /OUT:os_checking.exe"
    $cmd2 = "cl /nologo /I src src\\terminal_test.c src\\terminal.c /link /OUT:terminal_test.exe"
} else {
    Write-Error "No supported compiler found (gcc or cl). Install MSYS2/MinGW or Visual Studio, or use WSL."
    exit 1
}

Write-Output "Running: $cmd"
Invoke-Expression $cmd

Write-Output "Running: $cmd2"
Invoke-Expression $cmd2
