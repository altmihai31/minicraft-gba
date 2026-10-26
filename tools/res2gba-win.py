# Run res2gba on Windows: its converters are extensionless Python scripts
# launched via os.system, which cmd.exe cannot execute. Prefix the interpreter.
#
# usage: python tools/res2gba-win.py tools/res2gba/res2gba res/resources.json
import os, runpy, subprocess, sys

def system(cmd):
    return subprocess.call('"%s" %s' % (sys.executable, cmd), shell=True)

os.system = system
script = sys.argv[1]
sys.argv = sys.argv[1:]
sys.path[0] = os.path.dirname(os.path.abspath(script))
runpy.run_path(script, run_name='__main__')
