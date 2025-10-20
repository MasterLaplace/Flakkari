#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
# File name: increment_version.py
# Author: MasterLaplace
# Created on: 2023-10-12

import re
import os
import sys

def get_new_version(calculated_version: str) -> str:
    match = re.search(r'v(\d+\.\d+\.\d+)', calculated_version)
    if match:
        version_number = match.group(1)
        major, minor, patch = map(int, version_number.split('.'))
        return f'{major}.{minor}.{patch}'

    raise ValueError(f"Invalid version format: {calculated_version}")


def get_current_version() -> str:
    with open("VERSION", "r") as version_file:
        current_version = version_file.read().strip()
        match = re.search(r'v(\d+\.\d+\.\d+)', current_version)
        if match:
            return match.group(1)

        raise ValueError(f"Invalid version format: {current_version}")

def update_doxyfile_version(new_version: str) -> None:
    """
    Updates the version number in the Doxyfile.cfg file for the Flakkari-Server project.

    Args:
        new_version (str): The new version number to set in the format 'major.minor.patch'.

    Returns:
        None
    """
    with open("Doxyfile.cfg", "r") as f:
        content = f.read()

    content = re.sub(r'PROJECT_NUMBER\s+=\s+\d+\.\d+\.\d+', f'PROJECT_NUMBER         = {new_version}', content)

    with open("Doxyfile.cfg", "w", newline='\n') as f:
        f.write(content)

def update_cmakelists_version(new_version: str) -> None:
    """
    Updates the version number in the CMakeLists.txt file for the Flakkari-Server project.

    Args:
        new_version (str): The new version number to set in the format 'major.minor.patch'.

    Returns:
        None
    """
    with open("CMakeLists.txt", "r") as f:
        content = f.read()

    content = re.sub(r'set\(FLAKKARI_VERSION_MAJOR \d+', f'set(FLAKKARI_VERSION_MAJOR {new_version.split(".")[0]}', content)
    content = re.sub(r'set\(FLAKKARI_VERSION_MINOR \d+', f'set(FLAKKARI_VERSION_MINOR {new_version.split(".")[1]}', content)
    content = re.sub(r'set\(FLAKKARI_VERSION_PATCH \d+', f'set(FLAKKARI_VERSION_PATCH {new_version.split(".")[2]}', content)

    content = re.sub(r'project\(Flakkari-Server VERSION \d+\.\d+\.\d+', f'project(Flakkari-Server VERSION {new_version}', content)

    with open("CMakeLists.txt", "w", newline='\n') as f:
        f.write(content)

def update_xmake_version(new_version: str) -> None:
    """
    Updates the version number in the xmake.lua file for the Flakkari-Server project.

    Args:
        new_version (str): The new version number to set in the format 'major.minor.patch'.

    Returns:
        None
    """
    with open("xmake.lua", "r") as f:
        content = f.read()

    content = re.sub(r'set_version\("(\d+\.\d+\.\d+)"\)', f'set_version("{new_version}")', content)

    with open("xmake.lua", "w", newline='\n') as f:
        f.write(content)

def update_config_in_version(path: str, new_version: str) -> None:
    """
    Updates the version information in the specified configuration file.

    This function reads the content of the file at the given path, updates the version
    information to the new version provided, and writes the updated content back to the file.
    The version information is expected to be in the format 'major.minor.patch'.

    Args:
        path (str): The path to the configuration file to be updated.
        new_version (str): The new version string in the format 'major.minor.patch'.

    Returns:
        None
    """
    major, minor, patch = map(int, new_version.split('.'))

    with open(path, "r") as f:
        content = f.read()

    content = re.sub(r'(\d+\.\d+\.\d+)', f"{major}.{minor}.{patch}", content)
    content = re.sub(r'#define FLAKKARI_VERSION_MAJOR \d+', f'#define FLAKKARI_VERSION_MAJOR {major}', content)
    content = re.sub(r'#define FLAKKARI_VERSION_MINOR \d+', f'#define FLAKKARI_VERSION_MINOR {minor}', content)
    content = re.sub(r'#define FLAKKARI_VERSION_PATCH \d+', f'#define FLAKKARI_VERSION_PATCH {patch}', content)

    with open(path, "w", newline='\n') as f:
        f.write(content)

def loop_in_files(path: str, current_version: str, new_version: str) -> None:
    """
    Recursively updates the version number in specified file types within a given directory.

    Args:
        path (str): The root directory path to start the search.
        current_version (str): The current version number to be replaced.
        new_version (str): The new version number to replace the current version.

    Returns:
        None

    This function searches for files with extensions .cpp, .h, .md and .hpp within the given directory
    and its subdirectories. It updates the version number in these files from the current version to the new version.
    """
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith("flakkari_config.h"):
                print(f"Updating version {current_version} -> {new_version} in {file}")
                update_config_in_version(os.path.join(root, file), new_version)

            elif file.endswith(".cpp") or file.endswith(".h") or file.endswith(".md") or file.endswith(".hpp"):
                print(f"Updating version {current_version} -> {new_version} in {file}")
                match = re.search(r'(\d+\.\d+\.\d+)', current_version)
                if match:
                    current_version_number = match.group(1)
                    with open(os.path.join(root, file), "r") as f:
                        content = f.read()
                    with open(os.path.join(root, file), "w", newline='\n') as f:
                        f.write(content.replace(current_version_number, new_version))

def apply_new_version(current_version: str, new_version: str):
    """
    Updates the version information in various project files.

    This function updates the version number in the following files:
    - VERSION
    - Doxyfile.cfg
    - CMakeLists.txt

    It also updates the version number in all relevant files within the ./Flakkari/ directory.

    Args:
        current_version (str): The current version of the project.
        new_version (str): The new version to be applied.

    Returns:
        None
    """
    print(f"Updating version {current_version} -> {new_version} in VERSION")
    with open("VERSION", "w", newline='\n') as version_file:
        version_file.write(f"Flakkari v{new_version}\n")

    print(f"Updating version {current_version} -> {new_version} in Doxyfile.cfg")
    update_doxyfile_version(new_version)

    print(f"Updating version {current_version} -> {new_version} in CMakeLists.txt")
    update_cmakelists_version(new_version)

    print(f"Updating version {current_version} -> {new_version} in xmake.lua")
    update_xmake_version(new_version)

    loop_in_files("./Flakkari/", current_version, new_version)

def main(calculated_version: str):
    """
    Main function to apply a new version based on the calculated version.

    Args:
        calculated_version (str): The version string that has been calculated and needs to be applied.
    """
    apply_new_version(get_current_version(), get_new_version(calculated_version))

if __name__ == "__main__" and len(sys.argv) == 2:
    main(sys.argv[1])
