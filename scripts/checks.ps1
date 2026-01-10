# Statik kontrol ve derleme scripti (Windows PowerShell)
# - PHP sözdizimi denetimi
# - C derleme denemesi (g++ varsa)

Write-Host "Running PHP syntax checks..."
Get-ChildItem -Path . -Include *.php -Recurse | ForEach-Object {
    Write-Host "Checking: $_"
    php -l $_.FullName
}

Write-Host "Attempting to compile C demo (if g++ is available)..."
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address -o os_typing os_typing.c
    if ($LASTEXITCODE -eq 0) { Write-Host "C compile OK: ./os_typing" } else { Write-Host "C compile failed." }
} else {
    Write-Host "g++ not found; skipping C compile check.";
}

Write-Host "Done."