# U2IP — URL to IP

**U2IP** (Version 1.2) is a lightweight command-line tool written in C that resolves domain names or URLs to IPv4 addresses and fetches server geolocation details. Built using standard POSIX sockets and native libraries, it operates efficiently without external library dependencies.

---

## Features

- **Domain Resolution**: Converts web URLs (HTTP/HTTPS) or hostname inputs into IPv4 addresses using DNS lookup.
- **Server Geolocation**: Retrieves real-time server information including Country, Region, City, ISP, and Organization via `ip-api.com`.
- **Command-Line Interface**: Supports both short (`-`) and long (`--`) option flags.
- **Multilingual Support**: Supports console output in English (`en`) and Spanish (`es`).
- **Zero External Dependencies**: Standard POSIX C network sockets implementation.

---

## Prerequisites

To compile and run U2IP, you need:

- A C compiler (e.g., `gcc` or `clang`).
- POSIX-compliant operating system (Linux, macOS, WSL, FreeBSD).
- Active internet connection for DNS resolution and API querying.

---

## Installation & Compilation

Clone or download the repository, then compile `u2ip.c` using `gcc`:

```bash
gcc u2ip.c -o U2IP
```

Optionally, move the binary to your system PATH for global execution:

```bash
sudo mv U2IP /usr/local/bin/
```

---

## Usage

You can pass a domain or URL directly as an argument, or use flags for specific options.

### Basic Command Syntax

```bash
./U2IP [options] <URL or Domain>
```

### Options

| Short Flag | Long Flag | Description |
| :--- | :--- | :--- |
| `-h` | `--help` | Display the help message and exit |
| `-v` | `--version` | Print program version (`1.2`) |
| `-b` | `--build` | Display build timestamp and compiler details |
| `-l <lang>` | `--lang <en\|es>` | Select output language (`en` for English, `es` for Spanish) |
| `-u <url>` | `--url <URL>` | Specify target URL or hostname explicitly |

---

## Examples

1. **Resolve a URL in English (default):**
   ```bash
   ./U2IP google.com
   ```

2. **Specify target URL using `--url` flag:**
   ```bash
   ./U2IP --url https://www.github.com
   ```

3. **Run in Spanish (`--lang es`):**
   ```bash
   ./U2IP -l es -u https://www.wikipedia.org
   ```

4. **Check program version:**
   ```bash
   ./U2IP --version
   ```

5. **Display build info:**
   ```bash
   ./U2IP --build
   ```

---

## Output Sample

```text
[*] U2IP v1.2
[*] Analyzing URL: https://www.github.com
[*] Extracted Host: www.github.com
[+] Internet IP:   140.82.121.4
[*] Fetching geolocation data...

========================================
    U2IP v1.2 - GEOLOCATION DATA         
========================================
 Country:       United States (US)
 Region/State:  California
 City:          San Francisco
 ISP:           GitHub, Inc.
 Organization:  GitHub, Inc.
========================================
```

---

## License

This project is open-source software licensed under the **GNU General Public License v3.0 (GPL-3.0)**.
See the full [GNU General Public License v3.0](LICENSE) for details.
