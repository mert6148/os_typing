# Security and CI Policy (Summary)

This document summarizes the CI/security checks and recommendations for this repo.

- CI: `CI` workflow runs PHP syntax checks and attempts to compile the C demo on push/PR.
- Security: `Security` workflow runs CodeQL, Semgrep and Dependency Review on pushes and PRs.
- Dependabot updates are configured weekly for GitHub Actions.
- Pre-commit hooks are suggested using `pre-commit` and `detect-secrets`. To initialize locally:

  1. Install pre-commit (e.g., `pip install pre-commit`)
  2. Install hooks: `pre-commit install`
  3. Create an initial baseline for detect-secrets to avoid false-positive failures:
     - Run `detect-secrets scan --update .secrets.baseline` and review the output, or
     - Run `pre-commit run detect-secrets --all-files` and follow prompts to audit findings.

- CI integration:
  - The `CI` workflow now runs `pre-commit run --all-files` which will fail when new secrets are detected.
  - A minimal `.secrets.baseline` has been added to the repo; if you find a false positive, update the baseline locally and commit it.

- TLS / Certificate guidance:
  - For local development you can generate a self-signed certificate using `scripts/generate_cert.ps1` (Windows PowerShell). The script creates PFX and CER/PEM files under `certs/` (which is ignored by git).
  - For production, always use a CA-signed certificate (Let's Encrypt/Certbot or enterprise CA). Do not use self-signed certs in production.
  - Store private keys in a secrets manager or on the server filesystem with strict permissions; rotate certificates before expiry.
  - CI: A scheduled certificate job (`.github/workflows/certs.yml`) now runs weekly and will:
    - Run `scripts/generate_cert.ps1` on Windows to verify generation works in CI.
    - Run `scripts/check_cert_expiry.sh` on Ubuntu to scan `certs/` and fail when certs expire within 30 days.

- OS hardening guidance:
  - Linux:
    - Copy `deploy/linux/sysctl-os_typing.conf` to `/etc/sysctl.d/99-os_typing.conf` and run `sudo sysctl --system`.
    - Use `deploy/systemd/os_typing.service` as a hardened systemd unit template (tune `ExecStart` and `User`).
    - Use `deploy/linux/ufw-setup.sh` to configure UFW defaults (deny incoming, allow outgoing) and open the app port.
    - Run `scripts/os_hardening_check.sh` to perform basic sanity checks.

  - Windows:
    - Run `deploy/windows/hardening.ps1` in an elevated PowerShell session to apply baseline firewall and Defender settings and disable SMBv1.
    - Test changes in a non-production VM before applying widely; some registry changes or services modifications may be environment-specific.

- Operational notes:
  - Hardening scripts are conservative examples and not a one-size-fits-all solution. Validate in staging and keep backups of changed configuration files.
  - Consider converting these steps into Ansible playbooks or PowerShell DSC for repeatable, auditable application across servers.

- Recommendations for maintainers:
  - Enable GitHub secret scanning and code scanning in repository settings.
  - Review CodeQL findings and configure Semgrep rules as needed.
  - Add branch protection rules requiring passing CI and security checks before merging.
