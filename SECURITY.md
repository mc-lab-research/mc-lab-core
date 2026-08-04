# Security Policy

Security issues in **mc-lab-core** should be reported privately.

## Supported Versions

Only the latest release and the current default branch, `master`, are supported.

Older versions, forks, and modified distributions are not maintained for security fixes.

## Reporting a Vulnerability

Do not open a public issue or pull request for a suspected vulnerability.

Use GitHub's **[Private Vulnerability Reporting](https://github.com/mc-lab-research/mc-lab-core/security/advisories/new)** feature.
Alternatively, open the repository **Security** tab and choose **Report a vulnerability**.

Please include:

* a description of the issue;
* the affected version or commit;
* reproduction steps or a proof of concept;
* the expected security impact;
* your platform, compiler, and build configuration;
* any suggested mitigation.

Relevant reports may concern the C++ code, parsers, build system, dependencies, GitHub Actions, secrets, artifacts, or software supply chain.

The repository security baseline and its current validation evidence are tracked in [docs/security/baseline.md](docs/security/baseline.md).

## Response

Maintainers will aim to acknowledge reports within **7 days**, investigate the issue, prepare a fix, and coordinate disclosure with the reporter.

Please avoid public disclosure until a fix or mitigation is available.

## Safe Harbor

Good-faith security research that avoids harm, respects privacy, and follows coordinated disclosure is welcome.
