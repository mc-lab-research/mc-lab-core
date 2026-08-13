# A01: generated owning-value attempt

A01 generates transitions through a shared single-pass cursor, but dereference
returns an owning value containing copied label and target fields. A previously
returned witness therefore survives iterator advancement. The attempt did not
instantiate ARC-001's frozen ephemeral-proxy lifetime and is INCONCLUSIVE as
the controlled result.

Its successful unchanged-consumer run remains useful Exploratory evidence for
the narrower by-value representation. Historical observation tip:
`9a7aa04117d6826cd0f4216012ea882558107354`.
