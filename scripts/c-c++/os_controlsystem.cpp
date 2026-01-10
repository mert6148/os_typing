// os_controlsystem.cpp
// Portable system control/check utility for os_typing project.
// - On Linux: checks for sysctl file, systemd service status, and UFW rules.
// - On Windows: verifies presence of hardening script and prints suggested checks.
// Build: g++ -std=c++17 -O2 -o os_controlsystem os_controlsystem.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#define popen _popen
#define pclose _pclose
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

static bool file_exists(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

static bool file_contains(const std::string &path, const std::string &needle) {
    std::ifstream ifs(path);
    if (!ifs) return false;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.find(needle) != std::string::npos) return true;
    }
    return false;
}

// Executes a command and captures stdout. Returns exit code via out_exit.
static std::string run_cmd(const std::string &cmd, int &out_exit) {
    std::array<char, 256> buffer{};
    std::string result;
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        out_exit = -1;
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    int rc = pclose(pipe);
#ifndef _WIN32
    if (WIFEXITED(rc)) out_exit = WEXITSTATUS(rc);
    else out_exit = rc;
#else
    out_exit = rc;
#endif
    return result;
}

// Lightweight JSON reader for small config files. This is NOT a full JSON parser but
// sufficient for our simple structured `hardening_config.json` files used in CI.
// It fills optional fields: service_name, service_exec, service_port and sysctl map.
static bool load_config(const std::string &path, std::string &out_service_name, std::string &out_service_exec, int &out_service_port, std::map<std::string,std::string> &out_sysctl) {
    std::ifstream ifs(path);
    if (!ifs) return false;
    std::string s((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    auto get_string = [&](const std::string &key)->std::string {
        std::string pat = "\"" + key + "\"";
        auto pos = s.find(pat);
        if (pos == std::string::npos) return std::string();
        auto colon = s.find(':', pos);
        if (colon == std::string::npos) return std::string();
        auto first_quote = s.find('"', colon+1);
        if (first_quote == std::string::npos) return std::string();
        auto second_quote = s.find('"', first_quote+1);
        if (second_quote == std::string::npos) return std::string();
        return s.substr(first_quote+1, second_quote-first_quote-1);
    };

    auto get_int = [&](const std::string &key)->int {
        std::string pat = "\"" + key + "\"";
        auto pos = s.find(pat);
        if (pos == std::string::npos) return -1;
        auto colon = s.find(':', pos);
        if (colon == std::string::npos) return -1;
        size_t i = colon+1;
        while (i < s.size() && isspace((unsigned char)s[i])) ++i;
        size_t j = i;
        while (j < s.size() && (isdigit((unsigned char)s[j]) || s[j]=='-')) ++j;
        if (j <= i) return -1;
        std::string num = s.substr(i, j-i);
        try { return std::stoi(num); } catch (...) { return -1; }
    };

    // sysctl map: find the "sysctl" object and extract "key": "value" pairs
    auto pos_sys = s.find("\"sysctl\"");
    if (pos_sys != std::string::npos) {
        auto brace = s.find('{', pos_sys);
        if (brace != std::string::npos) {
            size_t p = brace+1;
            while (p < s.size()) {
                // find next key
                auto q = s.find('"', p);
                if (q == std::string::npos) break;
                auto r = s.find('"', q+1);
                if (r == std::string::npos) break;
                std::string k = s.substr(q+1, r-q-1);
                auto colon = s.find(':', r);
                if (colon == std::string::npos) break;
                auto v1 = s.find('"', colon);
                if (v1 == std::string::npos) break;
                auto v2 = s.find('"', v1+1);
                if (v2 == std::string::npos) break;
                std::string v = s.substr(v1+1, v2-v1-1);
                out_sysctl[k] = v;
                p = v2 + 1;
                // stop when closing brace found near p
                auto closing = s.find('}', p);
                if (closing != std::string::npos && closing < s.find(',', p)) {
                    break;
                }
            }
        }
    }

    std::string svc = get_string("service_name");
    if (!svc.empty()) out_service_name = svc;
    std::string exec = get_string("service_exec");
    if (!exec.empty()) out_service_exec = exec;
    int port = get_int("service_port");
    if (port != -1) out_service_port = port;

    return true;
}

static void usage(const char *prog) {
    std::cerr << "Usage: " << prog << " [--service-name NAME] [--service-port PORT] [--checks all|sysctl|service|firewall] [--config path]" << std::endl;
    std::cerr << "Examples:\n  " << prog << " --checks all\n  " << prog << " --service-name os_typing --service-port 12345 --checks service,firewall --config tests/hardening_config.json" << std::endl;
}

int main(int argc, char **argv) {
    std::string service_name = "os_typing";
    int service_port = 12345;
    std::vector<std::string> checks;

    // simple arg parsing
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--help" || a == "-h") { usage(argv[0]); return 0; }
        if (a == "--service-name" && i + 1 < argc) { service_name = argv[++i]; continue; }
        if (a == "--service-port" && i + 1 < argc) { service_port = std::atoi(argv[++i]); continue; }
        if (a == "--checks" && i + 1 < argc) {
            std::string arg = argv[++i];
            if (arg == "all") { checks = {"sysctl","service","firewall"}; }
            else {
                std::stringstream ss(arg);
                std::string item;
                while (std::getline(ss, item, ',')) checks.push_back(item);
            }
            continue;
        }
    }
    if (checks.empty()) checks = {"all"};
    // normalize
    bool do_sysctl = false, do_service = false, do_firewall = false;
    for (auto &c : checks) {
        if (c == "all") { do_sysctl = do_service = do_firewall = true; break; }
        if (c == "sysctl") do_sysctl = true;
        if (c == "service") do_service = true;
        if (c == "firewall") do_firewall = true;
    }

    bool is_linux = false;
#ifdef __linux__
    is_linux = true;
#endif

    int exit_code = 0;

    if (is_linux) {
        std::cout << "Platform: Linux (detected)\n";

        // If a config file is present, prefer config-driven checks
        std::string cfg_path = "tests/hardening_config.json";
        std::string cfg_service_name = service_name;
        std::string cfg_service_exec;
        int cfg_service_port = service_port;
        std::map<std::string,std::string> cfg_sysctl;
        if (!file_exists(cfg_path)) {
            // try relative to current working dir (already there) - nothing else
        } else {
            if (load_config(cfg_path, cfg_service_name, cfg_service_exec, cfg_service_port, cfg_sysctl)) {
                std::cout << "[config] Loaded config from " << cfg_path << "\n";
            }
        }

        if (do_sysctl) {
            if (!cfg_sysctl.empty()) {
                for (const auto &kv : cfg_sysctl) {
                    const auto &key = kv.first;
                    const auto &expected = kv.second;
                    int rc = 0;
                    std::string out = run_cmd(std::string("sysctl -n ") + key + " 2>&1", rc);
                    if (rc != 0 || out.find("no such file or directory") != std::string::npos || out.find("unknown oid") != std::string::npos) {
                        std::cout << "[sysctl:" << key << "] MISSING\n";
                        exit_code |= 1;
                    } else {
                        std::string value = out;
                        // trim
                        while (!value.empty() && (value.back() == '\n' || value.back() == '\r' || value.back() == ' ')) value.pop_back();
                        if (value == expected) {
                            std::cout << "[sysctl:" << key << "] OK\n";
                        } else {
                            std::cout << "[sysctl:" << key << "] MISMATCH expected=" << expected << " got=" << value << "\n";
                            exit_code |= 1;
                        }
                    }
                }
            } else {
                const std::string path = "/etc/sysctl.d/99-os_typing.conf";
                std::cout << "[sysctl] Checking " << path << " ... ";
                if (file_exists(path)) {
                    bool ok = file_contains(path, "kernel.randomize_va_space") || file_contains(path, "fs.file-max");
                    if (ok) std::cout << "OK\n";
                    else { std::cout << "MISSING expected keys\n"; exit_code |= 1; }
                } else { std::cout << "MISSING\n"; exit_code |= 1; }
            }
        }

        if (do_service) {
            std::cout << "[service] Checking systemd service '" << cfg_service_name << "' ... ";
            int rc = 0;
            std::string out = run_cmd("systemctl is-active " + cfg_service_name + " 2>&1", rc);
            if (rc == 0 && out.find("active") != std::string::npos) {
                std::cout << "active\n";
            } else {
                std::cout << "not active (output: " << out << ")\n";
                exit_code |= 2;
            }

            if (!cfg_service_exec.empty()) {
                std::string unit_path = std::string("/etc/systemd/system/") + cfg_service_name + ".service";
                if (file_exists(unit_path)) {
                    std::ifstream f(unit_path);
                    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                    if (content.find(cfg_service_exec) != std::string::npos) {
                        std::cout << "[service:exec] OK\n";
                    } else {
                        std::cout << "[service:exec] MISMATCH expected ExecStart contains: " << cfg_service_exec << "\n";
                        exit_code |= 2;
                    }
                } else {
                    std::cout << "[service:exec] unit file missing: " << unit_path << "\n";
                    exit_code |= 2;
                }
            }
        }

        if (do_firewall) {
            std::cout << "[firewall] Checking UFW status and port " << cfg_service_port << " ... ";
            int rc = 0;
            std::string out = run_cmd("ufw status verbose 2>&1", rc);
            if (rc == 0 && out.find("Status: active") != std::string::npos) {
                std::stringstream ss(out);
                std::string line;
                bool port_ok = false;
                while (std::getline(ss, line)) {
                    if (line.find(std::to_string(cfg_service_port)) != std::string::npos && (line.find("ALLOW") != std::string::npos)) {
                        port_ok = true; break;
                    }
                }
                if (port_ok) std::cout << "active and port allowed\n";
                else { std::cout << "active but port NOT allowed\n"; exit_code |= 4; }
            } else {
                std::cout << "ufw not active or ufw not installed (output: " << out << ")\n";
                exit_code |= 4;
            }
        }
    } else {
        std::cout << "Platform: Non-Linux (Windows or others). Running basic checks...\n";
        if (do_sysctl) {
            std::cout << "[sysctl] Not applicable on Windows — skip\n";
        }
        if (do_service) {
            std::string path = "deploy\\windows\\hardening.ps1";
            std::cout << "[service] Checking presence of " << path << " ... ";
            if (file_exists(path)) std::cout << "FOUND\n";
            else { std::cout << "MISSING\n"; exit_code |= 2; }
        }
        if (do_firewall) {
            std::cout << "[firewall] Suggestion: run 'Get-NetFirewallProfile' in an elevated PowerShell to inspect firewall status.\n";
        }
    }

    if (exit_code == 0) std::cout << "All requested checks passed.\n";
    else std::cout << "Some checks failed (exit code: " << exit_code << "). Review output above.\n";

    return exit_code;
}
