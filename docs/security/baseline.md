# Repository security baseline

This document records the security baseline reviewed for the **mcLab v0.1.0** engineering milestone and the evidence used to validate it.

Validated on **2026-07-30**.

## Dependency management

| Control | Evidence | Status |
| --- | --- | --- |
| Dependabot version updates | [`.github/dependabot.yml`](../../.github/dependabot.yml) configures weekly GitHub Actions updates with bounded open PRs, labels, and commit message prefixes. | Validated |
| Dependabot security updates | GitHub uses the same Dependabot configuration for GitHub Actions advisories and version updates in the configured ecosystem. | Validated |
| Dependabot security alerts | Alerts are reviewed in the repository **Security** tab. Direct alert enumeration is not available through the current integration, so maintainers must confirm the tab remains enabled before release. | Maintainer confirmation required |

## Static analysis

| Control | Evidence | Status |
| --- | --- | --- |
| CodeQL is operational | The repository exposes an active **CodeQL** workflow in GitHub Actions. | Validated |
| CodeQL executes successfully in CI | PR #25 check runs and workflow run `30523720182` completed successfully for both `Analyze (actions)` and `Analyze (c-cpp)` on 2026-07-30. | Validated |
| CodeQL results are reviewed | Latest PR CodeQL jobs uploaded SARIF successfully and finished without failed jobs. Ongoing alert triage still requires maintainer review in the **Security** tab. | Partially validated |

## Repository security

| Control | Evidence | Status |
| --- | --- | --- |
| Private vulnerability reporting | [`SECURITY.md`](../../SECURITY.md) and the public repository security page direct reporters to GitHub **Private Vulnerability Reporting**. | Policy validated |
| Secret scanning | Secret scanning alerts cannot be enumerated through the current integration (`403 Resource not accessible by integration`). Maintainers must confirm the feature remains enabled in repository settings. | Maintainer confirmation required |

## Supply chain

| Control | Evidence | Status |
| --- | --- | --- |
| OpenSSF Scorecard executed | No published Scorecard result or repository workflow was found during this review. | Follow-up required |
| Scorecard findings reviewed | Blocked until a Scorecard run is available. | Follow-up required |
| Follow-up issues created where appropriate | This review identifies Scorecard publication and repository-level security setting confirmations as required follow-up work. | Follow-up required |

## Security posture

| Control | Evidence | Status |
| --- | --- | --- |
| No unresolved Critical findings remain | The latest CodeQL run completed successfully. Direct code scanning alert enumeration is not available through the current integration, so maintainers must confirm there are no open Critical alerts in the **Security** tab. | Maintainer confirmation required |
| Remaining High findings are reviewed and justified or tracked | No public security tracking issues are open in the repository. Maintainers must review private alerts directly in GitHub Security. | Maintainer confirmation required |

## Follow-up actions

1. Confirm **Dependabot alerts**, **secret scanning**, and **private vulnerability reporting** remain enabled in repository settings.
2. Review the **Security** tab for any open CodeQL or Dependabot alerts before the v0.1.0 release decision.
3. Add an OpenSSF Scorecard execution path and track its findings in a dedicated follow-up issue.
