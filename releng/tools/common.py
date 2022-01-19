import sys
import os
from pathlib import Path

root = os.environ["ROFI_ROOT"]
buildCfg = os.environ["ROFI_BUILD_CONFIGURATION"]
buildDir = os.environ["ROFI_BUILD_DIR"]
suiteDir = os.path.join(root, "suites")

def availableSuites():
    return [x for x in os.listdir(suiteDir)
        if os.path.isdir(os.path.join(suiteDir, x))]

def configuredSuites():
    try:
        return os.listdir(buildDir)
    except FileNotFoundError:
        return []

def availableTestTargets():
    return [x for x in availableTargets() if x.target.startswith("test-")]

class TargetRecord:
    def __init__(self, target, path, suite):
        self.target = target
        self.path = path
        self.suite = suite

    def __repr__(self):
        return f"{self.suite}::{self.target} ({self.path})"

def availableTargets():
    targets = []
    for suite in configuredSuites():
        suiteTargets = []
        try:
            with open(os.path.join(buildDir, suite, "targets.txt")) as f:
                suiteTargets = f.readlines()
        except Exception as e:
            pass # Ingore uncofigured targets
        for path, target in [x.strip().split(":") for x in suiteTargets]:
            targets.append(TargetRecord(target, path, suite))
    return targets

def dependencyList():
    deps = {}
    for suite in configuredSuites():
        try:
            with open(os.path.join(buildDir, suite, "dependencies.txt")) as f:
                for l in f.readlines():
                    t, d = tuple(l.split(": "))
                    d = [x.strip() for x in d.strip().split(";") if len(x.strip()) > 0]
                    deps[t] = d
        except Exception as e:
            pass # Ingore uncofigured targets
    return deps

def sourceList():
    sources = {}
    for suite in configuredSuites():
        try:
            with open(os.path.join(buildDir, suite, "sources.txt")) as f:
                for l in f.readlines():
                    t, s = tuple(l.split(": "))
                    s = [x.strip() for x in s.strip().split(";") if len(x.strip()) > 0]
                    sources[t] = s
        except Exception as e:
            pass # Ingore uncofigured targets
    return sources
