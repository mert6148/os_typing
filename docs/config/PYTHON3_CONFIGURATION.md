# Python3 Configuration Guide

## Virtual Environment Setup

```bash
# Create virtual environment
python3 -m venv venv

# Activate (Linux/macOS)
source venv/bin/activate

# Activate (Windows)
.\venv\Scripts\activate

# Verify Python version
python --version  # Should be 3.8+
```

## Dependencies Installation

```bash
# Install from requirements.txt
pip install -r requirements.txt

# Install specific package
pip install pytest==7.4.0

# Install with extras
pip install pytest[cov]

# List installed packages
pip list

# Generate requirements from current environment
pip freeze > requirements.txt
```

## Project Structure

```
os_typing/
├── venv/                    # Virtual environment
├── src/                     # Source code
│   ├── __init__.py
│   ├── core/
│   │   ├── __init__.py
│   │   ├── os_detector.py
│   │   └── hardening.py
│   ├── utils/
│   │   ├── __init__.py
│   │   └── helpers.py
│   └── config.py
├── tests/
│   ├── __init__.py
│   ├── unit/
│   │   ├── test_os_detector.py
│   │   └── test_hardening.py
│   ├── integration/
│   │   └── test_integration.py
│   └── fixtures/
│       └── conftest.py
├── requirements.txt
├── pytest.ini
├── setup.py
└── README.md
```

## pytest Configuration

### pytest.ini Example

```ini
[pytest]
testpaths = tests
python_files = test_*.py
python_classes = Test*
python_functions = test_*
addopts = 
    -v
    --strict-markers
    --tb=short
    --disable-warnings
markers =
    unit: Unit tests
    integration: Integration tests
    slow: Slow running tests
    windows: Windows-specific tests
    linux: Linux-specific tests
```

## Running Tests

```bash
# Run all tests
pytest

# Run with verbose output
pytest -v

# Run with coverage report
pytest --cov=src --cov-report=html

# Run specific test file
pytest tests/unit/test_os_detector.py

# Run specific test function
pytest tests/unit/test_os_detector.py::test_detect_linux

# Run tests with specific marker
pytest -m unit

# Run tests in parallel (requires pytest-xdist)
pytest -n auto

# Run tests and show print statements
pytest -s

# Run tests with detailed traceback
pytest --tb=long
```

## Configuration Module

```python
# src/config.py
import os
from pathlib import Path
from dotenv import load_dotenv

# Load environment variables
env_path = Path(__file__).parent.parent / '.env'
load_dotenv(dotenv_path=env_path)

class Config:
    """Base configuration"""
    DEBUG = False
    TESTING = False
    LOG_LEVEL = os.getenv('LOG_LEVEL', 'INFO')
    
class DevelopmentConfig(Config):
    """Development configuration"""
    DEBUG = True
    LOG_LEVEL = 'DEBUG'
    
class TestingConfig(Config):
    """Testing configuration"""
    TESTING = True
    LOG_LEVEL = 'DEBUG'
    
class ProductionConfig(Config):
    """Production configuration"""
    DEBUG = False
    LOG_LEVEL = 'WARNING'

# Select configuration based on environment
config_name = os.getenv('FLASK_ENV', 'development')
config = {
    'development': DevelopmentConfig,
    'testing': TestingConfig,
    'production': ProductionConfig
}.get(config_name, DevelopmentConfig)
```

## Logging Configuration

```python
# src/logger.py
import logging
import logging.handlers
from src.config import config

def setup_logging(log_file='app.log', level=None):
    """Configure application logging"""
    
    level = level or getattr(logging, config.LOG_LEVEL)
    
    # Create logger
    logger = logging.getLogger('OsTyping')
    logger.setLevel(level)
    
    # File handler
    fh = logging.handlers.RotatingFileHandler(
        log_file,
        maxBytes=10485760,  # 10MB
        backupCount=5
    )
    fh.setLevel(level)
    
    # Console handler
    ch = logging.StreamHandler()
    ch.setLevel(level)
    
    # Formatter
    formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    
    # Add handlers
    logger.addHandler(fh)
    logger.addHandler(ch)
    
    return logger
```

## .env File Example

```bash
# Environment configuration
ENVIRONMENT=development
DEBUG=True
LOG_LEVEL=DEBUG

# Database
DATABASE_URL=postgresql://user:password@localhost/os_typing
DATABASE_ECHO=True

# API
API_HOST=0.0.0.0
API_PORT=8000
API_WORKERS=4

# Security
SECRET_KEY=your-secret-key-change-in-production
JWT_SECRET=your-jwt-secret-key
JWT_ALGORITHM=HS256

# Ansible
ANSIBLE_CONFIG=/etc/ansible/ansible.cfg
ANSIBLE_INVENTORY=inventory/hosts.ini
```

## Testinfra Fixtures

```python
# tests/conftest.py
import pytest
from testinfra import get_host

@pytest.fixture
def local_host():
    """Get local host for testing"""
    return get_host("local://")

@pytest.fixture
def docker_host():
    """Get Docker container for testing"""
    return get_host("docker://ubuntu:22.04")

@pytest.fixture
def ssh_host():
    """Get SSH host for testing"""
    return get_host("ssh://user@remote.host")

# Usage in tests
def test_kernel_parameters(local_host):
    ip_forward = local_host.sysctl("net.ipv4.ip_forward")
    assert ip_forward == 0
```

## Molecule Configuration

```yaml
# molecule/default/molecule.yml
---
dependency:
  name: galaxy
driver:
  name: docker
platforms:
  - name: ubuntu-22.04
    image: ubuntu:22.04
    pre_build_image: true
    privileged: true
provisioner:
  name: ansible
  playbooks:
    converge: converge.yml
    verify: verify.yml
verifier:
  name: testinfra
  directory: tests
  options:
    v: 1
scenario:
  name: default
  test_sequence:
    - create
    - converge
    - verify
    - destroy
```

## Running Molecule Tests

```bash
# Run full test lifecycle
molecule test

# Only converge (apply playbook)
molecule converge

# Only verify (run tests)
molecule verify

# Login to instance
molecule login -s default

# Show instances
molecule list
```

## Best Practices

- Use virtual environments to isolate dependencies
- Pin package versions in requirements.txt
- Use pytest markers to organize tests
- Implement fixtures for common test setup
- Use coverage reports to identify untested code
- Configure logging properly for debugging
- Use .env files for environment-specific configuration
- Keep test data separate from production data
- Use testinfra for infrastructure testing
- Document configuration changes in version control
