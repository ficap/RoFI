#!/usr/bin/env python3

import sys
import os
from common import *

flags = [
    "--all",
    "--list",
    "--doc",
    "--help",
    ]

word = int(sys.argv[1])
words = sys.argv[3:]

if len(words) > 0 and words[0].startswith("--"):
    if word == 1:
        candidates = [ s for s in flags if s.startswith(words[0]) ]
        print("\n".join(candidates))
    sys.exit(0)

candidates = []
if word == 1:
    candidates += flags

targets = availableTargets()
candidates += [t.target for t in targets]
candidates += [t.path for t in targets]

suites = availableSuites()
candidates += suites
candidates += ["clean-" + s for s in suites]

candidates += ["clean"]


if word >= 1 and word - 1 < len(words):
    candidates = [ s for s in candidates if s.startswith(words[word - 1]) ]
candidates = set(candidates).difference(words)

if len(candidates) > 0:
    print("\n".join(candidates))