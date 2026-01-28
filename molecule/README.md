---
# Molecule README - Testing Ansible Roles with Molecule

## Overview

This directory contains **Molecule** test scenarios for the **os_hardening** Ansible role.

Molecule is a testing framework that:
- Creates isolated test instances (Docker containers)
- Applies roles and verifies behavior
- Tests idempotency (running twice should be safe)
- Integrates with Testinfra for infrastructure testing
- Provides local and CI/CD testing capabilities

## Prerequisites

### Local Testing

```bash
# Install Molecule and dependencies
pip install molecule molecule-docker ansible testinfra
```

### Docker

- Docker must be installed and running (for local container-based testing)

### For CI

- GitHub Actions with Ubuntu runners (has Docker pre-installed)

## Directory Structure

```
molecule/
├── default/                    # Default test scenario
│   ├── molecule.yml           # Molecule configuration
│   ├── prepare.yml            # Test environment setup
│   ├── converge.yml           # Role application playbook
│   ├── verify.yml             # Post-deployment verification
│   └── tests/
│       └── test_default.py    # Testinfra assertions
└── README.md                   # This file
```

## Quick Start

### Run All Molecule Tests (Local)

```bash
cd roles/os_hardening
molecule test
```

### Run Specific Steps

```bash
# Create test containers
molecule create

# Apply role
molecule converge

# Verify idempotency (apply role twice)
molecule idempotence

# Run tests
molecule verify

# Cleanup
molecule destroy
```

### Run Tests with Specific Scenario

```bash
# Default scenario (Ubuntu 22.04 + Debian 11)
molecule test -s default
```

### Debug Tests

```bash
# Run with verbose output
molecule test -vvv

# Login to container for manual inspection
molecule login -h ubuntu-22.04
```

## Test Scenarios

### Default Scenario

Tests the os_hardening role on:
- **Ubuntu 22.04 LTS** — Modern Ubuntu
- **Debian 11** — Stable Debian

**What is tested:**
1. **Sysctl parameters** — Kernel hardening settings (IP forwarding, redirects, logging)
2. **UFW firewall** — Firewall installation and enablement
3. **Idempotency** — Role can be applied multiple times safely
4. **Connectivity** — Network interfaces and SSH access

## Test Assertions (Testinfra)

All tests are defined in `tests/test_default.py` using Testinfra:

### Sysctl Parameters

```python
def test_sysctl_ip_forward(host):
    """Verify IP forwarding disabled"""
    assert host.sysctl("net.ipv4.ip_forward") == 0
```

### Firewall Configuration

```python
def test_ufw_enabled(host):
    """Verify UFW is enabled"""
    svc = host.service("ufw")
    assert svc.is_enabled
```

### Network Configuration

```python
def test_hostname_configured(host):
    """Verify hostname is set"""
    hostname = host.check_output("hostname")
    assert hostname
```

### Idempotency

```python
def test_idempotent_sysctl(host):
    """Verify sysctl settings are idempotent"""
    result = host.run("sysctl -a | grep ip_forward")
    assert result.rc == 0
```

## Customization

### Adding New Test Cases

Edit `molecule/default/tests/test_default.py`:

```python
def test_my_hardening_check(host):
    """Verify my hardening setting"""
    # Check that something is configured correctly
    assert host.sysctl("some.parameter") == expected_value
```

### Changing Test Instances

Edit `molecule/default/molecule.yml` to add/modify platforms:

```yaml
platforms:
  - name: ubuntu-20.04
    image: docker.io/geerlingguy/docker-ubuntu2004-ansible:latest
  - name: centos-8
    image: docker.io/geerlingguy/docker-centos8-ansible:latest
```

### Modifying Role Variables

Edit `molecule/default/converge.yml` to override role variables:

```yaml
vars:
  os_hardening_skip_firewall: true
  os_hardening_sysctl_config:
    net.ipv4.ip_forward: 1  # Override default
```

## CI Integration

Molecule is automatically run in GitHub Actions on every push:

```yaml
- name: Test Ansible role with Molecule
  run: |
    pip install molecule molecule-docker ansible testinfra
    cd roles/os_hardening
    molecule test
```

## Troubleshooting

### Docker Connection Issues

```bash
# Ensure Docker is running
sudo service docker start

# Check Docker daemon
docker ps
```

### Testinfra Import Errors

```bash
# Reinstall Testinfra
pip install --upgrade testinfra
```

### Container Fails to Start

```bash
# Check container logs
docker logs <container_id>

# Rebuild image
molecule create --force-create
```

### Molecule Cache Issues

```bash
# Clean Molecule state
molecule reset

# Or remove .molecule directory
rm -rf .molecule
```

## Best Practices

1. **Run locally before CI** — Test changes locally with `molecule test`
2. **Add tests for new checks** — Each hardening feature should have a test
3. **Test on multiple distros** — Add test scenarios for different OSes
4. **Verify idempotency** — Ensure role can be applied multiple times
5. **Use skip conditions** — Skip tests for unavailable features (e.g., Windows-specific)
6. **Keep tests simple** — One assertion per test when possible

## Integration with Ansible Role

The role is tested as-is from `roles/os_hardening/`:

```
role structure:
roles/os_hardening/
├── tasks/
├── handlers/
├── defaults/
├── templates/
└── vars/

molecule/default/ tests this role
```

## Performance Tips

- Use `molecule converge` for quick iteration (skip destroy/create)
- Run `molecule verify` alone to re-run tests without role reapplication
- Use `--parallel` flag for multiple scenarios (requires Python 3.7+)

## References

- [Molecule Documentation](https://molecule.readthedocs.io/)
- [Testinfra Documentation](https://testinfra.readthedocs.io/)
- [Ansible Testing Best Practices](https://docs.ansible.com/ansible/latest/community/contributing_maintained.html)

## License

Same as os_typing project.
