"""Find location of test executable."""

import os

import layout_exceptions

build_target_suggestion_message = (
    'Did you forget to build the ninja target "lb_layout_tests" first?')


def FindTestExecutable(default_build_order, use_build, exe_name, exe_base_dir):
  def PathToExe(build):
    return os.path.join(exe_base_dir, build, exe_name)

  if use_build:
    path_to_exe = PathToExe(use_build)
    # If provided, search for and use the specified executable
    if os.path.exists(path_to_exe):
      return os.path.expanduser(path_to_exe)
    else:
      raise layout_exceptions.TestClientError(
          'Unable to find layout test executable\n{}\n{}'.format(
              path_to_exe, build_target_suggestion_message))
  else:
    # Search for the layout test executable in the project 'out' directory
    for build in default_build_order:
      path_to_exe = PathToExe(build)
      if os.path.exists(path_to_exe):
        return os.path.expanduser(path_to_exe)

    raise layout_exceptions.TestClientError(
        'Unable to find layout test executable in base directory\n'
        '"{}"\n after searching through sub-directories {}.\n'
        '{}'.format(exe_base_dir,
                    str(default_build_order),
                    build_target_suggestion_message))
