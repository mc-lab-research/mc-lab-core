# A02: generated ephemeral-proxy attempt

A02 returns a non-owning proxy containing only a range handle and generation
token. Label and target remain in range-owned storage for the current
occurrence, and iterator advancement makes prior proxies stale. Both copied
iterator handles observe the new shared position through fresh proxies.

Sami Lazreg accepted A02 as Controlled evidence supporting H-A within the
frozen scope. Final executable evidence:
`40c50553c1d64a6ed4e74faa951843966d609ca1`; accepted result tag:
`arc-001-a02-supports-ha`.
