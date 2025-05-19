# test_extra_script.py
import os
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

if env.get("PIOTEST_RUNNING", False): # Check if tests are running
    # Add the path to room.cpp relative to the project root
    project_dir = env.subst("$PROJECT_DIR")
    room_cpp_path = os.path.join(project_dir, "src", "room.cpp")

    # Add the source file to the build
    env.Append(SRCBUILD_DIR=[room_cpp_path])
    env.Append(LIBS=["${_LIBFLAGS}"]) # Ensure libraries are linked