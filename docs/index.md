# OS Typing — API Documentation

Welcome to the **OS Typing** project documentation. This site contains comprehensive API documentation for all C, C++, and Python modules.

## 📚 Documentation

- **[Doxygen API Docs](html/index.html)** — Full API reference including classes, functions, and modules

## 📋 Quick Links

### C/C++ Modules

- **os_checking**: Portable OS detection helpers (Windows, Unix, Linux, macOS)
- **terminal**: Simple command-line REPL framework for interactive applications
- **os_controlsystem**: OS hardening control utility with config-driven checks

### Python Utilities

- **os_controlsystem_to_junit**: Convert os_controlsystem output to JUnit XML for CI reporting

## 🚀 Getting Started

1. Browse the **[API Documentation](html/index.html)** for detailed information on each module
2. Visit the [Project Repository](../) for source code and deployment guides
3. Check [DEPLOY.md](../DEPLOY.md) for deployment and hardening instructions

## 🔧 Building Documentation Locally

```bash
# Install Doxygen and Graphviz
sudo apt-get install doxygen graphviz

# Generate documentation
doxygen Doxyfile

# Open docs/html/index.html in your browser
```

## 📝 License & Attribution

This documentation is auto-generated from source code comments using [Doxygen](https://www.doxygen.nl/).

---

*Last updated: $(date)*
