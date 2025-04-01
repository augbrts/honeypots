# honeypots
honeypots/ deception, info acquisition/high-interection

## Overview
This project is a deception-based honeypot designed to detect unauthorized access attempts. It simulates common services to trick attackers into interacting with fake systems and logs their actions for security monitoring.

### Features
- Fake HTTP API (port 8080): Mimics a RESTful API and logs unauthorized requests.
- Fake SSH Server (port 2222): Imitates an SSH service and denies all login attempts.
- Fake MySQL Database (port 3306): Responds to SQL queries with fake data while logging attacker interactions.

## Installation
### Prerequisites
- Linux-based system (Ubuntu, Debian, etc.)
- GCC compiler

### Compilation
```bash
gcc honeypot.c -o honeypot -pthread
```

### Running the Honeypot
```bash
sudo ./honeypot
```

## Testing
You can test the honeypot locally using:
```bash
# Test HTTP API
curl http://127.0.0.1:8080

# Test SSH Service
nc 127.0.0.1 2222

# Test Fake Database
echo "SELECT * FROM users;" | nc 127.0.0.1 3306
```

## Logging
All unauthorized access attempts are logged in `honeypot.log`.

## Security Considerations
- Run in an isolated environment (e.g., a VM or container) to prevent unintended exposure.
- Monitor network traffic to ensure only expected connections interact with the honeypot.

## Future Enhancements
- Implementing dynamic deception techniques
- Integration with SIEM systems for real-time alerts

## License
This project is open-source and free to use.

