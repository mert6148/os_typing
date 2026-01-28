---
# README — Ansible Automation for OS Typing Hardening

## Overview

This directory contains Ansible roles and playbooks for automated OS hardening of the **OS Typing** project. Supports both **Linux** (Debian/Ubuntu/RedHat) and **Windows** environments.

## Structure

```
├── site.yml                          # Main playbook
├── inventory/
│   └── hosts.ini                     # Host inventory (edit with your targets)
├── roles/
│   └── os_hardening/                 # Main hardening role
│       ├── tasks/
│       │   ├── main.yml              # Entry point (dispatches OS-specific tasks)
│       │   ├── linux.yml             # Linux-specific hardening
│       │   └── windows.yml           # Windows-specific hardening
│       ├── handlers/main.yml         # Service restart, sysctl reload handlers
│       ├── defaults/main.yml         # Default variables (can be overridden)
│       ├── templates/
│       │   └── os_typing.service.j2  # systemd unit template (Linux)
│       └── vars/main.yml             # Role-specific variables
└── ansible.cfg                       # Ansible configuration (optional)
```

## Quick Start

### Prerequisites

1. **Install Ansible** (Python 3.9+):
   ```bash
   pip install ansible
   ```

2. **SSH access** to target Linux hosts or **WinRM** for Windows hosts

3. **Sudo/admin privileges** on target systems

### Configuration

1. **Edit inventory** (`inventory/hosts.ini`):
   ```ini
   [linux]
   server1 ansible_host=192.168.1.10 ansible_user=ubuntu
   server2 ansible_host=192.168.1.11 ansible_user=ubuntu

   [windows]
   win_server1 ansible_host=192.168.1.20 ansible_user=Administrator
   ```

2. **Set password** (optional, use SSH keys when possible):
   ```bash
   export ANSIBLE_PASSWORD="your_password"
   ```

### Run Playbook

#### Apply hardening to all hosts:
```bash
ansible-playbook -i inventory/hosts.ini site.yml
```

#### Apply only to Linux hosts:
```bash
ansible-playbook -i inventory/hosts.ini site.yml --limit linux
```

#### Apply only to Windows hosts:
```bash
ansible-playbook -i inventory/hosts.ini site.yml --limit windows
```

#### Dry run (check mode):
```bash
ansible-playbook -i inventory/hosts.ini site.yml --check
```

#### Verbose output:
```bash
ansible-playbook -i inventory/hosts.ini site.yml -vvv
```

### What Gets Hardened

**Linux (Debian/Ubuntu/RedHat):**
- Kernel parameters (sysctl): IP forwarding, ICMP settings, logging
- Firewall (UFW): Default deny policy, SSH + service port rules
- Systemd service: User/group separation, resource limits, hardening flags
- Package management: Cache updates

**Windows:**
- Windows Firewall: Enable and configure inbound rules
- SMB v1: Disable (security risk)
- Windows Defender: Enable real-time monitoring

## Variables

Edit `roles/os_hardening/defaults/main.yml` to customize:

```yaml
# Sysctl parameters (Linux)
os_hardening_sysctl_config:
  net.ipv4.ip_forward: 0
  net.ipv4.conf.all.log_martians: 1
  # Add more...

# Firewall ports
os_hardening_firewall_allowed_ports:
  - port: 22
    proto: tcp
    comment: "SSH"
  - port: 12345
    proto: tcp
    comment: "OS Typing service"

# Skip certain components
os_hardening_skip_sysctl: false
os_hardening_skip_firewall: false
os_hardening_skip_service: false
```

## Verification

After running the playbook, verify hardening:

```bash
# Linux: Check sysctl values
sudo sysctl -a | grep 'forwarding\|log_martians'

# Linux: Check firewall
sudo ufw status numbered

# Linux: Check service
sudo systemctl status os_typing

# Windows: Check Firewall
Get-NetFirewallProfile

# Windows: Check SMB v1
Get-WindowsOptionalFeature -Online -FeatureName SMB1Protocol
```

## Ansible Vault (Secrets Management)

For sensitive data (passwords, keys):

```bash
# Create encrypted inventory
ansible-vault create inventory/hosts-secret.yml

# Run playbook with vault
ansible-playbook site.yml --ask-vault-pass

# Or use vault password file
ansible-playbook site.yml --vault-password-file ~/.vault_pass
```

Example vault structure:
```yaml
---
ansible_password: "{{ vault_ansible_password }}"
```

## CI Integration

The playbook can be validated in CI using `ansible-lint`:

```bash
pip install ansible-lint
ansible-lint site.yml
```

## Troubleshooting

### SSH Connection Fails
```bash
# Check connectivity
ansible all -i inventory/hosts.ini -m ping

# Debug SSH
ansible all -i inventory/hosts.ini -m ping -vvv
```

### WinRM Connection Fails (Windows)
```powershell
# Enable WinRM on target (run as Administrator)
Enable-PSRemoting -Force
Set-Item WSMan:\localhost\Client\TrustedHosts -Value "*" -Force
```

### Firewall Blocks Changes
```bash
# Run with limited firewall changes
ansible-playbook site.yml -e os_hardening_skip_firewall=true
```

## Best Practices

1. **Test in dev first** — Use `--check` mode and test VMs
2. **Use SSH keys** — Avoid password authentication
3. **Version control** — Keep inventory and roles in git
4. **Idempotency** — Playbook is safe to run multiple times
5. **Audit changes** — Review ansible output and logs
6. **Backup configs** — Snapshot systems before running

## References

- [Ansible Documentation](https://docs.ansible.com/)
- [Ansible Best Practices](https://docs.ansible.com/ansible/latest/user_guide/playbooks_best_practices.html)
- [Linux Hardening Guide](../DEPLOY.md)
- [Security Notes](../SECURITY.md)

## License

Same as os_typing project.
