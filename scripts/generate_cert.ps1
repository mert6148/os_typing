<#
PowerShell script to generate a development self-signed certificate for localhost

Usage:
  - Open PowerShell as Administrator (to trust the certificate system-wide) or without admin if you just want files.
  - ./scripts/generate_cert.ps1 -DnsName "localhost" -OutDir "certs" -ValidityDays 365

Notes:
  - This is intended for local development only. Do NOT use self-signed certificates in production.
  - The script exports a PFX and PEM files to the target folder.
#>
param(
    [string]$DnsName = "localhost",
    [string]$OutDir = "certs",
    [int]$ValidityDays = 365
)

if (-not (Test-Path $OutDir)) { New-Item -Path $OutDir -ItemType Directory | Out-Null }

$cert = New-SelfSignedCertificate -DnsName $DnsName -CertStoreLocation "Cert:\LocalMachine\My" -NotAfter (Get-Date).AddDays($ValidityDays) -FriendlyName "os-typing-dev-$DnsName"

# Export to PFX (passwordless for local development) and PEM files using OpenSSL if present
$pfxPath = Join-Path $OutDir "$DnsName.pfx"
$certPassword = ConvertTo-SecureString -String "" -Force -AsPlainText
Export-PfxCertificate -Cert $cert -FilePath $pfxPath -Password $certPassword | Out-Null

# Export public cert to CER
$cerPath = Join-Path $OutDir "$DnsName.cer"
Export-Certificate -Cert $cert -FilePath $cerPath | Out-Null

# Try to create PEM key + cert if OpenSSL is available
$pemCertPath = Join-Path $OutDir "$DnsName.pem"
$privKeyPath = Join-Path $OutDir "$DnsName.key"

if (Get-Command openssl -ErrorAction SilentlyContinue) {
    # Convert PFX to PEM and split key/cert
    openssl pkcs12 -in $pfxPath -nodes -passin pass:"" -out $pemCertPath
    # Extract private key
    openssl pkcs12 -in $pfxPath -nocerts -nodes -passin pass:"" -out $privKeyPath
    Write-Host "Generated: $pfxPath, $pemCertPath, $privKeyPath"
} else {
    Write-Host "OpenSSL not found; PFX and CER exported: $pfxPath, $cerPath"
}

Write-Host "To trust the certificate locally (Windows): open $cerPath and install to Trusted Root Certification Authorities (Local Machine) if running tests that require it (Admin required)."
Write-Host "Remember: Do NOT commit private keys to source control. The 'certs' folder is added to .gitignore by default."